////////////////////////////////////////////
// This file: cache2Dcols.c
// Semester: CS 354 Lecture 001 Fall 2024
// Grade group: gg 9
// Instructor: Deppeler
//
// Author: Katie Krause
// Email: klkrause5@wisc.edu
// CS Login: kkrause
////////////////////////////////////////////

#define COLUMNS 500
#define ROWS 3000

int arr2D[ROWS][COLUMNS];

int main() {
	for (int col = 0; col < COLUMNS; col++) {
		for (int row = 0; row < ROWS; row++) {
			arr2D[row][col] = row + col;
		}
	}
	return 0;
}
