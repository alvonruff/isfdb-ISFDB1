/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *	 ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)sort.c	1.6	06/10/97 SFdbase";

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
	struct recomp   *rc_next;
} recomp_t;

recomp_t *head = NULL;
recomp_t *tail = NULL;
extern pub_t *pub_list;
extern pub_t *pub_end;


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

int	fatal_error = 0;

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

char subpubs[100][20];
int  numpubs;

void
setpubs(char *pubtags)
{
	char *ptr1;
	char *ptr2;

	numpubs = 0;
	ptr1 = pubtags;
	ptr2 = (char *)strstr(ptr1, ",");
	while(ptr2) {
		*ptr2 = 0;
		strcpy( subpubs[numpubs++], ptr1);
		ptr1 = ptr2 + 1;
		ptr2 = (char *)strstr(ptr1, ",");
	}
	strcpy( subpubs[numpubs++], ptr1);
}


pub_t *
get_probe(char *tag)
{
	pub_t	*tmp;

	tmp = pub_list;
	while(tmp) {
		if (strcmp(tag, tmp->pu_abbreviation) == 0) {
			return(tmp);
		}
		tmp = tmp->pu_next;
	}
	return(tmp);
}


void
compare_pubs(char *pub1, char *pub2)
{
	pub_t		*probe1;
	pub_t		*probe2;
	int		same;

	probe1 = get_probe(pub1);
	probe2 = get_probe(pub2);

	if (probe1 == NULL) {
		printf("-----------------------------------\n");
		printf("probe1 for [%s] is NULL\n", pub1);
		return;
	}
	if (probe2 == NULL) {
		printf("-----------------------------------\n");
		printf("probe2 for [%s] is NULL\n", pub2);
		return;
	}

	/*
	 * Check the author
	 */
	same = 1;
	if (probe1->pu_author && probe2->pu_author) {
		if (strcmp(probe1->pu_author, probe2->pu_author)) {
			same = 0;
			printf("-----------------------------------\n");
			printf("[%s] has author [%s]\n",
			probe1->pu_abbreviation, probe1->pu_author);
			printf("[%s] has author [%s]\n",
			probe2->pu_abbreviation, probe2->pu_author);
		}
	} else if (!probe1->pu_author && probe2->pu_author) {
		printf("-----------------------------------\n");
		printf("[%s] needs author [%s] (%s)\n",
			probe1->pu_abbreviation, 
			probe2->pu_author,
			probe2->pu_abbreviation);
	} else if (probe1->pu_author && !probe2->pu_author) {
		printf("-----------------------------------\n");
		printf("[%s] needs author [%s] (%s)\n",
			probe2->pu_abbreviation, 
			probe1->pu_author,
			probe1->pu_abbreviation);
	}

	/*
	 * Check the year
	 */
	if (probe1->pu_year && probe2->pu_year) {
		if (strcmp(probe1->pu_year, probe2->pu_year)) {
			same = 0;
		}
#ifdef REMOVE
	} else if (!probe1->pu_year && probe2->pu_year) {
		printf("[%s] needs year [%s]\n",
			probe1->pu_abbreviation, probe2->pu_year);
	} else if (probe1->pu_year && !probe2->pu_year) {
		printf("[%s] needs year [%s]\n",
			probe2->pu_abbreviation, probe1->pu_year);
#endif
	}

	/*
	 * Check the isbn
	 */
	if (probe1->pu_isbn && probe2->pu_isbn) {
		if (strcmp(probe1->pu_isbn, probe2->pu_isbn)) {
			same = 0;
		}
#ifdef REMOVE
	} else if (!probe1->pu_isbn && probe2->pu_isbn) {
		printf("[%s] needs isbn [%s]\n",
			probe1->pu_abbreviation, probe2->pu_isbn);
	} else if (probe1->pu_isbn && !probe2->pu_isbn) {
		printf("[%s] needs isbn [%s]\n",
			probe2->pu_abbreviation, probe1->pu_isbn);
#endif
	}

	/*
	 * Check the publisher
	 */
	if (probe1->pu_publisher && probe2->pu_publisher) {
		if (strcmp(probe1->pu_publisher, probe2->pu_publisher)) {
			same = 0;
		}
#ifdef REMOVE
	} else if (!probe1->pu_publisher && probe2->pu_publisher) {
		printf("[%s] needs publisher [%s]\n",
			probe1->pu_abbreviation, probe2->pu_publisher);
	} else if (probe1->pu_publisher && !probe2->pu_publisher) {
		printf("[%s] needs publisher [%s]\n",
			probe2->pu_abbreviation, probe1->pu_publisher);
#endif
	}

	/*
	 * Check the price
	 */
	if (probe1->pu_price && probe2->pu_price) {
		if (strcmp(probe1->pu_price, probe2->pu_price)) {
			same = 0;
		}
#ifdef REMOVE
	} else if (!probe1->pu_price && probe2->pu_price) {
		printf("[%s] needs price [%s]\n",
			probe1->pu_abbreviation, probe2->pu_price);
	} else if (probe1->pu_price && !probe2->pu_price) {
		printf("[%s] needs price [%s]\n",
			probe2->pu_abbreviation, probe1->pu_price);
#endif
	}

	/*
	 * Check the pages
	 */
	if (probe1->pu_pages && probe2->pu_pages) {
		if (strcmp(probe1->pu_pages, probe2->pu_pages)) {
			same = 0;
		}
#ifdef REMOVE
	} else if (!probe1->pu_pages && probe2->pu_pages) {
		printf("[%s] needs pages [%s]\n",
			probe1->pu_abbreviation, probe2->pu_pages);
	} else if (probe1->pu_pages && !probe2->pu_pages) {
		printf("[%s] needs pages [%s]\n",
			probe2->pu_abbreviation, probe1->pu_pages);
#endif
	}

	/*
	 * Check the type
	 */
	if (probe1->pu_type && probe2->pu_type) {
		if (strcmp(probe1->pu_type, probe2->pu_type)) {
			same = 0;
		}
#ifdef REMOVE
	} else if (!probe1->pu_type && probe2->pu_type) {
		printf("[%s] needs type [%s]\n",
			probe1->pu_abbreviation, probe2->pu_type);
	} else if (probe1->pu_type && !probe2->pu_type) {
		printf("[%s] needs type [%s]\n",
			probe2->pu_abbreviation, probe1->pu_type);
#endif
	}

	/*
	 * Check the cover
	 */
	if (probe1->pu_cover && probe2->pu_cover) {
		if (strcmp(probe1->pu_cover, probe2->pu_cover)) {
			same = 0;
		}
#ifdef REMOVE
	} else if (!probe1->pu_cover && probe2->pu_cover) {
		printf("[%s] needs cover [%s]\n",
			probe1->pu_abbreviation, probe2->pu_cover);
	} else if (probe1->pu_cover && !probe2->pu_cover) {
		printf("[%s] needs cover [%s]\n",
			probe2->pu_abbreviation, probe1->pu_cover);
#endif
	}

	if (same) {
		printf("-----------------------------------\n");
		printf("[%s] and [%s] are duplicates\n",
			probe1->pu_abbreviation, 
			probe2->pu_abbreviation);
	}
}


void
check_pubs()
{
	recomp_t	*current;
	int		loop1;
	int		loop2;

	current = head;
	while(current) {

		if (current->rc_pubtags == NULL ) {
			current = current->rc_next;
			continue;
		} else {
			setpubs(current->rc_pubtags);
			if (numpubs == 1) {
				current = current->rc_next;
				continue;
			}
		}

		for(loop1=0; loop1<numpubs; loop1++) {
			for(loop2=loop1+1; loop2<numpubs; loop2++) {
				compare_pubs(subpubs[loop1], subpubs[loop2]);
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
	search_t	*tmp;
	char		path[256];

	if ((argc < 2) || (argc > 3)) {
		printf("usage: pubint <file> [year]\n");
		exit(1);
	}

	search_file(argv[1]);

	strcpy(path, BASE);
	strcat(path, "/dbase.compiled");
	result = chdir(path);
        if (result != 0) {
                printf("CHDIR to %s failed\n", path);
                exit(1);
        }

	if (argc == 3) {
		load_pubs(F_YEAR, argv[2]);
	} else {
		load_pubs(F_NOOPT, NULL);
	}

	check_pubs();
	return(0);
}
