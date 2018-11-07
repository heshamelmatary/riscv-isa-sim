// See LICENSE_CHERI for license details.

cheri_reg_t temp = CS1;
temp.perms = CD.perms & (RS2 & MASK(CHERI_PERM_BITS));
temp.uperms = CD.uperms & ((RS2 >> CHERI_PERM_BITS) & MASK(CHERI_USER_PERM_BITS));
WRITE_CD(temp);
