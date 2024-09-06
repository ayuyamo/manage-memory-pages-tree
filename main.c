#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "pageTableLevel.h"
#include "tracereader.h"
#include "log.h"

void extractBitsPerLevel(char *stringInput, int *numOfLevels, int **numBitsPerLevelAry)
{
    /*pointer to first char of the
    input string that specifies number of bits per level*/
    char *numBitsPerLevelStr;
    int currBitsNum;  /* placeholder for current bit that is detected as a digit*/
    *numOfLevels = 0; /*holds value indicating number of levels in the tree*/

    /* extract integers from the string value and place them in an array,
    each index represents a level and stores the num of bits as specified in the argument */
    for (numBitsPerLevelStr = stringInput; *numBitsPerLevelStr != '\0'; ++numBitsPerLevelStr)
    {                                     /*interate through the string */
        if (isdigit(*numBitsPerLevelStr)) /*if current char is a digit*/
        {
            currBitsNum = *numBitsPerLevelStr - '0';                                /*convert char to int*/
            if (isdigit(*(numBitsPerLevelStr + 1)))                                 /*check if the following char is also a digit*/
            {                                                                       /*checks if user input a two digit number*/
                currBitsNum = currBitsNum * 10 + (*(numBitsPerLevelStr + 1) - '0'); /*combine two digits together*/
                ++numBitsPerLevelStr;
            }
            *numOfLevels += 1;
            /*resize array to append the bits */
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

uint32_t *getMaskForEachIndice(int *numBitsPerLevelAry, int numOfLevels, int addressLength, int *maskedValRightShiftAmt)
{
    unsigned int currMask;                                                     /*counter representing mask value for current level */
    unsigned int numOfMaskBits;                                                /*number of bits to mask the address value for current level*/
    unsigned int numOfPreviousMaskBits = 0;                                    /*number of mask bits from previous levels*/
    uint32_t *bitMaskAry = (uint32_t *)malloc(numOfLevels * sizeof(uint32_t)); /*initalize array to store mask value for address of each level*/
    /*Assign appropriate mask values for each level into array */
    for (int i = 0; i < numOfLevels; ++i)
    {
        currMask = 1;
        numOfMaskBits = numBitsPerLevelAry[i];
        for (unsigned int b = 1; b < numOfMaskBits; ++b)
        {
            currMask = currMask << 1;
            currMask = currMask | 1;
        }
        currMask = currMask << (maskedValRightShiftAmt[i]);
        numOfPreviousMaskBits += numBitsPerLevelAry[i];
        bitMaskAry[i] = currMask;
    }
    return bitMaskAry;
}

int *getShiftAmtPerPageIndice(int addressLength, int *numBitsPerLevelAry, int numOfLevels)
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

Level createRootNode(PageTable *pgTblPtr, int *entryCount)
{
    /*Initialize level 0 object*/
    Level **nextLevelPtr = (Level **)malloc(entryCount[0] * sizeof(Level *));
    for (int i = 0; i < entryCount[0]; ++i)
    {
        nextLevelPtr[i] = NULL;
    }
    Level level0Obj = {0, 0, pgTblPtr, nextLevelPtr};
    return level0Obj;
}

int main(int argc, char *argv[])
{ // extract arguments from user input
    if (argc != 3)
    {
        printf("Need to enter 2 arguments (excluding name of executable file)\n");
        exit(1);
    }

    int *numBitsPerLevelAry = NULL; /*initialize pointer to an array that holds bits value for each level*/
    int *entryCount = NULL;         /*pointer to an array storing number of entries for nextLevelPtr[] for each Level object*/
    int numOfLevels;                /*holds value indicating number of levels in the tree*/

    // extract integers from the string value and place them in an array,
    extractBitsPerLevel(argv[2], &numOfLevels, &numBitsPerLevelAry);
    /*initialize array storing number of entries for nextLevelPtr[] per Level*/
    entryCount = getEntryCountPerLevel(numOfLevels, numBitsPerLevelAry);

    int addressLength = 32;
    int pageNumberLength = getPageNumberLength(addressLength, numOfLevels, numBitsPerLevelAry);

    int *maskedValRightShiftAmt = NULL;
    uint32_t *bitMaskAry = NULL; /*initalize array to store mask value for address of each level*/
    maskedValRightShiftAmt = getShiftAmtPerPageIndice(addressLength, numBitsPerLevelAry, numOfLevels);
    bitMaskAry = getMaskForEachIndice(numBitsPerLevelAry, numOfLevels, addressLength, maskedValRightShiftAmt);
    /*print bit masks information to standard output*/
    log_bitmasks(numOfLevels, bitMaskAry);

    FILE *ifp;           /* trace file */
    unsigned long i = 0; /* instructions processed */
    p2AddrTr trace;      /* traced address */
    char *tracefile = argv[1];

    /*Initialize PageTable object*/
    PageTable pgTbl = {numOfLevels, numBitsPerLevelAry, bitMaskAry, maskedValRightShiftAmt, entryCount};
    PageTable *pgTblPtr = &pgTbl;

    /*Initialize level 0 object*/
    Level **nextLevelPtr = (Level **)malloc(entryCount[0] * sizeof(Level *));
    for (int i = 0; i < entryCount[0]; ++i)
    {
        nextLevelPtr[i] = NULL;
    }

    Level rootNode = createRootNode(pgTblPtr, entryCount);
    Level *rootNodePtr = &rootNode;
    pgTblPtr->rootLevel = rootNodePtr;

    /* attempt to open trace file */
    if ((ifp = fopen(tracefile, "rb")) == NULL)
    {
        fprintf(stderr, "Unable to open <<%s>>\n", tracefile);
        exit(1);
    }
    uint32_t *maskedAddrByLevelAry = (uint32_t *)malloc(numOfLevels * sizeof(uint32_t)); /*store masked address per level*/
    int numOfAccesses = 0;                                                               /*number of accesses for currently read address*/
    while (!feof(ifp))                                                                   /*continue loop while there's still more value to read from file*/
    {
        /* get next address and process */
        if (NextAddress(ifp, &trace))
        {
            /* mask address by level */
            for (int i = 0; i < numOfLevels; ++i)
            {
                maskedAddrByLevelAry[i] = extractPageNumberFromAddress(trace.addr, bitMaskAry[i], maskedValRightShiftAmt[i]);
            }
            numOfAccesses = recordPageAccess(rootNodePtr, maskedAddrByLevelAry);
            /* print address, its page indices per level and number of accesses to standard output*/
            log_pgindices_numofaccesses(trace.addr, numOfLevels, maskedAddrByLevelAry, numOfAccesses);
        }
    }

    /* clean up and return success */
    fclose(ifp);
    // deleteAllLevelNodes(rootNodePtr);
    // free(pgTblPtr);
    free(numBitsPerLevelAry);
    free(maskedAddrByLevelAry);
    free(bitMaskAry);
    free(entryCount);

    return 0;
}