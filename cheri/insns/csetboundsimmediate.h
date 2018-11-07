// See LICENSE_CHERI for license details.

cheri_reg_t temp = CS1;
temp.base = CS1.offset;
temp.length = insn.i_imm();
temp.offset = 0;
WRITE_CD(temp);

