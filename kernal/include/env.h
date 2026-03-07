/**
 * @file env.h
 * @brief Process environment variables API for PinguinOS.
 *
 * Feature #82 – Environment Variables
 *
 * A global kernel environment table plus per-process env blocks.
 * Processes inherit the kernel env on creation; changes are copy-on-write.
 */
#ifndef _ENV_H
#define _ENV_H

#include "types.h"

#define ENV_MAX_VARS    64     /* Max variables per env block  */
#define ENV_KEY_LEN     64     /* Max key length               */
#define ENV_VAL_LEN    256     /* Max value length             */

/* ── Environment block ───────────────────────────────────────────── */
typedef struct env_var {
    char key[ENV_KEY_LEN];
    char val[ENV_VAL_LEN];
    bool used;
} env_var_t;

typedef struct env_block {
    env_var_t vars[ENV_MAX_VARS];
} env_block_t;

/* ── Public API ──────────────────────────────────────────────────── */

/** Initialise the global kernel environment. */
void env_init(void);

/**
 * @brief Get the value of an environment variable.
 * @return Pointer to static storage, or NULL if not found.
 */
const char *env_get(const env_block_t *env, const char *key);

/**
 * @brief Set or overwrite an environment variable.
 * @return 0 on success, -1 if env is full.
 */
int env_set(env_block_t *env, const char *key, const char *value);

/**
 * @brief Remove an environment variable.
 * @return 0 if found and removed, -1 if not found.
 */
int env_unset(env_block_t *env, const char *key);

/**
 * @brief Duplicate an env block (for fork/exec).
 * @return Newly allocated copy, caller must kfree().
 */
env_block_t *env_clone(const env_block_t *src);

/** Print all variables in an env block to serial. */
void env_dump(const env_block_t *env);

/** @return Pointer to the global kernel environment. */
env_block_t *env_global(void);

#endif /* _ENV_H */