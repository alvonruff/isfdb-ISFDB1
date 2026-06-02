/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)AwGen.c	1.12	05/01/98 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "sfdbase.h"

entry		entries[MAX_ENTRIES];
search_t	*title_list = NULL;


int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int	result;
	char	path[256];
	char	sourcefile[32];
	char	type[8];
	char	year[16];
	int	content_length;
	int	loop;
	int	max;
	char	*startptr;
	char	*endptr;
	char	*ptr;

	printf("Content-type: text/html\n\n");

	if(strcmp(getenv("REQUEST_METHOD"), "POST") ) {
		print_fatal("This script should only be referenced with a METHOD of POST.");
	}
	
	if(strcmp(getenv("CONTENT_TYPE"), "application/x-www-form-urlencoded")) {
		print_fatal("This script can only be used to decode form results.");
	}

	content_length = atoi(getenv("CONTENT_LENGTH"));
	for(loop=0; content_length && (!feof(stdin)); loop++) {
		max = loop;
		entries[loop].val = (char *)fmakeword(stdin, '&', &content_length);
		plustospace(entries[loop].val);
		unescape_url(entries[loop].val);
		entries[loop].name = (char *)makeword(entries[loop].val, '=');
	}

	for(loop=0; loop <= max; loop++) {
		if (strncmp(entries[loop].name, "sourcefile", 6) == 0) {
			if (entries[loop].val[0]) {
				strcpy(sourcefile, entries[loop].val);
			} else {
				sourcefile[0] = 0;
			}
		}
	}

	if ( strncmp(sourcefile, "cmpl", 4) == 0) {
		strcpy(type, "CMPL");
	} else if ( strncmp(sourcefile, "data ", 4) == 0) {
		strcpy(type, "DATA");
	} else if ( strncmp(sourcefile, "frame", 5) == 0) {
		strcpy(type, "FRAME");
	} else if ( strncmp(sourcefile, "html", 4) == 0) {
		strcpy(type, "HTML");
	} else if ( strncmp(sourcefile, "private_tools", 13) == 0) {
		strcpy(type, "PRIVATE");
	} else if ( strncmp(sourcefile, "src", 3) == 0) {
		strcpy(type, "SRC");
	} else if ( strncmp(sourcefile, "tools", 5) == 0) {
		strcpy(type, "TOOLS");
	}

#ifdef CHDIR
        result = chdir(CGIBIN);
        if (result != 0) {
                printf("CHDIR to %s failed\n", CGIBIN);
                exit(1);
        }
#endif

	printf("<html><head>\n");
	printf("<title>DBase Update: %s </title></head>\n", type );
	printf("<body bgcolor=#ffffff>\n");
	printf("<h1>Submitting [%s] for Update</h1>\n", type);
	printf("<hr>\n");
	printf("Select a local file for submission:\n");
	printf("<form\n");
	printf("  ENCTYPE=\"multipart/form-data\"\n");
	printf("  METHOD=\"POST\"\n");
	printf("  ACTION=\"http:/%s/upload.cgi\">\n", HTFAKE);
	printf("<br>\n");
	printf("<p>\n");
	printf("<INPUT\n");
	printf("  NAME=\"%s\"\n", type);
	printf("  TYPE=\"file\">\n");
	printf("<BR>\n");
	printf("<INPUT TYPE=\"submit\" VALUE=\"Submit file for checkin\">\n");
	printf("<INPUT TYPE=\"reset\" VALUE=\"Reset\">\n");
	printf("<HR>\n");
	printf("Checkin comments:\n");
	printf("<BR>\n");
	printf("<TEXTAREA NAME=\"NT\" ROWS=10 COLS=60></TEXTAREA>\n");
	printf("</FORM>\n");

	return(0);
}
