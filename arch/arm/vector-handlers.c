#include "vectors.h"

extern vector_t reset_handler;
extern vector_t undef_handler;
extern vector_t swi_handler;
extern vector_t pabt_handler;
extern vector_t dabt_handler;
extern vector_t irq_handler;
extern vector_t fiq_handler;

void arm_install_reset_handler(vector_t vector) { reset_handler = vector; }
void arm_install_undef_handler(vector_t vector) { undef_handler = vector; }
void arm_install_swi_handler(vector_t vector) { swi_handler = vector; }
void arm_install_pabt_handler(vector_t vector) { pabt_handler = vector; }
void arm_install_dabt_handler(vector_t vector) { dabt_handler = vector; }
void arm_install_irq_handler(vector_t vector) { irq_handler = vector; }
void arm_install_fiq_handler(vector_t vector) { fiq_handler = vector; }
