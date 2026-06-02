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


extern object_t	*Objlist;
FILE            *notes_fp    = NULL;
FILE            *titles_fp   = NULL;
int             notes_offset = 0;
int             need_type    = 0;


#define NEED_NOVEL	1
#define NEED_ANTH	2
#define NEED_COLL	3
#define NEED_OMNI	4
#define NEED_NONF	5

int
is_novel(attr_t *list)
{
	attr_t		*attr;

	attr = list;
	while (attr) {
		if (strncmp(attr->at_name, "CT", 2) == 0) {
			switch(need_type) {
			case NEED_NOVEL:
				if (strcmp(attr->at_value, "n") == 0) {
					return(1);
				}
				break;
			case NEED_ANTH:
				if (strcmp(attr->at_value, "a") == 0) {
					return(1);
				}
				break;
			case NEED_COLL:
				if (strcmp(attr->at_value, "c") == 0) {
					return(1);
				}
				break;
			case NEED_OMNI:
				if (strcmp(attr->at_value, "om") == 0) {
					return(1);
				}
				break;
			case NEED_NONF:
				if (strcmp(attr->at_value, "nf") == 0) {
					return(1);
				}
				break;
			default:
				printf("Bad need type\n");
				exit(1);
				break;
			}
			return(0);
		}
		attr = attr->at_next;
	}
	return(0);
}


void
print_attribute(char *targetattr, attr_t *list, int force)
{
	attr_t		*attr;

	attr = list;
	while (attr) {
		if (force) {
			if (strncmp(attr->at_name, targetattr, 2) == 0) {
				if (attr->at_value[0]) {
					printf("\t%s=|%s|\n", targetattr, attr->at_value);
					return;
				}
			}
		} else if (strncmp(attr->at_name, targetattr, 2) == 0) {
			if (strcmp("AB", targetattr) == 0) {
				printf("\tPB=|%s|\n", attr->at_value);
			} else if (strcmp("FT", targetattr) == 0) {
				printf("\tTG=|%s|\n", attr->at_value);
			} else if (strcmp("BT", targetattr) == 0) {
				printf("\tTG=|%s|\n", attr->at_value);
			} else if ((strcmp("CV", targetattr) == 0) || (strcmp("BC", targetattr) == 0)){
				char *forptr;

				forptr = (char *)strstr(attr->at_value, "for");
				if (forptr) {
					forptr--;
					*forptr = 0;
				}
				printf("\tAE=|%s|\n", attr->at_value);
			} else {
				printf("\t%s=|%s|\n", targetattr, attr->at_value);
			}
			return;
		}
		attr = attr->at_next;
	}
	if (force == 1) {
		printf("\t%s=||\n", targetattr);
	}
}

void
gen_newlist(char *filename)
{
	object_t	*tmp;
	attr_t		*attr;
	extern		int line_number;

	line_number = 1;
	parse_pubs(filename);

	tmp = Objlist;
	while(tmp) {
		if (is_novel(tmp->ob_attrs)) {
			printf("%s {\n", tmp->ob_name);
			print_attribute("AE", tmp->ob_attrs, 0);
			print_attribute("YR", tmp->ob_attrs, 0);
			print_attribute("AB", tmp->ob_attrs, 0);
			printf("}\n");
		}
		tmp = tmp->ob_next;
	}
	Objlist = NULL;
}

void
gen_emptylist(char *filename)
{
	object_t	*tmp;
	attr_t		*attr;
	extern		int line_number;

	line_number = 1;
	parse_pubs(filename);

	tmp = Objlist;
	while(tmp) {
		printf("%s {\n", tmp->ob_name);
		print_attribute("AB", tmp->ob_attrs, 1);
		print_attribute("AE", tmp->ob_attrs, 1);
		print_attribute("YR", tmp->ob_attrs, 1);
		print_attribute("PB", tmp->ob_attrs, 1);
		print_attribute("TP", tmp->ob_attrs, 1);
		print_attribute("CT", tmp->ob_attrs, 1);
		print_attribute("IS", tmp->ob_attrs, 1);
		print_attribute("PR", tmp->ob_attrs, 1);
		print_attribute("PP", tmp->ob_attrs, 1);
		print_attribute("CV", tmp->ob_attrs, 1);
		print_attribute("NT", tmp->ob_attrs, 1);
		printf("}\n");
		tmp = tmp->ob_next;
	}
	Objlist = NULL;
}


void
gen_striplist(char *filename)
{
	object_t	*tmp;
	attr_t		*attr;
	extern		int line_number;

	line_number = 1;
	parse_pubs(filename);

	tmp = Objlist;
	while(tmp) {
		printf("%s {\n", tmp->ob_name);
		print_attribute("AB", tmp->ob_attrs, 2);
		print_attribute("AE", tmp->ob_attrs, 2);
		print_attribute("YR", tmp->ob_attrs, 2);
		print_attribute("PB", tmp->ob_attrs, 2);
		print_attribute("TP", tmp->ob_attrs, 2);
		print_attribute("CT", tmp->ob_attrs, 2);
		print_attribute("IS", tmp->ob_attrs, 2);
		print_attribute("PR", tmp->ob_attrs, 2);
		print_attribute("PP", tmp->ob_attrs, 2);
		print_attribute("CV", tmp->ob_attrs, 2);
		print_attribute("NT", tmp->ob_attrs, 2);
		printf("}\n");
		tmp = tmp->ob_next;
	}
	Objlist = NULL;
}


#define OPTARGS "nacofes"

int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int             option;

	while ( (option = getopt(argc, argv, OPTARGS)) != -1) {
		switch(option) {
		case 'n':
			need_type = NEED_NOVEL;
			gen_newlist("FORTHCOMING");
			break;
		case 'a':
			need_type = NEED_ANTH;
			gen_newlist("FORTHCOMING");
			break;
		case 'c':
			need_type = NEED_COLL;
			gen_newlist("FORTHCOMING");
			break;
		case 'o':
			need_type = NEED_OMNI;
			gen_newlist("FORTHCOMING");
			break;
		case 'f':
			need_type = NEED_NONF;
			gen_newlist("FORTHCOMING");
			break;
		case 'e':
			gen_emptylist("FORTHCOMING");
			break;
		case 's':
			gen_striplist("FORTHCOMING");
			break;
		}
	}
	exit(0);
}
