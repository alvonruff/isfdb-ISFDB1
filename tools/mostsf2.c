/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *	 ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 */

static char sccsid[] = "@(#)mostsf.c	1.3	06/10/97 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sfdbase.h"

typedef struct recomp {
	char		*rc_title;
	char		*rc_author;
	char		*rc_year;
	char		*rc_series;
	char		*rc_superseries;
	char		*rc_seriesnum;
	char		*rc_storylen;
	char		*rc_pubtags;
	char		*rc_notes;
	char		*rc_synopsis;
	int		rc_pubs;
	struct recomp   *rc_next;
} recomp_t;

recomp_t *head = NULL;
recomp_t *tail = NULL;
extern int dbaseloc;


int	edited_flag = 0;
char    tmptitle[256];
char    tmpauthor[256];
char    tmpyear[256];
char	tmpseries[256];
char	tmpsuperseries[256];
char	tmppubs[256];
char	tmptype[256];
char	tmpstorylen[256];
char	tmpnotes[16];
char	tmpsynopsis[16];
char	tmpseriesnum[16];


extern object_t	*Objlist;
extern char	*optarg;
extern review_t *rv_list;


void
do_attribute(char *targetattr, attr_t *list, recomp_t *target)
{
	attr_t		*attr;
	char		*ptr;

	attr = list;
	while (attr) {
		if (strncmp(attr->at_name, targetattr, 2) == 0) {
			if ( strcmp(targetattr, "AE") == 0) {
				target->rc_author = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_author, attr->at_value);
			} else if ( strcmp(targetattr, "YR") == 0) {
				target->rc_year = (char *)malloc( strlen(attr->at_value) + 1);
				if (strncmp(attr->at_value, "9999", 4) == 0) {
					ptr = attr->at_value;
					ptr += 4;
					if(*ptr) {
						ptr += 3;
						if (ptr[0] == '9') {
							sprintf(target->rc_year, "199%c", ptr[1]);
						} else {
							sprintf(target->rc_year, "20%c%c", ptr[0], ptr[1]);
						}
					} else {
						strcpy(target->rc_year, attr->at_value);
					}
				} else {
					strcpy(target->rc_year, attr->at_value);
				}
			} else if ( strcmp(targetattr, "SS") == 0) {
				target->rc_superseries = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_superseries, attr->at_value);
			} else if ( strcmp(targetattr, "SE") == 0) {
				target->rc_series = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_series, attr->at_value);
			} else if ( strcmp(targetattr, "SN") == 0) {
				target->rc_seriesnum = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_seriesnum, attr->at_value);
			} else if ( strcmp(targetattr, "SL") == 0) {
				target->rc_storylen = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_storylen, attr->at_value);
			} else if ( strcmp(targetattr, "PB") == 0) {
				target->rc_pubtags = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_pubtags, attr->at_value);
			} else if ( strcmp(targetattr, "NT") == 0) {
				target->rc_notes = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_notes, attr->at_value);
			} else if ( strcmp(targetattr, "SY") == 0) {
				target->rc_synopsis = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_synopsis, attr->at_value);
			}
			break;
		}
		attr = attr->at_next;
	}
}


void
search_file(char *filename)
{
	object_t	*tmp;
	attr_t		*attr;
	recomp_t	*target;
	extern int	line_number;

	line_number = 1;
	parse_pubs(filename);
	tmp = Objlist;
	while(tmp) {
		target = (recomp_t *)malloc( sizeof(recomp_t) );
		target->rc_title = (char *)malloc( strlen(tmp->ob_name) + 1);
		strcpy(target->rc_title, tmp->ob_name);
		target->rc_author = NULL;
		target->rc_year = NULL;
		target->rc_series = NULL;
		target->rc_superseries = NULL;
		target->rc_seriesnum = NULL;
		target->rc_storylen = NULL;
		target->rc_pubtags = NULL;
		target->rc_notes = NULL;
		target->rc_synopsis = NULL;
		target->rc_next = NULL;
		target->rc_pubs = 0;
		if (head) {
			tail->rc_next = target;
			tail = target;
		} else {
			head = tail = target;
		}

		do_attribute("AE", tmp->ob_attrs, target);
		do_attribute("YR", tmp->ob_attrs, target);
		do_attribute("SE", tmp->ob_attrs, target);
		do_attribute("SS", tmp->ob_attrs, target);
		do_attribute("PB", tmp->ob_attrs, target);
		do_attribute("SL", tmp->ob_attrs, target);
		do_attribute("NT", tmp->ob_attrs, target);
		do_attribute("SY", tmp->ob_attrs, target);
		do_attribute("SN", tmp->ob_attrs, target);
		tmp = tmp->ob_next;
	}
	Objlist = NULL;
}


void
do_synopsis(char *note)
{
	char *ptr;
	int  first = 1;
	int  dolast = 1;

	while( strlen(note) > 60) {
		ptr = (char *)&note[60];
		while(*ptr != ' ') {
			if(*ptr == 0) {
				dolast = 0;
				break;
			}
			ptr++;
		}
		*ptr = 0;
		if (first) {
			first = 0;
			printf("     - %s\n", note);
		} else {
			printf("     %s\n", note);
		}
		note = ptr+1;
	}
	if (dolast) {
		printf("     %s\n", note);
	}
}

void
printauthor(char *author)
{
	char *ptr, *ptr2;
	int  serialcomma = 0;

	if (edited_flag) {
		printf("(edited by ");
	} else {
		printf("(by ");
	}
	ptr = strstr(author, "+");
	while(ptr) {
		*ptr = 0;
		ptr++;
		if ( strstr(author, "^")) {
			ptr2 = strstr(author, "^");
			ptr2++;
			printf("%s", ptr2);
		} else {
			printf("%s", author);
		}
		if ( strstr(ptr, "+")) {
			printf(", ");
			serialcomma = 1;
		} else if (serialcomma) {
			printf(", and ");
		} else {
			printf(" and ");
		}
		author = ptr;
		ptr = strstr(author, "+");
	}
	if ( strstr(author, "^")) {
		ptr2 = strstr(author, "^");
		ptr2++;
		printf("%s", ptr2);
	} else {
		printf("%s", author);
	}
	printf(")\n");
}

void
output_result(char *year, int doheading, char *banner, int threshold)
{
	recomp_t	*myhead = NULL;
	recomp_t	*current;
	recomp_t	*target;
	review_t        *rtmp;
	int		max = 0;
	int		loop;

#ifdef DEBUG
	return;
#endif

	/*
	 * Zero out the marker value of all of the reviews.
	 */
	rtmp = rv_list;
	while (rtmp) {
		rtmp->rv_marker = 0;
		rtmp = rtmp->rv_next;
	}

	/*
	 * Move the publication hits to back over to the reviews
	 */
	current = head;
	while(current) {
		if (current->rc_pubs) {
			rtmp = rv_list;
			while (rtmp) {
				if ( (strcmp(current->rc_title, rtmp->rv_title) == 0) &&
				     (strcmp(current->rc_author, rtmp->rv_author) == 0) ) {
					rtmp->rv_marker = current->rc_pubs;
					sprintf(rtmp->rv_revtitle,"%x", current->rc_synopsis);
					break;
				}
				rtmp = rtmp->rv_next;
			}
		}
		current = current->rc_next;
	}

	printf("\n\n<b>%s:</b>\n", banner);
	rtmp = rv_list;
	while(rtmp) {
		if (rtmp->rv_marker > max) {
			max = rtmp->rv_marker;
		}
		rtmp = rtmp->rv_next;
	}

	for(loop=max; loop>threshold; loop--) {
		rtmp = rv_list;
		while(rtmp) {
			if (rtmp->rv_marker == loop) {

				printf("%2d  ", rtmp->rv_marker);
				print_title(4, rtmp->rv_title, RIGHTMARGIN, T_PREINDENT, NULL);
				html_print_authors(rtmp->rv_author, RIGHTMARGIN);
			}
			rtmp = rtmp->rv_next;
		}
	}
}


void
count_reviews(char *year)
{
	recomp_t	*current;
	review_t        *rtmp;

	current = head;
	while(current) {
		if (current->rc_year == 0) {
			current = current->rc_next;
			continue;
		}
		if (strcmp(current->rc_year, year) == 0) {
			rtmp = rv_list;
			while (rtmp) {
				if ( (strcmp(current->rc_title, rtmp->rv_title) == 0) &&
				     (strcmp(current->rc_author, rtmp->rv_author) == 0) ) {
#ifdef DEBUG
					printf("%s\n", rtmp->rv_abbreviation);
#endif
					current->rc_pubs++;
				}
				rtmp = rtmp->rv_next;
			}
		}
		current = current->rc_next;
	}
}


int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int		option;
	int		index;
	int		result;
	int		year;
	search_t	*tmp;
	char		path[256];

	printf("<html><head>\n");
	printf("<title>Most-Reviewed Books (%s)</title></head>\n", argv[1]);
	printf("<body bgcolor=ffffff>\n");
	printf("<h1>Most-Reviewed Books (%s)</h1>", argv[1]);
	printf("<hr><b>Updated: ");
	fflush(stdout);
	system("/bin/date");
	printf("</b><hr>\n");
	printf("<pre>\n");

	sscanf(argv[1], "%d", &year);

	dbaseloc = 1;
	strcpy(path, BASE);
	strcat(path, "/dbase.ascii");
	result = chdir(path);
        if (result != 0) {
                printf("CHDIR to %s failed\n", path);
                exit(1);
        }

	search_file("NOVELS");
	result = chdir("../dbase.compiled");
        if (result != 0) {
                printf("CHDIR to %s failed\n", path);
                exit(1);
        }
	load_reviews(F_NOOPT, 0);
	count_reviews(argv[1]);
	if (year < 1990) {
		output_result(argv[1], 1, "Novels", 1);
	} else {
		output_result(argv[1], 1, "Novels", 2);
	}

	result = chdir("../dbase.ascii");
        if (result != 0) {
                printf("CHDIR to %s failed\n", path);
                exit(1);
        }
	head = tail = NULL;
	search_file("ANTHOLOGIES");
	count_reviews(argv[1]);
	edited_flag = 1;
	output_result(argv[1], 0, "Anthologies", 1);
	edited_flag = 0;

	head = tail = NULL;
	search_file("COLLECTIONS");
	count_reviews(argv[1]);
	output_result(argv[1], 0, "Collections", 1);

	head = tail = NULL;
	search_file("NONFICTION");
	count_reviews(argv[1]);
	output_result(argv[1], 0, "Non-Fiction", 1);

	printf("</pre>\n");
	return(0);
}
