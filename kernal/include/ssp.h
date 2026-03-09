/**
 * @file ssp.h
 * @brief Stack-Smashing Protection (SSP) interface.
 */
#ifndef SSP_H
#define SSP_H

#include "types.h"

/**
 * @brief Randomise the stack canary.
 * Must be called after entropy_init().
 */
void ssp_init(void);

/**
 * @brief Failure handler called by GCC on stack corruption.
 */
__attribute__((noreturn)) void __stack_chk_fail(void);

#endif /* SSP_H */
