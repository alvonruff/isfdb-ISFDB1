/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *	 ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)cap.c	1.3	02 May 1997 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
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
	struct recomp   *rc_next;
} recomp_t;

recomp_t *head = NULL;
recomp_t *tail = NULL;


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

#define NUMWORDS 43
static char *wordlist[] = {
        " A ",
        " An ",
        " The ",
        " As ",
        " At ",
        " By ",
        " For ",
        " From ",
        " In ",
        " Into ",
        " Near ",
        " Of ",
        " Off ",
        " On ",
        " Onto ",
        " Out ",
        " Over ",
        " Past ",
        " Till ",
        " To ",
        " Up ",
        " Upon ",
        " With ",
        " Also ",
        " Next ",
        " Now ",
        " Then ",
        " Thus ",
        " And ",
        " But ",
        " Nor ",
        " Or ",
        " For ",
        " Yet ",
        " As ",
        " If ",
        " Once ",
        " Than ",
        " That ",
        " When ",
        " That ",
        " What ",
        " Who ",
};


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
output_result()
{
	recomp_t	*target;

	target = head;
	while(target) {
		if (target->rc_title == 0) {
			target = target->rc_next;
			continue;
		}
		printf("%s {\n", target->rc_title);
		if (target->rc_author)
			printf("\tAE=|%s|\n", target->rc_author);
		if (target->rc_year)
			printf("\tYR=|%s|\n", target->rc_year);
		if (target->rc_series)
			printf("\tSE=|%s|\n", target->rc_series);
		if (target->rc_superseries)
			printf("\tSS=|%s|\n", target->rc_superseries);
		if (target->rc_seriesnum)
			printf("\tSN=|%s|\n", target->rc_seriesnum);
		if (target->rc_storylen)
			printf("\tSL=|%s|\n", target->rc_storylen);
		if (target->rc_pubtags)
			printf("\tPB=|%s|\n", target->rc_pubtags);
		if (target->rc_notes)
			printf("\tNT=|%s|\n", target->rc_notes);
		if (target->rc_synopsis)
			printf("\tSY=|%s|\n", target->rc_synopsis);
		printf("}\n");
		target = target->rc_next;
	}
}


void
fix_caps()
{
	recomp_t *current;
	char    *tmp;
	char	pred;
	int	wasspace, loop, changed;

	current = head;
	while(current) {

		/*
		 * Uncapitalize words in the list which don't follow ':'
		 */
		strcpy(tmptitle, current->rc_title);
		changed = 0;
		for(loop=0; loop<NUMWORDS; loop++) {
			tmp = (char *)strstr(current->rc_title, wordlist[loop]);
			while (tmp) {
				pred = (char) *(tmp-1);
				if ( (pred != ':') && (pred != '-') && (pred != '/')) {
					tmp++;
					*tmp = tolower( *tmp );
					changed = 1;
				} else {
					tmp++;
				}
				tmp = (char *)strstr(tmp, wordlist[loop]);
			}
		}
		if (changed) {
			printf("--------------------------------------------------------\n");
			printf("FROM: [%s] (%s)\n", tmptitle, current->rc_author);
			printf("TO:   [%s]\n", current->rc_title);
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

	if (argc != 2) {
		printf("usage: merge <file>\n");
		exit(1);
	}

	search_file(argv[1]);
	fix_caps();
	return(0);
}
