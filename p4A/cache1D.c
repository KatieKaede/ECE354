////////////////////////////////////////////
// This file: cache1D.c
// Semester: CS 354 Lecture 001 Fall 2024
// Grade group: gg 9
// Instructor: Deppeler
//
// Author: Katie Krause
// Email: klkrause5@wisc.edu
// CS Login: kkrause
////////////////////////////////////////////

#define ARRAY_SIZE 100000

// Global array
int arr[ARRAY_SIZE];

int main () {
	for (int i = 0; i < ARRAY_SIZE; i++) {
		arr[i] = i;
	}
	return 0;
}
