/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *	 ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)merge.c	1.1	11 Nov 1996 SFdbase";

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
	char		*rc_notes;
	char		*rc_synopsis;
	struct recomp   *rc_next;
} recomp_t;

recomp_t *head = NULL;
recomp_t *tail = NULL;
extern pub_t *pub_list;


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


char *
findpubtitle(char *target)
{
	pub_t *tmp;

	tmp = pub_list;
	while(tmp) {
		if (strcmp(target, tmp->pu_abbreviation) == 0) {
			return(tmp->pu_title);
		}
		tmp = tmp->pu_next;
	}
	return(NULL);
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
		if ( (strcmp(target->rc_title, "Introduction") == 0) ||
		     (strcmp(target->rc_title, "Read This") == 0) ||
		     (strcmp(target->rc_title, "Reflections") == 0) ||
		     (strcmp(target->rc_title, "Contributors") == 0) ||
		     (strcmp(target->rc_title, "Books") == 0) ||
		     (strcmp(target->rc_title, "Editorial") == 0) ||
		     (strcmp(target->rc_title, "Mission Nocternal") == 0) ||
		     (strcmp(target->rc_title, "Obituaries") == 0) ||
		     (strcmp(target->rc_title, "Hector") == 0) ||
		     (strcmp(target->rc_title, "TZ Video") == 0) ||
		     (strcmp(target->rc_title, "TZ Illuminations") == 0) ||
		     (strcmp(target->rc_title, "TZ Tech") == 0) ||
		     (strcmp(target->rc_title, "Screen") == 0) ||
		     (strcmp(target->rc_title, "Book Notes") == 0) ||
		     (strcmp(target->rc_title, "The Authors") == 0) ||
		     (strcmp(target->rc_title, "Tomorrow's Books") == 0) ||
		     (strcmp(target->rc_title, "Science Briefs") == 0) ||
		     (strcmp(target->rc_title, "SF in Brazil") == 0) ||
		     (strcmp(target->rc_title, "The Bad Boy's View of the World and Himself") == 0) ||
		     (strcmp(target->rc_title, "Treasurer's Report") == 0) ||
		     (strcmp(target->rc_title, "Galaxy's Five Star Shelf") == 0) ||
		     (strcmp(target->rc_title, "The Alien Viewpoint") == 0) ||
		     (strcmp(target->rc_title, "Reading Room") == 0) ||
		     (strcmp(target->rc_title, "Editor's Page") == 0) ||
		     (strcmp(target->rc_title, "Galaxy Bookshelf") == 0) ||
		     (strcmp(target->rc_title, "Abbey Rations") == 0) ||
		     (strcmp(target->rc_title, "Artist Profiles") == 0) ||
		     (strcmp(target->rc_title, "Aberrant Behavior") == 0) ||
		     (strcmp(target->rc_title, "Odd Ends") == 0) ||
		     (strcmp(target->rc_title, "Reviews") == 0) ||
		     (strcmp(target->rc_title, "Bibliography") == 0) ||
		     (strcmp(target->rc_title, "Personalities in Science Fiction") == 0) ||
		     (strcmp(target->rc_title, "What Is Your Science IQ?") == 0) ||
		     (strcmp(target->rc_title, "About the Authors") == 0) ||
		     (strcmp(target->rc_title, "My Life Among the Stars") == 0) ||
		     (strcmp(target->rc_title, "Excavations") == 0) ||
		     (strcmp(target->rc_title, "Foreword") == 0) ||
		     (strcmp(target->rc_title, "Words & Pictures") == 0) ||
		     (strcmp(target->rc_title, "Canadian Convention Calendar") == 0) ||
		     (strcmp(target->rc_title, "Book Review") == 0) ||
		     (strcmp(target->rc_title, "Bitch, Bitch, Bitch") == 0) ||
		     (strcmp(target->rc_title, "Coming Home") == 0) ||
		     (strcmp(target->rc_title, "Who's Come to Visit") == 0) ||
		     (strcmp(target->rc_title, "Eldritch Tomery") == 0) ||
		     (strcmp(target->rc_title, "Eldritch Lair - Dungeon Level") == 0) ||
		     (strcmp(target->rc_title, "Eldritch Eye") == 0) ||
		     (strcmp(target->rc_title, "The Eldritch Eye") == 0) ||
		     (strcmp(target->rc_title, "They Say") == 0) ||
		     (strcmp(target->rc_title, "Eldritch Lair") == 0) ||
		     (strcmp(target->rc_title, "Scoop: Australian Science Fiction News") == 0) ||
		     (strcmp(target->rc_title, "Calibrations") == 0) ||
		     (strcmp(target->rc_title, "Last Rites First") == 0) ||
		     (strcmp(target->rc_title, "Last Rites") == 0) ||
		     (strcmp(target->rc_title, "First Word") == 0) ||
		     (strcmp(target->rc_title, "Look Here") == 0) ||
		     (strcmp(target->rc_title, "Preface") == 0) ||
		     (strcmp(target->rc_title, "Webtalk") == 0) ||
		     (strcmp(target->rc_title, "Book Reviews") == 0) ||
		     (strcmp(target->rc_title, "The Word") == 0) ||
		     (strcmp(target->rc_title, "Fanorama") == 0) ||
		     (strcmp(target->rc_title, "Something to Read") == 0) ||
		     (strcmp(target->rc_title, "Scientifilm Previews") == 0) ||
		     (strcmp(target->rc_title, "The Arts: Sculpture") == 0) ||
		     (strcmp(target->rc_title, "The Arts: Books") == 0) ||
		     (strcmp(target->rc_title, "The Arts: Music") == 0) ||
		     (strcmp(target->rc_title, "The Arts: Film") == 0) ||
		     (strcmp(target->rc_title, "The Arts: Television") == 0) ||
		     (strcmp(target->rc_title, "Ask Mr. Science") == 0) ||
		     (strcmp(target->rc_title, "From the Editor's Desk") == 0) ||
		     (strcmp(target->rc_title, "Subversion") == 0) ||
		     (strcmp(target->rc_title, "Editorial Babble") == 0) ||
		     (strcmp(target->rc_title, "Cyber-Cents") == 0) ||
		     (strcmp(target->rc_title, "The Incredible Two-Headed TV Casualty") == 0) ||
		     (strcmp(target->rc_title, "For Your Information") == 0) ||
		     (strcmp(target->rc_title, "The Den") == 0) ||
		     (strcmp(target->rc_title, "S.M.O.G.") == 0) ||
		     (strcmp(target->rc_title, "The Zookeeper Speaks") == 0) ||
		     (strcmp(target->rc_title, "R.I.P.") == 0) ||
		     (strcmp(target->rc_title, "That Moon Plaque") == 0) ||
		     (strcmp(target->rc_title, "Introduction") == 0) ||
		     (strcmp(target->rc_title, "Afterword") == 0) ||
		     (strcmp(target->rc_title, "Book Reviews: Fantasy Books") == 0) ||
		     (strcmp(target->rc_title, "If - and When") == 0) ) {
			char *paren;

			paren = findpubtitle(target->rc_pubtags);
			if (paren) {
				printf("%s (%s) {\n", target->rc_title, paren);
			} else {
				printf("%s {\n", target->rc_title);
			}
		} else {
			printf("%s {\n", target->rc_title);
		}
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
remove_dups()
{
	recomp_t	*current;
	recomp_t	*probe;

	current = head;
	while(current) {
		probe = current->rc_next;
		while(probe) {
			if (strcmp(current->rc_title, probe->rc_title) == 0) {
				if (strcmp(current->rc_author, probe->rc_author) == 0) {
					
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
						if (year1 != year2) {
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
						tmp = (char *)malloc (strlen(current->rc_pubtags) +
							strlen(probe->rc_pubtags)+2);
						sprintf(tmp, "%s,%s", current->rc_pubtags, probe->rc_pubtags);
						current->rc_pubtags = tmp;
					}
				}
			}
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

	if (argc != 2) {
		printf("usage: fixessays <file>\n");
		exit(1);
	}

	search_file(argv[1]);
	load_pubs(F_NOOPT, NULL);
	output_result();
	return(0);
}
