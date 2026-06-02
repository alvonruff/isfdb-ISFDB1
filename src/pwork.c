/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)pwork.c	1.10	01/28/98 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef SUNOS
#include <sys/unistd.h>
#else
#include <unistd.h>
#endif
#include "sfdbase.h"

#define OPTARGS "t:a:y:?"

static char	tmptitle[256];
static char	tmpauthor[256];
static char	tmpauthor2[256];
static char	exact_author[256] = {0};
static char	tmpyear[256];
static char	tmptitle2[256];
static char	tmpseries[256];
static char	tmpsuperseries[256];
static char	tmppub[256];
static char	tmpyear2[256];
static char	tmptype[256];
static char	tmpstorylen[256];
static char	tmpnotes[16];
static char	tmpsynopsis[16];
static char	tmpseriesnum[16];
static char	filter_author[256];
search_t        *title_list  = NULL;

extern pub_t	*pub_list;

void
clear_markers(search_t *list)
{
	search_t        *tmp;
	award_t		*awtmp;

	tmp = list;
	while( tmp ) {
		tmp->se_marker = 0;
		if (tmp->se_awards) {
			awtmp = tmp->se_awards;
			while (awtmp) {
				awtmp->aw_marker = 0;
				awtmp = awtmp->aw_next;
			}
		}

		tmp = tmp->se_next;
	}
}


void
add_title(search_t *tmp)
{
	char		*ptr;

	tmp->se_next = title_list;
	title_list = tmp;
}

void
parse_work(char *offset)
{
	FILE	*fp;
	int	dummy = 0;
	int	int_offset;
	search_t *set;

	fp = fopen("titles.dbase", "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}


	sscanf(offset, "%x", &int_offset);
	fseek(fp, int_offset, SEEK_SET);
	set = parse_title_entry(fp, (int *)&dummy, PARSE_MALLOC);
	if (set == NULL) {
		fclose(fp);
		return;
	}

	fclose(fp);
	add_title(set);
}



int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int		option;
	int		index;
	int		result;
	search_t	*tmp;
	audata_t	*auth;
	char		path[256];

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
	parse_work( argv[1] );

	/*
	 * Prep the list for printing. First sort it by
	 * year, and then clear out the print markers.
	 */
	clear_markers(title_list);
	load_pubs(F_PLIST, title_list->se_pubs );
	if ( (char *)strstr(title_list->se_pubs, ",") == NULL ) {
#ifdef CHDIR
                result = chdir(CGIBIN);
                if (result != 0) {
                        printf("CHDIR to %s failed\n", CGIBIN);
                        exit(1);
                }
#endif
		execl("./plist.cgi", "plist.cgi", title_list->se_pubs, 0);
	}

	printf("<html><head>\n");
	printf("<title>Publication History</title></head>\n");
	printf("<body bgcolor=#ffffff>\n");

	printf("<h1>Publication History</h1><hr>\n");
	print_menus2( argv[1] );
	printf("<hr>\n<pre>\n");

	if ( strchr(title_list->se_title, '^') ) {
		printf("<b>For:</b>\n");
		print_title(4, title_list->se_title, RIGHTMARGIN2, T_NOPAD, NULL);
		printf("\n");
	} else {
		printf("<b>For:</b> <i>%s</i>\n", title_list->se_title);
	}
	printf("</pre>\n");
	print_pub_citations( title_list->se_pubs, 0 );
	printf("<pre>\n");
	postamble("pwork", argv[1]);

	printf("</pre>\n");
}
