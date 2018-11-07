// See LICENSE_CHERI for license details.

cheri_reg_t temp = CS1;
temp.offset = CS1.offset + insn.i_imm();
WRITE_CD(temp);
