#include "pageTableLevel.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Level createRootNode(PageTable *pgTblPtr, int *entryCount)
{
    /*Initialize level 0 node*/
    Level **nextLevelPtr = (Level **)malloc(entryCount[0] * sizeof(Level *));
    for (int i = 0; i < entryCount[0]; ++i)
    {
        nextLevelPtr[i] = NULL;
    }
    Level level0Node = {0, 0, pgTblPtr, nextLevelPtr};
    return level0Node;
}

void extractBitsPerLevel(char *stringInput, int *numOfLevels, int **numBitsPerLevelAry)
{
    char *numBitsPerLevelStr; /*Pointer to first char of the input string
                                that specifies number of bits per level*/
    int currBitsNum;          /*Placeholder for current bit that is detected as a digit*/
    *numOfLevels = 0;         /*Number of levels in the tree*/

    for (numBitsPerLevelStr = stringInput; *numBitsPerLevelStr != '\0'; ++numBitsPerLevelStr)
    {
        if (isdigit(*numBitsPerLevelStr)) /*check if current char is a digit*/
        {
            currBitsNum = *numBitsPerLevelStr - '0'; /*convert char to int*/
            if (isdigit(*(numBitsPerLevelStr + 1)))  /*check if the following char is also a digit*/
            {                                        /*checks if user input a two digit number*/
                currBitsNum = currBitsNum * 10 +
                              (*(numBitsPerLevelStr + 1) - '0'); /*combine two digits together*/
                ++numBitsPerLevelStr;
            }
            *numOfLevels += 1;
            /*Resize array to append the bits value */
            *numBitsPerLevelAry = (int *)realloc(*numBitsPerLevelAry, (*(numOfLevels)) * sizeof(int));
            (*numBitsPerLevelAry)[*(numOfLevels)-1] = currBitsNum;
        }
    }
}

int getPageNumberLength(int addressLength, int numOfLevels, int *numBitsPerLevelAry)
{
    int pageNumberLength = 0;
    for (int i = 0; i < numOfLevels; ++i)
    {
        pageNumberLength += numBitsPerLevelAry[i];
    }

    return pageNumberLength;
}

int *getEntryCountPerLevel(int numOfLevels, int *numBitsPerLevelAry)
{
    int *entryCount = (int *)malloc(numOfLevels * sizeof(int));
    for (int i = 0; i < numOfLevels; ++i)
    {
        entryCount[i] = pow(2, numBitsPerLevelAry[i]);
    }

    return entryCount;
}

uint32_t *getBitMaskForEachLevel(int *numBitsPerLevelAry, int numOfLevels, int addressLength, int *maskRightShiftAmt)
{
    unsigned int currMask;                                                     /*counter representing mask value for current level */
    unsigned int bitMaskLength;                                                /*number of bits to mask the address value for current level*/
    unsigned int numOfPreviousMaskBits = 0;                                    /*number of mask bits from previous levels*/
    uint32_t *bitMaskAry = (uint32_t *)malloc(numOfLevels * sizeof(uint32_t)); /*initalize array to store mask value for address of each level*/
    /*Assign appropriate mask values for each level into array */
    for (int i = 0; i < numOfLevels; ++i)
    {
        currMask = 1;
        bitMaskLength = numBitsPerLevelAry[i];
        for (unsigned int b = 1; b < bitMaskLength; ++b)
        {
            currMask = currMask << 1;
            currMask = currMask | 1;
        }
        currMask = currMask << (maskRightShiftAmt[i]);
        numOfPreviousMaskBits += numBitsPerLevelAry[i];
        bitMaskAry[i] = currMask;
    }
    return bitMaskAry;
}

int *getShiftAmtPerLevel(int addressLength, int *numBitsPerLevelAry, int numOfLevels)
{
    unsigned int numOfMaskBits;                                             /*number of bits to mask the address value for current level*/
    unsigned int numOfPreviousMaskBits = 0;                                 /*number of mask bits from previous levels*/
    int *maskedValRightShiftAmt = (int *)malloc(numOfLevels * sizeof(int)); /*shift mask bits to the right to for output format*/
    /*Assign appropriate mask values for each level into array */
    for (int i = 0; i < numOfLevels; ++i)
    {
        numOfMaskBits = numBitsPerLevelAry[i];
        maskedValRightShiftAmt[i] = addressLength - numOfMaskBits - numOfPreviousMaskBits;
        numOfPreviousMaskBits += numBitsPerLevelAry[i];
    }

    return maskedValRightShiftAmt;
}

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

unsigned int extractPageIndiceFromAddress(unsigned int address, unsigned int mask, unsigned int shift)
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