
#ifndef PAGE_TABLE_LEVEL_H
#define PAGE_TABLE_LEVEL_H
#include <stdint.h>
#include <ctype.h>

typedef struct Level Level; /*Declare struct Level*/
/**
 * @brief  - Contains paging information for each level in the implemented tree
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
 * @brief - Initialize and return Level object at level 0
 *
 * @param pgTblPtr - Pointer to page table containing information about the tree
 * @param entryCount - Store number of entries for nextLevelPtr[] of Level nodes in each level
 */
Level createRootNode(PageTable *pgTblPtr, int *entryCount);

/**
 * @brief - Extract integers from the string value and place them in an array,
             each index represents a level and stores the num of bits as specified in the argument

    @param stringInput - String input by user specifying the numbers of bits
                         page number will be split to for each level
    @param numOfLevels - Pointer to a variable holding value that indicates number of levels
                         in a tree value is modified while interating through user input string
    @param numBitsPerLevelArray - Array to be modified to store number of bits per page table level

 */
void extractBitsPerLevel(char *stringInput, int *numOfLevels, int **numBitsPerLevelAry);

/**
 * @brief - Get the length of page number given address and
 *          sum of bits per page table level
 * @param addressLength - Store number of bits in address
 * @param numOfLevels - Number of levels in the tree
 * @param numBitsPerLevelAry - Store number of bits per page table level
 */
int getPageNumberLength(int addressLength, int numOfLevels, int *numBitsPerLevelAry);

/**
 * @brief - Get number of entries in next level pointers array of Level objects by depth
 *
 * @param numOfLevels - Number of levels in the tree
 * @param numBitsPerLevelAry - Store number of page number bits per level
 */
int *getEntryCountPerLevel(int numOfLevels, int *numBitsPerLevelAry);

/**
 * @brief - Get mask value for page numbers in each level
 *
 * @param numBitsPerLevelAry - Store size of page indice in each level
 * @param numOfLevels - Store number of levels in the tree
 * @param addressLength - Length of address to be stored in the tree
 * @param maskedValRightShiftAmt - Store shift amount of masks for page indices in each level
 */
uint32_t *getBitMaskForEachLevel(int *numBitsPerLevelAry, int numOfLevels, int addressLength, int *maskRightShiftAmt);

/**
 * @brief - Return array storing shift amount (in bits) for page indices in each level
 *
 * @param addressLength - Length (in bits) of address to be stored in the tree
 * @param numBitsPerLevelAry - Store size of a page indice per level
 * @param numOfLevels - Number of levels in the tree
 */
int *getShiftAmtPerLevel(int addressLength, int *numBitsPerLevelAry, int numOfLevels);

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
 * @brief - Extract page indice of any level given an address, mask and the right shift amount (in #bits)
 *
 * @param address - The full address read from input
 * @param mask - Mask value to AND with the given address
 * @param shift - Right shift amount in bits
 */
unsigned int extractPageIndiceFromAddress(unsigned int address, unsigned int mask, unsigned int shift);

/**
 * @brief - Deallocate memory from the tree, including freeing memory of
 *          nextLevelPtr[] in each Level object and the Level node itself
 *          for each node in the tree
 * @param node - Starting node from where you start traversing the tree
 */
void deleteAllLevelNodes(Level *node);

#endif