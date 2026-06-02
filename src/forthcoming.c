/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)forthcoming.c	1.10	01/28/98 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "sfdbase.h"

char	outbuffer[8192];
char	outbuf2[8192];

char	filter_author[256]	= {0};
char	filter_title[256]	= {0};

search_t	*title_list  = NULL;
search_t	*title_end   = NULL;
extern pub_t	*pub_list;

char *Month[] = {
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec",
};


void
add_title(char *title, char *author, char *year, char *type, char *pubs)
{
	search_t	*tmp;
	search_t	*p1;
	search_t	*p2;

	tmp = (search_t *)malloc( sizeof(search_t) );
	if ( tmp == NULL ) {
		perror("out of memory");
		exit(1);
	}

	tmp->se_title  = (char *)malloc( strlen(title) + 1);
	tmp->se_author = (char *)malloc( strlen(author) + 1);
	tmp->se_year   = (char *)malloc( strlen(year) + 1);
	tmp->se_type   = (char *)malloc( strlen(type) + 1);
	tmp->se_next   = NULL;

	strcpy(tmp->se_title,  title);
	strcpy(tmp->se_author, author);
	strcpy(tmp->se_year,   year);
	strcpy(tmp->se_type,   type);

	if (pubs && pubs[0]) {
		tmp->se_pubs = (char *)malloc( strlen(pubs) + 1);
		strcpy(tmp->se_pubs, pubs);
	} else {
		tmp->se_pubs = NULL;
	}

	if ( title_list == NULL) {
		title_list = title_end = tmp;
	} else {
		title_end->se_next = tmp;
		title_end = tmp;
	}
}


void
load_titles(char *filename, int year)
{
        FILE    *fp;
        int     dummy;
	char	syear[8];

	sprintf(syear, "20%d", year);

        fp = fopen(filename, "rb");
        if (fp == NULL) {
                perror("Couldn't open shortfiction.dbase");
                exit(1);
        }

        while(1) {
		search_t *set;

		set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
		if ( set == NULL) {
                        goto finish;
                }
                if ((strncmp(set->se_year, "9999", 4) == 0) ||
                    ( strcmp(set->se_year, syear) == 0)) {
			add_title(set->se_title, set->se_author, set->se_year, 
				set->se_type, set->se_pubs);
                } 
        }

finish:
        fclose(fp);
}


/*
 * This specialized search_titles function only loads in
 * forthcoming titles.
 */
search_t *
search_titles(char *tag)
{
	search_t	*tmp;

	tmp = title_list;
	while(tmp) {
		if ( tmp->se_pubs && strstr(tmp->se_pubs, tag) ) {
			if ( strncmp(tmp->se_title, "Cover;", 6) == 0) {
				tmp = tmp->se_next;
				continue;
			} else {
				return(tmp);
			}
		} 
		tmp = tmp->se_next;
	}

	return(NULL);
}


int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int		loop;
	char		path[256];
	pub_t		*tmp;
	search_t	*tmp2;
	int		year;
	int		month;
	time_t		ltime;
	struct tm	*ltm;
	int		needcomma;
	char		*location;
	char		matcher[16];
	int		needheader = 1;
        int             result;

	
	printf("Content-type: text/html\n\n");

#ifdef CHDIR
        result = chdir(CGIBIN);
        if (result != 0) {
                printf("CHDIR to %s failed\n", CGIBIN);
                exit(1);
        }
#endif

	printf("<html><head>\n");
	printf("<title>Forthcoming Books</title></head>\n");
	printf("<body bgcolor=ffffff>\n");
	printf("<h1>Forthcoming Books</h1>");
	printf("<hr>\n");

	time( &ltime );
	ltm = localtime( &ltime );
	month = ltm->tm_mon;

	year  = ltm->tm_year;
	if (year >= 100) {
		year -= 100;
	}

	load_pubs(F_FORTH, "9999");
	load_titles("titles.dbase", year);

	tmp = pub_list;
	if (tmp == NULL) {
		printf("<p>Couldn't find any forthcoming books\n");
		goto finish;
	}

	printf("Forthcoming books are derived from a number of sources including\n");
	printf("the <i>amazon.com</i> and <i>Barnes & Noble</i> web sites, online\n");
	printf("newsletters, web pages supported by the various publishing houses,\n");
	printf("and print advertisements.<p>\n");
	printf("<i>Note:</i> books will remain on the forthcoming list, even\n");
	printf("after a book has appeared in bookstores, until the month is\n");
	printf("retired. This allows the earliest month listing to double\n");
	printf("as a \"recently appeared in bookstores\" listing (it's also\n");
	printf("easier to change a month's worth of dates all at once).<p>\n");
	printf("The following abbreviations are used on this page:\n");
	printf("<pre>\n");
	printf("     n  = Novel                    hc = Hardcover\n");
	printf("     c  = Collection               tp = Trade Paperback\n");
	printf("     a  = Anthology                pb = Paperback\n");
	printf("     nf = Non-Fiction              ph = Pamphlet\n");
	printf("     sf = Short Fiction\n");

	for(loop=0; loop<12; loop++) {

		needheader = 1;
		sprintf(matcher, "9999%s%02d",  Month[month], year);

		tmp = pub_list;
		while(tmp) {

			/*
			 * Look at every forthcoming entry in the 
			 * database, searching for a date match.
			 */
			if ( strncmp(matcher, tmp->pu_year, 9) == 0) {

				/*
				 * If this is the first entry for the new
				 * month, output the month header.
				 */
				if (needheader) {
					needheader = 0;
					printf("\n<b>%s %02d:</b>\n", Month[month], year);
					printf("<hr>\n");
				}

				tmp2 = search_titles(tmp->pu_abbreviation);
				if (tmp2) {
					needcomma = 0;
					printf("<i>%s</i>\n", tmp->pu_title);
					printf(" ");

					sprintf(outbuffer, "[");
					if (tmp2->se_type) {
						if (needcomma) {
							sprintf(outbuf2, ", %s", tmp2->se_type);
						} else {
							sprintf(outbuf2, "%s", tmp2->se_type);
							needcomma = 1;
						}
						strcat(outbuffer, outbuf2);
					}
					if (tmp->pu_publisher[0]) {
						if (needcomma) {
							sprintf(outbuf2, ", %s", tmp->pu_publisher);
						} else {
							sprintf(outbuf2, "%s", tmp->pu_publisher);
							needcomma = 1;
						}
						strcat(outbuffer, outbuf2);
					}
					if (tmp->pu_type[0]) {
						if (needcomma) {
							sprintf(outbuf2, ", %s", tmp->pu_type);
						} else {
							sprintf(outbuf2, "%s", tmp->pu_type);
							needcomma = 1;
						}
						strcat(outbuffer, outbuf2);
					}

					location = (char *)strstr(tmp->pu_year, " ");
					if (location) {
						location++;
						if (location[0]) {
							if (needcomma) {
								sprintf(outbuf2, ", %s", location);
							} else {
								sprintf(outbuf2, "%s", location);
								needcomma = 1;
							}
						}
						strcat(outbuffer, outbuf2);
					}
					sprintf(outbuf2, "]");
					strcat(outbuffer, outbuf2);
					print_title(1, outbuffer, RIGHTMARGIN, T_PREINDENT, NULL);

					/*
					 * Output the authors
					 */
					html_print_authors(tmp2->se_author, RIGHTMARGIN);
				}
			}
			tmp = tmp->pu_next;
		}

		/*
		 * Don't forget to wrap around the end of the
		 * year, if needed.
		 */
		month++;
		if (month == 12) {
			month = 0;
			year++;
		}
	}

	needheader = 1;
	sprintf(matcher, "9999");
	tmp = pub_list;
	while(tmp) {

		/*
		 * Look at every forthcoming entry in the 
		 * database, searching for a date match.
		 */
		if ( strncmp(matcher, tmp->pu_year, 9) == 0) {

			/*
			 * If this is the first entry for the new
			 * month, output the month header.
			 */
			if (needheader) {
				needheader = 0;
				printf("\n<b>Apocrypha</b>\n");
				printf("<hr>\n");
			}

			tmp2 = search_titles(tmp->pu_abbreviation);
			if (tmp2) {
				needcomma = 0;
				printf("<i>%s</i>\n", tmp->pu_title);
				printf(" ");

				sprintf(outbuffer, "[");
				if (tmp2->se_type) {
					if (needcomma) {
						sprintf(outbuf2, ", %s", tmp2->se_type);
					} else {
						sprintf(outbuf2, "%s", tmp2->se_type);
						needcomma = 1;
					}
					strcat(outbuffer, outbuf2);
				}
				if (tmp->pu_publisher[0]) {
					if (needcomma) {
						sprintf(outbuf2, ", %s", tmp->pu_publisher);
					} else {
						sprintf(outbuf2, "%s", tmp->pu_publisher);
						needcomma = 1;
					}
					strcat(outbuffer, outbuf2);
				}
				if (tmp->pu_type[0]) {
					if (needcomma) {
						sprintf(outbuf2, ", %s", tmp->pu_type);
					} else {
						sprintf(outbuf2, "%s", tmp->pu_type);
						needcomma = 1;
					}
					strcat(outbuffer, outbuf2);
				}

				location = (char *)strstr(tmp->pu_year, " ");
				if (location) {
					location++;
					if (location[0]) {
						if (needcomma) {
							sprintf(outbuf2, ", %s", location);
						} else {
							sprintf(outbuf2, "%s", location);
							needcomma = 1;
						}
					}
					strcat(outbuffer, outbuf2);
				}
				sprintf(outbuf2, "]");
				strcat(outbuffer, outbuf2);
				print_title(1, outbuffer, RIGHTMARGIN, T_PREINDENT, NULL);

				/*
				 * Output the authors
				 */
				html_print_authors(tmp2->se_author, RIGHTMARGIN);
			}
		}
		tmp = tmp->pu_next;
	}

finish:
	postamble("forthcoming", "XX");
	printf("</pre>\n");
}
