#ifndef ARM_VECTORS_H
#define ARM_VECTORS_H

typedef void (*vector_t)();

void arm_install_reset_handler(vector_t vector);
void arm_install_undef_handler(vector_t vector);
void arm_install_swi_handler(vector_t vector);
void arm_install_pabt_handler(vector_t vector);
void arm_install_dabt_handler(vector_t vector);
void arm_install_irq_handler(vector_t vector);
void arm_install_fiq_handler(vector_t vector);

#endif
