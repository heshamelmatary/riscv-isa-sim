// See LICENSE_CHERI for license details.

// TODO

if (insn.cd() == 0)
  CHERI->get_mmu()->get_tracer()->reset();

if (insn.cd() == 1)
  CHERI->get_mmu()->get_tracer()->printstats();
