/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)AwGen.c	1.13	01/05/00 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sfdbase.h"

static entry		entries[MAX_ENTRIES];
search_t	*title_list = NULL;

extern void get_award_info(char *exact_author, search_t *title_list, char *awardname);
extern void print_awards(char *type, char *year);


int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int	result;
	char	path[256];
	char	award[32];
	char	type[8];
	char	year[16];
	int	content_length;
	int	loop;
	int	max;
	char	*startptr;
	char	*endptr;
	char	*ptr;

	printf("Content-type: text/html\n\n");

#ifdef POST_METHOD
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
		if (strncmp(entries[loop].name, "year", 4) == 0) {
			if (entries[loop].val[0]) {
				strcpy(year, entries[loop].val);
			} else {
				year[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "award", 6) == 0) {
			if (entries[loop].val[0]) {
				strcpy(award, entries[loop].val);
			} else {
				award[0] = 0;
			}
		} else {
		}
	}
#else
	if (argc != 2) {
		/* Error */
	}

        /*
         * Replace any '+' characters with a space
         */
        ptr = argv[1];
        while( *ptr ) {
                if (*ptr == '+') {
                        *ptr = ' ';
                } else if (strncmp(ptr, "%27", 3) == 0) {
                        char *ptr2;
                        char *ptr3;

                        ptr2 = ptr;
                        *ptr2 = '\'';
                        ptr2++;
                        ptr3 = ptr+3;
                        while(*ptr3) {
                                *ptr2 = *ptr3;
                                ptr2++;
                                ptr3++;
                        }
                } else if (strncmp(ptr, "%20", 3) == 0) {
                        char *ptr2;
                        char *ptr3;

                        ptr2 = ptr;
                        *ptr2 = ' ';
                        ptr2++;
                        ptr3 = ptr+3;
                        while(*ptr3) {
                                *ptr2 = *ptr3;
                                ptr2++;
                                ptr3++;
                        }
                }
                ptr++;
        }

	startptr = (char *)strstr(argv[1], "year=");
	if (startptr) {
		startptr += 5;
		endptr = (char *)strstr(startptr, "&");
		if (endptr) {
			*endptr = 0;
			strcpy(year, startptr);
			startptr = endptr + 1;
		}

	} else {
		/* Error */
	}

	startptr = (char *)strstr(startptr, "award=");
	if (startptr) {
		startptr += 6;
		strcpy(award, startptr);
	} else {
		/* Error */
	}

#endif

	if ( strncmp(award, "Hugo", 4) == 0) {
		strcpy(type, "Hu");
	} else if ( strncmp(award, "Nebula ", 6) == 0) {
		strcpy(type, "Ne");
	} else if ( strncmp(award, "WFA", 3) == 0) {
		strcpy(type, "Wf");
	} else if ( strncmp(award, "Campbell", 8) == 0) {
		strcpy(type, "Ca");
	} else if ( strncmp(award, "Chesley", 7) == 0) {
		strcpy(type, "Cy");
	} else if ( strncmp(award, "Analog", 6) == 0) {
		strcpy(type, "An");
	} else if ( strncmp(award, "Clarke", 6) == 0) {
		strcpy(type, "Cl");
	} else if ( strncmp(award, "Locus Poll", 10) == 0) {
		strcpy(type, "Lc");
	} else if ( strncmp(award, "Mythopoeic", 10) == 0) {
		strcpy(type, "My");
	} else if ( strncmp(award, "PK Dick", 7) == 0) {
		strcpy(type, "Pk");
	} else if ( strncmp(award, "Apollo", 6) == 0) {
		strcpy(type, "Ap");
	} else if ( strncmp(award, "Stoker", 6) == 0) {
		strcpy(type, "St");
	} else if ( strncmp(award, "Aurora", 6) == 0) {
		strcpy(type, "Au");
	} else if ( strncmp(award, "Sturgeon", 8) == 0) {
		strcpy(type, "Su");
	} else if ( strncmp(award, "Tiptree", 7) == 0) {
		strcpy(type, "Tp");
	} else if ( strncmp(award, "Lambda", 6) == 0) {
		strcpy(type, "Lm");
	} else if ( strncmp(award, "HOMer", 5) == 0) {
		strcpy(type, "Hm");
	} else if ( strncmp(award, "Hall of Fame", 12) == 0) {
		strcpy(type, "Hm");
	} else if ( strncmp(award, "JWCA", 4) == 0) {
		strcpy(type, "Jc");
	} else if ( strncmp(award, "Prometheus", 10) == 0) {
		strcpy(type, "Pr");
	} else if ( strncmp(award, "RetroHugo", 9) == 0) {
		strcpy(type, "Rh");
	} else if ( strncmp(award, "SFBC", 4) == 0) {
		strcpy(type, "Sf");
	} else if ( strncmp(award, "SF Chronicle", 12) == 0) {
		strcpy(type, "Sc");
	} else if ( strncmp(award, "BFA", 3) == 0) {
		strcpy(type, "Bf");
	} else if ( strncmp(award, "Ditmar", 6) == 0) {
		strcpy(type, "Dt");
	} else if ( strncmp(award, "Aurealis", 8) == 0) {
		strcpy(type, "As");
	} else if ( strncmp(award, "AsimovR", 7) == 0) {
		strcpy(type, "Ar");
	} else if ( strncmp(award, "AsimovU", 7) == 0) {
		strcpy(type, "Ax");
	} else if ( strncmp(award, "Gandalf", 7) == 0) {
		strcpy(type, "Ga");
	} else if ( strncmp(award, "BSFA", 4) == 0) {
		strcpy(type, "Bs");
	} else if ( strncmp(award, "Balrog", 6) == 0) {
		strcpy(type, "Bl");
	} else if ( strncmp(award, "IFA", 4) == 0) {
		strcpy(type, "If");
	} else if ( strncmp(award, "Sidewise", 8) == 0) {
		strcpy(type, "Sw");
	} else if ( strncmp(award, "ComptonCrook", 12) == 0) {
		strcpy(type, "Cc");
	} else if ( strncmp(award, "Skylark", 7) == 0) {
		strcpy(type, "Sk");
	} else if ( strncmp(award, "Gaughan", 7) == 0) {
		strcpy(type, "Gg");
	} else if ( strncmp(award, "Imaginaire", 7) == 0) {
		strcpy(type, "Im");
	} else if ( strncmp(award, "IHG", 3) == 0) {
		strcpy(type, "Ih");
	} else if ( strncmp(award, "Deathrealm", 10) == 0) {
		strcpy(type, "Dr");
	} else if ( strncmp(award, "Rhysling", 8) == 0) {
		strcpy(type, "Ry");
	}

#ifdef CHDIR
        result = chdir(CGIBIN);
        if (result != 0) {
                printf("CHDIR to %s failed\n", CGIBIN);
                exit(1);
        }
#endif

	printf("<html><head>\n");
	printf("<title>%s</title></head>\n", year );
	printf("<body bgcolor=#ffffff>\n");
	printf("<h1>%s ", year);
	print_awardname(type);
	printf("</h1><pre>\n");

	get_award_info(year, title_list, type );

	print_awards(type, year);
	printf("\n");
	postamble("AwGen", "XXX");
	printf("</pre>\n");
	return(0);
}
