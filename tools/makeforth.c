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

search_t        *title_list  = NULL;
extern object_t	*Objlist;
FILE            *titles_fp   = NULL;
FILE            *notes_fp   = NULL;
int             notes_offset = 0;

void
print_attribute(char *targetattr, attr_t *list)
{
	attr_t		*attr;

	attr = list;
	while (attr) {
		if (strncmp(attr->at_name, targetattr, 2) == 0) {
			printf("\t%s=|%s|\n", targetattr, attr->at_value);
			return;
		}
		attr = attr->at_next;
	}
}

void
search_file(char *filename)
{
	object_t	*tmp;
	attr_t		*attr;
	int		forth;
	extern		int line_number;

	line_number = 1;
	parse_pubs(filename);

	tmp = Objlist;
	while(tmp) {
		attr = tmp->ob_attrs;
		forth = 0;
		while(attr) {
			if (strncmp(attr->at_name, "YR", 2) == 0) {
				if (strncmp(attr->at_value, "9999", 4) == 0) {
					forth = 1;
				}
				break;
			}
			attr = attr->at_next;
		}
		if (forth) {
			printf("%s {\n", tmp->ob_name);
			print_attribute("AB", tmp->ob_attrs);
			print_attribute("AE", tmp->ob_attrs);
			print_attribute("YR", tmp->ob_attrs);
			print_attribute("IS", tmp->ob_attrs);
			print_attribute("PB", tmp->ob_attrs);
			print_attribute("PR", tmp->ob_attrs);
			print_attribute("PP", tmp->ob_attrs);
			print_attribute("TP", tmp->ob_attrs);
			print_attribute("CV", tmp->ob_attrs);
			print_attribute("BC", tmp->ob_attrs); 
			print_attribute("NT", tmp->ob_attrs);
			printf("}\n");
		}
		tmp = tmp->ob_next;
	}

	Objlist = NULL;
}


int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	search_file("BOOKS");

	exit(0);
}
