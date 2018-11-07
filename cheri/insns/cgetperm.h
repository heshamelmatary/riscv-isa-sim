// See LICENSE_CHERI for license details.

WRITE_RD((CS1.perms & MASK(CHERI_PERM_BITS)) | (CS1.uperms << CHERI_PERM_BITS));

