/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)psynop.c	1.4	01/28/98 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sfdbase.h"

int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int		result;
	char		path[256];
	char		*ptr1;
	char		*ptr2;
	char		goodtitle[256];

	printf("Content-type: text/html\n\n");

	if (argc != 2) {
		printf("Bad offset input\n");	
		exit(1);
	}

#ifdef CHDIR
        result = chdir(CGIBIN);
        if (result != 0) {
                printf("CHDIR to %s failed\n", CGIBIN);
                exit(1);
        }
#endif
	printf("<html><head>\n");
	printf("<title>Synopsis</title></head>\n");
        printf("<body bgcolor=#ffffff>\n");

	if ( argv[1] ) {
		ptr1 = (char *)strstr(argv[1], ",");
		*ptr1 = 0;
		ptr1++;

		ptr2 = goodtitle;
		while( *ptr1 ) {
			if (*ptr1 == '_') {
				*ptr2 = ' ';
			} else if (*ptr1 == '\\') {
				/* don't copy backslashes */
				*ptr2--;
			} else {
				*ptr2 = *ptr1;
			}
			ptr1++; ptr2++;
		}
		*ptr2 = 0;

		printf("<h1>Synopsis</h1> <i>%s</i><hr>\n", goodtitle);
		printf("<pre>\n");

		read_and_printwrap(USE_SYNOPSIS, argv[1], 0, RIGHTMARGIN2, 1);
	}

	postamble("psynop", argv[1]);
	printf("</pre>\n");
}
