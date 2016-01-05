#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>

typedef void (*putch_fn_t)(int ch);

extern void printf_register_putch(putch_fn_t putch_fn);

extern int snprintf(char *buffer, int size, const char *fmt, ...);
extern int sprintf(char *buffer, const char *fmt, ...);
extern int vsnprintf(char *buffer_base, int size, const char *fmt_base, va_list args);
extern int printf(const char *fmt, ...);

#endif /* PRINTF_H */

