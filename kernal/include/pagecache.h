/**
 * @file pagecache.h
 * @brief Block-I/O page cache interface.
 */
#ifndef PAGECACHE_H
#define PAGECACHE_H

#include "types.h"

/**
 * @brief Initialise the page cache subsystem.
 */
void pagecache_init(void);

#endif /* PAGECACHE_H */
