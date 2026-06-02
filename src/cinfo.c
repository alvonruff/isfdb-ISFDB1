/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)cinfo.c	1.11	01/05/00 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef SUNOS
#include <sys/unistd.h>
#else
#include <unistd.h>
#endif
#include "sfdbase.h"

#define OPTARGS "t:a:y:?"

static char	tmpauthor[MEDIUMSIZE];
static char	tmpauthor2[MEDIUMSIZE];
static char	exact_author[MEDIUMSIZE] = {0};
static char	filter_author[MEDIUMSIZE];
search_t        *title_list  = NULL;


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
add_title(search_t *tmp, char *offset)
{
	char		*ptr;

	tmp->se_offset = (char *)malloc( strlen(offset) +1);
	strcpy(tmp->se_offset, offset);

	tmp->se_next = title_list;
	title_list = tmp;
}


void
parse_authors(char *filename)
{
	FILE	*fp;
	FILE	*fp2;
	int	line_number = 1;
	int	index;
	char	input;
	char	*author;
	int	dummy = 0;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}
	fp2 = fopen("titles.dbase", "rb");
	if (fp2 == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		if ( parse_field(fp, tmpauthor, (int *)&dummy, MEDIUMSIZE) == -1 ) {
			goto finish;
		}

		/*
		 * Reduce the author to lower case for a caseless match
		 */
		index = 0;
		while( tmpauthor[index] != 0 ) {
			tmpauthor2[index] = tolower( tmpauthor[index] );
			index++;
		}
		tmpauthor2[index] = 0;

		author = tmpauthor2;
		if ( strstr(author, filter_author) &&
		     ((strcmp(author, filter_author) == 0) ||
		       strstr(author, "^") ) ) {

			if ( ( strcmp(author, filter_author) == 0) &&
			     ( exact_author[0] == 0) ) {
				strcpy(exact_author, tmpauthor);
			}

			while(1) {
				char offset[16];
				int eol;
				int input;
				int int_offset;
				search_t *set;

				parse_field_or_eol(fp, offset, &eol, 16);
				sscanf(offset, "%x", &int_offset);
				fseek(fp2, int_offset, SEEK_SET);
				set = parse_title_entry(fp2, (int *)&dummy, PARSE_MALLOC);
				if ( set == NULL ) {
					break;
				}
				add_title(set, offset);
				if ( eol ) {
					break;
				}
			}
		} else if ( parse_to_eol(fp, (int *)&dummy) == -1 ) {
			goto finish;
		}
		line_number++;
	}

finish:
	fclose(fp2);
	fclose(fp);
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
		printf("Bad author input\n");	
		exit(1);
	}

	/*
	 * Convert to lower case
	 */
	index = 0;
	while( argv[1][index] != 0 ) {
		filter_author[index] = tolower( argv[1][index] );
		if (filter_author[index] == '_')
			filter_author[index] = ' ';
		index++;
	}

	/*
	 * Remove backquotes
	 */
	while ( strstr(filter_author, "\\") ) {
		char *ptr;

		ptr = (char *)strstr(filter_author, "\\");
		while( *ptr ) {
			*ptr = *(ptr+1);
			ptr++;
		}
	}

#ifdef CHDIR
        result = chdir(CGIBIN);
        if (result != 0) {
                printf("CHDIR to %s failed\n", CGIBIN);
                exit(1);
        }

#endif
	parse_authors("titles.xba");

	/*
	 * Prep the list for printing. First sort it by
	 * year, and then clear out the print markers.
	 */
	sort_by_year( &title_list );
	clear_markers(title_list);

	printf("<html><head>\n");
	printf("<title>%s Correction Page</title></head>\n", exact_author);
	printf("<body bgcolor=#ffff80>\n");

	printf("<h1>%s Correction Page</h1><hr>\n", exact_author);
	printf("\nTo make a correction to a particular work, select the [c] anchor\n");
	printf("to the left of the errant work.\n");
	printf("<pre>\n");

	print_series(  title_list, exact_author, 1 , 0);
	print_entries( title_list, exact_author, "n",  "Novels", 1 );
	print_entries( title_list, exact_author, "c",  "Collections", 1 );

	print_series(  title_list, exact_author, 1 , 1);
	print_entries( title_list, exact_author, "a",  "Anthologies", 1 );

	print_series(  title_list, exact_author, 1 , 2);
	print_entries( title_list, exact_author, "nf", "Nonfiction", 1 );
	print_entries( title_list, exact_author, "sf", "Short Fiction", 1 );
	print_entries( title_list, exact_author, "p",  "Poems", 1 );
	print_entries( title_list, exact_author, "ng", "Non-Genre", 1 );
	print_entries( title_list, exact_author, "ar", "Cover Artwork", 1 );
	print_entries( title_list, exact_author, "ai", "Interior Artwork", 1 );
	print_entries( title_list, exact_author, "se", "Serials", 1 );

	print_series(  title_list, exact_author, 1 , 3);
	print_entries( title_list, exact_author, "es", "Essays/Articles", 1 );
	postamble("cinfo", argv[1]);

	printf("</pre>\n");
}
