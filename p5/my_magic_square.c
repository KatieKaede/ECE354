////////////////////////////////////////////////////////////////////////////////
// Main File:        my_magic_square.c
// This File:        my_magic_square.c
// Other Files:      N/A
// Semester:         CS 354 Lecture 001 Fall 2024
// Grade Group:      gg9  (See canvas.wisc.edu/groups for your gg#)
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
//////////////////////////// 80 columns wide ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Copyright 2020 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission, CS 354 Fall 2024, Deb Deppeler
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure that represents a magic square
typedef struct {
        int size;           // dimension of the square
        int **magic_square; // 2D heap array that stores magic square
} MagicSquare;

/* TODO:
 * Prompts the user for the magic square's size, read size,
 * check if it's an odd number >= 3 
 * (if not valid size, display the required error message and exit)
 *
 * return the valid number
 */
int getSize() {
	int squareSize;

	printf("Enter magic square's size (odd integer >=3)\n");

	scanf("%d", &squareSize);

	if (squareSize % 2 == 0) {
		printf("Magic square size must be odd.\n");
		exit(1);
	}

	if (squareSize < 3) {
                printf("Magic square size must be >= 3.\n");
                exit(1);
        }

	return squareSize;
}

/* TODO:
 * Creates a magic square of size n on the heap.
 *
 * May use the Siamese magic square algorithm or alternate
 * valid algorithm that produces a magic square. 
 *
 * n - the number of rows and columns
 *
 * returns a pointer to the completed MagicSquare struct.
 */
MagicSquare *generateMagicSquare(int n) {
	MagicSquare *square = (MagicSquare *)malloc(sizeof(MagicSquare));
	if (square == NULL) {
		printf("Couldn't do memory allocation\n");
		exit(1);
	}

	// Grab the size from the structure
	square->size = n;

	// Make space for the structure
	square->magic_square = (int **)malloc(n * sizeof(int *));
	if (square->magic_square == NULL) {
                printf("Couldn't do memory allocation for square\n");
                exit(1);
        }

	// Make memory for array of pointers for rows
	for (int i = 0; i < n; i++) {
		*(square->magic_square + i) = (int *)malloc(n * sizeof(int));
		if (*(square->magic_square + i) == NULL) {
	                printf("Couldn't do memory allocation for row\n");
        	        exit(1);
		}
	}

	// Make all the spaces 0
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			*(*(square->magic_square + i) + j) = 0;
		}
	}

	int num = 1;
	int row = 0;
	int col = n / 2;

	// n^2
	while (num <= n * n) {
		*(*(square->magic_square + row) + col) = num;

		// Move diagonally up right by one row and column
		int moved_row = (row - 1 + n) % n;
		int moved_col = (col - 1 + n) % n;

		//  If next position is filled, place one row below
		if (*(*(square->magic_square + moved_row) + moved_col) != 0) {
			row = (row + 1) % n;
		} else {
			row = moved_row;
			col = moved_col;
		}
		num++;
	}
	return square;
}


/* TODO:
 * Open a new file (or overwrites the existing file)
 * and write the magic square values to the file
 * in the format specified by assignment.
 *
 * See assignment for required file format.
 *
 * magic_square - the magic square to write to a file
 * filename - the name of the output file
 */
void fileOutputMagicSquare(MagicSquare *magic_square, char *filename) {
	FILE *file = fopen(filename, "w");

	// Check if the file can be opened
	if  (file == NULL) {
		printf("Couldn't open file\n");
		exit(1);
	}

	// Write to the file the square size
	fprintf(file, "%d\n", magic_square->size);

	// Add in the numbers of the square
	for (int i = 0; i < magic_square->size; i++) {
		for  (int j = 0; j < magic_square->size; j++) {
			fprintf(file, "%d", *(*(magic_square->magic_square + i) + j));
			if (j < magic_square->size - 1) {
				fprintf(file, ",");
			}
		}
		fprintf(file, "\n");
	}
	if (fclose(file) != 0) {
		printf("File couldn't close properly\n");
		exit(1);
	}
}


/* TODO:
 * Call other functions to generate a magic square 
 * of the user specified size and outputs 
 * the created square to the output filename.
 * 
 * Add description of required CLAs here
 */
int main(int argc, char **argv) {
	// TODO: Check input arguments to get output filename
	if (argc != 2) {
		printf("Usage: ./my_magic_square <output_filename>\n");
		exit(1);
	}

	char *filename = argv[1];

	// TODO: Get magic square's size from user
	int size = getSize();

	// TODO: Generate the magic square by correctly interpreting 
	//       the algorithm(s) in the write-up or by writing or your own.  
	//       You must confirm that your program produces a 
	//       Magic Square. See description in the linked Wikipedia page.
	MagicSquare *square = generateMagicSquare(size);

	// TODO: Output the magic square
	fileOutputMagicSquare(square, filename);

	for (int i = 0; i < square->size; i++) {
		free(*(square->magic_square + i));
	}
	free(square->magic_square);
	free(square);

	return 0;
}
