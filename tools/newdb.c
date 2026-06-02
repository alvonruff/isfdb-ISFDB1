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
	char		*rc_lowt;
	char		*rc_author;
	char		*rc_lowa;
	char		*rc_year;
	char		*rc_series;
	char		*rc_superseries;
	char		*rc_seriesnum;
	char		*rc_storylen;
	char		*rc_pubtags;
	char		*rc_notes;
	char		*rc_synopsis;
	int		rc_dontuse;
	struct recomp   *rc_next;
} recomp_t;

typedef struct repub {
	char            *rp_title;
	char            *rp_abbreviation;
	char            *rp_author;
	char            *rp_year;
	char            *rp_isbn;
	char            *rp_publisher;
	char            *rp_price;
	char            *rp_pages;
	char            *rp_type;
	char            *rp_cover;
	char            *rp_notes;
	int		rp_dontuse;
	struct repub    *rp_next;
} repub_t;

recomp_t *head = NULL;
recomp_t *tail = NULL;
recomp_t *olddb_shortfiction = NULL;
recomp_t *olddb_novels = NULL;
recomp_t *olddb_anthologies = NULL;
recomp_t *olddb_collections = NULL;
recomp_t *olddb_essays = NULL;
recomp_t *olddb_serials = NULL;
repub_t  *phead = NULL;
repub_t  *ptail = NULL;


FILE *fp_shortfiction = NULL;
FILE *fp_novels = NULL;
FILE *fp_anthologies = NULL;
FILE *fp_collections = NULL;
FILE *fp_essays = NULL;
FILE *fp_serials = NULL;
FILE *fp_books = NULL;

typedef struct keymap {
	char		km_newdb[512];
	char		km_isfdb[512];
	struct keymap	*km_next;
} keymap_t;
keymap_t *keyhead = NULL;

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
				target->rc_lowa = (char *)malloc( strlen(attr->at_value) + 1);
				target->rc_lowa[0] = 0;
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
		target->rc_lowt = (char *)malloc( strlen(tmp->ob_name) + 1);
		if (target->rc_lowt == NULL) {
			fprintf(stderr, "OUT OF MEMORY\n");
			exit(1);
		}
		target->rc_lowt[0] = 0;
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
		target->rc_dontuse = 0;
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
do_attribute2(char *targetattr, attr_t *list, repub_t *target)
{
	attr_t		*attr;

	attr = list;
	while (attr) {
		if (strncmp(attr->at_name, targetattr, 2) == 0) {
			if ( strcmp(targetattr, "AE") == 0) {
				target->rp_author = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rp_author, attr->at_value);
			} else if ( strcmp(targetattr, "AB") == 0) {
				target->rp_abbreviation = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rp_abbreviation, attr->at_value);
			} else if ( strcmp(targetattr, "YR") == 0) {
				target->rp_year = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rp_year, attr->at_value);
			} else if ( strcmp(targetattr, "IS") == 0) {
				target->rp_isbn = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rp_isbn, attr->at_value);
			} else if ( strcmp(targetattr, "PB") == 0) {
				target->rp_publisher = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rp_publisher, attr->at_value);
			} else if ( strcmp(targetattr, "PR") == 0) {
				target->rp_price = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rp_price, attr->at_value);
			} else if ( strcmp(targetattr, "PP") == 0) {
				target->rp_pages = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rp_pages, attr->at_value);
			} else if ( strcmp(targetattr, "TP") == 0) {
				target->rp_type = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rp_type, attr->at_value);
			} else if ( strcmp(targetattr, "CV") == 0) {
				target->rp_cover = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rp_cover, attr->at_value);
			} else if ( strcmp(targetattr, "NT") == 0) {
				target->rp_notes = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rp_notes, attr->at_value);
			}
			break;
		}
		attr = attr->at_next;
	}
}


void
search_file2(char *filename)
{
	object_t	*tmp;
	attr_t		*attr;
	repub_t		*target;
	extern int	line_number;

	line_number = 1;
	parse_pubs(filename);
	tmp = Objlist;
	while(tmp) {
		target = (repub_t *)malloc( sizeof(repub_t) );
		target->rp_title = (char *)malloc( strlen(tmp->ob_name) + 1);
		strcpy(target->rp_title, tmp->ob_name);
		target->rp_abbreviation = NULL;
		target->rp_author = NULL;
		target->rp_year = NULL;
		target->rp_isbn = NULL;
		target->rp_publisher = NULL;
		target->rp_price = NULL;
		target->rp_pages = NULL;
		target->rp_type = NULL;
		target->rp_cover = NULL;
		target->rp_notes = NULL;
		target->rp_next = NULL;
		if (phead) {
			ptail->rp_next = target;
			ptail = target;
		} else {
			phead = ptail = target;
		}

		do_attribute2("AB", tmp->ob_attrs, target);
		do_attribute2("AE", tmp->ob_attrs, target);
		do_attribute2("YR", tmp->ob_attrs, target);
		do_attribute2("IS", tmp->ob_attrs, target);
		do_attribute2("PB", tmp->ob_attrs, target);
		do_attribute2("PR", tmp->ob_attrs, target);
		do_attribute2("PP", tmp->ob_attrs, target);
		do_attribute2("TP", tmp->ob_attrs, target);
		do_attribute2("CV", tmp->ob_attrs, target);
		do_attribute2("NT", tmp->ob_attrs, target);
		tmp = tmp->ob_next;
	}
	Objlist = NULL;
}

void
read_keymap(char *path)
{
	FILE *fp;
	unsigned char input;
	int  offset;

	fp = fopen(path, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Couldn't find the KEYMAP\n");
		exit(1);
	}

	while(1) {
		keymap_t *tmp;

		tmp = (keymap_t *)malloc( sizeof(keymap_t) );
		if ( tmp == NULL ) {
			fprintf(stderr, "Out of Memory\n");
			exit(1);
		}

		input  = 0;
		offset = 0;
		while(input != ':') {
			input = getc(fp);
			if (input == 255) {
				return;
			}
			tmp->km_newdb[offset++] = input;
		}
		tmp->km_newdb[--offset] = 0;

		input  = 0;
		offset = 0;
		while(input != '\n') {
			input = getc(fp);
			if (input == 255) {
				fprintf(stderr, "Mangled line\n");
				exit(1);
			}
			tmp->km_isfdb[offset++] = input;
		}
		tmp->km_isfdb[--offset] = 0;

		tmp->km_next = keyhead;
		keyhead = tmp;
	}
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
remove_key(char *taglist, char *tag)
{
	char *ptr1;
	char *ptr2;
	char tmptag[256];

	if ( strcmp(taglist, tag) == 0) {
		taglist[0] = 0;
	} else if ( strstr(taglist, ",") ) {
		strcpy(tmptag, taglist);
		ptr1 = tmptag;
		ptr2 = strstr(ptr1, ",");
		while(ptr2) {
			*ptr2 = 0;
			if ( strcmp(ptr1, tag) == 0) {
				ptr2++;
				while(*ptr2) {
					*ptr1 = *ptr2;
					ptr1++;
					ptr2++;
				}
				*ptr1 = *ptr2;
				strcpy(taglist, tmptag);
				return;
			}
			ptr1 = ptr2 + 1;
			*ptr2 = ',';
			ptr2 = strstr(ptr1, ",");
		}
		ptr1--;
		*ptr1 = 0;
		strcpy(taglist, tmptag);
	}
}


void
remove_keys()
{
	keymap_t *tmp;
	recomp_t *newdb;
	repub_t  *newpub;

	tmp = keyhead;
	while(tmp) {

		/*
		 * Novels
		 */
		newdb = olddb_novels;
		while(newdb) {
			if (newdb->rc_dontuse) {
				newdb = newdb->rc_next;
				continue;
			}
			if (keymatch(newdb->rc_pubtags, tmp->km_newdb)) {
				remove_key(newdb->rc_pubtags, tmp->km_newdb);
				if (newdb->rc_pubtags[0] == 0) {
					newdb->rc_dontuse = 1;
				}
			}
			newdb = newdb->rc_next;
		}

		/*
		 * Shortfiction
		 */
		newdb = olddb_shortfiction;
		while(newdb) {
			if (newdb->rc_dontuse) {
				newdb = newdb->rc_next;
				continue;
			}
			if (keymatch(newdb->rc_pubtags, tmp->km_newdb)) {
				remove_key(newdb->rc_pubtags, tmp->km_newdb);
				if (newdb->rc_pubtags[0] == 0) {
					newdb->rc_dontuse = 1;
				}
			}
			newdb = newdb->rc_next;
		}

		/*
		 * Essays
		 */
		newdb = olddb_essays;
		while(newdb) {
			if (newdb->rc_dontuse) {
				newdb = newdb->rc_next;
				continue;
			}
			if (keymatch(newdb->rc_pubtags, tmp->km_newdb)) {
				remove_key(newdb->rc_pubtags, tmp->km_newdb);
				if (newdb->rc_pubtags[0] == 0) {
					newdb->rc_dontuse = 1;
				}
			}
			newdb = newdb->rc_next;
		}

		/*
		 * Collections
		 */
		newdb = olddb_collections;
		while(newdb) {
			if (newdb->rc_dontuse) {
				newdb = newdb->rc_next;
				continue;
			}
			if (keymatch(newdb->rc_pubtags, tmp->km_newdb)) {
				remove_key(newdb->rc_pubtags, tmp->km_newdb);
				if (newdb->rc_pubtags[0] == 0) {
					newdb->rc_dontuse = 1;
				}
			}
			newdb = newdb->rc_next;
		}

		/*
		 * Anthologies
		 */
		newdb = olddb_anthologies;
		while(newdb) {
			if (newdb->rc_dontuse) {
				newdb = newdb->rc_next;
				continue;
			}
			if (keymatch(newdb->rc_pubtags, tmp->km_newdb)) {
				remove_key(newdb->rc_pubtags, tmp->km_newdb);
				if (newdb->rc_pubtags[0] == 0) {
					newdb->rc_dontuse = 1;
				}
			}
			newdb = newdb->rc_next;
		}

		/*
		 * Serials
		 */
		newdb = olddb_serials;
		while(newdb) {
			if (newdb->rc_dontuse) {
				newdb = newdb->rc_next;
				continue;
			}
			if (keymatch(newdb->rc_pubtags, tmp->km_newdb)) {
				remove_key(newdb->rc_pubtags, tmp->km_newdb);
				if (newdb->rc_pubtags[0] == 0) {
					newdb->rc_dontuse = 1;
				}
			}
			newdb = newdb->rc_next;
		}

		/*
		 * Books
		 */
		newpub = phead;
		while(newpub) {
			if (newpub->rp_dontuse) {
				newpub = newpub->rp_next;
				continue;
			}
			if (strcmp(newpub->rp_abbreviation, tmp->km_newdb) == 0) {
				newpub->rp_abbreviation[0] = 0;
				newpub->rp_dontuse = 1;
			}
			newpub = newpub->rp_next;
		}

		tmp = tmp->km_next;
	}
	
}

void
output_files()
{
	recomp_t *newdb;
	repub_t  *newpub;

	/*
	 * Novels
	 */
	newdb = olddb_novels;
	while(newdb) {
		if (newdb->rc_dontuse) {
			newdb = newdb->rc_next;
			continue;
		}
		fprintf(fp_novels, "%s {\n", newdb->rc_title);
		fprintf(fp_novels, "\tAE=|%s|\n", newdb->rc_author);
		fprintf(fp_novels, "\tYR=|%s|\n", newdb->rc_year);
		fprintf(fp_novels, "\tPB=|%s|\n", newdb->rc_pubtags);
		fprintf(fp_novels, "}\n");
		newdb = newdb->rc_next;
	}

	/*
	 * Shortfiction
	 */
	newdb = olddb_shortfiction;
	while(newdb) {
		if (newdb->rc_dontuse) {
			newdb = newdb->rc_next;
			continue;
		}
		fprintf(fp_shortfiction, "%s {\n", newdb->rc_title);
		fprintf(fp_shortfiction, "\tAE=|%s|\n", newdb->rc_author);
		fprintf(fp_shortfiction, "\tYR=|%s|\n", newdb->rc_year);
		fprintf(fp_shortfiction, "\tPB=|%s|\n", newdb->rc_pubtags);
		if (newdb->rc_storylen[0])
			fprintf(fp_shortfiction, "\tSL=|%s|\n", newdb->rc_storylen);
		fprintf(fp_shortfiction, "}\n");
		newdb = newdb->rc_next;
	}

	/*
	 * Essays
	 */
	newdb = olddb_essays;
	while(newdb) {
		if (newdb->rc_dontuse) {
			newdb = newdb->rc_next;
			continue;
		}
		fprintf(fp_essays, "%s {\n", newdb->rc_title);
		fprintf(fp_essays, "\tAE=|%s|\n", newdb->rc_author);
		fprintf(fp_essays, "\tYR=|%s|\n", newdb->rc_year);
		fprintf(fp_essays, "\tPB=|%s|\n", newdb->rc_pubtags);
		if (newdb->rc_storylen[0])
			fprintf(fp_essays, "\tSL=|%s|\n", newdb->rc_storylen);
		fprintf(fp_essays, "}\n");
		newdb = newdb->rc_next;
	}

	/*
	 * Collections
	 */
	newdb = olddb_collections;
	while(newdb) {
		if (newdb->rc_dontuse) {
			newdb = newdb->rc_next;
			continue;
		}
		fprintf(fp_collections, "%s {\n", newdb->rc_title);
		fprintf(fp_collections, "\tAE=|%s|\n", newdb->rc_author);
		fprintf(fp_collections, "\tYR=|%s|\n", newdb->rc_year);
		fprintf(fp_collections, "\tPB=|%s|\n", newdb->rc_pubtags);
		if (newdb->rc_storylen[0])
			fprintf(fp_collections, "\tSL=|%s|\n", newdb->rc_storylen);
		fprintf(fp_collections, "}\n");
		newdb = newdb->rc_next;
	}

	/*
	 * Anthologies
	 */
	newdb = olddb_anthologies;
	while(newdb) {
		if (newdb->rc_dontuse) {
			newdb = newdb->rc_next;
			continue;
		}
		fprintf(fp_anthologies, "%s {\n", newdb->rc_title);
		fprintf(fp_anthologies, "\tAE=|%s|\n", newdb->rc_author);
		fprintf(fp_anthologies, "\tYR=|%s|\n", newdb->rc_year);
		fprintf(fp_anthologies, "\tPB=|%s|\n", newdb->rc_pubtags);
		if (newdb->rc_storylen[0])
			fprintf(fp_anthologies, "\tSL=|%s|\n", newdb->rc_storylen);
		fprintf(fp_anthologies, "}\n");
		newdb = newdb->rc_next;
	}

	/*
	 * Serials
	 */
	newdb = olddb_serials;
	while(newdb) {
		if (newdb->rc_dontuse) {
			newdb = newdb->rc_next;
			continue;
		}
		fprintf(fp_serials, "%s {\n", newdb->rc_title);
		fprintf(fp_serials, "\tAE=|%s|\n", newdb->rc_author);
		fprintf(fp_serials, "\tYR=|%s|\n", newdb->rc_year);
		fprintf(fp_serials, "\tPB=|%s|\n", newdb->rc_pubtags);
		if (newdb->rc_storylen[0])
			fprintf(fp_serials, "\tSL=|%s|\n", newdb->rc_storylen);
		fprintf(fp_serials, "}\n");
		newdb = newdb->rc_next;
	}

	/*
	 * Books
	 */
	newpub = phead;
	while(newpub) {
		if (newpub->rp_dontuse) {
			newpub = newpub->rp_next;
			continue;
		}
		fprintf(fp_books, "%s {\n", newpub->rp_title);
		fprintf(fp_books, "\tAE=|%s|\n", newpub->rp_author);
		fprintf(fp_books, "\tYR=|%s|\n", newpub->rp_year);
		fprintf(fp_books, "\tAB=|%s|\n", newpub->rp_abbreviation);
		if (newpub->rp_isbn[0])
			fprintf(fp_books, "\tIS=|%s|\n", newpub->rp_isbn);
		if (newpub->rp_publisher[0])
			fprintf(fp_books, "\tPB=|%s|\n", newpub->rp_publisher);
		if (newpub->rp_price[0])
			fprintf(fp_books, "\tPR=|%s|\n", newpub->rp_price);
		if (newpub->rp_pages[0])
			fprintf(fp_books, "\tPP=|%s|\n", newpub->rp_pages);
		if (newpub->rp_type[0])
			fprintf(fp_books, "\tTP=|%s|\n", newpub->rp_type);
		if (newpub->rp_notes[0])
			fprintf(fp_books, "\tNT=|%s|\n", newpub->rp_notes);
		fprintf(fp_books, "}\n");
		newpub = newpub->rp_next;
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

	search_file("NOVELS");
	olddb_novels = head;

	search_file("SHORTFICTION");
	olddb_shortfiction = head;

	head = NULL;
	search_file("ANTHOLOGIES");
	olddb_anthologies = head;

	head = NULL;
	search_file("COLLECTIONS");
	olddb_collections = head;

	head = NULL;
	search_file("ESSAYS");
	olddb_essays = head;

	head = NULL;
	search_file("SERIALS");
	olddb_serials = head;

	search_file2("BOOKS");

	fp_novels 	= fopen("NOVELS.new", "wb+");
	fp_shortfiction = fopen("SHORTFICTION.new", "wb+");
	fp_anthologies  = fopen("ANTHOLOGIES.new", "wb+");
	fp_collections  = fopen("COLLECTIONS.new", "wb+");
	fp_essays       = fopen("ESSAYS.new", "wb+");
	fp_serials      = fopen("SERIALS.new", "wb+");
	fp_books        = fopen("BOOKS.new", "wb+");

	read_keymap("KEYMAP");
	remove_keys();
	output_files();
	return(0);
}
