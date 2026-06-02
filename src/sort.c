/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)sort.c	1.5	06/10/97 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sfdbase.h"

search_t	*array_s[2048] = {NULL};
search_t	*array_e[2048] = {NULL};
search_t	*unpub_s = NULL;
search_t	*unpub_e = NULL;
search_t	*forth_s = NULL;
search_t	*forth_e = NULL;


/*
 * Well, you won't find this sorting algorithm in any cookbook.
 * This is pretty unconventional, relying on the fact that years
 * range from 0-present, plus 9999 and 8888. Instead of actually
 * sorting the list, we instead just pull data structures off the
 * front of oldlist and put them on one of the "hash" bucket linked
 * lists above (it just so happens that the hash algorithm is to
 * just use the year. 9999 and 8888 are treated separately.
 *
 * To generate the sorted list quickly, we just sew the end of
 * one linked list to the start of the next available year, and
 * then tack on the unpublished and the forthcoming. This code
 * will have to be modified in 2048. 
 */
void
sort_by_year(search_t **oldlist)
{
	int		loop;
	search_t	*tmp;
	search_t	*tmp2;
	search_t	*end;

	/*
	 * Take apart the old list, placing each data structure
	 * on the appropriate linked list.
	 */
	tmp  = *oldlist;
	while(tmp) {
		tmp2 = tmp->se_next;
		if (tmp->se_seriesnum) {
			sscanf(tmp->se_seriesnum, "%d", &(tmp->se_numyear) );
		} else {
			if (tmp->se_year) {
				sscanf(tmp->se_year, "%d", &(tmp->se_numyear) );
			}
		}
		if ( tmp->se_numyear < 2048) {
			if ( array_e[tmp->se_numyear] == NULL ) {
				array_e[tmp->se_numyear] = tmp;
				array_s[tmp->se_numyear] = tmp;
				tmp->se_next = NULL;
			} else {
				tmp->se_next = array_s[tmp->se_numyear];
				array_s[tmp->se_numyear] = tmp;
			}
		} else if ( tmp->se_numyear == 9999) {
			if ( forth_e == NULL ) {
				forth_e = tmp;
				forth_s = tmp;
				tmp->se_next = NULL;
			} else {
				tmp->se_next = forth_s;
				forth_s = tmp;
			}
		} else {
			if ( unpub_e == NULL ) {
				unpub_e = tmp;
				unpub_s = tmp;
				tmp->se_next = NULL;
			} else {
				tmp->se_next = unpub_s;
				unpub_s = tmp;
			}
		}
		tmp = tmp2;
	}

	/*
	 * Now construct a new linked list by following the
	 * sort order.
	 */
	end = NULL;
	*oldlist = NULL;
	for(loop=0; loop<2048; loop++) {
		if (array_s[loop] != NULL) {
			if ( end == NULL ) {
				*oldlist = array_s[loop];
				end = array_e[loop];
			} else {
				end->se_next = array_s[loop];
				end = array_e[loop];
			}
		}
	}
	if (unpub_s != NULL) {
		if (end) {
			end->se_next = unpub_s;
		} else {
			*oldlist = unpub_s;
		}
		end = unpub_e;
	}
	if (forth_s != NULL) {
		if (end) {
			end->se_next = forth_s;
		} else {
			*oldlist = forth_s;
		}
		end = forth_e;
	}
}


void
sort_by_yearold(search_t **oldlist)
{
	search_t	*tmp;
	search_t	*tmp2;
	search_t	*tmp3;
	int		minyear = 30000;
	int		maxyear = 0;
	int		loop;
	search_t	*newlist = NULL;

	tmp = *oldlist;
	while(tmp) {
		if (tmp->se_seriesnum) {
			sscanf(tmp->se_seriesnum, "%d", &(tmp->se_numyear) );
		} else {
			if (tmp->se_year) {
				sscanf(tmp->se_year, "%d", &(tmp->se_numyear) );
			} else if (tmp->se_numyear == 0) {
				if (tmp->se_title && tmp->se_author) {
					fprintf(stderr, "Title/Author [%s/%s] is missing year data\n", 
						tmp->se_title, tmp->se_author);
				} else if (tmp->se_title) {
					fprintf(stderr, "Title [%s] missing year, has mangled author\n", 
						tmp->se_title);
				} else if (tmp->se_author) {
					fprintf(stderr, "Author [%s] missing year, has mangled title\n", 
						tmp->se_author);
				} else {
					fprintf(stderr, "Missing year and all is mangled.\n");
				}
			}
		}
		if ( tmp->se_numyear <= minyear) {
			minyear = tmp->se_numyear;
		}
		if ( tmp->se_numyear >= maxyear)
			maxyear = tmp->se_numyear;
		tmp = tmp->se_next;
	}

	for(loop=maxyear; loop>=minyear; loop--) {
		tmp = *oldlist;
		tmp2 = NULL;
		while(tmp) {
			if (tmp->se_numyear == loop) {
				if (tmp2 == NULL) {
					*oldlist = tmp->se_next;
				} else {
					tmp2->se_next = tmp->se_next;
				}
				tmp3 = tmp->se_next;
				tmp->se_next = newlist;
				newlist = tmp;
				tmp = tmp3;
			} else {
				tmp2 = tmp;
				tmp = tmp->se_next;
			}
		}
	}
	*oldlist = newlist;
}


void
sortpub_by_year(pub_t **oldlist)
{
	pub_t	*tmp;
	pub_t	*tmp2;
	pub_t	*tmp3;
	int		minyear = 3000;
	int		maxyear = 0;
	int		loop;
	pub_t	*newlist = NULL;

	tmp = *oldlist;
	while(tmp) {
		sscanf(tmp->pu_year, "%d", &(tmp->pu_numyear) );
		if ( tmp->pu_numyear <= minyear)
			minyear = tmp->pu_numyear;
		if ( tmp->pu_numyear >= maxyear)
			maxyear = tmp->pu_numyear;
		tmp = tmp->pu_next;
	}

	for(loop=maxyear; loop>=minyear; loop--) {
		tmp = *oldlist;
		tmp2 = NULL;
		while(tmp) {
			if (tmp->pu_numyear == loop) {
				if (tmp2 == NULL) {
					*oldlist = tmp->pu_next;
				} else {
					tmp2->pu_next = tmp->pu_next;
				}
				tmp3 = tmp->pu_next;
				tmp->pu_next = newlist;
				newlist = tmp;
				tmp = tmp3;
			} else {
				tmp2 = tmp;
				tmp = tmp->pu_next;
			}
		}
	}
	*oldlist = newlist;
}


void
sortpub_by_year2(pub_t **oldlist)
{
	pub_t	*tmp;
	pub_t	*tmp2;
	pub_t	*tmp3;
	int		minyear = 20000;
	int		maxyear = 0;
	int		loop;
	pub_t	*newlist = NULL;

	tmp = *oldlist;
	while(tmp) {
		if ( tmp->pu_numyear <= minyear)
			minyear = tmp->pu_numyear;
		if ( tmp->pu_numyear >= maxyear)
			maxyear = tmp->pu_numyear;
		tmp = tmp->pu_next;
	}

	for(loop=minyear; loop<=maxyear; loop++) {
		tmp = *oldlist;
		tmp2 = NULL;
		while(tmp) {
			if (tmp->pu_numyear == loop) {
				if (tmp2 == NULL) {
					*oldlist = tmp->pu_next;
				} else {
					tmp2->pu_next = tmp->pu_next;
				}
				tmp3 = tmp->pu_next;
				tmp->pu_next = newlist;
				newlist = tmp;
				tmp = tmp3;
			} else {
				tmp2 = tmp;
				tmp = tmp->pu_next;
			}
		}
	}
	*oldlist = newlist;
}

void
insert_pub(pub_t *current, pub_t **newlist)
{
	pub_t *tmp;
	pub_t *prev;

	if (*newlist == NULL) {
		*newlist = current;
		current->pu_next = NULL;
		return;
	}

	tmp = *newlist;
	prev = NULL;
	while (tmp) {
		if (strcmp(current->pu_title, tmp->pu_title) < 0) {
			if (prev == NULL) {
				current->pu_next = *newlist;
				*newlist = current;
				return;
			} else {
				current->pu_next = tmp;
				prev->pu_next = current;
				return;
			}
		}
		prev = tmp;
		tmp = tmp->pu_next;
	}
}

void
sortpub_by_title(pub_t **oldlist)
{
	pub_t	*current;
	pub_t	*next;
	pub_t	*newlist = NULL;
	pub_t	*tmp;

	current = *oldlist;
	while( current ) {
		next = current->pu_next;
		insert_pub(current, &newlist);
		current = next;
	}

	*oldlist = newlist;
}
