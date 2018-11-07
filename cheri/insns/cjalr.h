// See LICENSE_CHERI for license details.

cheri_reg_t temp = PCC;
temp.offset = pc + 8;
WRITE_CD(temp);

PCC = CS1;

/* FIXME: Is this right? */
return CS1.offset;
