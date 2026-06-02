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
	char		*rc_translator;
	char		*rc_notes;
	char		*rc_synopsis;
	struct recomp   *rc_next;
} recomp_t;

recomp_t *head = NULL;
recomp_t *tail = NULL;

#define OPTARGS "n"

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
			} else if ( strcmp(targetattr, "TR") == 0) {
				target->rc_translator = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_translator, attr->at_value);
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
		target->rc_translator = NULL;
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
		do_attribute("TR", tmp->ob_attrs, target);
		tmp = tmp->ob_next;
	}
	Objlist = NULL;
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
		if (target->rc_awtags)
			printf("\tTG=|%s|\n", target->rc_awtags);
		if (target->rc_translator)
			printf("\tTR=|%s|\n", target->rc_translator);
		if (target->rc_notes)
			do_note(0, target->rc_notes);
		if (target->rc_synopsis)
			do_note(1, target->rc_synopsis);
		printf("}\n");
		target = target->rc_next;
	}
}

char tmptags[8192];

char *
mergetags(char *tagset1, char *tagset2)
{
	char *tmp1, *ptr1;
	char *tmp2, *ptr2;
	int doit;


	/*
	 * First copy first set of tags into the tagset
	 */
	strcpy(tmptags, tagset1);

	tmp2 = tagset2;
	while(tmp2 && *tmp2) {
		ptr2 = strstr(tmp2, ",");
		if (ptr2) {
			*ptr2 = 0;
		}
		tmp1 = tmptags;
		doit = 1;
		while(tmp1 && *tmp1) {
			ptr1 = strstr(tmp1, ",");
			if (ptr1) {
				*ptr1 = 0;
			}
			if ( strcmp(tmp2, tmp1) == 0) {
				doit = 0;
			}
			if (ptr1) {
				*ptr1 = ',';
				tmp1 = ptr1+1;
			} else {
				tmp1 = NULL;
			}
		}
		if (doit) {
			strcat(tmptags, ",");
			strcat(tmptags, tmp2);
		}
		if (ptr2) {
			*ptr2 = ',';
			tmp2 = ptr2+1;
		} else {
			tmp2 = NULL;
		}
	}
	tmp1 = (char *)malloc( strlen(tmptags) + 1);
	strcpy(tmp1, tmptags);
	return( tmp1 );
}

void
remove_dups()
{
	recomp_t	*current;
	recomp_t	*probe;
	recomp_t	*last;

	current = head;
	while(current) {
		probe = current->rc_next;
		while(probe) {
			if (probe->rc_title == NULL) {
				probe = probe->rc_next;
				continue;
			}
			if (strcmp(current->rc_title, probe->rc_title) == 0) {
				if ( (current->rc_author == NULL) || (probe->rc_author == NULL)) {
					if (current->rc_author == NULL) {
						fprintf(stderr, "Title: %s is missing author\n", current->rc_title);
					} else {
						fprintf(stderr, "Title: %s is missing author\n", probe->rc_title);
					}
					exit(1);
				} else if (strcmp(current->rc_author, probe->rc_author) == 0) {
					
					probe->rc_title = 0;

					/*
					 * Match - merge the entries
					 */
					if (strcmp(current->rc_year, probe->rc_year)) {
						int	year1;
						int	year2;

						if (current->rc_year[0] == '9') {
							year1 = 9999;
						} else {
							sscanf(current->rc_year, "%d", &year1);
						}
						if (probe->rc_year[0] == '9') {
							year2 = 9999;
						} else {
							sscanf(probe->rc_year, "%d", &year2);
						}
						if (doyear && (year1 != year2)) {
							if ((year2 != 0) && (year2 < year1)) {
								current->rc_year = probe->rc_year;
								fprintf(stderr, "Merging year [%s] into (%s)\n", 
									probe->rc_year, current->rc_title);
							} else if (year1 == 0) {
								current->rc_year = probe->rc_year;
								fprintf(stderr, "Merging year [%s] into (%s)\n", 
									probe->rc_year, current->rc_title);
							}
						}

					}

					if ( (current->rc_series == NULL) && (probe->rc_series)) {
						fprintf(stderr, "Merging series [%s] into (%s)\n", 
							probe->rc_series, current->rc_title);
						current->rc_series = probe->rc_series;
					}
					if ( (current->rc_superseries == NULL) && (probe->rc_superseries)) {
						current->rc_superseries = probe->rc_superseries;
						fprintf(stderr, "Merging superseries [%s] into (%s)\n", 
							probe->rc_superseries, current->rc_title);
					}
					if ( (current->rc_seriesnum == NULL) && (probe->rc_seriesnum)) {
						current->rc_seriesnum = probe->rc_seriesnum;
						fprintf(stderr, "Merging seriesnum [%s] into (%s)\n", 
							probe->rc_seriesnum, current->rc_title);
					}
					if ( (current->rc_storylen == NULL) && (probe->rc_storylen)) {
						current->rc_storylen = probe->rc_storylen;
						fprintf(stderr, "Merging storylen [%s] into (%s)\n", 
							probe->rc_storylen, current->rc_title);
					}
					if ( (current->rc_notes == NULL) && (probe->rc_notes)) {
						current->rc_notes = probe->rc_notes;
						fprintf(stderr, "Merging notes [%s] into (%s)\n", 
							probe->rc_notes, current->rc_title);
					} else if ( (current->rc_notes) && (probe->rc_notes)) {
						char *tmp;

						fprintf(stderr, "Concatinating notes [%s] AND [%s] into (%s)\n", 
							current->rc_notes, probe->rc_notes, current->rc_title);
						tmp = (char *)malloc (strlen(current->rc_notes) +
							strlen(probe->rc_notes) +2);
						sprintf(tmp, "%s %s", current->rc_notes, probe->rc_notes);
						current->rc_notes = tmp;
					}
					if ( (current->rc_synopsis == NULL) && (probe->rc_synopsis)) {
						current->rc_synopsis = probe->rc_synopsis;
						fprintf(stderr, "Merging synopsis [%s] into (%s)\n", 
							probe->rc_synopsis, current->rc_title);
					} else if ( (current->rc_synopsis) && (probe->rc_synopsis)) {
						char *tmp;

						fprintf(stderr, "Concatinating synopsis [%s] AND [%s] into (%s)\n", 
							current->rc_synopsis, 
							probe->rc_synopsis, 
							current->rc_title);
						tmp = (char *)malloc (strlen(current->rc_synopsis) +
							strlen(probe->rc_synopsis) +2);
						sprintf(tmp, "%s %s", current->rc_synopsis, probe->rc_synopsis);
						current->rc_synopsis = tmp;
					}

					if ( (current->rc_pubtags == NULL) && (probe->rc_pubtags)) {
						current->rc_pubtags = probe->rc_pubtags;
						fprintf(stderr, "Merging pubtags [%s] into (%s)\n", 
							probe->rc_pubtags, current->rc_title);
					} else if ( (current->rc_pubtags) && (probe->rc_pubtags)) {
						char *tmp;

						fprintf(stderr, "Concatinating pubtags [%s] AND [%s] into (%s)\n", 
							current->rc_pubtags, 
							probe->rc_pubtags, 
							current->rc_title);
						tmp = mergetags(current->rc_pubtags, probe->rc_pubtags);
						current->rc_pubtags = tmp;
					}

					if ( (current->rc_awtags == NULL) && (probe->rc_awtags)) {
						current->rc_awtags = probe->rc_awtags;
						fprintf(stderr, "Merging award tags [%s] into (%s)\n", 
							probe->rc_awtags, current->rc_title);
					} else if ( (current->rc_awtags) && (probe->rc_awtags)) {
						char *tmp;

						fprintf(stderr, "Concatinating award tags [%s] AND [%s] into (%s)\n", 
							current->rc_awtags, 
							probe->rc_awtags, 
							current->rc_title);
						tmp = mergetags(current->rc_awtags, probe->rc_awtags);
						current->rc_awtags = tmp;
					}
				}
			}
			last  = probe;
			probe = probe->rc_next;
		}
		current = current->rc_next;
		while (current && (current->rc_title == 0) ) {
			current = current->rc_next;
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

	if (argc == 3) {
		while ((option = getopt(argc, argv, OPTARGS)) != -1) {
			switch(option) {
			case 'n':	doyear = 0;
					break;
			}
		}
		strcpy(path, argv[2]);
	} else {
		strcpy(path, argv[1]);
	}

	search_file(path);
	remove_dups();
	output_result();
	return(0);
}
