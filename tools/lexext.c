/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *	 ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)sort.c	1.7	06/10/97 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
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
recomp_t *head2 = NULL;


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
int	total_records;
int	records_per_tick;
int	records_complete;

extern object_t	*Objlist;
extern char	*optarg;

char LOWER1[256];
char LOWER2[256];

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

	total_records = 0;
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
		total_records++;
	}
	Objlist = NULL;
	fprintf(stderr, "Total Records: %d\n", total_records);
	records_per_tick = total_records / 10;
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

float
similar2(string1, string2)
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


int
similar(char *string1, char *string2, char *lower1, char *lower2)
{
	char *tmp_fm;
	char *tmp_to;
	char	input;
	int	substrings1, substrings2;
	int	loop1, loop2;
	char	*ptr1, *ptr2;

	if (lower1[0] == 0) {
		tmp_fm = string1;
		tmp_to = lower1;
		while (*tmp_fm) {
			input = tolower( *tmp_fm );
			if (input != ' ') {
				*tmp_to = input;
				tmp_to++;
			}
			tmp_fm++;
		}
		*tmp_to = 0;
	}

	if (lower2[0] == 0) {
		tmp_fm = string2;
		tmp_to = lower2;
		while (*tmp_fm) {
			input = tolower( *tmp_fm );
			if (input != ' ') {
				*tmp_to = input;
				tmp_to++;
			}
			tmp_fm++;
		}
		if (tmp_to == NULL) {
			fatal_error = 1;
			return(0);
		} else {
			*tmp_to = 0;
		}
	}

	if (strcmp(lower1, lower2) == 0) {
		return(1);
	} else {
		if (similar2(lower1, lower2) > 0.85 ) {
			return(1);
		}
	}

	strcpy(LOWER1, lower1);
	strcpy(LOWER2, lower2);

	substrings1 = 1;
	for (loop1=0; loop1<strlen(LOWER1); loop1++) {
		if ((LOWER1[loop1] == '^') || (LOWER1[loop1] == '+')) {
			LOWER1[loop1] = 0;
			substrings1++;
		}
	}
	substrings2 = 1;
	for (loop2=0; loop2<strlen(LOWER2); loop2++) {
		if ((LOWER2[loop2] == '^') || (LOWER2[loop2] == '+')) {
			LOWER2[loop2] = 0;
			substrings2++;
		}
	}

	if ((substrings1 == 1) && (substrings2 == 1))
		return(0);

	ptr1 = LOWER1;
	for(loop1=0; loop1<substrings1; loop1++) {
		ptr2 = LOWER2;
		for(loop2=0; loop2<substrings2; loop2++) {
			if (similar2(ptr1, ptr2) > 0.85 ) {
				return(1);
			}
			while( *ptr2 != 0)
				ptr2++;
			ptr2++;
		}
		while( *ptr1 != 0)
			ptr1++;
		ptr1++;
	}

	return(0);
}


void
find_dups()
{
	recomp_t	*current;
	recomp_t	*probe;
	int		found;

	fprintf(stderr,"PASS 1:\n");
	records_complete = 0;
	current = head;
	while(current) {
		probe = head2;
		while(probe) {
			found = 0;
#ifdef EXACT_TITLE
			if ( strcmp(current->rc_title, probe->rc_title) == 0) {
#else
			if (similar(current->rc_title, probe->rc_title, current->rc_lowt, probe->rc_lowt)) {
#endif
#ifdef EXACT_AUTHOR
				if ( strcmp(current->rc_author, probe->rc_author) == 0) {
#else
				if (similar(current->rc_author, probe->rc_author, current->rc_lowa, probe->rc_lowa)) {
#endif
					if ( strcmp(current->rc_title, probe->rc_title) ||
					     strcmp(current->rc_author, probe->rc_author)) {
						printf("--------------------------------\n");
						found = 1;
						printf("[%s] by [%s] (NEWDB) AND\n", 
							current->rc_title, current->rc_author);
						printf("[%s] by [%s] (ISFDB)\n", 
							probe->rc_title, probe->rc_author);
#ifdef NOTNOW
						printf("%s|%s|%s|%s\n",
							current->rc_title, current->rc_author,
							probe->rc_title, probe->rc_author);
#endif
					}
				}
			} 

			if ( !found && strstr(current->rc_title, ":") && probe->rc_series) {
				char *ptr1, *ptr2;
				char bogus1[128];
				char bogus2[128];

				ptr1 = (char *)strstr(current->rc_title, ":");
				*ptr1 = 0;
				ptr2 = ptr1+1;
				while(*ptr2 == ' ')
					ptr2++;
				bogus1[0] = bogus2[0] = 0;
				if (similar(ptr2, probe->rc_title, bogus1, bogus2)) {
					bogus1[0] = bogus2[0] = 0;
					if (similar(current->rc_title, probe->rc_series, bogus1, bogus2)) {
						*ptr1 = ':';
						found = 1;
						printf("--------------------------------\n");
						printf("[%s] by [%s] AND\n", current->rc_title, current->rc_author);
						printf("[%s] by [%s]\n", probe->rc_title, probe->rc_author);
					}
				}
				*ptr1 = ':';
			} 

			if (!found && strstr(probe->rc_title, ":") && current->rc_series) {
				char *ptr1, *ptr2;
				char bogus1[128];
				char bogus2[128];

				ptr1 = (char *)strstr(probe->rc_title, ":");
				*ptr1 = 0;
				ptr2 = ptr1+1;
				while(*ptr2 == ' ')
					ptr2++;
				bogus1[0] = bogus2[0] = 0;
				if (similar(ptr2, probe->rc_title, bogus1, bogus2)) {
					bogus1[0] = bogus2[0] = 0;
					if (similar(probe->rc_title, current->rc_series, bogus1, bogus2)) {
						*ptr1 = ':';
						printf("--------------------------------\n");
						printf("[%s] by [%s] AND\n", probe->rc_title, current->rc_author);
						printf("[%s] by [%s]\n", probe->rc_title, probe->rc_author);
					}
				}
				*ptr1 = ':';
			}

			if (fatal_error) {
				fprintf(stderr, "Got fatal on [%s] vs [%s]\n", 
					current->rc_title, probe->rc_title);
				fprintf(stderr, "Got fatal on [%s] vs [%s]\n", 
					current->rc_lowt, probe->rc_lowt);
				fprintf(stderr, "Authors [%s] vs [%s]\n", 
					current->rc_author, probe->rc_author);
				fprintf(stderr, "Authors [%s] vs [%s]\n", 
					current->rc_lowa, probe->rc_lowa);
				exit(1);
			}
			probe = probe->rc_next;
		}
		current = current->rc_next;
		records_complete++;
		if (records_per_tick) {
			if ((records_complete % records_per_tick) == 0) {
				fprintf(stderr, "   %02d percent complete\n", 10 * records_complete/records_per_tick);
			}
		}
	}
}

void
find_dups2()
{
	recomp_t	*current;
	recomp_t	*probe;

	fprintf(stderr,"PASS 2:\n");
	records_complete = 0;
	current = head;
	while(current) {
		probe = head2;
		while(probe) {
			if (similar(current->rc_title, probe->rc_title, current->rc_lowt, probe->rc_lowt)) {
				if ( (strcmp(current->rc_year, "0") == 0) &&
				     (strcmp(probe->rc_year, "0") == 0) ) {
					probe = probe->rc_next;
					continue;
				}
				if ( (strcmp(current->rc_year, probe->rc_year) == 0) &&
				      strcmp(current->rc_title, probe->rc_title)) {
					printf("--------------------------------\n");
					printf("[%s] by [%s] (NEWDB) AND\n", 
						current->rc_title, current->rc_author);
					printf("[%s] by [%s] (ISFDB)\n", 
						probe->rc_title, probe->rc_author);
				}
			}
			probe = probe->rc_next;
		}
		current = current->rc_next;
		records_complete++;
		if (records_per_tick) {
			if ((records_complete % records_per_tick) == 0) {
				fprintf(stderr, "   %02d percent complete\n", 10 * records_complete/records_per_tick);
			}
		}
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
		printf("usage: lexext <isfdb file> <new file>\n");
		exit(1);
	}

	search_file(argv[1]);
	head2 = head;
	head = NULL;

	search_file(argv[2]);

	printf("============= similar title and author ============== \n");
	find_dups();
	printf("============= similar titles in the same year ============== \n");
#ifdef NOTHANKS
	find_dups2();
#endif
	return(0);
}
