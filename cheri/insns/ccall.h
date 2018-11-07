// See LICENSE_CHERI for license details.

#if DEBUG
printf("CHERI: CCall wants to jump to %p\n", CD.offset);
#endif
PCC.offset = CD.offset;

cheri_reg_t temp;

temp.sealed = 0;
temp.otype = 0;

/* IDC */
//CHERI_STATE.reg_file[0] = CS1;
//CS1.sealed = 0;
//CS1.otype = 0;

/* Set ra to pc + 4 to mimic jr */
WRITE_REG(1, pc + 4);

return PCC.offset;
