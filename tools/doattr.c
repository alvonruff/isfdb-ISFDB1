/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "%W%	%G% %Q%";

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#ifdef SUNOS
#include <sys/unistd.h>
#else
#include <unistd.h>
#endif
#include "sfdbase.h"
 
extern FILE  *titles_fp;
extern FILE  *notes_fp;
extern int    notes_offset;


int
do_attribute(char *targetattr, attr_t *list, int options)
{
	attr_t		*attr;

	attr = list;
	while (attr) {
		if (strncmp(attr->at_name, targetattr, 2) == 0) {
			if (options & DO_TITLES) {
				if (strncmp(attr->at_name, "IT", 2) == 0) {
					fprintf(titles_fp, "\tAE=|%s|\n", attr->at_value);
				} else if (strncmp(attr->at_name, "YR", 2) == 0) {
					fprintf(titles_fp, "\tYR=|%s|\n", attr->at_value);
				} else if (strncmp(attr->at_name, "PB", 2) == 0) {
					fprintf(titles_fp, "\tPB=|%s|\n}\n", attr->at_value);
				}
			}
			if (options & DO_REVIEWS) {
				if (strncmp(attr->at_name, "RV", 2) == 0) {
					fprintf(titles_fp, "\tAE=|%s|\n", attr->at_value);
				} else if (strncmp(attr->at_name, "YR", 2) == 0) {
					fprintf(titles_fp, "\tYR=|%s|\n", attr->at_value);
				} else if (strncmp(attr->at_name, "AE", 2) == 0) {
					fprintf(titles_fp, "\tSL=|%s|\n", attr->at_value);
				} else if (strncmp(attr->at_name, "PB", 2) == 0) {
					fprintf(titles_fp, "\tPB=|%s|\n}\n", attr->at_value);
				}
			}
			if ( strcmp(targetattr, "NT") == 0) {
				if (options & DO_NOTES) {
					printf("%x|", notes_offset);
					fprintf(notes_fp, "NOTE=|%s|\n", attr->at_value);
					notes_offset += (8 + strlen(attr->at_value));
				} else {
					printf("%s|", attr->at_value);
				}
			} else if ( strcmp(targetattr, "SY") == 0) {
				if (options & DO_SYNOP) {
					printf("%x|", notes_offset);
					fprintf(notes_fp, "NOTE=|%s|\n", attr->at_value);
					notes_offset += (8 + strlen(attr->at_value));
				}
			} else {
				printf("%s|", attr->at_value);
			}
			break;
		}
		attr = attr->at_next;
	}
	if (attr == NULL) {
		if ( strcmp(targetattr, "AB") == 0) {
			return(1);
		} else if ( strcmp(targetattr, "YR") == 0) {
			return(1);
		} else {
			printf("|");
		}
	}
	return(0);
}
