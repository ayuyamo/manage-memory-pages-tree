#include "pageTableLevel.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Level *newNode(int depth, int numOfAccess, PageTable *pageTablePtr)
{
    Level *newNode = (Level *)malloc(sizeof(Level));
    int numOfEntries = pageTablePtr->entryCount[depth];
    Level **nextLvlPtr = (Level **)malloc(numOfEntries * sizeof(Level *));
    for (int i = 0; i < numOfEntries; ++i)
    {
        nextLvlPtr[i] = NULL;
    }
    newNode->depth = depth;
    newNode->numOfAccesses = numOfAccess;
    newNode->pageTablePtr = pageTablePtr;
    newNode->nextLevelPtr = nextLvlPtr;
    return newNode;
}

void destroyPageTable(PageTable *pgTblPtr)
{
    free(pgTblPtr->bitsPerLevel);
    free(pgTblPtr->bitMaskAry);
    free(pgTblPtr->shiftAry);
    free(pgTblPtr->entryCount);
}

unsigned int extractPageNumberFromAddress(unsigned int address, unsigned int mask, unsigned int shift)
{
    unsigned int extractedNum;
    extractedNum = address & mask;
    extractedNum = extractedNum >> shift;
    return extractedNum;
}

unsigned int recordPageAccess(Level *nodePtr, uint32_t *bitMaskAry)
{
    int currDepth = nodePtr->depth;
    int numOfLevels = nodePtr->pageTablePtr->levelCount;
    uint32_t currMask = bitMaskAry[currDepth];

    if (nodePtr->depth == 0) /* Increment numOfAccess everytime the root node pointer was accessed*/
    {
        nodePtr->numOfAccesses += 1;
    }
    if (currDepth <= numOfLevels - 1) /*Executes when page indices are still being inserted into the tree */
    {
        if (nodePtr->nextLevelPtr[currMask] != NULL)
        { /*Increment num of access of current Level node if page indice
            already exists in its nextLevelPtr array*/
            nodePtr->nextLevelPtr[currMask]->numOfAccesses += 1;
        }
        else
        { /* If page indice does not exist, create & initalize a new
             level node pointer and pass it into nextLevelPtr array */
            Level *newNodePtr;
            newNodePtr = newNode(currDepth + 1, 1, nodePtr->pageTablePtr);
            nodePtr->nextLevelPtr[currMask] = newNodePtr;
        }
        nodePtr = nodePtr->nextLevelPtr[currMask];    /* Update nodePtr to the next Level node with page indice of the target address */
        return recordPageAccess(nodePtr, bitMaskAry); /*Call on the function itself to traverse down the tree recursively*/
    }
    else if (nodePtr->depth == numOfLevels) /* Return the number of accesses when the leaf node is reached*/
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