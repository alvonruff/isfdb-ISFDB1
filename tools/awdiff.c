/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *	 ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)sort.c	1.6	02 May 1997 SFdbase";

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
	char		*rc_awtag;
	char		*rc_notes;
	char		*rc_synopsis;
	char		*rc_translation;
	int		rc_checked;
	struct recomp   *rc_next;
} recomp_t;

recomp_t *head = NULL;
recomp_t *tail = NULL;
recomp_t *isfdb_list = NULL;
recomp_t *newdb_list = NULL;

char	tmpseries[256];
char	tmpsuperseries[256];
char	tmptype[256];
char	tmpstorylen[256];
char	tmpnotes[16];
char	tmpsynopsis[16];
char	tmpseriesnum[16];

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
				target->rc_awtag = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_awtag, attr->at_value);
			} else if ( strcmp(targetattr, "NT") == 0) {
				target->rc_notes = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_notes, attr->at_value);
			} else if ( strcmp(targetattr, "SY") == 0) {
				target->rc_synopsis = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_synopsis, attr->at_value);
			} else if ( strcmp(targetattr, "TR") == 0) {
				target->rc_translation = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_translation, attr->at_value);
			}
			break;
		}
		attr = attr->at_next;
	}
}


void
clone(recomp_t *target, char *awtag)
{
	recomp_t *tmp;

	tmp = (recomp_t *)malloc( sizeof(recomp_t) );
	tmp->rc_title = (char *)malloc( strlen(target->rc_title) + 1);
	strcpy(tmp->rc_title, target->rc_title);

	tmp->rc_author = (char *)malloc( strlen(target->rc_author) + 1);
	strcpy(tmp->rc_author, target->rc_author);

	tmp->rc_year = (char *)malloc( strlen(target->rc_year) + 1);
	strcpy(tmp->rc_year, target->rc_year);

	tmp->rc_awtag = (char *)malloc( strlen(awtag) + 1);
	strcpy(tmp->rc_awtag, awtag);

	tmp->rc_checked = 0;

	if (head) {
		tail->rc_next = tmp;
		tail = tmp;
	} else {
		head = tail = tmp;
	}
}


void
search_file(char *filename, int expand)
{
	object_t	*tmp;
	object_t	*obj_free;
	attr_t		*attr;
	attr_t		*atr_free;
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
		target->rc_awtag = NULL;
		target->rc_notes = NULL;
		target->rc_synopsis = NULL;
		target->rc_translation = NULL;
		target->rc_next = NULL;
		target->rc_checked = 0;
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
		do_attribute("TR", tmp->ob_attrs, target);
		do_attribute("TG", tmp->ob_attrs, target);

		attr = tmp->ob_attrs;
		while (attr) {
			atr_free = attr;
			attr = attr->at_next;
			free( atr_free->at_value );
			free( atr_free );
		}

		obj_free = tmp;
		tmp = tmp->ob_next;
		free( obj_free );

		if ( expand && strstr(target->rc_awtag, ",")) {
			char *ptr1, *ptr2;
			int	first = 1;

			ptr1 = target->rc_awtag;
			while(1) {
				ptr2 =  (char *)strstr(ptr1, ",");
				if (ptr2) {
					*ptr2 = 0;
					if (first) {
						first = 0;
					} else {
						clone(target, ptr1);
					}
					ptr1 = ptr2 + 1;
				} else {
					clone(target, ptr1);
					break;
				}
			}
		}
	}
	Objlist = NULL;
}


int
keymatch(char *taglist, char *tag)
{
	char *ptr1;
	char *ptr2;
	char tmptag[256];

	if (taglist == NULL)
		return(0);

	if (strstr(taglist, ",")) {
		strcpy(tmptag, taglist);
		ptr1 = tmptag;
		ptr2 = strstr(ptr1, ",");
		while(ptr2) {
			*ptr2 = 0;
			if ( strcmp(ptr1, tag) == 0) {
				return(1);
			}
			ptr1 = ptr2 + 1;
			ptr2 = strstr(ptr1, ",");
		}
		if ( strcmp(ptr1, tag) == 0) {
			return(1);
		} else {
			return(0);
		}
	} else if ( strcmp(taglist, tag) == 0) {
		return(1);
	} else {
		return(0);
	}
}


void
check_content()
{
	recomp_t *newdb;
	recomp_t *isfdb;

	newdb = newdb_list;
	while(newdb) {
		newdb->rc_checked = 0;
		newdb = newdb->rc_next;
	}

	newdb = newdb_list;
	while(newdb) {
		if (newdb->rc_awtag == NULL) {
			newdb = newdb->rc_next;
			continue;
		}

		isfdb = isfdb_list;
		while(isfdb) {
			if (isfdb->rc_awtag == NULL) {
				isfdb = isfdb->rc_next;
				continue;
			} else if (keymatch(isfdb->rc_awtag, newdb->rc_awtag)) {
				newdb->rc_checked = 1;
			}
			isfdb = isfdb->rc_next;
		}
		newdb = newdb->rc_next;
	}

	newdb = newdb_list;
	while(newdb) {
		if (newdb->rc_checked == 0) {
			printf("%s {\n", newdb->rc_title);
			printf("\tAE=|%s|\n", newdb->rc_author);
			printf("\tYR=|%s|\n", newdb->rc_year);
			printf("\tTG=|%s|\n", newdb->rc_awtag);
			printf("}\n"); 
		}
		newdb = newdb->rc_next;
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

	if (argc != 3) {
		printf("usage: awdiff <isfdb file> <award-derived file>\n");
		exit(1);
	}

	search_file(argv[1], 0);
	isfdb_list = head;

	head = NULL;
	search_file(argv[2], 1);
	newdb_list = head;

	check_content();

	return(0);
}
