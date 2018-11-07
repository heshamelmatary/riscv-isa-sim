// See LICENSE_CHERI for license details.

cheri_reg_t csr = CHERI_STATE.csrs_reg_file[insn.chs()];
cheri_reg_t cs1 = CS1;

cheri_reg_t temp = CSR;

/* If source register is c0, don't write CSR (used to read CSR) */
if (insn.cs1())
  CSR = CS1;

/* If destination register is c0, do nothing */
if (insn.cd())
  WRITE_CD(temp);
