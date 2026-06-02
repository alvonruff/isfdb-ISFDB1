/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *	 ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)merge.c	1.3	06/10/97 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sfdbase.h"

typedef struct badauth {
	char		*bd_author;
	struct badauth	*bd_next;
} badauth_t;
badauth_t *bhead = NULL;

typedef struct recomp {
	char		*rc_title;
	char		*rc_author;
	char		*rc_year;
	char		*rc_series;
	char		*rc_superseries;
	char		*rc_seriesnum;
	char		*rc_storylen;
	char		*rc_pubtags;
	char		*rc_awtags;
	char		*rc_notes;
	char		*rc_synopsis;
	struct recomp   *rc_next;
} recomp_t;
recomp_t *head = NULL;
recomp_t *tail = NULL;

int	doyear = 1;


extern object_t	*Objlist;
extern char	*optarg;


void
do_attribute(char *targetattr, attr_t *list, recomp_t *target)
{
	attr_t		*attr;

	attr = list;
	while (attr) {
		if (strncmp(attr->at_name, targetattr, 2) == 0) {
			if ( strcmp(targetattr, "AE") == 0) {
				target->rc_author = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_author, attr->at_value);
			} else if ( strcmp(targetattr, "YR") == 0) {
				target->rc_year = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_year, attr->at_value);
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
			} else if ( strcmp(targetattr, "TG") == 0) {
				target->rc_awtags = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_awtags, attr->at_value);
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
		target->rc_awtags = NULL;
		target->rc_notes = NULL;
		target->rc_synopsis = NULL;
		target->rc_next = NULL;
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
		do_attribute("TG", tmp->ob_attrs, target);
		tmp = tmp->ob_next;
	}
	Objlist = NULL;
}



void
load_bad_authors(char *path)
{
	char 		line[256];
	badauth_t 	*tmp;
	int 		input;
	int  		index;
	FILE 		*fp;

	fp = fopen(path, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Can't open %s\n", path);
		exit(1);
	}

	index = 0;
	while(1) {
		input = getc(fp);
		if (input == -1) {
			return;
		} else if (input == '\n') {
			line[index] = 0;
			index = 0;
			tmp = (badauth_t *)malloc( sizeof(badauth_t) );
			tmp->bd_author = (char *)malloc ( strlen(line)+1);
			strcpy(tmp->bd_author, line);
			tmp->bd_next = bhead;
			bhead = tmp;
		} else {
			line[index++] = input;
		}

	}

}


void
do_note(int synopsis, char *note)
{
	char *ptr;
	int  dolast = 1;

	if (synopsis) {
		printf("\tSY=|");
	} else {
		printf("\tNT=|");
	}
	while( strlen(note) > 80) {
		ptr = (char *)&note[80];
		while(*ptr != ' ') {
			if(*ptr == 0) {
				dolast = 0;
				break;
			}
			ptr++;
		}
		*ptr = 0;
		if (dolast) {
			printf("%s\n\t", note);
		} else {
			printf("%s|\n", note);
		}
		note = ptr+1;
	}
	if (dolast) {
		printf("%s|\n", note);
	}
}

void
check_one(char *author)
{
	badauth_t 	*tmp;

	if (strstr(author, " & ")) {
		printf("Bad Author: %s\n", author);
		return;
	}
	if (strstr(author, " and ")) {
		printf("Bad Author: %s\n", author);
		return;
	}

	tmp = bhead;
	while(tmp) {
		if(strcmp(author, tmp->bd_author) == 0) {
			printf("Bad Author: %s\n", author);
			return;
		}
		tmp = tmp->bd_next;
	}
}

void
check_authors()
{
	recomp_t	*current;
	char		*author;
	char		*ptr;

	current = head;
	while(current) {
		author = current->rc_author;
		if ( strstr(author, "^") ) {
			auset_t *tmp2;

			tmp2 = (auset_t *)decompose(author);
			separate(tmp2);
			while( tmp2 ) {
				char	auth2[80];

				author = tmp2->au_actual;
				while ( strstr(author, "+") ) {
					ptr = (char *)strstr(author, "+");
					*ptr = 0;
					check_one(author);
					author = ++ptr;
				}
				check_one(author);
				tmp2 = tmp2->au_next;
			}
		} else {
			while ( strstr(author, "+") ) {
				ptr = (char *)strstr(author, "+");
				*ptr = 0;
				check_one(author);
				author = ++ptr;
			}
			check_one(author);
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
	search_t	*tmp;
	char		path[256];

	if (argc == 2) {
		strcpy(path, argv[1]);
	} else {
		fprintf(stderr, "Usage: checkauthor <title file>\n");
		exit(1);
	}

	search_file(path);
	load_bad_authors("BADAUTH");
	check_authors();
	return(0);
}
