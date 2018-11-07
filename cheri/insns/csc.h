// See LICENSE_CHERI for license details.

reg_t addr = READ_REG(insn.scrs()) + insn.s_imm();
cheri_reg_t cs = CHERI_STATE.reg_file[insn.cs2()];

reg_t paddr = CHERI->get_mmu()->translate(addr, 1, STORE);

if (cs.tag) {
  CHERI->cheriMem_setTag(paddr);
  /* Zero tag for saving in memory */
  cs.tag = 0;
} else {
#if DEBUG
  printf("CHERI: Trying to store untagged cap register\n");
#endif

  /* set capcause */
  CAPCAUSE = (CAUSE_CHERI_TAG_FAULT << 8) | insn.cs2();

  throw trap_cheri_trap();
}

CHERI->get_mmu()->store_cheri_reg(paddr, cs);
