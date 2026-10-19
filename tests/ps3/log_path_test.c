#include <stdio.h>
#include <string.h>
#include <cell/fs/cell_fs_file_api.h>
#include "../../libpcsxcore/lightrec/ps3.h"

static char *arguments[2];
static char **process_arguments = arguments;
static char directory[1024];
static char filename[1024];
static char long_path[1100];
static unsigned int mkdir_calls;
static unsigned int open_calls;
static int mkdir_result;
static int failures;

char **getargv(void)
{
   return process_arguments;
}

int cellFsMkdir(const char *path, unsigned int mode)
{
   mkdir_calls++;
   snprintf(directory, sizeof(directory), "%s", path);
   if (mode != 0777)
      failures++;
   return mkdir_result;
}

FILE *ps3_test_fopen(const char *path, const char *mode)
{
   open_calls++;
   snprintf(filename, sizeof(filename), "%s", path);
   if (strcmp(mode, "w"))
      failures++;
   return NULL;
}

static void check(int condition, const char *name)
{
   if (!condition) {
      printf("FAIL: %s\n", name);
      failures++;
   }
}

int main(void)
{
   arguments[0] = "/dev_hdd0/game/OTHER0001/USRDIR/cores/pcsx.self";
   lightrec_ps3_log_init();
   lightrec_ps3_open_log("test.log", "w");
   check(mkdir_calls == 1 && open_calls == 1 &&
      !strcmp(directory, "/dev_hdd0/game/OTHER0001/USRDIR/cores/logs") &&
      !strcmp(filename, "/dev_hdd0/game/OTHER0001/USRDIR/cores/logs/test.log"),
      "directory follows the core executable and title ID");

   arguments[0] = "/app_home/Nested/Core.SELF";
   mkdir_result = CELL_FS_EEXIST;
   lightrec_ps3_log_init();
   lightrec_ps3_open_log("test.log", "w");
   check(open_calls == 2 && !strcmp(filename, "/app_home/Nested/logs/test.log"),
      "existing directory and path case");

   mkdir_result = -1;
   lightrec_ps3_log_init();
   lightrec_ps3_open_log("test.log", "w");
   check(open_calls == 2, "unwritable directory disables file output");

   arguments[0] = "relative/Core.SELF";
   lightrec_ps3_log_init();
   lightrec_ps3_open_log("test.log", "w");
   arguments[0] = NULL;
   lightrec_ps3_log_init();
   process_arguments = NULL;
   lightrec_ps3_log_init();
   check(mkdir_calls == 3 && open_calls == 2, "missing or relative executable path");

   process_arguments = arguments;
   memset(long_path, 'a', sizeof(long_path) - 1);
   long_path[0] = '/';
   long_path[sizeof(long_path) - 3] = '/';
   arguments[0] = long_path;
   lightrec_ps3_log_init();
   lightrec_ps3_open_log("test.log", "w");
   check(mkdir_calls == 3 && open_calls == 2, "oversized executable path");

   arguments[0] = "/core.self";
   mkdir_result = 0;
   lightrec_ps3_log_init();
   lightrec_ps3_open_log(long_path, "w");
   check(mkdir_calls == 4 && open_calls == 2, "oversized output filename");
   if (failures)
      return 1;
   puts("PASS: core-relative log paths, unavailable paths and write failures");
   return 0;
}
