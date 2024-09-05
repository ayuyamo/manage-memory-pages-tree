
#ifndef PAGE_TABLE_LEVEL_H
#define PAGE_TABLE_LEVEL_H
#include <stdint.h>
typedef struct
{
    int levelCount;
    int *bitsPerLevel;
    int *bitMaskAry;
    int *shiftAry;
    int *entryCount;
} PageTable;

typedef struct Level Level;
struct Level
{
    int depth;
    int numOfAccesses;
    PageTable *pageTablePtr;
    Level **nextLevelPtr;
};
Level *newNode(int depth, int numOfAccess, PageTable *PageTablePtr);
void initializeNewNodeAry(Level *node, int numOfEntries);
unsigned int recordPageAccess(Level *nodePtr, uint32_t *maskedAddrAry, int currDepth, int numOfLevels);
void destroyPageTable(PageTable pgTbl);
unsigned int extractNumberFromAddress(unsigned int address, unsigned int mask, unsigned int shift);
void printPageTableInfo(PageTable pgTbl);
void deleteAllLevelNodes(Level *node);

#endif