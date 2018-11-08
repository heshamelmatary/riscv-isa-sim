/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 Hesham Almatary <Hesham.Almatary@cl.cam.ac.uk>
 * All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory (Department of Computer Science and
 * Technology) under DARPA contract HR0011-18-C-0016 ("ECATS"), as part of the
 * DARPA SSITH research programme.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <config.h>
#ifdef ENABLE_CHERI
#include "cheri.h"
#include "trap.h"
#include "mmu.h"
#include <cstdlib>
#include <tgmath.h>
#include <stdio.h>
#include <stdarg.h>

void cheri_t::cheriMem_setTag(reg_t addr) {

  reg_t paddr = CHERI->get_mmu()->translate(addr, 1, LOAD);

  paddr -= DRAM_BASE;
  paddr >>= (int) log2(sizeof(cheri_reg_t));
  /* Bit location of the 64-bit work */
  uint32_t wordShift = (uint32_t) log2(sizeof(uint64_t) * 8);
  uint32_t bitLoc_in_64bit = paddr & MASK(wordShift);
  uint32_t wordIndex = paddr >> wordShift;

#if DEBUG
  printf("CHERI: Setting %lu tag bit\n", paddr);
  printf("CHERI: Word %u, bit %u\n", wordIndex, bitLoc_in_64bit);
#endif

  CHERI_STATE.tag_bits[wordIndex] |= BIT(bitLoc_in_64bit);

}

bool cheri_t::cheriMem_getTag(reg_t addr) {
#if DEBUG
  printf("CHERI: Getting tag bit for addr = %p\n", addr);
#endif

  reg_t paddr = CHERI->get_mmu()->translate(addr, 1, LOAD);

  paddr -= DRAM_BASE;
  paddr >>= (int) log2(sizeof(cheri_reg_t));

  /* Bit location of the 64-bit work */
  uint32_t wordShift = (uint32_t) log2(sizeof(uint64_t) * 8);
  uint32_t bitLoc_in_64bit = paddr & MASK(wordShift);
  uint32_t wordIndex = paddr >> wordShift;

#if DEBUG
  printf("CHERI: Getting %lu tag bit\n", paddr);
  printf("CHERI: Word %u, bit %u\n", wordIndex, bitLoc_in_64bit);
#endif

  return !!(CHERI_STATE.tag_bits[wordIndex] & BIT(bitLoc_in_64bit));
}

void cheri_t::cheriMem_clearTag(reg_t addr) {
  reg_t paddr = CHERI->get_mmu()->translate(addr, 1, STORE);

  paddr -= DRAM_BASE;
  paddr >>= (int) log2(sizeof(cheri_reg_t) * 8);
  uint32_t wordShift = (uint32_t) log2(sizeof(uint64_t) * 8);
  /* Bit location of the 64-bit work */
  uint32_t bitLoc_in_64bit = paddr & MASK(wordShift);
  uint32_t wordIndex = paddr >> wordShift;
#if DEBUG
  printf("CHERI: Clearing %lu tag bit\n", paddr);
  printf("CHERI: Word %d, bit %d\n", wordIndex, bitLoc_in_64bit);
#endif

  CHERI_STATE.tag_bits[wordIndex] &= ~BIT(bitLoc_in_64bit);
}

mmu_t* cheri_t::get_mmu(void) {
  return p->get_mmu();
}

static inline long unsigned int poweroff(processor_t* p, insn_t y, long unsigned int z) {
  CHERI->get_mmu()->get_tracer()->printstats();
  exit(0);
}

#define CHERI_REGISTER_INSN(cheri, name, match, mask) \
  extern reg_t rv32_##name(processor_t*, insn_t, reg_t); \
  extern reg_t rv64_##name(processor_t*, insn_t, reg_t); \
  cheri->register_insn((insn_desc_t){match, mask, rv32_##name, rv64_##name});

void cheri_t::register_insn(insn_desc_t desc) {
  instructions.push_back(desc);
}

std::vector<insn_desc_t> cheri_t::get_instructions() {
  std::vector<insn_desc_t> insns;

#define DECLARE_INSN(name, match, mask) \
      insn_bits_t name##_match = (match), name##_mask = (mask);
#include "encoding.h"
#undef DECLARE_INSN

#define DEFINE_INSN(name) \
      CHERI_REGISTER_INSN(this, name, name##_match, name##_mask)
#include "cheri_insn_list.h"
#undef DEFINE_INSN

  /* Shutdown instruction emulation */
  register_insn((insn_desc_t) {
    0, 0xffffffff, poweroff, poweroff
  });

  return instructions;
}

std::vector<disasm_insn_t*> cheri_t::get_disasms() {
  std::vector<disasm_insn_t*> insns;
  return insns;
}

void cheri_t::create_tagged_memory(size_t memsz) {
  /* Rounded down, because last chuck of memory can't fit a cap */
  uint64_t needed_bits = (memsz / sizeof(cheri_reg_t));
#if DEBUG
  printf("CHERI: Creating tagged memory for %u bytes of phys memory\n", memsz);
  printf("CHERI: Cap-size is %u-bits\n", sizeof(cheri_reg_t) * 8);
  printf("CHERI: Allocating %u bits (%u KiB) (%d uint64_t words) of memory\n", needed_bits, (needed_bits / 8) / 1024 / 1024, (needed_bits / 8) / sizeof(uint64_t));
#endif

  /* Allocate needed bits in bytes, and zero it */
  state.tag_bits = (uint64_t *) calloc((needed_bits / 8) / sizeof(uint64_t), sizeof(uint64_t));

  if (state.tag_bits == NULL) {
    printf("CHERI: Failed to allocate memory for tagged bits\n");
    exit(-1);
  }
}

/* Override extension functions */
void cheri_t::reset() {
  uint64_t length_mask = MASK(((sizeof(state.csrs_reg_file[CHERI_CSR_PCC].length) * 8)) - 1);
  memset(&state.reg_file, 0, sizeof(state.reg_file));
  memset(&state.csrs_reg_file, 0, sizeof(state.csrs_reg_file));

  /* Nullify all CHERI GPRs */
  for (int i = 0; i < NUM_CHERI_REGS; i++) {
    state.reg_file[i].length = length_mask;
  }

  /* Initialize pcc and ddc */
  /* FIXME: Need to decide what permissions to be set for PCC (i.e. no store) */
  state.csrs_reg_file[CHERI_CSR_PCC] = {0, length_mask, 0, 0, 0x7ffu, 0, 0, 0, 1};
  /* FIXME: Need to decide what permissions to be set for DDC (i.e. no execute) */
  state.csrs_reg_file[CHERI_CSR_DDC] = {0, length_mask, 0, 0, 0x7ffu, 0, 0, 0, 1};

  /* Set cap size to 2*xlen; i.e., 128 cap size for RV64 and 64 for RV32 */
#ifdef ENABLE_CHERI128
  clen = 2 * p->get_xlen();
#else
  clen = 4 * p->get_xlen();
#endif
};
#endif
