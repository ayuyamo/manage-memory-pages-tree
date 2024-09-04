#include <stdio.h>
#include "log.h"

/* Handle C++ namespaces, ignore if compiled in C 
 * C++ usually uses this #define to declare the C++ standard.
 * It will not be defined if a C compiler is used.
 */
#ifdef __cplusplus
using namespace std;
#endif

/**
 * @brief Print out bitmasks for all page table levels.
 * 
 * @param levels - Number of levels
 * @param masks - Pointer to array of bitmasks
 */
void log_bitmasks(int levels, uint32_t *masks) {
  printf("Bitmasks\n");
  for (int idx = 0; idx < levels; idx++) 
    /* show mask entry and move to next */
    printf("level %d mask %08X\n", idx, masks[idx]);

  fflush(stdout);
}

/**
 * @brief log page indices at all levels and the number of page accesses
 * 
 * @param address
 * @param levels      - number of levels in page table
 * @param pageIndices - pageIndices[idx] is the page number associated with 
 *	                    level idx (0 < idx < levels)
 * @param numOfAccesses - number of page accesses 
 */
void log_pgindices_numofaccesses(uint32_t address, 
                                 int levels, 
                                 uint32_t *pageIndices, 
                                 uint32_t numOfAccesses) {
    
  fprintf(stdout, "0x%08X -> page ", address);
  /* output pages */
  for (int idx = 0; idx < levels; idx ++)
    fprintf(stdout, "0x%X ", pageIndices[idx]);
  /* output number of accesses */
  fprintf(stdout, "accessed %d times\n", numOfAccesses);

  fflush(stdout);
}