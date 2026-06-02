/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)ccawards.c	1.9	05/01/98 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BIGTITLE
#include "sfdbase.h"

extern int do_attribute(char *targetattr, attr_t *list, int options);

#define OPTARGS "aytmo"

char	 tmptitle1[MEDIUMSIZE];
char	 tmptitle2[MEDIUMSIZE];
char	 tmpauthor1[MEDIUMSIZE];
char	 tmpauthor2[MEDIUMSIZE];
FILE	 *novels_fp;
FILE	 *short_fp;
FILE	 *coll_fp;
FILE	 *anth_fp;
FILE	 *nonf_fp;
FILE     *notes_fp    = NULL;
FILE     *titles_fp   = NULL;
int      notes_offset = 0;
search_t *title_list  = NULL;
author_t *author_list = NULL;

extern char     *optarg;
extern object_t *Objlist;

typedef struct mapping {
	char	mp_title1[256];
	char	mp_author1[256];
	char	mp_title2[256];
	char	mp_author2[256];
	struct mapping *mp_next;
} mapping_t;
mapping_t *map_head = NULL;


void
print_author_entries()
{
	author_t	*tmp;

	tmp = author_list;
	while(tmp) {
		printf("%s|%s\n", tmp->au_author, tmp->au_offsets);
		tmp = tmp->au_next;
	}
}

void
add_author_entry(char *author, char *offset)
{
	author_t	*tmp;
	char		newoffset[16];

	tmp = author_list;
	while(tmp) {
		if ( strcmp(tmp->au_author, author) == 0 ) {
			sprintf(newoffset, "|%s", offset);
			if (tmp->au_small) {
				tmp->au_length += strlen(newoffset);
				if (tmp->au_length > (LARGESIZE-1)) {
					fprintf(stderr, "%s: exceeds %d bytes\n", author, LARGESIZE);
					exit(1);
				}
				if (tmp->au_length > (SMALLSIZE-1)) {
					char *ptr;

					ptr = (char *)malloc(LARGESIZE);
					strcpy(ptr, tmp->au_offsets);
					free(tmp->au_offsets);
					tmp->au_offsets = ptr;
					tmp->au_small = 0;
				}
			}
			strcat(tmp->au_offsets, newoffset);
			return;
		}
		tmp = tmp->au_next;
	}

	tmp = (author_t *)malloc( sizeof(author_t) );
	if ( tmp == NULL ) {
		perror("out of memory");
		exit(1);
	}

	tmp->au_author = (char *)malloc( strlen(author) + 1);
	strcpy(tmp->au_author, author);

	tmp->au_offsets = (char *)malloc(SMALLSIZE);
	strcpy(tmp->au_offsets, offset);
	tmp->au_length = strlen(offset);
	tmp->au_small  = 1;
	tmp->au_next = author_list;
	author_list = tmp;
}

void
create_author_list(search_t *list)
{
	search_t	*tmp;
	int		firsttime = 1;
	char		*author;
	char		*ptr;

	tmp = list;
	while( tmp ) {
		author = tmp->se_author;
		while ( strstr(author, "+") ) {
			ptr = (char *)strstr(author, "+");
			*ptr = 0;
			add_author_entry(author, tmp->se_title);
			author = ++ptr;
		}
		add_author_entry(author, tmp->se_title);
		tmp = tmp->se_next;
	}
}


void
add_title(char *title, char *author, char *year, int offset)
{
	search_t	*tmp;

	tmp = (search_t *)malloc( sizeof(search_t) );
	if ( tmp == NULL ) {
		perror("out of memory");
		exit(1);
	}

	tmp->se_title  = (char *)malloc(10);
	tmp->se_author = (char *)malloc( strlen(author) + 1 );

	sprintf(tmp->se_title, "%x", offset);
	strcpy(tmp->se_author, author);
	tmp->se_next = title_list;
	title_list = tmp;
}


#define BYAUTHOR 1
#define BYYEAR   2
#define MAKESRC  3

void
parse_file(char *filename, int mode)
{
	FILE	*fp;
	int	line_number = 1;
	int	index;
	char	input;
	int	this_offset = 0;
	int	next_offset = 0;
	mapping_t *tmp;
	award_t	  *awt;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		this_offset = next_offset;
		awt = parse_award_entry(fp, (int *)&next_offset, 1);
		if ( awt == NULL) {
			goto finish;
		}
		line_number++;

		/*
		 * Replace the title with a mapping if needed
		 */
		tmp = map_head;
		while(tmp) {
			if (strcmp(tmp->mp_title1, awt->aw_title) == 0) {
				if (strcmp(tmp->mp_author1, awt->aw_author) == 0) {
					strcpy(awt->aw_title, tmp->mp_title2);
					strcpy(awt->aw_author, tmp->mp_author2);
					break;
				}
			}
			tmp = tmp->mp_next;
		}

		if (mode == BYAUTHOR) {
			if ( strstr(awt->aw_author, "^") ) {
				auset_t *tmp2;

				tmp2 = (auset_t *)decompose(awt->aw_author);
				separate(tmp2);
				while( tmp2 ) {
					char    auth2[80];
					char	*author,*ptr;

					author = tmp2->au_actual;
					while ( strstr(author, "+") ) {
						ptr = (char *)strstr(author, "+");
						*ptr = 0;
						add_title(awt->aw_title, author, awt->aw_year, this_offset);
						author = ++ptr;
					}
					add_title(awt->aw_title, author, awt->aw_year, this_offset);
					tmp2 = tmp2->au_next;
				}
			} else {
				add_title(awt->aw_title, awt->aw_author, awt->aw_year, this_offset);
			}
		} else if (mode == BYYEAR) {
			add_title(awt->aw_title, awt->aw_year, awt->aw_author, this_offset);
		} else if ((mode == MAKESRC) && (awt->aw_tag[0])) {
			int year;

			sscanf(awt->aw_year, "%d", &year);
			year--;
			if ( strcmp(awt->aw_types, "n") == 0) {
				fprintf(novels_fp, "%s {\n", awt->aw_title);
				fprintf(novels_fp, "\tAE=|%s|\n", awt->aw_author);
				fprintf(novels_fp, "\tYR=|%d|\n", year);
				if (awt->aw_tag[0]) {
					fprintf(novels_fp, "\tTG=|%s|\n", awt->aw_tag);
				}
				fprintf(novels_fp, "}\n");
			} else if ( (strcmp(awt->aw_types, "nv") == 0) ||
			     (strcmp(awt->aw_types, "ss") == 0) ||
			     (strcmp(awt->aw_types, "sf") == 0) ||
			     (strcmp(awt->aw_types, "nt") == 0)) {
				fprintf(short_fp, "%s {\n", awt->aw_title);
				fprintf(short_fp, "\tAE=|%s|\n", awt->aw_author);
				fprintf(short_fp, "\tYR=|%d|\n", year);
				fprintf(short_fp, "\tTG=|%s|\n", awt->aw_tag);
				fprintf(short_fp, "}\n");
			} else if (strcmp(awt->aw_types, "c") == 0){
				fprintf(coll_fp, "%s {\n", awt->aw_title);
				fprintf(coll_fp, "\tAE=|%s|\n", awt->aw_author);
				fprintf(coll_fp, "\tYR=|%d|\n", year);
				fprintf(coll_fp, "\tTG=|%s|\n", awt->aw_tag);
				fprintf(coll_fp, "}\n");
			} else if (strcmp(awt->aw_types, "nf") == 0){
				fprintf(nonf_fp, "%s {\n", awt->aw_title);
				fprintf(nonf_fp, "\tAE=|%s|\n", awt->aw_author);
				fprintf(nonf_fp, "\tYR=|%d|\n", year);
				fprintf(nonf_fp, "\tTG=|%s|\n", awt->aw_tag);
				fprintf(nonf_fp, "}\n");
			} else if (strcmp(awt->aw_types, "a") == 0){
				fprintf(anth_fp, "%s {\n", awt->aw_title);
				fprintf(anth_fp, "\tAE=|%s|\n", awt->aw_author);
				fprintf(anth_fp, "\tYR=|%d|\n", year);
				fprintf(anth_fp, "\tTG=|%s|\n", awt->aw_tag);
				fprintf(anth_fp, "}\n");
			}
		}
	}

finish:
	fclose(fp);
}


void
search_file(char *filename, char *exten)
{
	object_t        *tmp;
	attr_t          *attr;
	extern int	line_number;

	line_number = 1;
	parse_pubs(filename);

	tmp = Objlist;
	while(tmp) {
		printf("%s|", tmp->ob_name);
		do_attribute("AE", tmp->ob_attrs, NOOPTIONS);
		do_attribute("YR", tmp->ob_attrs, NOOPTIONS);
		do_attribute("TP", tmp->ob_attrs, NOOPTIONS);
		do_attribute("TA", tmp->ob_attrs, NOOPTIONS);
		do_attribute("LV", tmp->ob_attrs, NOOPTIONS);
		do_attribute("MV", tmp->ob_attrs, NOOPTIONS);
		do_attribute("TG", tmp->ob_attrs, NOOPTIONS);
		printf("%s|\n", exten);

		tmp = tmp->ob_next;
	}

	Objlist = NULL;
}

void
open_awardfiles()
{
	novels_fp = fopen("NOVELS.aw", "wb+");
	if ( novels_fp == NULL ) {
		fprintf(stderr, "Couldn't open new NOVELS file\n");
		exit(1);
	}
	short_fp = fopen("SHORTFICTION.aw", "wb+");
	if ( short_fp == NULL ) {
		fprintf(stderr, "Couldn't open new SHORTFICTION file\n");
		exit(1);
	}
	coll_fp = fopen("COLLECTIONS.aw", "wb+");
	if ( coll_fp == NULL ) {
		fprintf(stderr, "Couldn't open new COLLECTIONS file\n");
		exit(1);
	}
	anth_fp = fopen("ANTHOLOGIES.aw", "wb+");
	if ( anth_fp == NULL ) {
		fprintf(stderr, "Couldn't open new ANTHOLOGIES file\n");
		exit(1);
	}
	nonf_fp = fopen("NONFICTION.aw", "wb+");
	if ( nonf_fp == NULL ) {
		fprintf(stderr, "Couldn't open new NONFICTION file\n");
		exit(1);
	}
}

void
close_awardfiles()
{
	fclose(novels_fp);
	fclose(short_fp);
	fclose(coll_fp);
	fclose(anth_fp);
	fclose(nonf_fp);
}

void
load_mappings()
{
	FILE    *fp;
	mapping_t *tmp;
	int	next_offset;

	fp = fopen("AWARDMAP", "rb");
	if (fp == NULL) {
		return;
	}

	while(1) {
		if ( parse_field(fp, tmptitle1, (int *)&next_offset, MEDIUMSIZE) == -1 ) {
			return;
		}
		if ( parse_field(fp, tmpauthor1, (int *)&next_offset, MEDIUMSIZE) == -1 ) {
			return;
		}
		if ( parse_field(fp, tmptitle2, (int *)&next_offset, MEDIUMSIZE) == -1 ) {
			return;
		}
		parse_field_or_eol(fp, tmpauthor2, (int *)&next_offset, MEDIUMSIZE);

		tmp = (mapping_t *)malloc( sizeof(mapping_t) );
		if (tmp) {
			strcpy(tmp->mp_title1, tmptitle1);
			strcpy(tmp->mp_author1, tmpauthor1);
			strcpy(tmp->mp_title2, tmptitle2);
			strcpy(tmp->mp_author2, tmpauthor2);
			tmp->mp_next = map_head;
			map_head = tmp;
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
	int		mode = 0;
	search_t	*tmp;
	char		path[256];
	FILE		*fp;

	if (argc == 3) {
		close(1);
		fp = freopen(argv[2], "wb", stdout);
		if (fp == NULL) {
			perror("ccawards: Couldn't open output file\n");
			exit(1);
		}
	} else if (argc == 2) {
		/* Nothing */
	} else {
		printf("ccawards [-t][-a][-y] <output file>\n");
		exit(1);
	}

	strcpy(path, BASE);
	strcat(path, "/dbase.ascii");
	result = chdir(path);
        if (result != 0) {
                printf("CHDIR to %s failed\n", path);
                exit(1);
        }
	load_mappings();

	while ( (option = getopt(argc, argv, OPTARGS)) != -1) {
		switch(option) {
		case 'a':
			strcpy(path, BASE);
			strcat(path, "/dbase.compiled");
			result = chdir(path);
                        if (result != 0) {
                                printf("CHDIR to %s failed\n", path);
                                exit(1);
                        }
			mode = BYAUTHOR;
			break;

		case 'y':
			strcpy(path, BASE);
			strcat(path, "/dbase.compiled");
			result = chdir(path);
                        if (result != 0) {
                                printf("CHDIR to %s failed\n", path);
                                exit(1);
                        }
			mode = BYYEAR;
			break;
		case 'o':
			strcpy(path, BASE);
			strcat(path, "/dbase.ascii");
			result = chdir(path);
                        if (result != 0) {
                                printf("CHDIR to %s failed\n", path);
                                exit(1);
                        }
			search_file(argv[2],   "xx");
			break;
		case 't':
			strcpy(path, BASE);
			strcat(path, "/dbase.ascii");
			result = chdir(path);
                        if (result != 0) {
                                printf("CHDIR to %s failed\n", path);
                                exit(1);
                        }
			search_file("awards/ANALOGAW", "An");
			search_file("awards/APOLLO",   "Ap");
			search_file("awards/ASIMOVR",  "Ar");
			search_file("awards/ASIMOVU",  "Ax");
			search_file("awards/AUREALIS", "As");
			search_file("awards/AURORA",   "Au");
			search_file("awards/BALROG",   "Bl");
			search_file("awards/BFA",      "Bf");
			search_file("awards/BSFA",     "Bs");
			search_file("awards/CAMPBELL", "Ca");
			search_file("awards/CHESLEY",  "Cy");
			search_file("awards/CHRON",    "Sc");
			search_file("awards/CLARKE",   "Cl");
			search_file("awards/COMPTON",  "Cc");
			search_file("awards/DEATHR",   "Dr");
			search_file("awards/DITMAR",   "Dt");
			search_file("awards/GANDALF",  "Ga");
			search_file("awards/GAUGHAN",  "Gg");
			search_file("awards/HOMER",    "Hm");
			search_file("awards/HUGOS",    "Hu");
			search_file("awards/IFA",      "If");
			search_file("awards/IHG",      "Ih");
			search_file("awards/IMAGINAIRE", "Im");
			search_file("awards/JWCA",     "Jc");
			search_file("awards/LAMBDA",   "Lm");
			search_file("awards/LOCUS",    "Lc");
			search_file("awards/MYTHO",    "My");
			search_file("awards/NEBULAS",  "Ne");
			search_file("awards/PKDICK",   "Pk");
			search_file("awards/PROMETHEUS","Pr");
			search_file("awards/RETROH",   "Rh");
			search_file("awards/RHYSLING", "Ry");
			search_file("awards/SFBC",     "Sf");
			search_file("awards/SKYLARK",  "Sk");
			search_file("awards/STOKER",   "St");
			search_file("awards/STURGEON", "Su");
			search_file("awards/SIDEWISE", "Sw");
			search_file("awards/TIPTREE",  "Tp");
			search_file("awards/WFA",      "Wf");
			exit(0);
		case 'm':
			strcpy(path, BASE);
			strcat(path, "/dbase.compiled");
			result = chdir(path);
                        if (result != 0) {
                                printf("CHDIR to %s failed\n", path);
                                exit(1);
                        }

			open_awardfiles();
			parse_file( "awards.dbase", MAKESRC );
			close_awardfiles();
			exit(0);
		}
	}

	parse_file( "awards.dbase", mode );
	create_author_list(title_list);
	print_author_entries();
	exit(0);
}
