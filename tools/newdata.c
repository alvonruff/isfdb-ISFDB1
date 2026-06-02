/*
 *     (C) COPYRIGHT 1997-2000   Al von Ruff
 *	 ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)parsenov.c	1.1	02 May 1997 ISFDB";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char current_target[32];

int
parse_entry(FILE *fp)
{
	int input;
	int input2;
	int index;
	int loop;
	char entry_type[32];
	char entry_label[32];
	
	/*
	 * Parse past the REMOTE_HOST line
	 */
restart:
	while(1) {
		input = getc(fp);
		if (input == -1) {
			return(1);
		} else if (input == '\n') {
			char targetstr[]="# [REMOTE_HOST";

			index = 0;
			for(loop=0; loop<strlen(targetstr); loop++) {
				input = getc(fp);
				if (input == -1) {
					return(1);
				} else if (input != targetstr[index++]) {
					goto restart;
				}
			}
			while(input != '\n') {
				input = getc(fp);
				if (input == -1) {
					return(1);
				}
			}
			break;
		}
	}

	/*
	 * Parse in the label
	 */
	index = 0;
	for(loop=0; loop<10; loop++) {
		input = getc(fp);
	}
	while(input != '\n') {
		entry_label[index++] = input;
		input = getc(fp);
	}
	entry_label[index] = 0;
#ifdef REMOVE
	while(input != '[') {
		input = getc(fp);
	}
	input = getc(fp);
	while(input != ']') {
		entry_label[index++] = input;
		input = getc(fp);
	}
	entry_label[index] = 0;
	while(input != '\n') {
		input = getc(fp);
	}
#endif

	/*
	 * Parse the entry type
	 */
	index = 0;
	for(loop=0; loop<11; loop++) {
		input = getc(fp);
	}
	while(input != ' ') {
		entry_type[index++] = input;
		input = getc(fp);
	}
	entry_type[index] = 0;

	/*
	 * Check to see if this entry type matches 
	 * what we are looking for.
	 */
	if (strcmp(entry_type, current_target)) {
		goto restart;
	}

	/*
	 * Parse the rest of this line
	 */
	input = getc(fp);
	while(input != '\n') {
		input = getc(fp);
	}


	printf("============================ %s\n", entry_label);
	input = getc(fp);
	while(1) {
		if (input == '\n') {
			printf("%c", input);
			input = getc(fp);
			if (input == '#') {
				return(0);
			} else {
				printf("%c", input);
				input = getc(fp);
			}
		} else {
			printf("%c", input);
			input = getc(fp);
		}
	}
}


int
main(argc, argv)
	int argc;
	char *argv[];
{
	FILE *fp;

	if (argc != 3) {
		fprintf(stderr, "newdata <file> <target>\n");
		exit(1);
	}

	fp = fopen(argv[1], "rb");
	if (fp == NULL) {
		fprintf(stderr, "Couldn't Open %s\n", argv[1]);
		exit(1);
	}

	strcpy(current_target, argv[2]);
	while(1) {
		if (parse_entry(fp)) {
			break;
		}
	}
	exit(0);
}
