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

typedef struct repub {
	char		*rp_title;
	char		*rp_abbreviation;
	char		*rp_author;
	char		*rp_year;
	char		*rp_isbn;
	char		*rp_publisher;
	char		*rp_price;
	char		*rp_pages;
	char		*rp_type;
	char		*rp_cover;
	char		*rp_notes;
	struct repub    *rp_next;
} repub_t;

recomp_t *head  = NULL;
recomp_t *tail  = NULL;
recomp_t *head2 = NULL;
repub_t  *phead = NULL;
repub_t  *ptail = NULL;
extern pub_t *pub_list;
extern pub_t *pub_end;
FILE *keymap_fp;


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
char	subpubs1[100][20];
char	subpubs2[100][20];
int	numpubs1;
int	numpubs2;
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
setpubs(char *pubtags, int which)
{
	char *ptr1;
	char *ptr2;

	if (which == 1) {
		numpubs1 = 0;
	} else {
		numpubs2 = 0;
	}
	ptr1 = pubtags;
	ptr2 = (char *)strstr(ptr1, ",");
	while(ptr2) {
		*ptr2 = 0;
		if (which == 1) {
			strcpy( subpubs1[numpubs1++], ptr1);
		} else {
			strcpy( subpubs2[numpubs2++], ptr1);
		}
		ptr1 = ptr2 + 1;
		ptr2 = (char *)strstr(ptr1, ",");
	}
	if (which == 1) {
		strcpy( subpubs1[numpubs1++], ptr1);
	} else {
		strcpy( subpubs2[numpubs2++], ptr1);
	}
}


pub_t *
get_probe1(char *tag)
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


repub_t *
get_probe2(char *tag)
{
	repub_t	*tmp;

	tmp = phead;
	while(tmp) {
		if (strcmp(tag, tmp->rp_abbreviation) == 0) {
			return(tmp);
		}
		tmp = tmp->rp_next;
	}
	return(tmp);
}


int
isbncheck(char *isbn)
{
	char *ptr;
	char newisbn[64];
	int  index = 0;
	int  loop;
	int  sum = 0;
	char check;

	/*
	 * First, get rid of the dashes
	 */
	ptr = isbn;
	if (ptr == 0) {
		return(1);
	}

	while(*ptr) {
		if (*ptr != '-') {
			newisbn[index++] = *ptr;
		}
		ptr++;
	}
	newisbn[index] = 0;
	if (strlen(newisbn) != 10) {
		return(1);
	}

	for(loop=0; loop<9; loop++) {
		int digit;

		digit = newisbn[loop] - '0';
		sum += (loop+1) * digit;
	}
	sum = sum%11;
	if (sum > 9) {
		check = 'X';
	} else {
		check = sum + '0';
	}
	if (newisbn[9] != check) {
		return(1);
	} else {
		return(0);
	}
}

float
similar(string1, string2)
	char *string1;
	char *string2;
{
	char *tmp1;
	char *tmp2;
	int  total = 0;
	int  length;
	float ratio;

	tmp1 = string1;
	tmp2 = string2;
	while((*tmp1) && (*tmp2)) {
		if (*tmp1 == *tmp2)
			total++;
		tmp1++;
		tmp2++;
	}

	/*
	 * Align the tmp pointers to the end of the strings
	 */
	tmp1 = string1;
	while(*tmp1)
		tmp1++;
	tmp1--;
	tmp2 = string2;
	while(*tmp2)
		tmp2++;
	tmp2--;

	while((*tmp1) && (*tmp2)) {
		if (*tmp1 == *tmp2)
			total++;
		tmp1--;
		tmp2--;
	}

	if (strlen(string1) > strlen(string2))
		length = strlen(string1);
	else
		length = strlen(string2);

	ratio = ((float)total)/length;
	return(ratio);
}

void
compare_pubs(char *newdb, char *isfdb)
{
	pub_t		*probe1;
	repub_t		*probe2;
	int		same;
	int		needline = 1;
	int		docheck = 0;

	probe1 = get_probe1(isfdb);
	probe2 = get_probe2(newdb);
	if (probe1 == NULL)  {
		printf("-----------------------------------------------\n");
		printf("probe1 NULL for %s\n", isfdb);
		return;
	}
	if (probe2 == NULL)  {
		printf("-----------------------------------------------\n");
		printf("probe2 NULL for %s\n", newdb);
		return;
	}

	/*
	 * Since there can be multiple publications, try to determine
	 * if the check should be done or not.
	 *
	 * o If the isfdb year is zero:
	 *	- and there's no publisher - check
	 *      - and the price diff > 1.50 - check
	 *      - and the publishers are similar - check
	 *	- otherwise, don't check
	 *
	 * o If the ISBNs match, then all data must match.
	 *	- and the year difference < 2
	 *	- and the price diff < 1.50, then check
	 *
	 * o if the ISBNs don't match, and at least one of
	 *   the ISBNs is bogus, and the bogus ISBN is not
	 *   an SFBC number, then check.
	 *
	 * o If the years are the same, we *might* be talking about
	 *   the same publication.
	 */
	if ( probe1->pu_year[0] && (strcmp(probe1->pu_year, "0") == 0) ) {
		if ( !probe1->pu_publisher[0]) {
			docheck = 1;
		} else if (probe1->pu_price[0] && probe2->rp_price[0]) {
			float price1, price2;

			sscanf( (char *)&(probe1->pu_price[1]), "%f", &price1);
			sscanf( (char *)&(probe2->rp_price[1]), "%f", &price2);
			docheck = 1;
			if (price1 > price2) {
				if ( (price1-price2) > 1.50)
					docheck = 0;
			} else {
				if ( (price2-price1) > 1.50)
					docheck = 0;
			}
		} else if (probe1->pu_publisher[0] && probe2->rp_publisher[0] &&
			(similar(probe1->pu_publisher, probe2->rp_publisher) > 0.85) ) {
			docheck = 1;
		} else {
			docheck = 0;
		}
	} else if (probe1->pu_isbn[0] && probe2->rp_isbn[0] && 
		(strcmp(probe1->pu_isbn, probe2->rp_isbn) == 0) ) {
		docheck = 0;
		if (probe1->pu_price[0] && probe2->rp_price[0]) {
			float price1, price2;

			sscanf( (char *)&(probe1->pu_price[1]), "%f", &price1);
			sscanf( (char *)&(probe2->rp_price[1]), "%f", &price2);
			docheck = 1;
			if (price1 > price2) {
				if ( (price1-price2) > 1.50)
					docheck = 0;
			} else {
				if ( (price2-price1) > 1.50)
					docheck = 0;
			}
		} else {
			docheck = 1;
		}
		if (probe1->pu_year[0] && probe2->rp_year[0]) {
			int year1, year2;

			sscanf(probe1->pu_year, "%d", &year1);
			sscanf(probe2->rp_year, "%d", &year2);
			docheck = 1;
			if (year1 > year2) {
				if ( (year1-year2) > 2)
					docheck = 0;
			} else {
				if ( (year2-year1) > 2)
					docheck = 0;
			}
		} else {
			docheck = 1;
		}
	} else if (probe1->pu_isbn[0] && probe2->rp_isbn[0] && 
	      strcmp(probe1->pu_isbn, probe2->rp_isbn)) {
		int ret1, ret2;

		/*
		 * If the ISBNs are different, then either at
		 * least one ISBN is busted, or they are different
		 * books. Try to determine if they are different
		 * books in order to reduce the false positives.
		 *
		 * If an ISBN is busted, OR the price is identical,
		 * then do the check.
		 */
		ret1 = isbncheck(probe1->pu_isbn);
		ret2 = isbncheck(probe2->rp_isbn);

		/*
		 * SFBC books have bad ISBNs by default
		 */
		if (ret1 && (probe1->pu_isbn[0] == '#')) {
			ret1 = 0;
		}
		if (ret2 && (probe2->rp_isbn[0] == '#')) {
			ret2 = 0;
		}

		if ( ret1 || ret2 ) {
			docheck = 1;
		}

	} else if (probe1->pu_year[0] && probe2->rp_year[0] && 
		(strcmp(probe1->pu_year, probe2->rp_year) == 0) ) {

		if (probe1->pu_publisher[0] && probe2->rp_publisher[0] &&
			(similar(probe1->pu_publisher, probe2->rp_publisher) > 0.85) ) {
			docheck = 1;
		}
	}

	if (!docheck)
		return;

#ifdef EXPERIMENTAL
	/*
	 * Check the title
	 */
	if (probe1->pu_title[0] && probe2->rp_title[0]) {
		if (strcmp(probe1->pu_title, probe2->rp_title)) {
			if (needline) {
				needline = 0;
				printf("-----------------------------------------------\n");
			}
			printf("isfdb [%s] has title [%s]\n",
			probe1->pu_abbreviation, probe1->pu_title);
			printf("newdb [%s] has title [%s]\n",
			probe2->rp_abbreviation, probe2->rp_title);
		}
	} else if (!probe1->pu_title[0] && probe2->rp_title[0]) {
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("isfdb [%s] needs title %s (%s)\n",
			probe1->pu_abbreviation, 
			probe2->rp_title,
			probe2->rp_abbreviation);
	} else if (probe1->pu_title[0] && !probe2->rp_title[0]) {
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("newdb [%s] needs title %s (%s)\n",
			probe2->rp_abbreviation, 
			probe1->pu_title,
			probe1->pu_abbreviation);
	}
#endif

	/*
	 * Check the author
	 */
	same = 1;
	if (probe1->pu_author[0] && probe2->rp_author[0]) {
		if (strcmp(probe1->pu_author, probe2->rp_author)) {
			same = 0;
			if (needline) {
				needline = 0;
				printf("-----------------------------------------------\n");
			}
			printf("isfdb [%s] has author [%s]\n",
			probe1->pu_abbreviation, probe1->pu_author);
			printf("newdb [%s] has author [%s]\n",
			probe2->rp_abbreviation, probe2->rp_author);
		}
	} else if (!probe1->pu_author[0] && probe2->rp_author[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("isfdb [%s] needs author AE=|%s| (%s)\n",
			probe1->pu_abbreviation, 
			probe2->rp_author,
			probe2->rp_abbreviation);
	} else if (probe1->pu_author[0] && !probe2->rp_author[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("newdb [%s] needs author AE=|%s| (%s)\n",
			probe2->rp_abbreviation, 
			probe1->pu_author,
			probe1->pu_abbreviation);
	}

	/*
	 * Check the year
	 */
	if (probe1->pu_year[0] && probe2->rp_year[0]) {
		if (strcmp(probe1->pu_year, probe2->rp_year)) {
			same = 0;
			if (needline) {
				needline = 0;
				printf("-----------------------------------------------\n");
			}
			printf("isfdb [%s] has year [%s]\n",
			probe1->pu_abbreviation, probe1->pu_year);
			printf("newdb [%s] has year [%s]\n",
			probe2->rp_abbreviation, probe2->rp_year);
		}
	} else if (!probe1->pu_year[0] && probe2->rp_year[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("isfdb [%s] needs year YR=|%s|\n",
			probe1->pu_abbreviation, probe2->rp_year);
	} else if (probe1->pu_year[0] && !probe2->rp_year[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("newdb [%s] needs year YR=|%s|\n",
			probe2->rp_abbreviation, probe1->pu_year);
	}

	/*
	 * Check the isbn
	 */
	if (probe1->pu_isbn[0] && probe2->rp_isbn[0]) {
		if (strcmp(probe1->pu_isbn, probe2->rp_isbn)) {
			int ret1, ret2;

			same = 0;
			if (needline) {
				needline = 0;
				printf("-----------------------------------------------\n");
			}

			ret1 = isbncheck(probe1->pu_isbn);
			ret2 = isbncheck(probe2->rp_isbn);

			printf("isfdb [%s] has isbn [%s] ",
			probe1->pu_abbreviation, probe1->pu_isbn);
			if (ret1) {
				printf(" << Bad ISBN\n");
			} else {
				printf("\n");
			}
			printf("newdb [%s] has isbn [%s] ",
			probe2->rp_abbreviation, probe2->rp_isbn);
			if (ret2) {
				printf(" << Bad ISBN\n");
			} else {
				printf("\n");
			}
		}
	} else if (!probe1->pu_isbn[0] && probe2->rp_isbn[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("isfdb [%s] needs isbn IS=|%s|\n",
			probe1->pu_abbreviation, probe2->rp_isbn);
	} else if (probe1->pu_isbn[0] && !probe2->rp_isbn[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("newdb [%s] needs isbn IS=|%s|\n",
			probe2->rp_abbreviation, probe1->pu_isbn);
	}

	/*
	 * Check the publisher
	 */
	if (probe1->pu_publisher[0] && probe2->rp_publisher[0]) {
		if (strcmp(probe1->pu_publisher, probe2->rp_publisher)) {
			same = 0;
			if (needline) {
				needline = 0;
				printf("-----------------------------------------------\n");
			}
			printf("isfdb [%s] has publisher [%s]\n",
			probe1->pu_abbreviation, probe1->pu_publisher);
			printf("newdb [%s] has publisher [%s]\n",
			probe2->rp_abbreviation, probe2->rp_publisher);
		}
	} else if (!probe1->pu_publisher[0] && probe2->rp_publisher[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("isfdb [%s] needs publisher PB=|%s|\n",
			probe1->pu_abbreviation, probe2->rp_publisher);
	} else if (probe1->pu_publisher[0] && !probe2->rp_publisher[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("newdb [%s] needs publisher PB=|%s|\n",
			probe2->rp_abbreviation, probe1->pu_publisher);
	}

	/*
	 * Check the price
	 */
	if (probe1->pu_price[0] && probe2->rp_price[0]) {
		if (strcmp(probe1->pu_price, probe2->rp_price)) {
			same = 0;
			if (needline) {
				needline = 0;
				printf("-----------------------------------------------\n");
			}
			printf("isfdb [%s] has price [%s]\n",
			probe1->pu_abbreviation, probe1->pu_price);
			printf("newdb [%s] has price [%s]\n",
			probe2->rp_abbreviation, probe2->rp_price);
		}
	} else if (!probe1->pu_price[0] && probe2->rp_price[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("isfdb [%s] needs price PR=|%s|\n",
			probe1->pu_abbreviation, probe2->rp_price);
	} else if (probe1->pu_price[0] && !probe2->rp_price[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("newdb [%s] needs price PR=|%s|\n",
			probe2->rp_abbreviation, probe1->pu_price);
	}

	/*
	 * Check the pages
	 */
	if (probe1->pu_pages[0] && probe2->rp_pages[0]) {
		if (strcmp(probe1->pu_pages, probe2->rp_pages)) {
			same = 0;
			if (needline) {
				needline = 0;
				printf("-----------------------------------------------\n");
			}
			printf("isfdb [%s] has pages [%s]\n",
			probe1->pu_abbreviation, probe1->pu_pages);
			printf("newdb [%s] has pages [%s]\n",
			probe2->rp_abbreviation, probe2->rp_pages);
		}
	} else if (!probe1->pu_pages[0] && probe2->rp_pages[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("isfdb [%s] needs pages PP=|%s|\n",
			probe1->pu_abbreviation, probe2->rp_pages);
	} else if (probe1->pu_pages[0] && !probe2->rp_pages[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("newdb [%s] needs pages PP=|%s|\n",
			probe2->rp_abbreviation, probe1->pu_pages);
	}

	/*
	 * Check the type
	 */
	if (probe1->pu_type[0] && probe2->rp_type[0]) {
		if (strcmp(probe1->pu_type, probe2->rp_type)) {
			same = 0;
			if (needline) {
				needline = 0;
				printf("-----------------------------------------------\n");
			}
			printf("isfdb [%s] has type [%s]\n",
			probe1->pu_abbreviation, probe1->pu_type);
			printf("newdb [%s] has type [%s]\n",
			probe2->rp_abbreviation, probe2->rp_type);
		}
	} else if (!probe1->pu_type[0] && probe2->rp_type[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("isfdb [%s] needs type TP=|%s|\n",
			probe1->pu_abbreviation, probe2->rp_type);
	} else if (probe1->pu_type[0] && !probe2->rp_type[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("newdb [%s] needs type TP=|%s|\n",
			probe2->rp_abbreviation, probe1->pu_type);
	}

	/*
	 * Check the cover
	 */
	if (probe1->pu_cover[0] && probe2->rp_cover[0]) {
		if (strcmp(probe1->pu_cover, probe2->rp_cover)) {
			same = 0;
			if (needline) {
				needline = 0;
				printf("-----------------------------------------------\n");
			}
			printf("isfdb [%s] has cover [%s]\n",
			probe1->pu_abbreviation, probe1->pu_cover);
			printf("newdb [%s] has cover [%s]\n",
			probe2->rp_abbreviation, probe2->rp_cover);
		}
	} else if (!probe1->pu_cover[0] && probe2->rp_cover[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("isfdb [%s] needs cover CV=|%s|\n",
			probe1->pu_abbreviation, probe2->rp_cover);
	} else if (probe1->pu_cover[0] && !probe2->rp_cover[0]) {
		same = 0;
		if (needline) {
			needline = 0;
			printf("-----------------------------------------------\n");
		}
		printf("newdb [%s] needs cover CV=|%s|\n",
			probe2->rp_abbreviation, probe1->pu_cover);
	}

	if (same) {
		fprintf(keymap_fp, "%s:%s\n", probe2->rp_abbreviation, probe1->pu_abbreviation);
	}

}


void
check_pubs()
{
	recomp_t	*current;
	recomp_t	*probe;
	int		loop1;
	int		loop2;

	/*
	 * Start with the NEWDB list
	 */
	current = head;
	while(current) {

		/*
		 * Set up the publication tags/keys from
		 * the new database  for this entry.
		 */
		if (current->rc_pubtags == NULL ) {
			current = current->rc_next;
			continue;
		} else {
			setpubs(current->rc_pubtags, 1);
		}

		/*
		 * Find the associated entry in the ISFDB
		 */
		probe = head2;
		while(probe) {
			if (strcmp(current->rc_title, probe->rc_title) == 0) {
				break;
			}
			probe = probe->rc_next;
		}
		if (probe == NULL) {
			current = current->rc_next;
			continue;
		}

		/*
		 * If there is no pubdata for the ISFDB
		 * emit the data for a copy
		 */
		if (probe->rc_pubtags[0]) {
			setpubs(probe->rc_pubtags, 2);
		} else {
#ifdef REMOVE
			printf("-----------------------------------------------------\n");
			printf("ISFDB: [%s] needs pub data:\n", probe->rc_title);
#endif
			current = current->rc_next;
			continue;
		}


		for(loop1=0; loop1<numpubs1; loop1++) {
			for(loop2=0; loop2<numpubs2; loop2++) {
				compare_pubs(subpubs1[loop1], subpubs2[loop2]);
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
	recomp_t	*tmp;
	repub_t		*ptmp;
	char		path[256];

	if (argc != 4) {
		printf("usage: pubext <isfdb file> <new file> <new BOOKS>\n");
		exit(1);
	}

	keymap_fp = fopen("KEYMAP", "wb+");
	if (keymap_fp == NULL) {
		fprintf(stderr, "Couldn't open keymap file\n");
	}

	search_file(argv[1]);
	head2 = head;
	head  = NULL;

	search_file(argv[2]);
	search_file2(argv[3]);

	strcpy(path, BASE);
	strcat(path, "/dbase.compiled");
	result = chdir(path);
        if (result != 0) {
                printf("CHDIR to %s failed\n", path);
                exit(1);
        }
	load_pubs(F_NOOPT, NULL);

	check_pubs();

	if (keymap_fp) {
		fclose(keymap_fp);
	}

	return(0);
}
