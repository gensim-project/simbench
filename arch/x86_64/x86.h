#ifndef X86_H
#define X86_H

#include "define.h"

struct mcontext
{
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
	uint64_t rbp;
	uint64_t rsi, rdi, rdx, rcx, rbx, rax;
	uint64_t flags, rsp, extra, rip, tempCS, tempFLAGS, tempSP, tempSS;
} __attribute__((packed));

#endif /* X86_H */
