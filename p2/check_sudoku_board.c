////////////////////////////////////////////////////////////////////////////////
// Main File:        check_sudoku_board.c
// This File:        check_sudoku_board.c 
// Other Files:      N/A
// Semester:         CS 354 Lecture 001 Fall 2024
// Grade Group:      gg 9  (See canvas.wisc.edu/groups for your gg#)
// Instructor:       deppeler
// 
// Author:           Katie Krause
// Email:            klkrause5@wisc.edu
// CS Login:         kkrause
//
///////////////////////////  OPTIONAL WORK LOG  //////////////////////////////
//  Document your work sessions here or on your copy http://tiny.cc/work-log
//  Keep track of commands, structures, code that you have learned.
//  This will help you focus your review on this from each program that
//  are new to you. There is no need to submit work log.
/////////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
// 
// AI chats:         save a transcript and submit with project.
//////////////////////////// 80 columns wide ////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Copyright 2021-24 Deb Deppeler
// Posting or sharing this file is prohibited, including any changes/additions.
//
// We have provided comments and structure for this program to help you get 
// started.  Later programs will not provide the same level of commenting,
// rather you will be expected to add same level of comments to your work.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *DELIM = ",";  // commas ',' are a common delimiter character for data strings

/* TODO: implement this function
 * Returns 1 if and only if the 2D array of ints in board 
 * is in a valid Sudoku board state.  Otherwise returns 0.
 *
 * DOES NOT PRODUCES ANY PRINTED OUTPUT
 * 
 * A valid row or column contains only blanks or the digits 1-size, 
 * with no duplicate digits, where size is the value 1 to 9.
 * 
 * Note: This function requires only that each row and each column are valid.
 * 
 * board: heap allocated 2D array of integers 
 * size:  number of rows and columns in the board
 */
int valid_sudoku_board(int **board, int size) {
	// creating space for rows and columns 1-9
	int *row_keeper = (int *)malloc(10 * sizeof(int));
    	int *col_keeper = (int *)malloc(10 * sizeof(int));

	if (row_keeper == NULL) {
		printf("row_keeper memory failed\n");
		exit(1);
	}

	if (col_keeper == NULL) {
                printf("col_keeper memory failed\n");
                exit(1);
        }

	// iterate through rows
	for (int i = 0; i < size; i++) {
		// check array
		for (int j = 0; j < 10; j++) {
		*(row_keeper + j) = 0;
            	*(col_keeper + j) = 0;
	        }

		for (int j = 0; j < size; j++) {
			int current_row = *(*(board +i) + j);
			if (current_row > 0 && current_row <= size) {
				if (*(row_keeper + current_row)) {
					return 0;
			}
			*(current_row + row_keeper) = 1;
		}

			int current_col = *(*(board + j) + i);
			if (current_col <= size && current_col > 0) {
				if (*(current_col + col_keeper)) {
					return 0;
				}
				*(col_keeper + current_col) = 1;
			}
		}
	}
	free(row_keeper);
	free(col_keeper);
	return 1;
}


/* COMPLETED (DO NOT EDIT)       
 * Read the first line of file to get the size of that board.
 * 
 * PRE-CONDITION #1: file exists
 * PRE-CONDITION #2: first line of file contains valid non-zero integer value
 *
 * fptr: file pointer for the board's input file
 * size: a pointer to an int to store the size
 *
 * POST-CONDITION: the integer whos address is passed in as size (int *) 
 * will now have the size (number of rows and cols) of the board being checked.
 */
void get_board_size(FILE *fptr, int *size) {      
	char *line = NULL;
	size_t len = 0;

	// 'man getline' to learn about <stdio.h> getline
	if ( getline(&line, &len, fptr) == -1 ) {
		printf("Error reading the input file.\n");
		free(line);
		exit(1);
	}

	char *size_chars = NULL;
	size_chars = strtok(line, DELIM); // 'man strtok' string tokenizer
	*size = atoi(size_chars);         // 'man atoi' alpha to integer
	free(line);                       // free memory allocated for line 
}


/* TODO: COMPLETE THE MAIN FUNCTION
 * This program prints "valid" (without quotes) if the input file contains
 * a valid state of a Sudoku puzzle board wrt to rows and columns only.
 * It prints "invalid" (without quotes) if the input file is not valid.
 *
 * Usage: A single CLA that is the name of a file that contains data.
 *
 * argc: the number of command line args (CLAs)
 * argv: the CLA strings, includes the program name
 *
 * Returns 0 if file exists and is readable.
 * Exit with any non-zero result if unable to open and read the file given.
 */
int main( int argc, char **argv ) {              

	// TODO: Check if number of command-line arguments is correct.

	if (argc != 2) {
                printf("invalid amount\n");
                exit(1);
        }

	// Open the file 
	FILE *fp = fopen(*(argv + 1), "r");
	if (fp == NULL) {
		printf("Can't open file for reading.\n");
		exit(1);
	}

	// will store the board's size, number of rows and columns
	int size;

	// Call get_board_size to read first line of file as the board size.
	 get_board_size(fp, &size);
	if (size == -1) {
		printf("invalid\n");
		fclose(fp);
		exit(1);
	}

	// TODO: Dynamically allocate a 2D array for given board size.
	// You must dyamically create a array of pointers to other arrays of ints
	int **board = (int **)malloc(size * sizeof(int *));
	if (board == NULL) {
		printf("board memory failed\n");
		exit(1);
	}

	for (int i = 0; i < size; i++) {
		*(board + i) = (int *)malloc(size * sizeof(int));
		if (*(board + i) == NULL) {
			for (int j = 0; j<1; j++) {
				free(*(board + j));
			}
			free(board);
			printf("board row failed\n");
			exit(1);
		}
	}

	// Read the remaining lines of the board data file.
	// Tokenize each line and store the values in your 2D array.
	char *line = NULL;
	size_t len = 0;
	char *token = NULL;
	for (int i = 0; i < size; i++) {

		// read the line
		if (getline(&line, &len, fp) == -1) {
			printf("Error while reading line %i of the file.\n", i+2);
			exit(1);
		}

		token = strtok(line, DELIM);
		for (int j = 0; j < size; j++) {
			// TODO: Complete the line of code below
			// to initialize elements of your 2D array.
			*(*(board + i) + j) = atoi(token); 
			token = strtok(NULL, DELIM);
		}
	}

	// TODO: Call valid_sudoku_board and print the appropriate
	//       output depending on the function's return value.
	if (valid_sudoku_board(board, size)) {
		printf("valid\n");
	} else {
		printf("invalid\n");
	}

	// TODO: Free dynamically allocated memory.
	for (int i = 0; i < size; i++) {
		free(*(board + i));
	}
	free(board);
	free(line);

	//Close the file.
	if (fclose(fp) != 0) {
		printf("Error while closing the file.\n");
		exit(1);
	} 

	return 0;       
}       

