#include "printf.h"

int snprintf(char *buffer, int size, const char *fmt, ...)
{
	int rc;
	va_list args;

	va_start(args, fmt);
	rc = vsnprintf(buffer, size, fmt, args);
	va_end(args);

	return rc;
}

int sprintf(char *buffer, const char *fmt, ...)
{
	int rc;
	va_list args;

	va_start(args, fmt);
	rc = vsnprintf(buffer, 0x1000, fmt, args);
	va_end(args);

	return rc;
}

static void prepend_to_buffer(char c, char *buffer, int size)
{
	int i;
	for (i = size; i > 0; i--) {
		buffer[i] = buffer[i - 1];
	}

	*buffer = c;
}

static int append_num(char *buffer, int size, unsigned long long int value, int base, int sgn, int pad, char pad_char)
{
	int n = 0;

	if (pad >= size) {
		pad = size - 1;
	}

	if (sgn && (signed long long int)value < 0) {
		prepend_to_buffer('-', buffer, n++);
		value = -value;
	}

	if (value > 0) {
		while (value > 0 && n < size) {
#ifdef __ARM
			// TODO: FIXME: HACK HACK HACK - need to work out how to get division and modulus working
			if (base != 16) return 0;
			int new_value = value >> 4;
			int digit_value = value & 0xf;
#else
			int new_value = value / base;
			int digit_value = value % base;
#endif

			switch(digit_value) {
			case 0 ... 9:
				prepend_to_buffer('0' + digit_value, buffer, n++);
				break;

			case 10 ... 35:
				prepend_to_buffer('a' + digit_value - 10, buffer, n++);
				break;
			}

			value = new_value;
		}
	} else if (n < size) {
		prepend_to_buffer('0', buffer, n++);
		*buffer = '0';
	}

	pad -= n;
	while (pad > 0 && n < size) {
		prepend_to_buffer(pad_char, buffer, n++);
		pad--;
	}

	return n;
}

static int append_str(char *buffer, int size, const char *text, int pad, char pad_char)
{
	int n = 0;

	while (*text && n < size) {
		*buffer = *text;

		buffer++;
		text++;

		n++;
	}

	while (n < size && n < pad) {
		*buffer++ = pad_char;
		n++;
	}

	return n;
}

int vsnprintf(char *buffer_base, int size, const char *fmt_base, va_list args)
{
	const char *fmt = fmt_base;
	char *buffer = buffer_base;

	// Handle a zero-sized buffer.
	if (size == 0) {
		return 0;
	}

	// Do the printing, while we are still consuming format characters, and
	// haven't exceeded 'size'.
	int count = 0;
	while (*fmt != 0 && count < (size - 1)) {
		if (*fmt == '%') {
			int pad_size = 0, rc;
			char pad_char = ' ';
			int number_size = 4;

retry_format:
			fmt++;

			switch (*fmt) {
			case 0:
				continue;

			case '0':
				if (pad_size > 0) {
					pad_size *= 10;
				} else {
					pad_char = '0';
				}
				goto retry_format;

			case '1' ... '9':
				pad_size *= 10;
				pad_size += *fmt - '0';
				goto retry_format;

			case 'd':
			case 'u':
			{
				long long int v;

				if (number_size == 8) {
					v = va_arg(args, long long int);
				} else {
					if (*fmt == 'u') {
						v = (unsigned long long int)va_arg(args, int);
					} else {
						v = (long long int)va_arg(args, int);
					}
				}

				rc = append_num(buffer, size - 1 - count, v, 10, *fmt == 'd', pad_size, pad_char);
				count += rc;
				buffer += rc;
				break;
			}

			case 'b':
			case 'x':
			case 'p':
			{
				unsigned long long int v;

				if (number_size == 8 || *fmt == 'p') {
					v = va_arg(args, unsigned long long int);
				} else {
					v = (unsigned long long int)va_arg(args, unsigned int);
				}

				if (*fmt == 'p') {
					rc = append_str(buffer, size - 1 - count, "0x", 0, ' ');
					count += rc;
					buffer += rc;
				}

				rc = append_num(buffer, size - 1 - count, v, (*fmt == 'b' ? 2 : 16), 0, pad_size, pad_char);
				count += rc;
				buffer += rc;
				break;
			}

			case 'l':
				number_size = 8;
				goto retry_format;

			case 's':
				rc = append_str(buffer, size - 1 - count, va_arg(args, const char *), pad_size, pad_char);
				count += rc;
				buffer += rc;
				break;

			case 'c':
				*buffer = va_arg(args, int);

				buffer++;
				count++;
				break;
				
			default:
				*buffer = *fmt;

				buffer++;
				count++;
				break;
			}

			fmt++;
		} else {
			*buffer = *fmt;

			buffer++;
			fmt++;
			count++;
		}
	}

	// Null-terminate the buffer
	*buffer = 0;
	return count;
}

static FILE stdout_file;
static FILE uart_file;

FILE *STDOUT = &stdout_file;
FILE *UART = &uart_file;

void printf_register_stdout(putch_fn_t putch_fn)
{
	stdout_file.putch_fn = putch_fn;
}

void printf_register_uart(putch_fn_t putch_fn)
{
	uart_file.putch_fn = putch_fn;
}

int fprintf(FILE *f, const char *fmt, ...)
{
	char buffer[0x1000];
	int rc, i;
	va_list args;
	
	if (!f) return 0;
	if (!f->putch_fn) return 0;

	va_start(args, fmt);
	rc = vsnprintf(buffer, 0x1000, fmt, args);
	va_end(args);

	for (i = 0; i < 0x1000; i++) {
		if (buffer[i] == 0) {
			break;
		}

		f->putch_fn(buffer[i]);
	}

	return rc;
}

int printf(const char *fmt, ...)
{
	char buffer[0x1000];
	int rc, i;
	va_list args;
	
	if (!STDOUT) return 0;
	if (!STDOUT->putch_fn) return 0;

	va_start(args, fmt);
	rc = vsnprintf(buffer, 0x1000, fmt, args);
	va_end(args);

	for (i = 0; i < 0x1000; i++) {
		if (buffer[i] == 0) {
			break;
		}

		STDOUT->putch_fn(buffer[i]);
	}

	return rc;
}
