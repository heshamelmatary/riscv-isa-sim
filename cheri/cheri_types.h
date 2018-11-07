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

#ifndef _RISCV_CHERI_TYPES_H
#define _RISCV_CHERI_TYPES_H

#include <config.h>

/* 256-bit Caps register formant *
 * -------------------------------------------------------------------------
 * | length | base | offset | uperms | perms | S | reserved | otype | Tag  |
 * -------------------------------------------------------------------------
 * |  64    |  64  |   64   |   16   |  15   | 1 |   7     |   24   |  1   |
 * -------------------------------------------------------------------------
 */
struct cheri_reg_t {
 public:
  uint64_t base;
  uint64_t length;
  uint64_t offset;

  uint32_t uperms : 20;
  uint32_t perms  : 11;
  uint32_t sealed : 1;

  uint32_t otype    : 24;
  uint32_t reserved : 7;
  uint32_t tag      : 1;
};

#ifdef ENABLE_CHERI128
typedef cheri_reg_t cap_register_t;
#define  HAVE_CAP_REGISTER_T 1

/* CHERI compressed format in memory */
struct cheri_reg_inmem_t {
 public:
  uint64_t cursor;
  uint64_t pesbt;
};
#else
/* In memory is same as in register (except for tag) */
typedef cheri_reg_t cheri_reg_inmem_t;
#endif

#endif
