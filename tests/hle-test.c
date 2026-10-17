#include <stdio.h>
#include <string.h>
#include "../libpcsxcore/psxmem.h"
#include "../libpcsxcore/psxbios.h"
#include "../libpcsxcore/psxhle.h"

static void write32(u32 address, u32 value)
{
	psxMu32ref(address) = SWAP32(value);
}

static int test_exception_return(unsigned int kind)
{
	u32 chains, pcb, tcb;

	memset(&psxRegs.GPR, 0, sizeof(psxRegs.GPR));
	psxBiosInit();
	psxRegs.CP0.n.SR = 0x404;
	psxRegs.GPR.n.k0 = 0x12345678;
	psxRegs.psxNextCounter = 0x10000000;
	psxRegs.next_interupt = psxRegs.cycle + 0x10000000;
	chains = psxMu32(0x100);
	pcb = psxMu32(0x108);
	tcb = psxMu32(pcb);
	memset(psxRegs.ptrs.psxM + (chains & 0x1fffff), 0, 0x20);
	write32(chains, 0x80020000);
	write32(0x80020000, 0);
	write32(0x80020004, 0);
	write32(0x80020008, 0x80021000);
	write32(tcb + 0x88, 0x80030000); /* saved EPC */
	if (kind == 0) {
		write32(0x80021000, 0x3c1a8003); /* lui k0, 0x8003 */
		write32(0x80021004, 0x03400008); /* jr k0 */
		write32(0x80021008, 0x42000010); /* rfe */
	}
	else if (kind == 1) {
		write32(0x80021000, 0x03e00008); /* jr ra */
		write32(0x80021004, 0x00000000); /* nop */
	}
	else {
		write32(0x80021000, 0x24090017); /* addiu t1, zero, 0x17 */
		write32(0x80021004, 0x0800002c); /* j 0xb0 */
		write32(0x80021008, 0x00000000); /* nop */
	}
	/* Reentry must fail if execution overruns the exception return. */
	write32(0x80030000, (0x3bu << 26) | hleop_exception);
	psxCpu->Clear(0, ~0u);
	psxBiosException();
	if (psxRegs.pc != 0x80030000 || psxRegs.GPR.n.k0 != 0x80030000 ||
	    psxRegs.cpuInRecursion != 0) {
		fprintf(stderr, "FAIL: return=%u pc=%08x k0=%08x recursion=%u\n",
			kind, psxRegs.pc, psxRegs.GPR.n.k0, psxRegs.cpuInRecursion);
		return 1;
	}
	return 0;
}

static int test_pending_interrupt(void)
{
	memset(&psxRegs.GPR, 0, sizeof(psxRegs.GPR));
	psxBiosInit();
	psxRegs.CP0.n.SR = 0x401;
	psxRegs.CP0.n.Cause = 0;
	psxRegs.GPR.n.k0 = 0x12345678;
	psxRegs.pc = 0x80021000;
	psxRegs.psxNextCounter = 0x10000000;
	psxRegs.next_interupt = psxRegs.cycle + 0x10000000;
	write32(0x80021000, 0x1000ffff); /* beq zero, zero, -1 */
	write32(0x80021004, 0); /* nop */
	write32(0x80000080, (0x3bu << 26) | hleop_exception);
	psxHu32ref(0x1070) = SWAP32(1);
	psxHu32ref(0x1074) = SWAP32(1);
	psxRegs.cpuInRecursion = 1;
	psxCpu->Clear(0, ~0u);
	psxCpu->Notify(R3000ACPU_NOTIFY_AFTER_LOAD, (void *)(size_t)1);
	psxCpu->ExecuteBlock(&psxRegs, EXEC_CALLER_HLE);
	psxCpu->Notify(R3000ACPU_NOTIFY_BEFORE_SAVE, (void *)(size_t)1);
	psxRegs.cpuInRecursion--;
	psxHu32ref(0x1070) = 0;
	psxHu32ref(0x1074) = 0;
	if (psxRegs.pc != 0x80000080 || psxRegs.cpuInRecursion) {
		fprintf(stderr, "FAIL: pending interrupt pc=%08x recursion=%u\n",
			psxRegs.pc, psxRegs.cpuInRecursion);
		return 1;
	}
	return 0;
}

int main(void)
{
	unsigned int engine, kind, pass;
	setvbuf(stdout, NULL, _IONBF, 0);
	for (engine = CPU_DYNAREC; engine <= CPU_INTERPRETER; engine++) {
		Config.HLE = 1;
		Config.Cpu = engine;
		Config.cycle_multiplier = CYCLE_MULT_DEFAULT;
		if (psxInit())
			return 1;
		for (pass = 0; pass < 2; pass++)
			for (kind = 0; kind < 3; kind++)
				if (test_exception_return(kind))
					return 1;
		if (test_pending_interrupt())
			return 1;
		psxCpu->Shutdown();
		psxMemShutdown();
		printf("PASS: HLE exception returns engine=%u cases=7\n", engine);
	}
	return 0;
}
