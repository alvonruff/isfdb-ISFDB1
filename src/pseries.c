/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)pseries.c	1.5	01/28/98 SFdbase";

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

static char	tmpseries[MEDIUMSIZE];
static char	tmpseries2[MEDIUMSIZE];
static char	exact_series[MEDIUMSIZE] = {0};
static char	filter_series[MEDIUMSIZE];
static char	tmpbuf[BIGSIZE];
search_t        *title_list  = NULL;
search_t        *title_end  = NULL;


void
add_title(search_t *tmp, char *offset)
{
	char		*ptr;

	tmp->se_offset = (char *)malloc( strlen(offset) +1);
	strcpy(tmp->se_offset, offset);

	if ( title_list == NULL) {
		title_list = title_end = tmp;
	} else {
		title_end->se_next = tmp;
		title_end = tmp;
	}
}


void
parse_series(char *filename)
{
	FILE	*fp;
	FILE	*fp2;
	int	line_number = 1;
	int	index;
	char	input;
	char	*series;
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
		if ( parse_field(fp, tmpseries, (int *)&dummy, MEDIUMSIZE) == -1 ) {
			goto finish;
		}

		/*
		 * Reduce the series to lower case for a caseless match
		 */
		index = 0;
		while( tmpseries[index] != 0 ) {
			tmpseries2[index] = tolower( tmpseries[index] );
			index++;
		}
		tmpseries2[index] = 0;

		series = tmpseries2;
		if ( strcmp(series, filter_series) == 0 ) {

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
				if (set == NULL) {
					goto finish;
				}
				add_title(set, offset);
				if ( eol ) {
					break;
				}
			}
			strcpy(exact_series, tmpseries);
			goto finish;

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
	int		rightmargin;
	int		indent;
	search_t	*tmp;
	audata_t	*auth;
	char		path[256];

	printf("Content-type: text/html\n\n");

	if (argc != 2) {
		printf("Bad series input\n");	
		exit(1);
	}

	/*
	 * Convert to lower case
	 */
	index = 0;
	while( argv[1][index] != 0 ) {
		filter_series[index] = tolower( argv[1][index] );
		if (filter_series[index] == '_')
			filter_series[index] = ' ';
		index++;
	}

	/*
	 * Remove backquotes
	 */
	while ( strstr(filter_series, "\\") ) {
		char *ptr;

		ptr = (char *)strstr(filter_series, "\\");
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
	parse_series("titles.xbs");

	/*
	 * Prep the list for printing. 
	 */
	sort_by_year( &title_list );

	printf("<html><head>\n");
	printf("<title>%s - Series Listing</title></head>\n", exact_series);
	printf("<body bgcolor=#ffffff>\n");
	printf("<h1>%s - Series Listing</h1><hr>\n", exact_series);
	printf("<pre>");

	tmp = title_list;
	rightmargin = RIGHTMARGIN2;
	indent = 4;
	while(tmp) {
		int position, length;

		if (tmp->se_seriesnum) {
			length = strlen(tmp->se_seriesnum);
			while(length != 3) {
				printf(" ");
				length++;
			}
			printf("%s - ", tmp->se_seriesnum);
		} else {
			printf("      ");
		}

		position = print_title(0, tmp->se_title, RIGHTMARGIN2, T_NOPAD, NULL);
		if ( strcmp(tmp->se_year, "0") == 0) {
			sprintf(tmpbuf, " (<b>unknown</b>)");
		} else if ( strncmp(tmp->se_year, "9999", 4) == 0) {
			if ( strlen(tmp->se_year) > 4) {
				sprintf(tmpbuf, " (<b>forthcoming %s</b>)", (char *)&(tmp->se_year[4]));
			} else {
				sprintf(tmpbuf, " (<b>forthcoming</b>)");
			}
		} else if ( strncmp(tmp->se_year, "8888", 4) == 0) {
			sprintf(tmpbuf, " (<b>unpublished</b>)");
		} else {
			sprintf(tmpbuf, " (<b>%s</b>)", tmp->se_year);
		}
		position = printwrap(tmpbuf, position, rightmargin, indent);
		position = posit_check(position, rightmargin, indent);

		if ( strcmp(tmp->se_type, "sf") == 0) {
			printf(" [<b>SF</b>]");
			position += 5;
		} else if ( strcmp(tmp->se_type, "c") == 0) {
			printf(" [<b>C</b>]");
			position += 4;
		}
		position = posit_check(position, rightmargin, indent);

		if ( strcmp(tmp->se_type, "n") == 0) {
			if ( tmp->se_storylen && (strcmp(tmp->se_storylen, "nvz") == 0) ) {
				printf(" [<b>nvz</b>]");
				position += 6;
			} else if ( tmp->se_storylen && (strcmp(tmp->se_storylen, "jvn") == 0) ) {
				printf(" [<b>jvn</b>]");
				position += 6;
			}
		}
		printf("  ");
		position += 2;
		position = posit_check(position, rightmargin, indent);

		html_print_all_authors(tmp->se_author, indent, position, rightmargin);
		printf("\n");
		tmp = tmp->se_next;
	}

	postamble("pseries", argv[1]);

	printf("</pre>\n");
}
