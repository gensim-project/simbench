#include "decode.h"

const char *x86_register_names[] = {
	"rax",		// 0
	"rbx",
	"rcx",
	"rdx",
	"rsp",
	"rbp",
	"rsi",
	"rdi",

	"eax",		// 8
	"ebx",
	"ecx",
	"edx",
	"esp",
	"ebp",
	"esi",
	"edi",

	"ax",		// 16
	"bx",
	"cx",
	"dx",
	"sp",
	"bp",
	"si",
	"di",

	"ah",		// 24
	"bh",
	"ch",
	"dh",
	"al",
	"bl",
	"cl",
	"dl",

	"spl",		// 32
	"bpl",
	"sil",
	"dil",

	"r8b",		// 36
	"r9b",
	"r10b",
	"r11b",
	"r12b",
	"r13b",
	"r14b",
	"r15b",

	"r8w",		// 44
	"r9w",
	"r10w",
	"r11w",
	"r12w",
	"r13w",
	"r14w",
	"r15w",

	"r8d",		// 52
	"r9d",
	"r10d",
	"r11d",
	"r12d",
	"r13d",
	"r14d",
	"r15d",

	"r8",		// 60
	"r9",
	"r10",
	"r11",
	"r12",
	"r13",
	"r14",
	"r15",

	"riz"
};

typedef enum
{
	NONE = 0x00,

	ADDRESS_SIZE_OVERRIDE = 0x01,
	OPERAND_SIZE_OVERRIDE = 0x02,

	REX   = 0x04,
	REX_W = 0x08,
	REX_R = 0x10,
	REX_X = 0x20,
	REX_B = 0x40,
} X86InstructionPrefixes;

typedef enum
{
	O_R8,
	O_R16_32_64,

	O_R_M16_32_64,

	O_R_M8,
	O_R_M16,

	O_IMM16_32,

	O_MOFFS_16_32_64,

	O_RAX,
} X86OperandTypes;

typedef struct
{
	int mod, reg, rm;
} X86ModRM;

static X86InstructionPrefixes read_prefixes(const uint8_t **code)
{
	X86InstructionPrefixes p = NONE;

	int prefixes_complete = 0;
	while (!prefixes_complete) {
		switch (**code) {
		case 0x67: p = (X86InstructionPrefixes)(p | ADDRESS_SIZE_OVERRIDE); break;
		case 0x66: p = (X86InstructionPrefixes)(p | OPERAND_SIZE_OVERRIDE); break;

		case 0x40: p = (X86InstructionPrefixes)(p | REX); break;
		case 0x41: p = (X86InstructionPrefixes)(p | REX | REX_B); break;
		case 0x42: p = (X86InstructionPrefixes)(p | REX | REX_X); break;
		case 0x43: p = (X86InstructionPrefixes)(p | REX | REX_X | REX_B); break;
		case 0x44: p = (X86InstructionPrefixes)(p | REX | REX_R); break;
		case 0x45: p = (X86InstructionPrefixes)(p | REX | REX_R | REX_B); break;
		case 0x46: p = (X86InstructionPrefixes)(p | REX | REX_R | REX_X); break;
		case 0x47: p = (X86InstructionPrefixes)(p | REX | REX_R | REX_X | REX_B); break;
		case 0x48: p = (X86InstructionPrefixes)(p | REX | REX_W); break;
		case 0x49: p = (X86InstructionPrefixes)(p | REX | REX_W | REX_B); break;
		case 0x4a: p = (X86InstructionPrefixes)(p | REX | REX_W | REX_X); break;
		case 0x4b: p = (X86InstructionPrefixes)(p | REX | REX_W | REX_X | REX_B); break;
		case 0x4c: p = (X86InstructionPrefixes)(p | REX | REX_W | REX_R); break;
		case 0x4d: p = (X86InstructionPrefixes)(p | REX | REX_W | REX_R | REX_B); break;
		case 0x4e: p = (X86InstructionPrefixes)(p | REX | REX_W | REX_R | REX_X); break;
		case 0x4f: p = (X86InstructionPrefixes)(p | REX | REX_W | REX_R | REX_X | REX_B); break;

		default: prefixes_complete = 1; continue;
		}

		(*code)++;
	}

	return p;
}

static X86ModRM read_modrm(const uint8_t **code)
{
	uint8_t val = **code;

	X86ModRM ret;
	ret.mod = (val >> 6) & 3;
	ret.reg = (val >> 3) & 7;
	ret.rm = (val >> 0) & 7;

	(*code)++;

	return ret;
}

static int decode_reg_operand(struct operand *oper, X86InstructionPrefixes pfx, uint8_t reg, X86OperandTypes type)
{
	oper->type = OPERAND_REGISTER;

	switch (type) {
	case O_R8:
		if (pfx & REX_R) {
			switch (reg) {
			case 0: oper->reg = REG_R8B; break;
			case 1: oper->reg = REG_R9B; break;
			case 2: oper->reg = REG_R10B; break;
			case 3: oper->reg = REG_R11B; break;
			case 4: oper->reg = REG_R12B; break;
			case 5: oper->reg = REG_R13B; break;
			case 6: oper->reg = REG_R14B; break;
			case 7: oper->reg = REG_R15B; break;
			default: return 0;
			}
		} else {
			switch (reg) {
			case 0: oper->reg = REG_AL; break;
			case 1: oper->reg = REG_CL; break;
			case 2: oper->reg = REG_DL; break;
			case 3: oper->reg = REG_BL; break;
			case 4: if (pfx & REX) oper->reg = REG_SPL; else oper->reg = REG_AH; break;
			case 5: if (pfx & REX) oper->reg = REG_BPL; else oper->reg = REG_CH; break;
			case 6: if (pfx & REX) oper->reg = REG_SIL; else oper->reg = REG_DH; break;
			case 7: if (pfx & REX) oper->reg = REG_DIL; else oper->reg = REG_BH; break;
			default: return 0;
			}
		}
		break;

	case O_R16_32_64:
		if (pfx & REX_R) {
			switch (reg) {
			case 0: if (pfx & REX_W) oper->reg = REG_R8; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_R8W; else oper->reg = REG_R8D; break;
			case 1: if (pfx & REX_W) oper->reg = REG_R9; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_R9W; else oper->reg = REG_R9D; break;
			case 2: if (pfx & REX_W) oper->reg = REG_R10; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_R10W; else oper->reg = REG_R10D; break;
			case 3: if (pfx & REX_W) oper->reg = REG_R11; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_R11W; else oper->reg = REG_R11D; break;
			case 4: if (pfx & REX_W) oper->reg = REG_R12; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_R12W; else oper->reg = REG_R12D; break;
			case 5: if (pfx & REX_W) oper->reg = REG_R13; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_R13W; else oper->reg = REG_R13D; break;
			case 6: if (pfx & REX_W) oper->reg = REG_R14; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_R14W; else oper->reg = REG_R14D; break;
			case 7: if (pfx & REX_W) oper->reg = REG_R15; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_R15W; else oper->reg = REG_R15D; break;
			default: return 0;
			}
		} else {
			switch (reg) {
			case 0: if (pfx & REX_W) oper->reg = REG_RAX; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_AX; else oper->reg = REG_EAX; break;
			case 1: if (pfx & REX_W) oper->reg = REG_RCX; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_CX; else oper->reg = REG_ECX; break;
			case 2: if (pfx & REX_W) oper->reg = REG_RDX; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_DX; else oper->reg = REG_EDX; break;
			case 3: if (pfx & REX_W) oper->reg = REG_RBX; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_BX; else oper->reg = REG_EBX; break;
			case 4: if (pfx & REX_W) oper->reg = REG_RSP; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_SP; else oper->reg = REG_ESP; break;
			case 5: if (pfx & REX_W) oper->reg = REG_RBP; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_BP; else oper->reg = REG_EBP; break;
			case 6: if (pfx & REX_W) oper->reg = REG_RSI; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_SI; else oper->reg = REG_ESI; break;
			case 7: if (pfx & REX_W) oper->reg = REG_RDI; else if (pfx & OPERAND_SIZE_OVERRIDE) oper->reg = REG_DI; else oper->reg = REG_EDI; break;
			default: return 0;
			}
		}

		break;

	default:
		return 0;
	}

	switch (oper->reg) {
	case REG_RAX:
	case REG_RBX:
	case REG_RCX:
	case REG_RDX:
	case REG_RSP:
	case REG_RBP:
	case REG_RSI:
	case REG_RDI:
	case REG_R8:
	case REG_R9:
	case REG_R10:
	case REG_R11:
	case REG_R12:
	case REG_R13:
	case REG_R14:
	case REG_R15:
		oper->size = 8;
		break;

	case REG_EAX:
	case REG_EBX:
	case REG_ECX:
	case REG_EDX:
	case REG_ESP:
	case REG_EBP:
	case REG_ESI:
	case REG_EDI:
	case REG_R8D:
	case REG_R9D:
	case REG_R10D:
	case REG_R11D:
	case REG_R12D:
	case REG_R13D:
	case REG_R14D:
	case REG_R15D:
		oper->size = 4;
		break;

	case REG_AX:
	case REG_BX:
	case REG_CX:
	case REG_DX:
	case REG_SP:
	case REG_BP:
	case REG_SI:
	case REG_DI:
	case REG_R8W:
	case REG_R9W:
	case REG_R10W:
	case REG_R11W:
	case REG_R12W:
	case REG_R13W:
	case REG_R14W:
	case REG_R15W:
		oper->size = 2;
		break;

	case REG_AL:
	case REG_BL:
	case REG_CL:
	case REG_DL:
	case REG_AH:
	case REG_BH:
	case REG_CH:
	case REG_DH:
	case REG_SPL:
	case REG_BPL:
	case REG_SIL:
	case REG_DIL:
	case REG_R8B:
	case REG_R9B:
	case REG_R10B:
	case REG_R11B:
	case REG_R12B:
	case REG_R13B:
	case REG_R14B:
	case REG_R15B:
		oper->size = 1;
		break;
	}

	return 1;
}

static int decode_rm_operand(const uint8_t **code, struct operand *oper, X86InstructionPrefixes pfx, uint8_t mod, uint8_t rm, X86OperandTypes type)
{
	switch (mod) {
	case 0:
		oper->type = OPERAND_MEMORY;
		oper->mem.displacement = 0;
		oper->mem.index = REG_RIZ;
		oper->mem.scale = 0;

		if (pfx & REX_B) {
			switch (rm) {
			case 0: if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R8D; else oper->mem.base = REG_R8; break;
			case 1:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R9D; else oper->mem.base = REG_R9; break;
			case 2:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R10D; else oper->mem.base = REG_R10; break;
			case 3:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R11D; else oper->mem.base = REG_R11; break;
			case 4:	return 0;
			case 5:	return 0;
			case 6:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R14D; else oper->mem.base = REG_R14; break;
			case 7:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R15D; else oper->mem.base = REG_R15; break;
			}
		} else {
			switch (rm) {
			case 0: if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_EAX; else oper->mem.base = REG_RAX; break;
			case 1:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_ECX; else oper->mem.base = REG_RCX; break;
			case 2:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_EDX; else oper->mem.base = REG_RDX; break;
			case 3:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_EBX; else oper->mem.base = REG_RBX; break;
			case 4:	return 0;
			case 5:	return 0;
			case 6:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_ESI; else oper->mem.base = REG_RSI; break;
			case 7:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_EDI; else oper->mem.base = REG_RDI; break;
			}
		}

		break;

	case 1:
		oper->type = OPERAND_MEMORY;
		oper->mem.displacement = **code;
		oper->mem.index = REG_RIZ;
		oper->mem.scale = 0;

		(*code)++;

		if (pfx & REX_B) {
			switch (rm) {
			case 0: if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R8D; else oper->mem.base = REG_R8; break;
			case 1:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R9D; else oper->mem.base = REG_R9; break;
			case 2:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R10D; else oper->mem.base = REG_R10; break;
			case 3:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R11D; else oper->mem.base = REG_R11; break;
			case 4:	return 0;
			case 5:	return 0;
			case 6:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R14D; else oper->mem.base = REG_R14; break;
			case 7:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R15D; else oper->mem.base = REG_R15; break;
			}
		} else {
			switch (rm) {
			case 0: if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_EAX; else oper->mem.base = REG_RAX; break;
			case 1:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_ECX; else oper->mem.base = REG_RCX; break;
			case 2:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_EDX; else oper->mem.base = REG_RDX; break;
			case 3:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_EBX; else oper->mem.base = REG_RBX; break;
			case 4:	return 0;
			case 5:	return 0;
			case 6:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_ESI; else oper->mem.base = REG_RSI; break;
			case 7:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_EDI; else oper->mem.base = REG_RDI; break;
			}
		}

		break;

	case 2:
		oper->type = OPERAND_MEMORY;

		oper->mem.displacement = *(const int32_t *)(*code);
		oper->mem.index = REG_RIZ;
		oper->mem.scale = 0;

		*code = *code + 4;

		if (pfx & REX_B) {
			switch (rm) {
			case 0: if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R8D; else oper->mem.base = REG_R8; break;
			case 1:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R9D; else oper->mem.base = REG_R9; break;
			case 2:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R10D; else oper->mem.base = REG_R10; break;
			case 3:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R11D; else oper->mem.base = REG_R11; break;
			case 4:	return 0;
			case 5:	return 0;
			case 6:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R14D; else oper->mem.base = REG_R14; break;
			case 7:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_R15D; else oper->mem.base = REG_R15; break;
			}
		} else {
			switch (rm) {
			case 0: if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_EAX; else oper->mem.base = REG_RAX; break;
			case 1:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_ECX; else oper->mem.base = REG_RCX; break;
			case 2:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_EDX; else oper->mem.base = REG_RDX; break;
			case 3:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_EBX; else oper->mem.base = REG_RBX; break;
			case 4:	return 0;
			case 5:	return 0;
			case 6:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_ESI; else oper->mem.base = REG_RSI; break;
			case 7:	if (pfx & ADDRESS_SIZE_OVERRIDE) oper->mem.base = REG_EDI; else oper->mem.base = REG_RDI; break;
			}
		}

		break;

	default: return 0;
	}

	return 1;
}

static int decode_mov(X86InstructionPrefixes pfx, const uint8_t **code, struct instruction *inst, X86OperandTypes source, X86OperandTypes dest)
{
	inst->opcode = INST_MOV;

	struct operand *src = &inst->operands[0];
	struct operand *dst = &inst->operands[1];

	switch (source) {
	case O_R16_32_64:
	case O_R8:
	{
		X86ModRM modrm = read_modrm(code);
		if (!decode_reg_operand(src, pfx, modrm.reg, source)) return 0;
		if (!decode_rm_operand(code, dst, pfx, modrm.mod, modrm.rm, dest)) return 0;
		return 1;
	}

	case O_R_M8:
	case O_R_M16:
	case O_R_M16_32_64:
	{
		X86ModRM modrm = read_modrm(code);
		if (!decode_reg_operand(dst, pfx, modrm.reg, dest)) return 0;
		if (!decode_rm_operand(code, src, pfx, modrm.mod, modrm.rm, source)) return 0;
		return 1;
	}

	case O_IMM16_32:
		return 0;

	case O_MOFFS_16_32_64:
		src->type = OPERAND_MEM_OFF;
		src->mem_off = *(const uint64_t *)(*code);
		(*code) += 8;

		if (dest == O_RAX) {
			dst->type = OPERAND_REGISTER;
			if (pfx & OPERAND_SIZE_OVERRIDE)
				dst->reg = REG_RAX;
			else
				dst->reg = REG_EAX;
		} else {
			return 0;
		}

		return 1;
	default:
		return 0;
	}
}

static int decode_int(X86InstructionPrefixes pfx, const uint8_t **code, struct instruction *inst)
{
	inst->opcode = INST_INT;
	inst->operands[0].type = OPERAND_IMMEDIATE;
	inst->operands[0].size = 1;
	inst->operands[0].imm_val = **code;
	(*code)++;
}

int decode_instruction(const uint8_t *code, struct instruction *inst)
{
	const uint8_t *base = code;

	// Read Prefixes
	X86InstructionPrefixes p = read_prefixes(&code);

	// Read Opcode
	uint16_t opcode = 0;
	if (*code == 0x0f) {
		opcode |= 0x100;
		code++;
	}

	opcode |= *code++;

	switch (opcode) {
	case 0xa1: if (!decode_mov(p, &code, inst, O_MOFFS_16_32_64, O_RAX)) return 0; else break;
	case 0x88: if (!decode_mov(p, &code, inst, O_R8, O_R_M8)) return 0; else break;
	case 0x89: if (!decode_mov(p, &code, inst, O_R16_32_64, O_R_M16_32_64)) return 0; else break;
	case 0x8a: if (!decode_mov(p, &code, inst, O_R_M8, O_R8)) return 0; else break;
	case 0x8b: if (!decode_mov(p, &code, inst, O_R_M16_32_64, O_R16_32_64)) return 0; else break;
	case 0xc7: if (!decode_mov(p, &code, inst, O_IMM16_32, O_R_M16_32_64)) return 0; else break;
	case 0xcd: if (!decode_int(p, &code, inst)) return 0; else break;
	default: return 0;
	}

	inst->length = (code - base);

	return 1;
}

