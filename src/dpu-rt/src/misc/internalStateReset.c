/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * The strict minimum to reset the internal state of the DPU (that can only be changed by a DPU program).
 */

#include <dpu_characteristics.h>

#define __STR(x) __STR_AGAIN(x)
#define __STR_AGAIN(x) #x

/* To ensure compatibility with v1.4 chips, we should use up to 16 threads to reset Atomic bits */
#define NR_THREADS_TO_RESET_ATOMIC_BITS 16

void __attribute__((naked, used, section(".text.__bootstrap"))) __bootstrap()
{
    /* clang-format off */
    __asm__ volatile(
        "  jgtu id, " __STR(NR_THREADS_TO_RESET_ATOMIC_BITS) " - 1, reset_flags\n"
        "  sub r0, " __STR(DPU_NR_ATOMIC_BITS) " - 1, id\n"
        "before_release:\n"
        // Resetting Atomic bits
        "  release r0, 0, nz, after_release\n"
        "after_release:\n"
        "  sub r0, r0, " __STR(NR_THREADS_TO_RESET_ATOMIC_BITS) ", pl, before_release\n"
        "reset_flags:\n"
        // Resetting Zero and Carry flags + Preparing configuration for perfcounter register
        "  add r0, zero, 7\n"
        // Resetting Performance Counter
        "  time_cfg zero, r0\n"
        // Resetting Pc
        "  stop true, 0\n");
    /* clang-format on */
}
