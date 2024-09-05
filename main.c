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

    char *tracefile = argv[1];
    char *numBitsPerLevelStr;
    int *numBitsPerLevelAry = NULL;
    int *entryCount = NULL;
    int currBitsNum;
    int numOfLevels = 0;

    for (numBitsPerLevelStr = argv[2]; *numBitsPerLevelStr != '\0'; ++numBitsPerLevelStr)
    {
        if (isdigit(*numBitsPerLevelStr))
        {
            currBitsNum = *numBitsPerLevelStr - '0';
            if (isdigit(*(numBitsPerLevelStr + 1)))
            {
                currBitsNum = currBitsNum * 10 + (*(numBitsPerLevelStr + 1) - '0');
                ++numBitsPerLevelStr;
            }
            numOfLevels += 1;
            numBitsPerLevelAry = (int *)realloc(numBitsPerLevelAry, (numOfLevels) * sizeof(int));
            numBitsPerLevelAry[numOfLevels - 1] = currBitsNum;
        }
    }
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
    printf("Length (in bits) of page number: %d\n", pageNumberLength);

    unsigned int currMask;
    unsigned int numOfMaskBits;
    unsigned int numOfBitsMasked = 0;
    uint32_t *bitMaskAry = (uint32_t *)malloc(numOfLevels * sizeof(uint32_t));
    int *maskedValRightShiftAmt = (int *)malloc(numOfLevels * sizeof(int));

    for (int i = 0; i < numOfLevels; ++i)
    {
        currMask = 1;
        numOfMaskBits = numBitsPerLevelAry[i];
        for (int b = 1; b < numOfMaskBits; ++b)
        {
            currMask = currMask << 1;
            currMask = currMask | 1;
        }
        maskedValRightShiftAmt[i] = addressLength - numOfMaskBits - numOfBitsMasked;
        currMask = currMask << (maskedValRightShiftAmt[i]);
        numOfBitsMasked += numBitsPerLevelAry[i];
        bitMaskAry[i] = currMask;
    }

    log_bitmasks(numOfLevels, bitMaskAry);

    FILE *ifp;           /* trace file */
    unsigned long i = 0; /* instructions processed */
    p2AddrTr trace;      /* traced address */
    /*Initialize PageTable object*/
    PageTable pgTbl = {numOfLevels, numBitsPerLevelAry, bitMaskAry, maskedValRightShiftAmt, entryCount};
    PageTable *pgTblPtr = &pgTbl;
    // printPageTableInfo(pgTbl);
    /*Initialize level 0 object*/
    Level **nextLevelPtr1 = (Level **)malloc(entryCount[0] * sizeof(Level *));
    for (int i = 0; i < entryCount[0]; ++i)
    {
        nextLevelPtr1[i] = NULL;
    }
    Level level0 = {0, 0, pgTblPtr, nextLevelPtr1};
    Level *level0Ptr = &level0;

    /* attempt to open trace file */
    if ((ifp = fopen(tracefile, "rb")) == NULL)
    {
        fprintf(stderr, "Unable to open <<%s>>\n", tracefile);
        exit(1);
    }
    uint32_t *maskedAddrByLevelAry = (uint32_t *)malloc(numOfLevels * sizeof(uint32_t));
    uint32_t currMaskedAddr;
    int numOfAccesses = 0;
    while (!feof(ifp)) /*FIXME: now implement the PageTable and Level*/
    {
        /* get next address and process */
        if (NextAddress(ifp, &trace))
        {
            /*FIXME: try to mask values by level and call log function to print it */
            for (int i = 0; i < numOfLevels; ++i)
            {
                maskedAddrByLevelAry[i] = extractNumberFromAddress(trace.addr, bitMaskAry[i], maskedValRightShiftAmt[i]);
            }
            printf("currently inserting address 0x%08X into memory\n", trace.addr);
            numOfAccesses = recordPageAccess(level0Ptr, maskedAddrByLevelAry, 0, numOfLevels);
            log_pgindices_numofaccesses(trace.addr, numOfLevels, maskedAddrByLevelAry, numOfAccesses);
        }
    }

    /* clean up and return success */
    fclose(ifp);
    free(tracefile);
    deleteAllLevelNodes(level0Ptr);
    destroyPageTable(pgTbl);
    free(numBitsPerLevelStr);
    free(numBitsPerLevelAry);
    free(maskedAddrByLevelAry);
    free(bitMaskAry);
    free(entryCount);

    return 0;
}