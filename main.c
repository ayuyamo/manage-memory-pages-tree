/*
 * Name: Halie Do
 * RedID: 827707836
 */
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
        fprintf(stderr, "Usage: %s <trace_file_name> <bits_per_level_str>\n", argv[0]);
        exit(1);
    }

    int *numBitsPerLevelAry = NULL; /*Initialize pointer to an array that
                                      holds bits value for each level*/
    int *entryCount = NULL;         /*pointer to an array storing number of entries
                                      for nextLevelPtr[] for each Level object*/
    int numOfLevels;                /*holds value indicating number of levels in the tree*/

    /* Extract integers from the string value and place them in an array */
    extractBitsPerLevel(argv[2], &numOfLevels, &numBitsPerLevelAry);
    /*Initialize array storing number of entries for nextLevelPtr[] per Level*/
    entryCount = getEntryCountPerLevel(numOfLevels, numBitsPerLevelAry);

    int addressLength = 32;

    int *shiftAmt = NULL;        /*Store shift amount of masks for page indices in each level*/
    uint32_t *bitMaskAry = NULL; /*Initalize array to store mask value for address of each level*/
    shiftAmt = getShiftAmtPerLevel(addressLength, numBitsPerLevelAry, numOfLevels);
    bitMaskAry = getBitMaskForEachLevel(numBitsPerLevelAry, numOfLevels, addressLength, shiftAmt);
    /*print bit masks information to standard output*/
    log_bitmasks(numOfLevels, bitMaskAry);

    FILE *ifp;           /* trace file */
    unsigned long i = 0; /* instructions processed */
    p2AddrTr trace;      /* traced address */
    char *tracefile = argv[1];

    /*Initialize PageTable object*/
    PageTable pgTbl = {numOfLevels, numBitsPerLevelAry, bitMaskAry, shiftAmt, entryCount};
    PageTable *pgTblPtr = &pgTbl;

    /*Initialize level 0 object*/
    Level rootNode = createRootNode(pgTblPtr, entryCount);
    Level *rootNodePtr = &rootNode;

    pgTblPtr->rootLevel = rootNodePtr;

    /* attempt to open trace file */
    if ((ifp = fopen(tracefile, "rb")) == NULL)
    {
        fprintf(stderr, "Unable to open <<%s>>\n", tracefile);
        exit(1);
    }
    uint32_t *maskedAddrByLevelAry = (uint32_t *)malloc(numOfLevels * sizeof(uint32_t)); /*Store masked address per level*/
    int numOfAccesses = 0;                                                               /*Number of accesses for currently read address*/
    while (!feof(ifp))                                                                   /*Continue while there's still more value to read from file*/
    {
        /*Get next address and add to the tree*/
        if (NextAddress(ifp, &trace))
        {
            /*Mask address by level */
            for (i = 0; i < numOfLevels; ++i)
            {
                maskedAddrByLevelAry[i] = extractPageIndiceFromAddress(trace.addr, bitMaskAry[i], shiftAmt[i]);
            }
            numOfAccesses = recordPageAccess(rootNodePtr, maskedAddrByLevelAry);
            /*Print address, its page indices per level and number of accesses to standard output*/
            log_pgindices_numofaccesses(trace.addr, numOfLevels, maskedAddrByLevelAry, numOfAccesses);
        }
    }
    /* Close file after finish reading */
    fclose(ifp);
    /* Free allocated memory*/
    // deleteAllLevelNodes(rootNodePtr);
    free(numBitsPerLevelAry);
    free(maskedAddrByLevelAry);
    free(bitMaskAry);
    free(entryCount);

    return 0;
}