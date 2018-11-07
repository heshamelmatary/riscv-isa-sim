// See LICENSE_CHERI for license details.

cheri_reg_t temp = CS1;
temp.sealed = 1;
temp.otype = CS2.base + CS2.offset;
WRITE_CD(temp);

