#ifndef DEFINE_H
#define DEFINE_H

#define NULL 0

// stdint
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int int64_t;

typedef unsigned long size_t;
typedef signed long intptr_t;
typedef unsigned long uintptr_t;

// attributes
#define __weak __attribute__((weak))

#ifdef __ARM
#define __align12 asm (".align 12\n")
#endif

#ifdef __X86_64
#define __align12 asm (".align 4096\n")
#endif

#endif /* DEFINE_H */
