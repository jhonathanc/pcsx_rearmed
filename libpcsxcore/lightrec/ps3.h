#ifndef LIGHTREC_PS3_H
#define LIGHTREC_PS3_H

#include <stdio.h>

#ifndef PS3_LIGHTREC_LOG
#define PS3_LIGHTREC_LOG 0
#endif

extern unsigned char *lightrec_ps3_code;
int lightrec_ps3_init_code(FILE *log);
void lightrec_ps3_free_code(void);

#if PS3_LIGHTREC_LOG
void lightrec_ps3_log_init(void);
FILE *lightrec_ps3_open_log(const char *name, const char *mode);
#endif

#endif
