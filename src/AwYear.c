/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)AwYear.c	1.7	01/05/00 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sfdbase.h"

extern void get_award_info(char *exact_author, search_t *title_list, char *awardname);
extern void print_awards(char *type, char *year);

search_t	*title_list = NULL;


int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int		result;
	search_t	*tmp;
	audata_t	*auth;
	char		path[256];
	char		type[8];
	char		year[8];
	char		errmsg[BIGSIZE];

	printf("Content-type: text/html\n\n");

	if (argc != 2) {
		sprintf(errmsg, "Incorrect argument count = %d\n", argc);
		print_fatal(errmsg);
	}

#ifdef CHDIR
        result = chdir(CGIBIN);
        if (result != 0) {
                printf("CHDIR to %s failed\n", CGIBIN);
                exit(1);
        }
#endif


	strcpy(type, argv[1]);
	type[2] = 0;
	strcpy(year, (char *)&(argv[1][2]));

	printf("<html><head>\n");
	printf("<title>%s</title></head>\n", year );
	printf("<body bgcolor=#ffffff>\n");
	printf("<h1>%s ", year);
	print_awardname(type);
	printf("</h1><pre>\n");

	get_award_info(year, title_list, type );

	print_awards(type, year);
	printf("\n");
	postamble("AwYear", argv[1]);
	printf("</pre>\n");
	exit(0);
}
