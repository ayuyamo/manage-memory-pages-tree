# Managing memory pages

This program simulates a hierarchical page table system. It processes a trace file of memory accesses and maps virtual addresses to page table indices for each level. The simulation provides detailed logging of the page indices and access counts for each address.

## Features

- Parses user input for a trace file and page table configuration.
- Computes and logs the bit masks for each level of the page table.
- Processes memory addresses from a trace file and logs page table indices and access counts.
- Efficiently frees all allocated resources after execution.

## How to Compile

Make sure your Makefile includes `-lm` for linking the math library. Here's an example of how to compile the program manually:

`gcc -o pagetrace main.c pageTableLevel.c tracereader.c log.c -lm`

Or, use the provided Makefile:

`make`

## How to Run

**Run the program with the following syntax:**
`./pagetrace <trace_file_name> <bits_per_level_str>`

**Arguments:**

- `<trace_file_name>`: Path to the binary trace file containing memory addresses.
- `<bits_per_level_str>`: A comma-separated string specifying the number of bits allocated to each page table level (e.g., 10,10,12).

**Example:**

`./pagetrace trace.tr "10 10 12"`

## Code Walkthrough

1. Input Parsing

The program expects two command-line arguments:

- Trace file name: A binary file containing memory addresses.
- Page table configuration: A string of comma-separated integers indicating the number of bits per level in the page table hierarchy.

2. Page Table Initialization

   The number of levels and their bit allocations are extracted using `extractBitsPerLevel`.
   Arrays for bit masks, shift amounts, and entry counts are initialized for each level.

3. Processing the Trace File

The trace file is opened and read in binary mode.
For each address:

- The program calculates masked page indices for each level using bitwise operations.
- Page accesses are recorded in the page table.
- Results are logged, including the address, page indices, and the number of accesses.

4. Logging and Debugging

   The program logs the calculated bit masks for debugging purposes.
   Each address's page indices and access counts are printed to the standard output.

5. Resource Management

   All dynamically allocated memory (e.g., arrays for bit masks and shift amounts) is freed after processing.
   The program exits gracefully if any errors occur, such as failure to open the trace file.

## Output Example

For a trace file tracefile.tr and bit configuration 4,8,8, the program might produce output like:

```
./pagetrace trace.tr "4 8 8"
Bitmasks
level 0 mask F0000000
level 1 mask 0FF00000
level 2 mask 000FF000
0x0F562D88 -> page 0x0 0xF5 0x62 accessed 1 times
0x258F7100 -> page 0x2 0x58 0xF7 accessed 1 times
0x0F562D88 -> page 0x0 0xF5 0x62 accessed 2 times
0x10B95080 -> page 0x1 0xB 0x95 accessed 1 times
0x0F56ED88 -> page 0x0 0xF5 0x6E accessed 1 times
0x0F562D90 -> page 0x0 0xF5 0x62 accessed 3 times
0x0F562D90 -> page 0x0 0xF5 0x62 accessed 4 times
0x0F562D90 -> page 0x0 0xF5 0x62 accessed 5 times
0x0F562D98 -> page 0x0 0xF5 0x62 accessed 6 times
0x0F562D98 -> page 0x0 0xF5 0x62 accessed 7 times
...
```

## License

This project is distributed under the MIT License.
