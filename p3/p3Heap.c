////////////////////////////////////////////////////////////////////////////////
// Main File:        p3Heap.c (this is a shared library no main)
// This File:        p3Heap.c
// Other Files:      library is used by tests folder
// Semester:         CS 354 Lecture 001 Fall 2024
// Grade Group:      gg 9  (See canvas.wisc.edu/groups for your gg#)
// Instructor:       deppeler
// 
// Author:           Katie Krause
// Email:            klkrause5@wisc.edu
// CS Login:         kkrause
//
///////////////////////////  OPTIONAL WORK LOG  //////////////////////////////
//  Reviewed the free_block() and alloc() functions for correctness 
//  in handling memory allocation and deallocation.
//
//  Discovered potential issues in immediate coalescing 
//  logic and block size updates in the free_block() function.
//
//  Updated the free_block() function to correctly handle next 
//  and previous block coalescing.
//
//  Couldn't run test 202 and 212_immedcoal without error
/////////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   https://stackoverflow.com/questions/1766535/bit-hack-round-off-to-multiple-of-8
// Looked at stackoverflow forum to ensure I was understanding rounding correctly.
// 
// AI chats:         save a transcript and submit with project.
//////////////////////////// 80 columns wide ///////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020-2024 Deb Deppeler based on work by Jim Skrentny
// Posting or sharing this file is prohibited, including any changes.
// Used by permission SPRING 2024, CS354-deppeler
//
/////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include "p3Heap.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block.
 */
typedef struct blockHeader {           

    /*
     * 1) The size of each heap block must be a multiple of 8
     * 2) heap blocks have blockHeaders that contain size and status bits
     * 3) free heap block contain a footer, but we can use the blockHeader 
     *.
     * All heap blocks have a blockHeader with size and status
     * Free heap blocks have a blockHeader as its footer with size only
     *
     * Status is stored using the two least significant bits.
     *   Bit0 => least significant bit, last bit
     *   Bit0 == 0 => free block
     *   Bit0 == 1 => allocated block
     *
     *   Bit1 => second last bit 
     *   Bit1 == 0 => previous block is free
     *   Bit1 == 1 => previous block is allocated
     * 
     * Start Heap: 
     *  The blockHeader for the first block of the heap is after skip 4 bytes.
     *  This ensures alignment requirements can be met.
     * 
     * End Mark: 
     *  The end of the available memory is indicated using a size_status of 1.
     * 
     * Examples:
     * 
     * 1. Allocated block of size 24 bytes:
     *    Allocated Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 25
     *      If the previous block is allocated p-bit=1 size_status would be 27
     * 
     * 2. Free block of size 24 bytes:
     *    Free Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 24
     *      If the previous block is allocated p-bit=1 size_status would be 26
     *    Free Block Footer:
     *      size_status should be 24
     */
    int size_status;

} blockHeader;         

/* Global variable - DO NOT CHANGE NAME or TYPE. 
 * It must point to the first block in the heap and is set by init_heap()
 * i.e., the block at the lowest address.
 */
blockHeader *heap_start = NULL;     

/* Size of heap allocation padded to round to nearest page size.
 */
int alloc_size;

/*
 * Additional global variables may be added as needed below
 *  add global variables needed by your function
 */

// Added to keep track of free blocks in my heap for best-fit
blockHeader *pt_header_free = NULL;

/* 
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block (payload) on success.
 * Returns NULL on failure.
 *
 * This function must:
 * - Check size - Return NULL if size < 1 
 * - Determine block size rounding up to a multiple of 8 
 *   and possibly adding padding as a result.
 *
 * - Use BEST-FIT PLACEMENT POLICY to chose a free block
 *
 * - If the BEST-FIT block that is found is exact size match
 *   - 1. Update all heap blocks as needed for any affected blocks
 *   - 2. Return the address of the allocated block payload
 *
 * - If the BEST-FIT block that is found is large enough to split 
 *   - 1. SPLIT the free block into two valid heap blocks:
 *         1. an allocated block
 *         2. a free block
 *         NOTE: both blocks must meet heap block requirements 
 *       - Update all heap block header(s) and footer(s) 
 *              as needed for any affected blocks.
 *   - 2. Return the address of the allocated block payload
 *
 *   Return if NULL unable to find and allocate block for required size
 *
 * Note: payload address that is returned is NOT the address of the
 *       block header.  It is the address of the start of the 
 *       available memory for the requesterr.
 *
 * Tips: Be careful with pointer arithmetic and scale factors.
 */
void* alloc(int size) {

	// 1st check the size 
	if (size < 1 ) {
		return NULL;
	}

	// 2nd Calculate the size of the block rounded up
	// 4 bytes for header and padding
	int blocks_size = size + sizeof(blockHeader);
	blocks_size = (blocks_size + 7) & ~7;

	// 3rd Look for free block that is the best fit
	blockHeader *pt_best_fit = NULL;
	blockHeader *head_current = heap_start;
	int size_of_best = -1;


	while (head_current->size_status != 1) {
		int current_size = head_current->size_status & ~7;

		// check if the block is large enough and free
		if ((head_current->size_status & 1) == 0 && current_size >= blocks_size) {
			if (pt_best_fit == NULL || current_size < size_of_best) {
				pt_best_fit = head_current;
				size_of_best = current_size;
			}
		}

		//  calculate the next block
		head_current = (blockHeader*)((char*)head_current + current_size); 
	}

	// No blocks found that fit should return null
	if (pt_best_fit == NULL) {
		return NULL;
	}


	// 4th implement split when free block has extra room
	if (size_of_best >= blocks_size + 8) {
		blockHeader*free_block = (blockHeader*)((char*)pt_best_fit + blocks_size);
		free_block->size_status = (size_of_best - blocks_size) | 0;

		// footer needs to be updated to hold info for free block
		blockHeader *footer = (blockHeader*)((char*)free_block + (size_of_best - blocks_size) - 4);
		footer->size_status = free_block->size_status;
	}

	// Found our best fit!!
	pt_best_fit->size_status = blocks_size | (pt_best_fit->size_status & 2) | 1;

	// return payload's address
	return (void*)((char*)pt_best_fit + 4);
}

/*
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - Update header(s) and footer as needed.
 *
 * If free results in two or more adjacent free blocks,
 * they will be immediately coalesced into one larger free block.
 * so free blocks require a footer (blockHeader works) to store the size
 *
 * TIP: work on getting immediate coalescing to work after your code 
 *      can pass the tests in partA and partB of tests/ directory.
 *      Submit code that passes partA and partB to Canvas before continuing.
 */                    
int free_block(void *ptr) {

	// 1st fulfill ptr requirements, check if ptr is null
	if (ptr == NULL) {
		return -1;
	}

	// Check if ptr is a multiple of 8
	if ((unsigned long)ptr % 8 != 0) {
		return -1;
	}

	// Check if ptr is in/out of heap by checking head and end
	if (ptr < (void*)heap_start || ptr >= (void*)((char*)heap_start + alloc_size)) {
		return -1;
	}

	// Check if the block is free, calculate the header
	blockHeader *blocksHead = (blockHeader*)((char*)ptr - sizeof(blockHeader));

	if ((blocksHead->size_status & 1 ) == 0) {
		return -1;
	}

	//  2nd Update the block to signify it's free,, LSB should be 0
	int blocks_size = blocksHead->size_status & ~7;
	blocksHead->size_status &= ~1;

	blockHeader *footer = (blockHeader*)((char*)blocksHead + blocks_size - sizeof(blockHeader));
	footer->size_status = blocks_size;

	// 3rd Update the predecssor bit 
	blockHeader *next_block = (blockHeader*)((char*)blocksHead + blocks_size);
        if (next_block->size_status != 1) {
        	next_block->size_status &= ~2;
	}

	// 4th Implement coalescing if next block happens to be free
	if ((char*)next_block < (char*)heap_start + alloc_size) {
		if ((next_block->size_status & 1) == 0) {
     	   		int next_block_size = next_block->size_status & ~7;
			blocks_size += next_block_size;
	       		blocksHead->size_status = blocks_size;
        		footer = (blockHeader*)((char*)blocksHead + blocks_size - sizeof(blockHeader));
        		footer->size_status = blocks_size;
		}
	}

    	// Coalesce with the previous block if it is free
    	if ((blocksHead->size_status & 2) == 0) {
       		blockHeader *prev_footer = (blockHeader*)((char*)blocksHead - sizeof(blockHeader));
       		int prev_block_size = prev_footer->size_status;
	        blockHeader *prev_block = (blockHeader*)((char*)blocksHead - prev_block_size);
        	blocks_size += prev_block_size;
		prev_block->size_status = blocks_size;
		footer->size_status = blocks_size;
		blocksHead = prev_block;
    }

    	// Ensure the next block’s p-bit is set correctly if it's within bounds
    	next_block = (blockHeader*)((char*)blocksHead + blocks_size);
    	if ((char*)next_block < (char*)heap_start + alloc_size && next_block->size_status != 1) {
        	next_block->size_status &= ~2; 
    	}

	return 0;
} 


/* 
 * Initializes the memory allocator.
 * Called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */                    
int init_heap(int sizeOfRegion) {    

    static int allocated_once = 0; //prevent multiple myInit calls

    int   pagesize; // page size
    int   padsize;  // size of padding when heap size not a multiple of page size
    void* mmap_ptr; // pointer to memory mapped area
    int   fd;

    blockHeader* end_mark;

    if (0 != allocated_once) {
        fprintf(stderr, 
                "Error:mem.c: InitHeap has allocated space during a previous call\n");
        return -1;
    }

    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize from O.S. 
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    mmap_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == mmap_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }

    allocated_once = 1;

    // for double word alignment and end mark
    alloc_size -= 8;

    // Initially there is only one big free block in the heap.
    // Skip first 4 bytes for double word alignment requirement.
    heap_start = (blockHeader*) mmap_ptr + 1;

    // Set the end mark
    end_mark = (blockHeader*)((void*)heap_start + alloc_size);
    end_mark->size_status = 1;

    // Set size in header
    heap_start->size_status = alloc_size;

    // Set p-bit as allocated in header
    // note a-bit left at 0 for free
    heap_start->size_status += 2;

    // Set the footer
    blockHeader *footer = (blockHeader*) ((void*)heap_start + alloc_size - 4);
    footer->size_status = alloc_size;

    return 0;
} 

/* STUDENTS MAY EDIT THIS FUNCTION, but do not change function header.
 * TIP: review this implementation to see one way to traverse through
 *      the blocks in the heap.
 *
 * Can be used for DEBUGGING to help you visualize your heap structure.
 * It traverses heap blocks and prints info about each block found.
 * 
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block 
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block as stored in the block header
 */                     
void disp_heap() {     

    int    counter;
    char   status[6];
    char   p_status[6];
    char * t_begin = NULL;
    char * t_end   = NULL;
    int    t_size;

    blockHeader *current = heap_start;
    counter = 1;

    int used_size =  0;
    int free_size =  0;
    int is_used   = -1;

    fprintf(stdout, 
            "********************************** HEAP: Block List ****************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, 
            "--------------------------------------------------------------------------------\n");

    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;

        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "alloc");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "FREE ");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "alloc");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "FREE ");
        }

        if (is_used) 
            used_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;

        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%4i\n", counter, status, 
                p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);

        current = (blockHeader*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, 
            "--------------------------------------------------------------------------------\n");
    fprintf(stdout, 
            "********************************************************************************\n");
    fprintf(stdout, "Total used size = %4d\n", used_size);
    fprintf(stdout, "Total free size = %4d\n", free_size);
    fprintf(stdout, "Total size      = %4d\n", used_size + free_size);
    fprintf(stdout, 
            "********************************************************************************\n");
    fflush(stdout);

    return;  
} 


//		p3Heap.c              (FA24)                     
                                       
