/**
 * @file env.c
 * @brief Process environment variables for PinguinOS.
 *
 * Feature #82 – Environment Variables
 */

#include "../include/env.h"
#include "../include/heap.h"
#include "../include/klib.h"
#include "../include/serial.h"

/* ── Global kernel environment ───────────────────────────────────── */
static env_block_t g_env;

/* ── Public: env_init ────────────────────────────────────────────── */
void env_init(void)
{
    memset(&g_env, 0, sizeof(g_env));

    /* Seed with sensible kernel defaults */
    env_set(&g_env, "OS",      "PinguinOS");
    env_set(&g_env, "VERSION", "3.0");
    env_set(&g_env, "ARCH",    "i686");
    env_set(&g_env, "PATH",    "/bin:/usr/bin");
    env_set(&g_env, "HOME",    "/");
    env_set(&g_env, "TERM",    "vt100");

    KINFO("Env: global environment initialised\n");
}

/* ── Public: env_get ─────────────────────────────────────────────── */
const char *env_get(const env_block_t *env, const char *key)
{
    if (!env || !key) return NULL;
    for (int i = 0; i < ENV_MAX_VARS; i++) {
        if (env->vars[i].used && strncmp(env->vars[i].key, key, ENV_KEY_LEN) == 0)
            return env->vars[i].val;
    }
    return NULL;
}

/* ── Public: env_set ─────────────────────────────────────────────── */
int env_set(env_block_t *env, const char *key, const char *value)
{
    if (!env || !key || !value) return -1;

    /* Update existing entry */
    for (int i = 0; i < ENV_MAX_VARS; i++) {
        if (env->vars[i].used &&
            strncmp(env->vars[i].key, key, ENV_KEY_LEN) == 0) {
            strncpy(env->vars[i].val, value, ENV_VAL_LEN - 1);
            env->vars[i].val[ENV_VAL_LEN - 1] = '\0';
            return 0;
        }
    }

    /* Add new entry */
    for (int i = 0; i < ENV_MAX_VARS; i++) {
        if (!env->vars[i].used) {
            strncpy(env->vars[i].key, key,   ENV_KEY_LEN - 1);
            strncpy(env->vars[i].val, value, ENV_VAL_LEN - 1);
            env->vars[i].key[ENV_KEY_LEN - 1] = '\0';
            env->vars[i].val[ENV_VAL_LEN - 1] = '\0';
            env->vars[i].used = true;
            return 0;
        }
    }
    return -1;   /* Full */
}

/* ── Public: env_unset ───────────────────────────────────────────── */
int env_unset(env_block_t *env, const char *key)
{
    if (!env || !key) return -1;
    for (int i = 0; i < ENV_MAX_VARS; i++) {
        if (env->vars[i].used &&
            strncmp(env->vars[i].key, key, ENV_KEY_LEN) == 0) {
            memset(&env->vars[i], 0, sizeof(env->vars[i]));
            return 0;
        }
    }
    return -1;
}

/* ── Public: env_clone ───────────────────────────────────────────── */
env_block_t *env_clone(const env_block_t *src)
{
    if (!src) return NULL;
    env_block_t *copy = (env_block_t *)kmalloc(sizeof(env_block_t));
    if (!copy) return NULL;
    memcpy(copy, src, sizeof(env_block_t));
    return copy;
}

/* ── Public: env_dump ────────────────────────────────────────────── */
void env_dump(const env_block_t *env)
{
    if (!env) return;
    char buf[ENV_KEY_LEN + ENV_VAL_LEN + 4];
    serial_puts("[ENV] Variables:\n");
    for (int i = 0; i < ENV_MAX_VARS; i++) {
        if (!env->vars[i].used) continue;
        snprintf(buf, sizeof(buf), "  %s=%s\n",
                 env->vars[i].key, env->vars[i].val);
        serial_puts(buf);
    }
}

/* ── Public: env_global ──────────────────────────────────────────── */
env_block_t *env_global(void) { return &g_env; }