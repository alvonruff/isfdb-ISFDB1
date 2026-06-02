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
#include <string.h>
#include <unistd.h>
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
	char	award[32];
	char	type[32];
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

	if ( strncmp(award, "Hugo", 4) == 0) {
		strcpy(type, "HUGOS");
	} else if ( strncmp(award, "Nebula ", 6) == 0) {
		strcpy(type, "NEBULAS");
	} else if ( strncmp(award, "WFA", 3) == 0) {
		strcpy(type, "WFA");
	} else if ( strncmp(award, "Campbell", 8) == 0) {
		strcpy(type, "CAMPBELL");
	} else if ( strncmp(award, "Chesley", 7) == 0) {
		strcpy(type, "CHESLEY");
	} else if ( strncmp(award, "Analog", 6) == 0) {
		strcpy(type, "ANALOGAW");
	} else if ( strncmp(award, "Clarke", 6) == 0) {
		strcpy(type, "CLARKE");
	} else if ( strncmp(award, "Locus Poll", 10) == 0) {
		strcpy(type, "LOCUS");
	} else if ( strncmp(award, "Mythopoeic", 10) == 0) {
		strcpy(type, "MYTHO");
	} else if ( strncmp(award, "PK Dick", 7) == 0) {
		strcpy(type, "PKDICK");
	} else if ( strncmp(award, "Apollo", 6) == 0) {
		strcpy(type, "APOLLO");
	} else if ( strncmp(award, "Stoker", 6) == 0) {
		strcpy(type, "STOKER");
	} else if ( strncmp(award, "Aurora", 6) == 0) {
		strcpy(type, "AURORA");
	} else if ( strncmp(award, "Sturgeon", 8) == 0) {
		strcpy(type, "STURGEON");
	} else if ( strncmp(award, "Tiptree", 7) == 0) {
		strcpy(type, "TIPTREE");
	} else if ( strncmp(award, "Lambda", 6) == 0) {
		strcpy(type, "LAMBDA");
	} else if ( strncmp(award, "HOMer", 5) == 0) {
		strcpy(type, "HOMER");
	} else if ( strncmp(award, "Hall of Fame", 12) == 0) {
		strcpy(type, "BADFILE");
	} else if ( strncmp(award, "JWCA", 4) == 0) {
		strcpy(type, "JWCA");
	} else if ( strncmp(award, "Prometheus", 10) == 0) {
		strcpy(type, "PROMETHEUS");
	} else if ( strncmp(award, "RetroHugo", 9) == 0) {
		strcpy(type, "RETROH");
	} else if ( strncmp(award, "SFBC", 4) == 0) {
		strcpy(type, "SFBC");
	} else if ( strncmp(award, "SF Chronicle", 12) == 0) {
		strcpy(type, "CHRON");
	} else if ( strncmp(award, "BFA", 3) == 0) {
		strcpy(type, "BFA");
	} else if ( strncmp(award, "Ditmar", 6) == 0) {
		strcpy(type, "DITMAR");
	} else if ( strncmp(award, "Aurealis", 8) == 0) {
		strcpy(type, "AUREALIS");
	} else if ( strncmp(award, "AsimovR", 7) == 0) {
		strcpy(type, "ASIMOVR");
	} else if ( strncmp(award, "AsimovU", 7) == 0) {
		strcpy(type, "ASIMOVU");
	} else if ( strncmp(award, "Gandalf", 7) == 0) {
		strcpy(type, "GANDALF");
	} else if ( strncmp(award, "BSFA", 4) == 0) {
		strcpy(type, "BSFA");
	} else if ( strncmp(award, "Balrog", 6) == 0) {
		strcpy(type, "BALROG");
	} else if ( strncmp(award, "IFA", 4) == 0) {
		strcpy(type, "IFA");
	} else if ( strncmp(award, "Sidewise", 8) == 0) {
		strcpy(type, "SIDEWISE");
	} else if ( strncmp(award, "ComptonCrook", 12) == 0) {
		strcpy(type, "COMPTON");
	} else if ( strncmp(award, "Skylark", 7) == 0) {
		strcpy(type, "SKYLARK");
	} else if ( strncmp(award, "Gaughan", 7) == 0) {
		strcpy(type, "GAUGHAN");
	} else if ( strncmp(award, "Imaginaire", 7) == 0) {
		strcpy(type, "IMAGINAIRE");
	} else if ( strncmp(award, "IHG", 3) == 0) {
		strcpy(type, "IHG");
	} else if ( strncmp(award, "Deathrealm", 10) == 0) {
		strcpy(type, "DEATHR");
	} else if ( strncmp(award, "Rhysling", 8) == 0) {
		strcpy(type, "RHYSLING");
	}

#ifdef CHDIR
        result = chdir(CGIBIN);
        if (result != 0) {
                printf("CHDIR to %s failed\n", CGIBIN);
                exit(1);
        }
#endif

	printf("<html><head>\n");
	printf("<title>Award Checkin: %s </title></head>\n", type );
	printf("<body bgcolor=#ffffff>\n");
	printf("<h1>Submitting [%s] for Checkin</h1>\n", type);
	printf("<hr>\n");
	printf("Select a local file for submission:\n");
	printf("<form\n");
	printf("  ENCTYPE=\"multipart/form-data\"\n");
	printf("  METHOD=\"POST\"\n");
	printf("  ACTION=\"http:/%s/checkin.cgi\">\n", HTFAKE);
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
