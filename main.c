#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "pageTableLevel.h"
#include "tracereader.h"
#include "log.h"

/**
 * @brief - Extract integers from the string value and place them in an array,
             each index represents a level and stores the num of bits as specified in the argument

    @param stringInput - String input by user specifying the numbers of bits
                         page number will be split to for each level
    @param numOfLevels - Pointer to a variable holding value that indicates number of levels
                         in a tree value is modified while interating through user input string
    @param numBitsPerLevelArray - Array to be modified to store number of bits per page table level

 */
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

/**
 * @brief - Get the length of page number given address and
 *          sum of bits per page table level
 * @param addressLength - Store number of bits in address
 * @param numOfLevels - Number of levels in the tree
 * @param numBitsPerLevelAry - Store number of bits per page table level
 */

int getPageNumberLength(int addressLength, int numOfLevels, int *numBitsPerLevelAry)
{
    int pageNumberLength = 0;
    for (int i = 0; i < numOfLevels; ++i)
    {
        pageNumberLength += numBitsPerLevelAry[i];
    }

    return pageNumberLength;
}

/**
 * @brief - Get number of entries in next level pointers array of Level objects by depth
 *
 * @param numOfLevels - Number of levels in the tree
 * @param numBitsPerLevelAry - Store number of page number bits per level
 */
int *getEntryCountPerLevel(int numOfLevels, int *numBitsPerLevelAry)
{
    int *entryCount = (int *)malloc(numOfLevels * sizeof(int));
    for (int i = 0; i < numOfLevels; ++i)
    {
        entryCount[i] = pow(2, numBitsPerLevelAry[i]);
    }

    return entryCount;
}

/**
 * @brief - Get mask value for page numbers in each level
 *
 * @param numBitsPerLevelAry - Store size of page indice in each level
 * @param numOfLevels - Store number of levels in the tree
 * @param addressLength - Length of address to be stored in the tree
 * @param maskedValRightShiftAmt - Store shift amount of masks for page indices in each level
 */
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

/**
 * @brief - Return array storing shift amount (in bits) for page indices in each level
 *
 * @param addressLength - Length (in bits) of address to be stored in the tree
 * @param numBitsPerLevelAry - Store size of a page indice per level
 * @param numOfLevels - Number of levels in the tree
 */
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

/**
 * @brief - Initialize and return Level object at level 0
 *
 * @param pgTblPtr - Pointer to page table containing information about the tree
 * @param entryCount - Store number of entries for nextLevelPtr[] of Level nodes in each level
 */
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

int main(int argc, char *argv[])
{ // extract arguments from user input
    if (argc != 3)
    {
        printf("Need to enter 2 arguments (excluding name of executable file)\n");
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
    int pageNumberLength = getPageNumberLength(addressLength, numOfLevels, numBitsPerLevelAry);

    int *rightShiftAmt = NULL;   /*Store shift amount of masks for page indices in each level*/
    uint32_t *bitMaskAry = NULL; /*Initalize array to store mask value for address of each level*/
    rightShiftAmt = getShiftAmtPerLevel(addressLength, numBitsPerLevelAry, numOfLevels);
    bitMaskAry = getBitMaskForEachLevel(numBitsPerLevelAry, numOfLevels, addressLength, rightShiftAmt);
    /*print bit masks information to standard output*/
    log_bitmasks(numOfLevels, bitMaskAry);

    FILE *ifp;           /* trace file */
    unsigned long i = 0; /* instructions processed */
    p2AddrTr trace;      /* traced address */
    char *tracefile = argv[1];

    /*Initialize PageTable object*/
    PageTable pgTbl = {numOfLevels, numBitsPerLevelAry, bitMaskAry, rightShiftAmt, entryCount};
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
            for (int i = 0; i < numOfLevels; ++i)
            {
                maskedAddrByLevelAry[i] = extractPageNumberFromAddress(trace.addr, bitMaskAry[i], rightShiftAmt[i]);
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
    // free(pgTblPtr);
    free(numBitsPerLevelAry);
    free(maskedAddrByLevelAry);
    free(bitMaskAry);
    free(entryCount);

    return 0;
}