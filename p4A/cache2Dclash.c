////////////////////////////////////////////
// This file: cache2Dclash.c
// Semester: CS 354 Lecture 001 Fall 2024
// Grade group: gg 9
// Instructor: Deppeler
//
// Author: Katie Krause
// Email: klkrause5@wisc.edu
// CS Login: kkrause
////////////////////////////////////////////

#define ROWS 128
#define COLUMNS 8

int arr2D[ROWS][COLUMNS];

int main() {
	for (int iteration = 0; iteration < 100; iteration++) {
		for (int row = 0; row< ROWS; row += 64) {
			for (int col = 0; col < COLUMNS; col++) {
				arr2D[row][col] = iteration + row + col;
			}
		}
	}
	return 0;
}
