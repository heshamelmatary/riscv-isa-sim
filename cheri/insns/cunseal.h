// See LICENSE_CHERI for license details.

cheri_reg_t temp = CS1;
temp.sealed = 0;
temp.otype = 0;
/* FIXME: cs.perms.Global and ct.perms.Global */
temp.perms = CS1.perms & CS2.perms;
WRITE_CD(temp);

