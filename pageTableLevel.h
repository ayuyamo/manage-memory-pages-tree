
#ifndef PAGE_TABLE_LEVEL_H
#define PAGE_TABLE_LEVEL_H

typedef struct
{
    int levelCount;
    int *bitsPerLevel;
    int *bitMaskAry;
    int *shiftAry;
    int *entryCount;
} PageTable;

typedef struct
{
    int depth;
    int numOfAccesses;
    PageTable *PageTablePtr;
    int *nextLevelPtr;
} Level;

void destroyPageTable(PageTable pgTbl);
unsigned int extractNumberFromAddress(unsigned int address, unsigned int mask, unsigned int shift);
void printPageTableInfo(PageTable pgTbl);
// unsigned int recordPageAccess(unsigned int address);

#endif