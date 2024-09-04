#ifndef LOG_HELPERS_H
#define LOG_HELPERS_H

/*
 * Compilation notes:
 * C compilers:
 *    uses bool, must compile with -std=c99 or later (bool was introduced
 *    in the 1999 C standard.
 *
 * C++ compilers
 *    uses uint32_t, unsigned 32 bit integer type, introduced in C++11,
 *    The defaults in the g++ compiler on edoras should be fine with this
 */

/* C and C++ define some of their types in different places.
 * Check and see if we are using C or C++ and include appropriately
 * so that this will compile under C and C++
 */
#ifdef __cplusplus
/* C++ includes */
#include <stdint.h>
#else
/* C includes */
#include <inttypes.h>
#include <stdbool.h>
#endif 

/**
 * @brief Print out bitmasks for all page table levels.
 * 
 * @param levels - Number of levels
 * @param masks - Pointer to array of bitmasks
 */
void log_bitmasks(int levels, uint32_t *masks);

/**
 * @brief log page indices at all levels and the number of page accesses
 * 
 * @param address
 * @param levels      - number of levels in page table
 * @param pageIndices - pageIndices[idx] is the page number associated with 
 *	                    level idx (0 < idx < levels)
 * @param numOfAccesses - number of page accesses 
 */
void log_pgindices_numofaccesses(uint32_t address, int levels, 
                                 uint32_t *pageIndices, 
                                 uint32_t numOfAccesses);

#endif
