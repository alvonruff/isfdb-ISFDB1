/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *	 ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)exact_author.c	1.16	01/28/98 SFdbase";

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

extern void get_award_info(char *exact_author, search_t *title_list);

#define OPTARGS "t:a:y:?"

static char	tmpauthor[MEDIUMSIZE];
static char	tmpauthor2[MEDIUMSIZE];
static char	exact_author[MEDIUMSIZE] = {0};
static char	fauthor[MEDIUMSIZE] = {0};
static char	filter_author[MEDIUMSIZE];
search_t	*title_list  = NULL;
search_t	*title_end  = NULL;

extern audata_t *get_author(char *);
extern review_t *rv_list;
extern inter_t  *it_list;

void
clear_markers(search_t *list)
{
	search_t	*tmp;
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

	if ( title_list == NULL) {
		title_list = title_end = tmp;
	} else {
		title_end->se_next = tmp;
		title_end = tmp;
	}
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
	char	*ptr;

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
			if (tmpauthor[index] & 0x80) {
				tmpauthor2[index] = tmpauthor[index];
			} else {
				tmpauthor2[index] = tolower( tmpauthor[index] );
			}
			index++;
		}
		tmpauthor2[index] = 0;

		strcpy(fauthor, tmpauthor2);
		if ( strstr(fauthor, "^") ) {
			ptr = (char *)strstr(fauthor, "^");
			*ptr = 0;
		}
		
		author = tmpauthor2;
		if ( strcmp(fauthor, filter_author) == 0) {

			if ( exact_author[0] == 0) {
				if ( strcmp(author, filter_author) == 0) {
					strcpy(exact_author, tmpauthor);
				} else if ( strstr(tmpauthor, "^") ) {
					ptr = (char *)strstr(tmpauthor, "^");
					*ptr = 0;
					strcpy(exact_author, tmpauthor);
					*ptr = '^';
				}
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
				if (set == NULL) {
					goto finish;
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
	review_t	*rtmp;

	printf("Content-type: text/html\n\n");
	if (argc != 2) {
		printf("Bad author input\n");	
		exit(1);
	}

	unescape_url(argv[1]);

	/*
	 * Convert to lower case
	 */
	index = 0;
	while( argv[1][index] != 0 ) {
		if (argv[1][index] & 0x80) {
			filter_author[index] = argv[1][index];
		} else {
			filter_author[index] = tolower( argv[1][index] );
			if (filter_author[index] == '_')
				filter_author[index] = ' ';
		}
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

	get_award_info(exact_author, title_list);

	/*
	 * Prep the list for printing. First sort it by
	 * year, and then clear out the print markers.
	 */
	sort_by_year( &title_list );
	clear_markers(title_list);

	if ( exact_author[0] == 0) {
		strcpy(exact_author, filter_author);
	}

	printf("<html><head>\n");
	printf("<title>%s - Bibliography Summary</title></head>\n", exact_author);
	printf("<body bgcolor=#ffffff>\n");
	printf("<h1>%s - Bibliography Summary</h1><hr>\n", exact_author);
	print_menus( argv[1], title_list, 1);
	printf("<hr>\n");
	printf("<pre>");

	if ( title_list == NULL ) {
		printf("<b>No data found.</b>\n");
		postamble("exactauth (error)", argv[1]);
		printf("</pre>\n");
		exit(0);
	}

	load_interviews( F_TAUTHOR, exact_author);
	auth = get_author(exact_author);
	if (auth) {
		printf("<h2>%s     (%s, %s-%s)</h2>\n",
			auth->au_legalname,
			auth->au_birthplace,
			auth->au_birthdate,
			auth->au_deathdate);
		if ( auth->au_pseudonyms[0] ) {
			printf("</pre>");
			printf("(Pseudonyms: <b>%s</b>)\n", auth->au_pseudonyms );
			printf("<pre>");
		}
		print_preamble( title_list );

		if (it_list || auth->au_biog[0]) {
			printf("</pre>");
		}

		if (it_list) {
			inter_t *itmp;
			int     didone=0;

			printf("<b>Interviews:</b> ");
			itmp = it_list;
			while(itmp) {
				if (didone)
					printf(", ");
				printf("<a href=\"http:/%s/plist.cgi?%s\">", HTFAKE, itmp->it_abbreviation);
				printf("%s", itmp->it_title);
				printf("</a> (%s)", itmp->it_year);
				didone = 1;
				itmp = itmp->it_next;
			}
			printf(".<br>");
		}
		if(auth->au_email[0]) {
			char *link, *comma;

			link = auth->au_email;
			while(1) {
				comma = (char *)strstr(link, ",");
				if (comma) {
					*comma = 0;
					printf("<b>Email:</b> ");
					printf("<a href=\"mailto:%s\">%s</a><br>", link, link);
					link = ++comma;
				} else {
					printf("<b>Email:</b> ");
					printf("<a href=\"mailto:%s\">%s</a><br>", link, link);
					break;
				}
			}
		}
		if(auth->au_webpage[0]) {
			char *link, *comma;

			link = auth->au_webpage;
			while(1) {
				comma = (char *)strstr(link, ",");
				if (comma) {
					*comma = 0;
					printf("<b>Web Page:</b> ");
					printf("<a href=\"%s\">%s</a><br>", link, link);
					link = ++comma;
				} else {
					printf("<b>Web Page:</b> ");
					printf("<a href=\"%s\">%s</a><br>", link, link);
					break;
				}
			}
		}
		if(auth->au_biog[0]) {
			printf("<b>Biographic Data:</b> ");
			read_and_printwrap(USE_BIOG, auth->au_biog, 0, RIGHTMARGIN2, 0);
		}
		if (it_list || auth->au_biog[0]) {
			printf("<pre>");
		}
	} else {
		print_preamble( title_list );
		if (it_list) {
			inter_t *itmp;
			int     didone=0;

			printf("</pre>");
			printf("<b>Interviews:</b> ");
			itmp = it_list;
			while(itmp) {
				if (didone)
					printf(", ");
				printf("<a href=\"http:/%s/plist.cgi?%s\">", HTFAKE, itmp->it_abbreviation);
				printf("%s", itmp->it_title);
				printf("</a> (%s)", itmp->it_year);
				didone = 1;
				itmp = itmp->it_next;
			}
			printf(".<pre>");
		}
	}
	
	load_reviews( F_TAUTHOR, exact_author);

	print_series(  title_list, exact_author, 0, 0 );
	print_entries( title_list, exact_author, "n",  "Novels", 0 );
	print_entries( title_list, exact_author, "c",  "Collections", 0 );
	print_entries( title_list, exact_author, "o",  "Omnibus", 0 );
	print_series(  title_list, exact_author, 0, 1 );
	print_entries( title_list, exact_author, "a",  "Anthologies", 0 );
	print_series(  title_list, exact_author, 0, 2 );
	print_entries( title_list, exact_author, "e",  "Magazine Editor", 0 );
	print_entries( title_list, exact_author, "nf", "Non-Fiction", 0 );
	print_entries( title_list, exact_author, "sf", "Short Fiction", 0 );
	print_entries( title_list, exact_author, "p",  "Poems", 0 );
	print_entries( title_list, exact_author, "ng", "Non-Genre", 0 );
	print_entries( title_list, exact_author, "ar", "Cover Artwork", 0 );
	print_entries( title_list, exact_author, "ai", "Interior Artwork", 0 );
	print_entries( title_list, exact_author, "se", "Serials", 0 );
	print_series(  title_list, exact_author, 0, 3 );
	print_entries( title_list, exact_author, "es", "Essays/Articles", 0 );
	print_entries( title_list, exact_author, "r",  "Reviews", 0 );
	print_entries( title_list, exact_author, "i",  "Interviews", 0 );

	postamble("exactauth", argv[1]);

	printf("</pre>\n");
}
