
#ifndef PAGE_TABLE_LEVEL_H
#define PAGE_TABLE_LEVEL_H
#include <stdint.h>

typedef struct Level Level; /*Declare struct Level*/
/**
 * @brief Contains paging information for each level in the implemented tree
 *
 * @param levelCount - Number of levels in the tree
 * @param bitsPerLevel - Number of bits per level
 * @param bitMaskAry - Mask value per level
 * @param shiftAry - Shift right amount for address in each level
 * @param entryCount - Number of entries per level in nextLevelPtr[] of level object
 * @param rootLevel - Pointer to root level object
 */
typedef struct
{
    int levelCount;
    int *bitsPerLevel;
    uint32_t *bitMaskAry;
    int *shiftAry;
    int *entryCount;
    Level *rootLevel;
} PageTable;

/**
 * @brief - Representing a node in a tree
 *
 * @param depth - Depth of node - Level of the node in the tree
 * @param numOfAccesses - # of times the object has been accessed
 * @param pageTablePtr - Pointer to PageTable object containing information about the tree
 * @param nextLevelPtr - Pointer to an array of pointers to next level objects
 */
struct Level
{
    int depth;
    int numOfAccesses;
    PageTable *pageTablePtr;
    Level **nextLevelPtr;
};
/**
 * @brief Create a new Level node initialized to values passed in the parameters
 *
 * @param depth  - Depth of the new Level node
 * @param numOfAccess - Number of times the Level node was accessed
 * @param PageTablePtr - Pointer to PageTable object containing information about the tree
 */
Level *newNode(int depth, int numOfAccess, PageTable *PageTablePtr);
/**
 * @brief - Record the number of times a certain address was accessed
 *
 * @param nodePtr - The starting node pointer from where you start traversing the tree
 * @param bitMaskAry - Pointer to array containing mask values for each level
 */
unsigned int recordPageAccess(Level *nodePtr, uint32_t *bitMaskAry);

/**
 * @brief - Deallocate memory for variable pointers stored in PageTable object
 *
 * @param pgTblPtr - Pointer to PageTable object
 */
void destroyPageTable(PageTable *pgTblPtr);

/**
 * @brief - Extract page number of any level given an address, mask and the right shift amount (in #bits)
 *
 * @param address - The full address read from input
 * @param mask - Mask value to AND with the given address
 * @param shift - Right shift amount in bits
 */
unsigned int extractPageNumberFromAddress(unsigned int address, unsigned int mask, unsigned int shift);

/**
 * @brief - Deallocate memory from the tree, including freeing memory of
 *          nextLevelPtr[] in each Level object and the Level node itself
 *          for each node in the tree
 * @param node - Starting node from where you start traversing the tree
 */
void deleteAllLevelNodes(Level *node);

#endif