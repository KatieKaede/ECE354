////////////////////////////////////////////////////////////////////////////////
// Main File:        csim.c
// This File:        csim.c
// Other Files:      (name of all other files if any)
// Semester:         CS 354 Lecture 001 Fall 2024
// Grade Group:      gg9  (See canvas.wisc.edu/groups for your gg#)
// Instructor:       deppeler
// 
// Author:           Katie Krause
// Email:            klkrause5@wisc.edu
// CS Login:         kkrause
//
///////////////////////////  OPTIONAL WORK LOG  //////////////////////////////
// (I have no idea if I'm using this work log correctly, but I tried documenting my process)
// Added last variable to structure to determine age for LRU policy
// Implemented init_cache to dynamically allocate enough space for sets and lines
// Added in null checks for dynamic lib functions
// Finished free_cache function
// Completed the missing code in replay_trace
// Tried implementing access_data, woruldn't work was getting free() abort error
// Also implemented LRU with counter version
// Tried valgrind check, was getting invalid reads/writes
// Fixed abort error, had a variable being used incorrectly
// Was getting incorrect number of hits and misses
// Added in verbose commenting
// Realized I wasn't accounting for initial empty cache in my code
// Got the correct output compared to the the ref csim
// Tried against test-csim, got matching results (woohoo)!
//
/////////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
// 
// AI chats:         save a transcript and submit with project.
////////////////////////////////////////////////////////////////////////////////
// Copyright 2013,2019-2024
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission for Fall 2024
////////////////////////////////////////////////////////////////////////////////

/**
 * csim.c:  
 * Simulatate the contents of a cache with given configuration and 
 * count the number of hits, misses, and evictions for a given 
 * sequence of memory accesses for a program.
 *
 * If you want to create your own memory access traces, you can 
 * use valgrind to output traces that this simulator can analyze.
 *
 * What is the replacement policy implemented?  ___________________________
 *
 * Implementation and assumptions:
 *  1. (L) load or (S) store cause at most one cache miss and a possible eviction.
 *  2. (I) Instruction loads are ignored.
 *  3. (M) Data modify is treated as a load followed by a store to the same
 *     address. Hence, an (M) operation can result in two cache hits, 
 *     or a miss and a hit plus a possible eviction.
 *
 * Find the TODO tags to see where to make chanes for your simulator.
 */  

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/******************************************************************************/
/* DO NOT MODIFY THESE VARIABLE NAMES and TYPES                               */
/* DO UPDATE THEIR VALUES AS NEEDED BY YOUR CSIM                              */

//Globals set by command line args.
int b = 0; //number of (b) bits
int s = 0; //number of (s) bits
int E = 0; //number of lines per set

//Globals derived from command line args.
int B; //block size in bytes: B = 2^b
int S; //number of sets: S = 2^s

//Global counters to track cache statistics in access_data().
int hit_cnt = 0;
int miss_cnt = 0;
int evict_cnt = 0;

//Global to control trace output
int verbosity = 0; //print trace if set
/******************************************************************************/


// Type mem_addr_t: stores an addresses or address masks.
typedef unsigned long long int mem_addr_t;

// Type cache_line_t: stores "overhead" (v-bit and tag) for a cache line
// The cache block's data is not needed or stored for this simulator.
typedef struct cache_line {                    
    char valid; // 1 if valid, 0 if not
    mem_addr_t tag; // tag for memory block
    int last; // LRU policy, updated to show age
} cache_line_t;

// Type cache_set_t: Stores a pointer to the first cache line in a set.
// Note: Each set is a pointer to a heap array of one or more cache lines.
typedef cache_line_t* cache_set_t;

// Type cache_t: Stores a pointer to the first set in a cache
// Note: The cache is a pointer to a heap array of one or more sets.
typedef cache_set_t* cache_t;

// Create the cache we're simulating. 
// Note: The cache is a pointer to a heap array of one or more sets.
cache_t cache;  

/* init_cache:
 * Allocates the data structure for a cache with S sets and E lines per set.
 * Initializes all valid bits and tags with 0s.
 */                    
void init_cache() {
	// S = 2^s, B = 2^b
	S = 1 << s;
	B = 1 << b;

	// Make memory for the cache with an array of cache sets
	cache = (cache_set_t*)malloc(S * sizeof(cache_set_t));
	if (cache == NULL) {
		printf("Unable to allocate memory for sets \n");
		exit(1);
	}

	// With each set, make memory for how many cache lines
	for (int i = 0; i < S; i++) {
		cache[i] = (cache_line_t*)malloc(E * sizeof(cache_line_t));
		if (cache[i] == NULL) {
			printf("Unable to allocate memory for lines \n");
			exit(1);
		}

		// Initialize the lines detials
		for (int j = 0; j < E; j++) { 
			cache[i][j].valid = 0;
			cache[i][j].tag = 0;
			cache[i][j].last = 0;
		}
	}
}


/* free_cache:
 * Frees all heap allocated memory used by the cache.
 */                    
void free_cache() {             
	for (int i = 0; i < S; i ++) {
		if(cache[i] != NULL) {
			free(cache[i]);
		}
	}
	free(cache);
}

/* access_data:
 * Simulates data access at given "addr" memory address in the cache.
 *
 * If block containing addr is already in cache, increment hit_cnt
 * If block containing addr is not in cache, 
 *    cache it (set tag and valid), increment miss_cnt
 * If a block in a cache line is evicted, increment evict_cnt
 */                    
void access_data(mem_addr_t addr) {
   	mem_addr_t set_index_mask = (1 << s) - 1;
    	mem_addr_t set_index = (addr >> b) & set_index_mask;
    	mem_addr_t tag = addr >> (b + s);
    	cache_set_t set = cache[set_index];

    	bool hit = false;
    	int empty_index = -1;
    	int lru_index = -1;
    	int oldest = -1;

    	for (int i = 0; i < E; i++) {
       		cache_line_t* line = &set[i];
        
        	if (line->valid && line->tag == tag) {
	            	// Cache hit
        	    	hit = true;
			hit_cnt++;
            		line->last = 0;
			if (verbosity) printf("hit ");
			break;

        	} 
		else if (!line->valid && empty_index == -1) {
			empty_index = i;
		}
        	else if (line->last > oldest) {
            		oldest = line->last;
            		lru_index = i;
		}
	}

    	if (!hit) {
        	miss_cnt++;
		if (verbosity) printf("miss ");

	        int line_to_replace = (empty_index != -1) ? empty_index : lru_index;

	        if (set[line_to_replace].valid) {
        	    evict_cnt++;
                    if (verbosity) printf("eviction ");
       		 }

		set[line_to_replace].valid = 1;
        	set[line_to_replace].tag = tag;
       		set[line_to_replace].last = 0;

        	for (int i = 0; i < E; i++) {
            		if (i != line_to_replace && set[i].valid) {
                		set[i].last++;
			}
		}
	}
}

/* replay_trace:
 * Replays the given trace file against the cache.

 Example: subset of trace, shows type of access, address, size of access

 L 7ff0005b8,4
 S 7feff03ac,4
 M 7fefe059c,4
 
 * Reads the input trace file line by line.
 * Extracts the type of each memory access : L/S/M
 * TRANSLATE "L" as a Load i.e. one memory access
 * TRANSLATE "S" as a Store i.e. one memory access
 * TRANSLATE "M" as a Modify which is a load followed by a store, 2 mem accesses 
 */                    
void replay_trace(char* trace_fn) {           
    char buf[1000];  
    mem_addr_t addr = 0;
    unsigned int len = 0;
    FILE* trace_fp = fopen(trace_fn, "r"); 

    if (!trace_fp) { 
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);   
    }

    while (fgets(buf, 1000, trace_fp) != NULL) { 
        if (buf[1] == 'S' || buf[1] == 'L' || buf[1] == 'M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);

            if (verbosity)
                printf("%c %llx,%u ", buf[1], addr, len);

            switch (buf[1]) {
                case 'L':  // Load
                    access_data(addr);
                    break;

                case 'S':  // Store
                    access_data(addr);
                    break;

                case 'M':  // Modify
                    access_data(addr);
                    access_data(addr);
                    break;
            }

            if (verbosity)
                printf("\n");
        }
    }

    fclose(trace_fp);
}  


/*
 * print_usage:
 * Print information on how to use csim to standard output.
 */                    
void print_usage(char* argv[]) {                 
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Verbose flag.\n");
    printf("  -s <num>   Number of s bits for set index.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of b bits for word and byte offsets.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}  


/*
 * print_summary:
 * Prints a summary of the cache simulation statistics to a file.
 */                    
void print_summary(int hits, int misses, int evictions) {                
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}  


/*
 * main:
 * parses command line args, 
 * makes the cache, 
 * replays the memory accesses, 
 * frees the cache and 
 * prints the summary statistics.  
 */                    
int main(int argc, char* argv[]) {                      
    char* trace_file = NULL;
    char c;

    // Parse the command line arguments: -h, -v, -s, -E, -b, -t 
    while ((c = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
        switch (c) {
            case 'b':
                b = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'h':
                print_usage(argv);
                exit(0);
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'v':
                verbosity = 1;
                break;
            default:
                print_usage(argv);
                exit(1);
        }
    }

    // Make sure that all required command line args were specified.
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        print_usage(argv);
        exit(1);
    }

    // Initialize cache.
    init_cache();

    // Replay the memory access trace.
    replay_trace(trace_file);

    // Free memory allocated for cache.
    free_cache();

    // Print the statistics to a file.
    // DO NOT REMOVE: This function must be called for test_csim to work.
    print_summary(hit_cnt, miss_cnt, evict_cnt);
    return 0;   
}  

//			202409                                     

