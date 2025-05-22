////////////////////////////////////////////////////////////////////////////////
// Main File:        my_div0_handler.c
// This File:        my_div0_handler.c
// Other Files:      my_c_signal_handler.c, send_signal.c
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
//////////////////// REQUIRED -- OTHER SOURCES OF HELP ///////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   https://www.geeksforgeeks.org/use-fflushstdin-c/
//                   Looked up how to fix my output buffer because my code was
// submitting like Enter first integer: ^CTotal number of operations completed successfully: 1
// 
// AI chats:         save a transcript and submit with project.
//////////////////////////// 80 columns wide ///////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int success_counter = 0;

/* 
 * Handles the SIGFPE signal, which happens when dividing by 0
 * This function prints an error message, displays the total of successful operations, then exits
 *
 * Pre-conditions: The program must be set up to catch the SIGFPE signal which is triggered 
 * when a division by zero happens
 *
 * param signum The signal number (SIGFPE)
 *
 * retval n/a
 */
void handle_sigfpe(int signum) {
	printf("Error: a division by 0 operation was attempted. /n");
	printf("\nTotal number of operations completed successfully: %d\n", success_counter);
	printf("The program will be terminated.\n");
	fflush(stdout);
	exit(0);
}


/* 
 * Handles the SIGINT signal when ctrl c gets pressed
 * This function prints the total number of succcessful operations and then exits
 *
 * Pre-conditions: The program is setup to catch SIGINT signal
 *
 * param signum The signal number (SIGINT)
 *
 * retval n/a
 */
void handle_sigint(int signum) {
	printf("\nTotal number of operations completed successfully: %d\n", success_counter);
	printf("The program will be terminated.\n");
	fflush(stdout);
        exit(0);
}

/* 
 * This sets up the signal handlers for SIGINT and SIGFPE where it will enter a loop, asks for
 * two intergers and then divides them. Then it will print the result. If div by 0 happens, SIGFPE
 * handles this. To exit using CTRL C SIGINT prints the number of good operations and then will exit.
 *
 * Pre-conditions: N/a ?
 *
 * param argc The number of command line arguments.
 * param argv The array of command line arguments.
 *
 * retval 0 if the program finishes successfully!
 */
int main() {
	struct sigaction sa_fpe, sa_int;

	// initialize the handlers to be zero
	memset(&sa_fpe, 0, sizeof(sa_fpe));
	memset(&sa_int, 0, sizeof(sa_int));

	sa_fpe.sa_handler = handle_sigfpe;
   	sigaction(SIGFPE, &sa_fpe, NULL);

	sa_int.sa_handler = handle_sigint;
    	sigaction(SIGINT, &sa_int, NULL);

	char buffer[100];
    	int num1, num2;

	while (1) {
		printf("Enter first integer: ");
		fflush(stdout);
        	fgets(buffer, sizeof(buffer), stdin);
        	num1 = atoi(buffer);

		printf("Enter second integer: ");
		fflush(stdout);
        	fgets(buffer, sizeof(buffer), stdin);
       		num2 = atoi(buffer);

		if (num2 == 0) {
			raise(SIGFPE);
		} else {
			int quotient = num1 / num2;
			int remainder = num1 % num2;
			printf("%d / %d is %d with a remainder of %d\n", num1, num2, quotient, remainder);
			fflush(stdout);
			success_counter++;
		}
	}
	return 0;
}
