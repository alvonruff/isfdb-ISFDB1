/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)ccpubs.c	1.15	06/05/97 SFdbase";

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef SUNOS
#include <sys/unistd.h>
#else
#include <unistd.h>
#endif
#include "sfdbase.h"

#define OPTARGS "ap"

static char     tmppub[MEDIUMSIZE];
search_t        *title_list  = NULL;
extern object_t	*Objlist;
FILE            *notes_fp    = NULL;
FILE            *titles_fp   = NULL;
int             notes_offset = 0;


void
add_title(search_t *tmp)
{
	char		*ptr;

#ifdef REMOVE
	if ( title_list == NULL) {
		title_list = title_end = tmp;
	} else {
		title_end->se_next = tmp;
		title_end = tmp;
	}
#else
	tmp->se_next = title_list;
	title_list = tmp;
#endif
}


void
parse_works(char *filename, char *targetpub)
{
	FILE	*fp;
	FILE	*fp2;
	int	line_number = 1;
	int	dummy = 0;

	title_list = NULL;
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}
	fp2 = fopen("titles.dbase", "rb");
	if (fp2 == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		if ( parse_field(fp, tmppub, (int *)&dummy, MEDIUMSIZE) == -1 ) {
			goto finish;
		}
		if ( strcmp(tmppub, targetpub) == 0) {

			while(1) {
				char offset[16];
				int eol;
				int int_offset;
				search_t *set;

				parse_field_or_eol(fp, offset, &eol, 16);
				sscanf(offset, "%x", &int_offset);
				fseek(fp2, int_offset, SEEK_SET);

				set = parse_title_entry(fp2, (int *)&dummy, PARSE_MALLOC);
				if (set == NULL) {
					goto finish;
				}
				add_title(set);
				if ( eol ) {
					break;
				}
			}
		} else if ( parse_to_eol(fp, (int *)&dummy) == -1 ) {
			goto finish;
		}
		line_number++;
	}

finish:
	fclose(fp2);
	fclose(fp);
}


int
do_attribute(char *targetattr, attr_t *list, int options)
{
	attr_t          *attr;
	search_t	*tmp;
	int		counter;
	int		notcollanth;
	int		foundcoll;
	int		foundanth;
        int             result;
	char		path[256];

	strcpy(path, BASE);
	strcat(path, "/dbase.compiled");
	result = chdir(path);
        if (result != 0) {
                printf("CHDIR to %s failed\n", path);
                exit(1);
        }

	attr = list;
	while (attr) {
		if (strncmp(attr->at_name, targetattr, 2) == 0) {
			parse_works("titles.xbt", attr->at_value);
			tmp = title_list;
			counter = 0;
			while(tmp) {
				counter++;
				tmp = tmp->se_next;
			}
			if (counter < 3) {
				tmp = title_list;
				notcollanth = 0;
				foundcoll = 0;
				foundanth = 0;
				while(tmp) {
					if ( strcmp(tmp->se_type, "n") == 0) {
						notcollanth = 1;
					}
					if ( strcmp(tmp->se_type, "nf") == 0) {
						notcollanth = 1;
					}
					if ( strcmp(tmp->se_type, "ng") == 0) {
						notcollanth = 1;
					}
					if ( strcmp(tmp->se_type, "a") == 0) {
						foundanth = 1;
					}
					if ( strcmp(tmp->se_type, "c") == 0) {
						foundcoll = 1;
					}
					tmp = tmp->se_next;
				}
				if (!notcollanth) {
					if (foundanth) {
						printf("ANTH: ");
					} else if (foundcoll) {
						printf("COLL: ");
					}
					printf("%s\n", attr->at_value);
				}
			}
		}
		attr = attr->at_next;
	}
}

void
search_file(char *filename)
{
	object_t	*tmp;
	attr_t		*attr;
	extern		int line_number;

	line_number = 1;
	parse_pubs(filename);

	tmp = Objlist;
	while(tmp) {
		do_attribute("AB", tmp->ob_attrs, NOOPTIONS);
		tmp = tmp->ob_next;
	}

	Objlist = NULL;
}

int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	search_file(argv[1]);
	exit(0);
}
