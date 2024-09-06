#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "pageTableLevel.h"
#include "tracereader.h"
#include "log.h"

int main(int argc, char *argv[])
{ // extract arguments from user input
    if (argc != 3)
    {
        printf("Need to enter 2 arguments (excluding name of executable file)\n");
        exit(1);
    }

    /*first argument: name of trace file*/
    char *tracefile = argv[1];
    /* second argument: pointer to first char of the
    input string that specifies number of bits per level*/
    char *numBitsPerLevelStr;
    int *numBitsPerLevelAry = NULL; /*initialize pointer to an array that holds bits value for each level*/
    int *entryCount = NULL;         /*pointer to an array storing number of entries for nextLevelPtr[] for each Level object*/
    int currBitsNum;                /* placeholder for current bit that is detected as a digit*/
    int numOfLevels = 0;            /*holds value indicating number of levels in the tree*/

    /* extract integers from the string value and place them in an array,
    each index represents a level and stores the num of bits as specified in the argument */
    for (numBitsPerLevelStr = argv[2]; *numBitsPerLevelStr != '\0'; ++numBitsPerLevelStr)
    {                                     /*interate through the string */
        if (isdigit(*numBitsPerLevelStr)) /*if current char is a digit*/
        {
            currBitsNum = *numBitsPerLevelStr - '0';                                /*convert char to int*/
            if (isdigit(*(numBitsPerLevelStr + 1)))                                 /*check if the following char is also a digit*/
            {                                                                       /*checks if user input a two digit number*/
                currBitsNum = currBitsNum * 10 + (*(numBitsPerLevelStr + 1) - '0'); /*combine two digits together*/
                ++numBitsPerLevelStr;
            }
            numOfLevels += 1;
            /*resize array to append the bits */
            numBitsPerLevelAry = (int *)realloc(numBitsPerLevelAry, (numOfLevels) * sizeof(int));
            numBitsPerLevelAry[numOfLevels - 1] = currBitsNum;
        }
    }

    /*initialize array storing number of entries for nextLevelPtr[] per Level*/
    entryCount = (int *)malloc(numOfLevels * sizeof(int));
    for (int i = 0; i < numOfLevels; ++i)
    {
        entryCount[i] = pow(2, numBitsPerLevelAry[i]);
    }

    int pageNumberLength = 0;
    int addressLength = 32;
    /* calculate number of bits in page number*/
    for (int i = 0; i < numOfLevels; ++i)
    {
        pageNumberLength += numBitsPerLevelAry[i];
    }

    unsigned int currMask;                                                     /*counter representing mask value for current level */
    unsigned int numOfMaskBits;                                                /*number of bits to mask the address value for current level*/
    unsigned int numOfPreviousMaskBits = 0;                                    /*number of mask bits from previous levels*/
    uint32_t *bitMaskAry = (uint32_t *)malloc(numOfLevels * sizeof(uint32_t)); /*initalize array to store mask value for address of each level*/
    int *maskedValRightShiftAmt = (int *)malloc(numOfLevels * sizeof(int));    /*shift mask bits to the right to for output format*/
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
        maskedValRightShiftAmt[i] = addressLength - numOfMaskBits - numOfPreviousMaskBits;
        currMask = currMask << (maskedValRightShiftAmt[i]);
        numOfPreviousMaskBits += numBitsPerLevelAry[i];
        bitMaskAry[i] = currMask;
    }
    /*print bit masks information to standard output*/
    log_bitmasks(numOfLevels, bitMaskAry);

    FILE *ifp;           /* trace file */
    unsigned long i = 0; /* instructions processed */
    p2AddrTr trace;      /* traced address */

    /*Initialize PageTable object*/
    PageTable pgTbl = {numOfLevels, numBitsPerLevelAry, bitMaskAry, maskedValRightShiftAmt, entryCount};
    PageTable *pgTblPtr = &pgTbl;

    /*Initialize level 0 object*/
    Level **nextLevelPtr = (Level **)malloc(entryCount[0] * sizeof(Level *));
    for (int i = 0; i < entryCount[0]; ++i)
    {
        nextLevelPtr[i] = NULL;
    }
    Level level0 = {0, 0, pgTblPtr, nextLevelPtr};
    /*assigned level 0 object to a pointer*/
    Level *level0Ptr = &level0;
    pgTblPtr->rootLevel = level0Ptr;

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
            numOfAccesses = recordPageAccess(level0Ptr, maskedAddrByLevelAry);
            /* print address, its page indices per level and number of accesses to standard output*/
            log_pgindices_numofaccesses(trace.addr, numOfLevels, maskedAddrByLevelAry, numOfAccesses);
        }
    }

    /* clean up and return success */
    fclose(ifp);
    // deleteAllLevelNodes(level0Ptr);
    // free(pgTblPtr);
    free(numBitsPerLevelAry);
    free(maskedAddrByLevelAry);
    free(bitMaskAry);
    free(entryCount);

    return 0;
}