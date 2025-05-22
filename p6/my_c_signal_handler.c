////////////////////////////////////////////////////////////////////////////////
// Main File:        my_c_signal_handler.c
// This File:        my_c_signal_handler.c
// Other Files:      send_signal.c, my_div0_handler.c
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
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
// 
// AI chats:         save a transcript and submit with project.
//////////////////////////// 80 columns wide ///////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>

// The global variables
int siguser1count = 0;
int alrms_interval = 3;

/* This will handle SIGALRM signal, it prints the PID and time
 * and it makes the alarm go off after the specified interval
 *
 * Pre-conditions: a signal handler for the SIGALRM is set up, alarm interval has to be defined
 * 
 * param signum is the number given to the handler
 * retval n/a
 */
void handle_alarm(int signum) {
	pid_t pid = getpid();

    	time_t now;
    	time(&now);

    	// Prints our PID and time
    	printf("PID: %d CURRENT TIME: %s", pid, ctime(&now));

    	// Re-arm the alarm to go off in 3 seconds
    	alarm(alrms_interval);
}

/*
 * Handles the SIGUSR1 signal by incrementing a counter and printing a message
 *
 * Pre-conditions: A signal handler for SIGUSR1 has been set up, siguser1count is initialized
 *
 * param signum the number given to the handler
 *
 * retval n/a
 */
void handle_sigusr1(int signum) {
	siguser1count++;
	printf("Received SIGUSER1, user signal 1 counted.\n");
}

/*
 * Handles the SIGINT singal as it print a count and message of the SIGUSR1 signals given, then exits 
 *
 * Pre-conditions: A signal handler for SIGINT is set up and SIGUSR1's count gets updated
 *
 * param signum the number given to the handler
 *
 * retval n/a
 */
void handle_sigint(int int_signum) {
	printf("\nSIGINT handled.\n");
	printf("SIGUSR1 was handled %d times. Exiting now.\n", siguser1count);
	exit(0);
}

/* 
 * Sets up signal handlers for SIGALRM, SIGUSR1, and SIGINT, and then enters an infinite 
 * loop to handle signals. The program prints the PID and the current time every 3 seconds 
 * and handles user-defined and interrupt signals as specified.
 *
 * Pre-conditions: The signal handlers for SIGALRM, SIGUSR1, and SIGINT are added
 * The alarm interval (alrms_interval) is defined.
 * 
 * param None
 * 
 * retval Returns 0 on successful completion of the program.
 */
int main() {
	struct sigaction act;

	// SIGALRM handler
	memset(&act, 0, sizeof(act));
	act.sa_handler = handle_alarm;
	if (sigaction(SIGALRM, &act, NULL) < 0) {
		perror("sigaction for SIGALRM failed \n");
		exit(1);
	}

	// SIGUSR1 handler
	memset(&act, 0, sizeof(act));
	act.sa_handler = handle_sigusr1;
	if (sigaction(SIGUSR1, &act, NULL) < 0) {
		perror("sigaction for SIGUSR1 failed");
		exit(1);
	}

	// SIGINT handler
	memset(&act, 0, sizeof(act));
	act.sa_handler = handle_sigint;
	if (sigaction(SIGINT, &act, NULL) < 0) {
		perror("sigaction for SIGINT failed");
		exit(1);
	}

	alarm(alrms_interval);

    	printf("PID and current time: prints every 3 seconds.\n");
    	printf("Type Ctrl-C to end the program.\n");

    	// Infinite loop
    	while (1) {
        	pause();
	}

    	return 0;
}
