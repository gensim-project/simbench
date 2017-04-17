#ifndef ARMV8_TXLN_TABLE_H
#define ARMV8_TXLN_TABLE_H

#include "define.h"

#define PA_BITS 48
#define VA_BITS 48

#define PRIVS_DATA 1
#define PRIVS_CODE 3

void *get_txln_table();
void clear_txln_table();

#endif
