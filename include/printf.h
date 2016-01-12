#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>

typedef void (*putch_fn_t)(char ch);

typedef struct _FILE {
	putch_fn_t putch_fn;
} FILE;

extern void printf_register_debug(putch_fn_t putch_fn);
extern void printf_register_output(putch_fn_t putch_fn);
extern void printf_register_error(putch_fn_t putch_fn);

extern int snprintf(char *buffer, int size, const char *fmt, ...);
extern int sprintf(char *buffer, const char *fmt, ...);
extern int vsnprintf(char *buffer_base, int size, const char *fmt_base, va_list args);
extern int fprintf(FILE *f, const char *fmt, ...);

extern FILE *DEBUG, *OUTPUT, *ERROR;

#endif /* PRINTF_H */
