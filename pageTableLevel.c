#include "pageTableLevel.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Level *newNode(int depth, int numOfAccess, PageTable *pageTablePtr)
{
    // Level newNode = {depth, numOfAccess, pageTablePtr, NULL};
    Level *newNode = (Level *)malloc(sizeof(Level));
    int numOfEntries = pageTablePtr->entryCount[depth];
    Level **nextLvlPtr = (Level **)malloc(numOfEntries * sizeof(Level *));
    newNode->depth = depth;
    newNode->numOfAccesses = numOfAccess;
    newNode->pageTablePtr = pageTablePtr;
    newNode->nextLevelPtr = nextLvlPtr;
    return newNode;
}

void initializeNewNodeAry(Level *node, int numOfEntries)
{
    node->nextLevelPtr = (Level **)malloc(numOfEntries * sizeof(Level *));
    for (int i = 0; i < numOfEntries; ++i)
    {
        node->nextLevelPtr[i] = NULL;
    }
}
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

// Level *newNode(int depth, int numOfAccess, PageTable *pageTablePtr)
// {
//     Level newNode = {depth, numOfAccess, pageTablePtr, NULL};
//     return &newNode;
// }

// void initializeNewNodeAry(Level *node, int numOfEntries)
// {
//     node->nextLevelPtr = (Level **)malloc(numOfEntries * sizeof(Level *));
//     for (int i = 0; i < numOfEntries; ++i)
//     {
//         node->nextLevelPtr[i] = NULL;
//     }
// }

/* --Pseudo code for insertNode--
if depth < max_depth
        if (partial)address is found
            increment num of access for the level at index that == address mask
        else
            // create new node at said index
            nodePtr->nextLevelPtr[maskedAddrAry[nodePtr->depth]] = newNode(nodePtr->depth + 1, 1, nodePtr->pageTablePtr);

        insertnode(nodePtr->nextLevelPtr[maskedAddrAry[nodePtr->depth]], maskedAddrAry)

else if depth == max_depth
        return
*/
unsigned int recordPageAccess(Level *nodePtr, uint32_t *maskedAddrAry, int currDepth, int numOfLevels)
{
    // printf("Current depth: %d\t%d\n", nodePtr->depth, currDepth);
    uint32_t currMaskedAddr = maskedAddrAry[currDepth];
    Level *newNodePtr;
    if (nodePtr->depth == 0)
    {
        nodePtr->numOfAccesses += 1;
    }
    /* second part of expression account for case were there is only 1 level*/
    if (currDepth < numOfLevels - 1 || ((nodePtr->depth == 0) && numOfLevels == 1))
    {
        // printf("enter if statement\n");
        if (nodePtr->nextLevelPtr[currMaskedAddr] != NULL)
        {
            printf("found index address for 0x%X\n", currMaskedAddr);
            nodePtr->nextLevelPtr[currMaskedAddr]->numOfAccesses += 1;
            printf("successfully incremented value for index 0x%X\n", currMaskedAddr);
        }
        else
        {
            printf("level at index 0x%X hasnt been added to array, creating new node...\n", currMaskedAddr);
            newNodePtr = newNode(currDepth + 1, 1, nodePtr->pageTablePtr);
            printf("new node created\n");
            nodePtr->nextLevelPtr[currMaskedAddr] = newNodePtr;
            printf("new node added to array at index 0c%X, num of accesses = %d\n", currMaskedAddr, nodePtr->nextLevelPtr[currMaskedAddr]->numOfAccesses);
        }
        nodePtr = nodePtr->nextLevelPtr[currMaskedAddr];
        return recordPageAccess(nodePtr, maskedAddrAry, currDepth + 1, numOfLevels);
    } /* for mult. levels, curr works for numOfLevels = 3 and above*/
    else if (nodePtr->depth == nodePtr->pageTablePtr->levelCount - 1 || (nodePtr->depth == 1 && numOfLevels == 1))
    {
        return nodePtr->numOfAccesses;
    }
}
void deleteAllLevelNodes(Level *node)
{
    if (node == NULL)
        return;
    int nextLvlPtrLength = node->pageTablePtr->entryCount[node->depth];
    for (int i = 0; i < nextLvlPtrLength; ++i)
    {
        if (node->nextLevelPtr[i] == NULL)
            continue;
        deleteAllLevelNodes((Level *)node->nextLevelPtr[i]);
    }
    free(node->nextLevelPtr);
    free(node);
}