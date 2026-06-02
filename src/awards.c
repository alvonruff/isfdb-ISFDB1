/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)awards.c	1.11	01/05/00 SFdbase";

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

static char	tmpauthor[MEDIUMSIZE];

extern award_t   *misc_awards;


int
is_collanth(char *type)
{
	if ( strcmp(type, "c") == 0) {
		return(1);
	} else if ( strcmp(type, "a") == 0) {
		return(1);
	} else if ( strcmp(type, "ac") == 0) {
		return(1);
	} else {
		return(0);
	}
}

int
is_novel(char *type)
{
	if ( strcmp(type, "n") == 0) {
		return(1);
	} else if ( strcmp(type, "o") == 0) {
		return(1);
	} else if ( strcmp(type, "fn") == 0) {
		return(1);
	} else if ( strcmp(type, "nsf") == 0) {
		return(1);
	} else if ( strcmp(type, "nft") == 0) {
		return(1);
	} else if ( strcmp(type, "nho") == 0) {
		return(1);
	} else {
		return(0);
	}
}

int
is_longfiction(char *type)
{
	if ( strcmp(type, "lf") == 0) {
		return(1);
	} else if ( strcmp(type, "n") == 0) {
		return(1);
	} else if ( strcmp(type, "c") == 0) {
		return(1);
	} else if ( strcmp(type, "a") == 0) {
		return(1);
	} else {
		return(0);
	}
}

int
is_shortfiction(char *type)
{
	if ( strcmp(type, "nv") == 0) {
		return(1);
	} else if ( strcmp(type, "nt") == 0) {
		return(1);
	} else if ( strcmp(type, "ss") == 0) {
		return(1);
	} else if ( strcmp(type, "sf") == 0) {
		return(1);
	} else {
		return(0);
	}
}



void
add_award(award_t *awt, search_t *list)
{
	search_t        *tmp;

	/*
	 * Try to match this award to a title in the list. First we
	 * try to match up by using award tags.
	 */
	if (awt->aw_tag && awt->aw_tag[0]) {
		tmp = list;
		while( tmp ) {
			if (tmp->se_awtags && strstr(tmp->se_awtags, awt->aw_tag)) {
				char *ptr1,*ptr2;

				ptr1 = tmp->se_awtags;
				while ( strstr(ptr1, ",") ) {
					ptr2 = (char *)strstr(ptr1, ",");
					*ptr2 = 0;
					if (strcmp(awt->aw_tag, ptr1) == 0) {
						*ptr2 = ',';
						awt->aw_next = tmp->se_awards;
						tmp->se_awards = awt;
						return;
					}
					*ptr2 = ',';
					ptr1 = ptr2+1;
				}
				if (strcmp(awt->aw_tag, ptr1) == 0) {
					awt->aw_next = tmp->se_awards;
					tmp->se_awards = awt;
					return;
				}
			}
			tmp = tmp->se_next;
		}
	}

	/*
	 * Tag match failed, so go to old pattern-matching heuristic.
	 */
	tmp = list;
	while( tmp ) {
		if ( strchr(tmp->se_title, '^') && strstr(tmp->se_title, awt->aw_title)) {
			goto inexact_title;
		}
		if (strcmp(awt->aw_title, tmp->se_title)) {
			tmp = tmp->se_next;
			continue;
		}
inexact_title:
		if ( is_novel(awt->aw_types) && !is_novel(tmp->se_type) ) {
			tmp = tmp->se_next;
			continue;
		}
		if ( is_shortfiction(awt->aw_types) && !is_shortfiction(tmp->se_type) ) {
			tmp = tmp->se_next;
			continue;
		}
		if ( is_longfiction(awt->aw_types) && !is_longfiction(tmp->se_type) ) {
			tmp = tmp->se_next;
			continue;
		}
		if ( is_collanth(awt->aw_types) && !is_collanth(tmp->se_type) ) {
			tmp = tmp->se_next;
			continue;
		}

		awt->aw_next = tmp->se_awards;
		tmp->se_awards = awt;
		return;
	}

	awt->aw_next = misc_awards;
	misc_awards = awt;
}


void
parse_awards(char *exact_author, search_t *list)
{
	FILE	*fp;
	FILE	*fp2;
	int	line_number = 1;
	int	index;
	char	input;
	int	this_offset = 0;
	int	next_offset = 0;

	fp = fopen("awards.xba", "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		this_offset = next_offset;
		if ( parse_field(fp, tmpauthor, (int *)&next_offset, MEDIUMSIZE) == -1 ) {
			goto finish;
		}

		if (strcmp(tmpauthor, exact_author) == 0) {

			fp2 = fopen("awards.dbase", "rb");
			if (fp2 == NULL) {
				perror("Couldn't open dbase");
				exit(1);
			}

			while(1) {
				char offset[16];
				char awardname[4];
				int eol;
				int input;
				int int_offset;
				award_t *awt;

				parse_field_or_eol(fp, offset, &eol, 16);
				sscanf(offset, "%x", &int_offset);
				fseek(fp2, int_offset, SEEK_SET);

				awt = parse_award_entry(fp2, (int *)&next_offset, 0);
				if (awt == NULL) {
					break;
				}
				add_award(awt, list);
				if ( eol ) {
					break;
				}
			}
			fclose(fp2);
			goto finish;

		} else if ( parse_to_eol(fp, (int *)&next_offset) == -1 ) {
			goto finish;
		}
		line_number++;
	}

finish:
	fclose(fp);
}


void
get_award_info(char *exact_author, search_t *title_list)
{
        int     result;

#ifdef CHDIR
        result = chdir(CGIBIN);
        if (result != 0) {
                printf("CHDIR to %s failed\n", CGIBIN);
                exit(1);
        }

#endif

	parse_awards(exact_author, title_list);
}

