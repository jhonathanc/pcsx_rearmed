#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cell/fs/cell_fs_file_api.h>
#include "ps3.h"

static char log_directory[1024];

void lightrec_ps3_log_init(void)
{
	char **argv = getargv();
	const char *slash;
	size_t len;
	int ret;

	log_directory[0] = 0;
	if (!argv || !argv[0] || argv[0][0] != '/')
		return;
	slash = strrchr(argv[0], '/');
	len = slash - argv[0];
	if (len + sizeof("/logs") > sizeof(log_directory))
		return;
	memcpy(log_directory, argv[0], len);
	memcpy(log_directory + len, "/logs", sizeof("/logs"));
	ret = cellFsMkdir(log_directory, 0777);
	if (ret && ret != (int)CELL_FS_EEXIST)
		log_directory[0] = 0;
}

FILE *lightrec_ps3_open_log(const char *name, const char *mode)
{
	char path[1024];
	int len;

	if (!log_directory[0])
		return NULL;
	len = snprintf(path, sizeof(path), "%s/%s", log_directory, name);
	if (len < 0 || (size_t)len >= sizeof(path))
		return NULL;
	return fopen(path, mode);
}
