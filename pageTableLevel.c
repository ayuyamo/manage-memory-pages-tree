#include "pageTableLevel.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void destroyPageTable(PageTable pgTbl)
{
    free(pgTbl.bitsPerLevel);
    free(pgTbl.bitMaskAry);
    free(pgTbl.shiftAry);
    free(pgTbl.entryCount);
}

unsigned int extractNumberFromAddress(unsigned int address, unsigned int mask, unsigned int shift)
{
    unsigned int extractedNum;
    extractedNum = address & mask;
    extractedNum = extractedNum >> shift;
    return extractedNum;
}

void printPageTableInfo(PageTable pgTbl)
{
    printf("-----------------------------\n");
    printf("Current PageTable Obj Info:\n");
    printf("Number of levels: %d\n", pgTbl.levelCount);
    printf("# of bits per level, its respective mask val and right shift amount: ");
    for (int i = 0; i < pgTbl.levelCount; ++i)
    {
        printf("%d %08X %d\n", pgTbl.bitsPerLevel[i], pgTbl.bitMaskAry[i], pgTbl.shiftAry[i]);
    }
    printf("Entry Count for each level:\n");
    for (int i = 0; i < pgTbl.levelCount; ++i)
    {
        printf("Level %d: %d\n", i, pgTbl.entryCount[i]);
    }
}

// unsigned int PageTable::recordPageAccess(unsigned int address)
// {
//     return 0;
// }
