/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)submit.c	1.11	01/28/98 SFdbase";

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef SUNOS
#include <sys/unistd.h>
#else
#include <unistd.h>
#endif
#include "sfdbase.h"

char	finalfilename[256];
entry	entries[MAX_ENTRIES];

#ifdef CYGWIN
int errno;
#endif

int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int		loop;
	int		max;
	int		content_length;
	int		orig;
	int		fd;
	int		size;
	int		result;
	int		multiplier;
	char *		filesize;
	char *		target;
	char *		filestart;

printf("[%d]\n", __LINE__); fflush(stdout);
	printf("Content-type: text/html\n\n");
	if(strcmp(getenv("REQUEST_METHOD"), "POST") ) {
		printf("This script should be referenced with a METHOD of POST.\n");
		printf("<A HREF=\"http://www.ncsa.uiuc.edu/SDG/Software/Mosaic/Docs/fill-out-forms/overview.html\">forms overview</A>.%c",10);
		exit(1);
	}
	
printf("[%d]\n", __LINE__); fflush(stdout);
	orig = content_length = atoi(getenv("CONTENT_LENGTH"));
	for(loop=0; content_length && (!feof(stdin)); loop++) {
		max = loop;
		entries[loop].val = (char *)fmakeword(stdin, 0, &content_length);
		entries[loop].name = (char *)makeword(entries[loop].val, '=');
	}

printf("[%d]\n", __LINE__); fflush(stdout);
        printf("<html><head>\n");
        printf("<title>Upload</title></head>\n");
        printf("<body bgcolor=#ffffff>\n");
	printf("<pre>\n");

printf("[%d]\n", __LINE__); fflush(stdout);
	if (content_length != 0) {
		printf("Bad content length (%d)\n", content_length);
		printf("</pre>\n");
		fflush(stdout);
		exit(0);
	}

printf("[%d]\n", __LINE__); fflush(stdout);

#ifdef REMOVE
printf("%s",entries[0].val);
#endif
	/*
	 * [1] Find the 1st double quote. This will be the 
	 * beginning of the string that describes the value
	 * type. When found, look for the trailing double
	 * quote, and NULL out the string there. The variable
	 * 'target' will point to this string.
	 */
	target = (char *)strstr(entries[0].val, "\"");
	if (target) {
		target++;
		filesize = (char *)strstr(target, "\"");
		*filesize = 0;
		filesize++;
	} else {
		printf("Can't find file start\n");
		exit(0);
	}
printf("[%d]\n", __LINE__); fflush(stdout);

	if ( !strcmp(target, "CMPL")) {
		sprintf(finalfilename, "/tmp/cmpl.tar.Z");
	} else if ( !strcmp(target, "DATA")) {
		sprintf(finalfilename, "/tmp/data.tar.Z");
	} else if ( !strcmp(target, "FRAME")) {
		sprintf(finalfilename, "/tmp/frame.tar.Z");
	} else if ( !strcmp(target, "HTML")) {
		sprintf(finalfilename, "/tmp/html.tar.Z");
	} else if ( !strcmp(target, "PRIVATE")) {
		sprintf(finalfilename, "/tmp/private_tools.tar.Z");
	} else if ( !strcmp(target, "SRC")) {
		sprintf(finalfilename, "/tmp/src.tar.Z");
	} else if ( !strcmp(target, "TOOLS")) {
		sprintf(finalfilename, "/tmp/tools.tar.Z");
	}
printf("[%d]\n", __LINE__); fflush(stdout);

	fd = open(finalfilename, O_RDWR|O_CREAT|O_TRUNC, 0666);
	if (fd == -1) {
		printf("Can't open %s: %d\n", finalfilename, errno);
		exit(0);
	}
printf("[%d]\n", __LINE__); fflush(stdout);

	filesize = (char *)strstr(filesize, "FILESIZE");
	filesize += 9;

	size = 0;
	multiplier = 100000000;
	for(loop=0; loop<9; loop++) {
		int digit;

		digit = *filesize - '0';
		digit *= multiplier;
		size += digit;
		filesize++;
		multiplier = multiplier / 10;
	}

	while(size) {
		result = write(fd, filesize, size);
		size -= result;
		filesize += result;
	}
	close(fd);

	printf("<h1>Upload of %s complete.</h1>\n", target);
	printf("</pre>\n");
	fflush(stdout);
	close(fd);
	exit(0);
}
