////////////////////////////////////////////////////////////////////////////////
// Main File:        send_signal.c
// This File:        send_signal.c
// Other Files:      my_c_signal_handler.c, my_div0_handler.c
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
#include <signal.h>
#include <unistd.h>
#include <string.h>

/* 
 * Parses command line arguments to determine the signal type (SIGUSR1 or SIGINT) 
 * and sends the corresponding signal to the specified process ID (PID) using the kill system call
 * If invalid arguments are provided, the program prints a usage message and exits
 *
 * Pre-conditions: The program must receive two arguments either a -u or -i flag
 * and a valid process ID
 *
 * param argc The number of command line arguments passed to the program (should be 3).
 * param argv Array of command line arguments, where:
 *  - argv[1] is the signal type (-u or -i),
 *  - argv[2] is the process ID (PID) to where the signal is gonna be sent
 *
 * retval Returns 0 if the signal was successfully sent otherwise the program exits
 */
int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Usage: send_signal -u <pid> to send SIGUSR\n");
		printf("       send_signal -i <pid> to send SIGINT\n");
		exit(1);
	}

	int signal_type;
	pid_t pid;

	if (strcmp(argv[1], "-u") == 0) {
		signal_type = SIGUSR1;
	} else if (strcmp(argv[1], "-i") == 0) {
		signal_type = SIGINT;
	} else {
		printf("Usage: send_signal -u <pid> to send SIGUSR1\n");
		printf("       send_signal -i <pid> to send SIGINT\n");
		exit(1);
	}

	pid = atoi(argv[2]);
	if (pid <= 0) {
		printf("Invalid pid: %s\n", argv[2]);
		exit(1);
	}

	if (kill(pid, signal_type) == -1) {
		perror("Error sending signal");
		exit(1);
	}

	printf("Sent signal %d to pid %d\n", signal_type, pid);

	exit(0);
}
