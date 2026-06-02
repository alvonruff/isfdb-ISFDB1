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

char	publications[100][256];
int	pubno;

void
do_attribute(char *targetattr, attr_t *list, char *title)
{
	attr_t		*attr;
	char		*current;
	char		*nextguy;
	int		loop1;
	int		loop2;

	attr = list;
	while (attr) {
		if (strncmp(attr->at_name, targetattr, 2) == 0) {
			current = attr->at_value;
			pubno = 0;
			while(1) {
				nextguy = (char *)strstr(current, ",");
				if (nextguy) {
					*nextguy = 0;
					strcpy(publications[pubno++], current);
					current = nextguy+1;
				} else {
					strcpy(publications[pubno++], current);
					break;
				}
			}
			for(loop1=0; loop1<(pubno-1); loop1++) {
				for(loop2=loop1+1; loop2<pubno; loop2++) {
					if(strcmp(publications[loop1], publications[loop2]) == 0) {
						printf("DUP: [%s] - %s\n", publications[loop1], title);
					}
				}
			}
			return;
		}
		attr = attr->at_next;
	}
}


void
search_file(char *filename)
{
	object_t	*tmp;

	parse_pubs(filename);
	tmp = Objlist;
	while(tmp) {
		do_attribute("PB", tmp->ob_attrs, tmp->ob_name);
		tmp = tmp->ob_next;
	}
	Objlist = NULL;
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
	return(0);
}
