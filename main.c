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
    // int offset = 0;
    int addressLength = 32;
    /* calculate offset & number of bits in page number*/
    for (int i = 0; i < numOfLevels; ++i)
    {
        pageNumberLength += numBitsPerLevelAry[i];
    }
    printf("Length (in bits) of page number: %d\n", pageNumberLength);
    // offset = addressLength - pageNumberLength;
    // printf("Offset: %d\n", offset);

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
    PageTable pgTbl = {numOfLevels, numBitsPerLevelAry, bitMaskAry, maskedValRightShiftAmt, entryCount};
    printPageTableInfo(pgTbl);

    log_bitmasks(numOfLevels, bitMaskAry);

    // reading memory trace file
    // p2AddrTr mtrace;
    // unsigned int vAddr;
    // // tracef_h - file handle from fopen
    // FILE *tracef_h = fopen(tracefile, "r");

    // if (tracef_h == NULL)
    // {
    //     perror("Error opening file");
    // }
    // if (NextAddress(tracef_h, &mtrace))
    // {
    //     vAddr = mtrace.addr;
    //     printf("%d", vAddr);
    // }

    FILE *ifp;           /* trace file */
    unsigned long i = 0; /* instructions processed */
    p2AddrTr trace;      /* traced address */

    /* attempt to open trace file */
    if ((ifp = fopen(tracefile, "rb")) == NULL)
    {
        fprintf(stderr, "Unable to open <<%s>>\n", tracefile);
        exit(1);
    }
    uint32_t *maskedAddrByLevelAry = (uint32_t *)malloc(numOfLevels * sizeof(uint32_t));
    uint32_t currMaskedAddr;
    while (!feof(ifp)) /*FIXME: now implement the PageTable and Level*/
    {
        /* get next address and process */
        if (NextAddress(ifp, &trace))
        {
            /*FIXME: try to mask values by level and call log function to print it */
            for (int i = 0; i < numOfLevels; ++i)
            {
                // currMaskedAddr = trace.addr & bitMaskAry[i];
                // currMaskedAddr = currMaskedAddr >> maskedValRightShiftAmt[i];
                // maskedAddrByLevelAry[i] = currMaskedAddr;
            }
            // log_pgindices_numofaccesses(trace.addr, numOfLevels, maskedAddrByLevelAry, 1);
        }
    }

    /* clean up and return success */
    fclose(ifp);
    free(tracefile);
    destroyPageTable(pgTbl);
    free(numBitsPerLevelStr);
    free(numBitsPerLevelAry);
    free(maskedAddrByLevelAry);
    free(bitMaskAry);
    free(entryCount);

    return 0;
}