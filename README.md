PCSX-ReARMed - yet another PCSX fork
====================================

![CI (Linux)](https://github.com/notaz/pcsx_rearmed/workflows/CI%20(Linux)/badge.svg)

PCSX ReARMed is yet another PCSX fork based on the PCSX-Reloaded project,
which itself contains code from PCSX, PCSX-df and PCSX-Revolution. This
version was originally ARM architecture oriented (hence the name) with
its MIPS->ARM dynamic recompilation and assembly optimizations, but more
recently it targets other architectures too. A fork of this emulator was
used in [PS Classic](https://en.wikipedia.org/wiki/PlayStation_Classic)
(without any coordination or even notification).

## Features
* ARM/ARM64 dynamic recompiler by Ari64
* [lightrec](https://github.com/pcercuei/lightrec/) dynamic recompiler for other architectures
* NEON GPU by Exophase for ARM NEON and x86 SSE2+
* PCSX4ALL GPU by Una-i/senquack for other architectures
* heavily modified P.E.Op.S. SPU
* BIOS HLE emulation (most games run without proprietary BIOS)
* libretro support

## PS3 Cell SDK build

On a physical PS3, Lightrec requires HEN/CFW with PS3MAPI executable-memory
allocation available. It allocates its 8 MiB code buffer at runtime; use
the standard Cell SDK linker script without a separate `.ps3_jit` segment.
The startup self-test runs with diagnostic file output disabled by default
(`PS3_LIGHTREC_LOG=0`). To enable it, build with `PS3_LIGHTREC_LOG=1`.
The SDK's executable path is used to create a `logs` directory beside the
core SELF, containing `ps3-lightrec-selftest.log` and `ps3-lightrec-jit.bin`.
If the executable path is unavailable or writing fails, the self-test
continues without diagnostic files. These settings do not change the
frontend's normal logging.

The libretro core builds independently of the RetroArch source tree. With
GNU Make, Git Bash and the Cell SDK installed on Windows, set `CELL_SDK` to
the SDK directory and run from this repository's root:

```sh
make -f Makefile.libretro platform=ps3 DYNAREC=lightrec \
  CC="$CELL_SDK/host-win32/ppu/bin/ppu-lv2-gcc.exe" \
  CXX="$CELL_SDK/host-win32/ppu/bin/ppu-lv2-g++.exe" \
  AR="$CELL_SDK/host-win32/ppu/bin/ppu-lv2-ar.exe" \
  CFLAGS_OPT=-O2 ASSERTS=1 PS3_LIGHTREC_LOG=0
```

This produces `pcsx_rearmed_libretro_ps3.a`. Link this archive with a
RetroArch frontend built for the Cell SDK to produce the PS3 executable;
the core build alone does not produce an installable package.

Clean the core objects before changing `PS3_LIGHTREC_LOG`. Per-block JIT
tracing additionally requires `-DPS3_LIGHTREC_DIAGNOSTICS`; leave this
undefined for regular builds.

Host regression tests for log paths and startup without writable log files:

```sh
make -C tests/ps3 run
```

