#ifndef DECODE_H
#define DECODE_H

#include "define.h"

enum operand_type
{
	OPERAND_REGISTER,
	OPERAND_IMMEDIATE,
	OPERAND_MEMORY,
};

typedef enum {
	REG_AL,
	REG_BL,
	REG_CL,
	REG_DL,
	REG_AH,
	REG_BH,
	REG_CH,
	REG_DH,
	
	REG_SIL,
	REG_DIL,
	REG_BPL,
	REG_SPL,
	
	REG_AX,
	REG_BX,
	REG_CX,
	REG_DX,
	REG_SI,
	REG_DI,
	REG_BP,
	REG_SP,
	
	REG_EAX,
	REG_EBX,
	REG_ECX,
	REG_EDX,
	REG_ESI,
	REG_EDI,
	REG_EBP,
	REG_ESP,

	REG_RAX,
	REG_RBX,
	REG_RCX,
	REG_RDX,
	REG_RSI,
	REG_RDI,
	REG_RBP,
	REG_RSP,

	REG_R8,
	REG_R9,
	REG_R10,
	REG_R11,
	REG_R12,
	REG_R13,
	REG_R14,
	REG_R15,
	REG_R8B,
	REG_R9B,
	REG_R10B,
	REG_R11B,
	REG_R12B,
	REG_R13B,
	REG_R14B,
	REG_R15B,
	REG_R8W,
	REG_R9W,
	REG_R10W,
	REG_R11W,
	REG_R12W,
	REG_R13W,
	REG_R14W,
	REG_R15W,
	REG_R8D,
	REG_R9D,
	REG_R10D,
	REG_R11D,
	REG_R12D,
	REG_R13D,
	REG_R14D,
	REG_R15D,
	
	REG_RIZ
} X86Register;

struct operand
{
	enum operand_type type;
	int size;
	
	union
	{
		X86Register reg;
		
		struct {
			int displacement;
			X86Register base;
			
			int scale;
			X86Register index;
		} mem;
	};
};

enum opcode
{
	INST_MOV,
};

struct instruction
{
	enum opcode opcode;
	int length;
	
	struct operand operands[2];
};

extern int decode_instruction(const uint8_t *code, struct instruction *inst);

#endif /* DECODE_H */
