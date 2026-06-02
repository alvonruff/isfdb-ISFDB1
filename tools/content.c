/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *	 ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)sort.c	1.6	02 May 1997 SFdbase";

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
	char		*rc_translation;
	int		rc_checked;
	struct recomp   *rc_next;
} recomp_t;

recomp_t *head = NULL;
recomp_t *tail = NULL;
recomp_t *isfdb_shortfiction = NULL;
recomp_t *isfdb_anthologies = NULL;
recomp_t *isfdb_collections = NULL;
recomp_t *isfdb_essays = NULL;
recomp_t *isfdb_serials = NULL;
recomp_t *isfdb_poems = NULL;
recomp_t *isfdb_novels = NULL;
recomp_t *newdb_shortfiction = NULL;
recomp_t *newdb_anthologies = NULL;
recomp_t *newdb_collections = NULL;
recomp_t *newdb_essays = NULL;
recomp_t *newdb_serials = NULL;
recomp_t *newdb_poems = NULL;
recomp_t *newdb_novels = NULL;

typedef struct keymap {
	char		km_newdb[512];
	char		km_isfdb[512];
	struct keymap	*km_next;
} keymap_t;
keymap_t *keyhead = NULL;

char	tmpseries[256];
char	tmpsuperseries[256];
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
			} else if ( strcmp(targetattr, "TR") == 0) {
				target->rc_translation = (char *)malloc( strlen(attr->at_value) + 1);
				strcpy(target->rc_translation, attr->at_value);
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
	object_t	*obj_free;
	attr_t		*attr;
	attr_t		*atr_free;
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
		target->rc_translation = NULL;
		target->rc_next = NULL;
		target->rc_checked = 0;
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
		do_attribute("TR", tmp->ob_attrs, target);

		attr = tmp->ob_attrs;
		while (attr) {
			atr_free = attr;
			attr = attr->at_next;
			free( atr_free->at_value );
			free( atr_free );
		}

		obj_free = tmp;
		tmp = tmp->ob_next;
		free( obj_free );
	}
	Objlist = NULL;
}

void
read_keymap(char *path)
{
	FILE *fp;
	int input;
	int  offset;

	fp = fopen(path, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Couldn't find the KEYMAP\n");
		exit(1);
	}

	while(1) {
		keymap_t *tmp;

		tmp = (keymap_t *)malloc( sizeof(keymap_t) );
		if ( tmp == NULL ) {
			fprintf(stderr, "Out of Memory\n");
			exit(1);
		}

		input  = 0;
		offset = 0;
		while(input != ':') {
			input = getc(fp);
			if (input == -1) {
				return;
			}
			tmp->km_newdb[offset++] = input;
		}
		tmp->km_newdb[--offset] = 0;

		input  = 0;
		offset = 0;
		while(input != '\n') {
			input = getc(fp);
			if (input == -1) {
				fprintf(stderr, "Mangled line\n");
				exit(1);
			}
			tmp->km_isfdb[offset++] = input;
		}
		tmp->km_isfdb[--offset] = 0;

		tmp->km_next = keyhead;
		keyhead = tmp;
	}
}


int
keymatch(char *taglist, char *tag)
{
	char *ptr1;
	char *ptr2;
	char tmptag[256];

	if (taglist == NULL)
		return(0);

	if (strstr(taglist, ",")) {
		strcpy(tmptag, taglist);
		ptr1 = tmptag;
		ptr2 = strstr(ptr1, ",");
		while(ptr2) {
			*ptr2 = 0;
			if ( strcmp(ptr1, tag) == 0) {
				return(1);
			}
			ptr1 = ptr2 + 1;
			ptr2 = strstr(ptr1, ",");
		}
		if ( strcmp(ptr1, tag) == 0) {
			return(1);
		} else {
			return(0);
		}
	} else if ( strcmp(taglist, tag) == 0) {
		return(1);
	} else {
		return(0);
	}
}

void
check_content()
{
	keymap_t *tmp;
	recomp_t *newdb;
	recomp_t *isfdb;
	int	first;

	tmp = keyhead;
	while(tmp) {
		printf("=====================================================================\n");
		printf("Checking: NEWDB=[%s] ISFDB=[%s]\n", tmp->km_newdb, tmp->km_isfdb);


		/*
		 * Shortfiction
		 */
		isfdb = isfdb_shortfiction;
		while(isfdb) {
			isfdb->rc_checked = 0;
			isfdb = isfdb->rc_next;
		}
		newdb = newdb_shortfiction;
		first = 1;
		while(newdb) {

			/*
			 * If this title is in the current key
			 */
			if (keymatch(newdb->rc_pubtags, tmp->km_newdb)) {
				isfdb = isfdb_shortfiction;
				while(isfdb) {
					if (keymatch(isfdb->rc_pubtags, tmp->km_isfdb)) {
						if (strcmp(newdb->rc_title, isfdb->rc_title) == 0) {
							isfdb->rc_checked = 1;
							break;
						}
					}
					isfdb = isfdb->rc_next;
				}
				if (isfdb == NULL) {
					if (first) {
						printf("SHORTFICTION MISSING IN ISFDB:\n");
						first = 0;
					}
					printf("%s {\n", newdb->rc_title);
					printf("\tAE=|%s|\n", newdb->rc_author);
					printf("\tYR=|%s|\n", newdb->rc_year);
					printf("\tPB=|%s|\n", tmp->km_isfdb);
					if (newdb->rc_storylen && newdb->rc_storylen[0])
						printf("\tSL=|%s|\n", newdb->rc_storylen);
					printf("}\n");
				} else {
					if ((isfdb->rc_year == NULL) || (isfdb->rc_year[0] == 0)) {
						printf("FATAL: %s needs a year\n", isfdb->rc_title);
						exit(1);
					}
					if ( strcmp(isfdb->rc_year, "0") == 0) {
						printf("ISFDB (%s) needs year: %s\n", isfdb->rc_title, 
							newdb->rc_year);
					}
					if (newdb->rc_storylen && newdb->rc_storylen[0] && (isfdb->rc_storylen[0] == 0)) {
						printf("ISFDB (%s) needs SL: %s\n", isfdb->rc_title, 
							newdb->rc_storylen);
					}
				}
			}
			newdb = newdb->rc_next;
		}

		/*
		 * Essays
		 */
		isfdb = isfdb_essays;
		while(isfdb) {
			isfdb->rc_checked = 0;
			isfdb = isfdb->rc_next;
		}
		newdb = newdb_essays;
		first = 1;
		while(newdb) {

			/*
			 * If this title is in the current key
			 */
			if (keymatch(newdb->rc_pubtags, tmp->km_newdb)) {
				isfdb = isfdb_essays;
				while(isfdb) {
					if (keymatch(isfdb->rc_pubtags, tmp->km_isfdb)) {
						if (strcmp(newdb->rc_title, isfdb->rc_title) == 0) {
							isfdb->rc_checked = 1;
							break;
						}
					}
					isfdb = isfdb->rc_next;
				}
				if (isfdb == NULL) {
					if (first) {
						printf("ESSAYS MISSING IN ISFDB:\n");
						first = 0;
					}
					printf("%s {\n", newdb->rc_title);
					printf("\tAE=|%s|\n", newdb->rc_author);
					printf("\tYR=|%s|\n", newdb->rc_year);
					printf("\tPB=|%s|\n", tmp->km_isfdb);
					if (newdb->rc_storylen && newdb->rc_storylen[0])
						printf("\tSL=|%s|\n", newdb->rc_storylen);
					printf("}\n");
				} else if ( strcmp(isfdb->rc_year, "0") == 0) {
					printf("ISFDB (%s) needs year: %s\n", isfdb->rc_title, newdb->rc_year);
				}
			}
			newdb = newdb->rc_next;
		}

		/*
		 * Serials
		 */
		isfdb = isfdb_serials;
		while(isfdb) {
			isfdb->rc_checked = 0;
			isfdb = isfdb->rc_next;
		}
		newdb = newdb_serials;
		first = 1;
		while(newdb) {

			/*
			 * If this title is in the current key
			 */
			if (keymatch(newdb->rc_pubtags, tmp->km_newdb)) {
				isfdb = isfdb_serials;
				while(isfdb) {
					if (keymatch(isfdb->rc_pubtags, tmp->km_isfdb)) {
						if (strcmp(newdb->rc_title, isfdb->rc_title) == 0) {
							isfdb->rc_checked = 1;
							break;
						}
					}
					isfdb = isfdb->rc_next;
				}
				if (isfdb == NULL) {
					if (first) {
						printf("SERIALS MISSING IN ISFDB:\n");
						first = 0;
					}
					printf("%s {\n", newdb->rc_title);
					printf("\tAE=|%s|\n", newdb->rc_author);
					printf("\tYR=|%s|\n", newdb->rc_year);
					printf("\tPB=|%s|\n", tmp->km_isfdb);
					if (newdb->rc_storylen && newdb->rc_storylen[0])
						printf("\tSL=|%s|\n", newdb->rc_storylen);
					printf("}\n");
				} else if ( strcmp(isfdb->rc_year, "0") == 0) {
					printf("ISFDB (%s) needs year: %s\n", isfdb->rc_title, newdb->rc_year);
				}
			}
			newdb = newdb->rc_next;
		}

		/*
		 * Poems
		 */
		isfdb = isfdb_poems;
		while(isfdb) {
			isfdb->rc_checked = 0;
			isfdb = isfdb->rc_next;
		}
		newdb = newdb_poems;
		first = 1;
		while(newdb) {

			/*
			 * If this title is in the current key
			 */
			if (keymatch(newdb->rc_pubtags, tmp->km_newdb)) {
				isfdb = isfdb_poems;
				while(isfdb) {
					if (keymatch(isfdb->rc_pubtags, tmp->km_isfdb)) {
						if (strcmp(newdb->rc_title, isfdb->rc_title) == 0) {
							isfdb->rc_checked = 1;
							break;
						}
					}
					isfdb = isfdb->rc_next;
				}
				if (isfdb == NULL) {
					if (first) {
						printf("POEMS MISSING IN ISFDB:\n");
						first = 0;
					}
					printf("%s {\n", newdb->rc_title);
					printf("\tAE=|%s|\n", newdb->rc_author);
					printf("\tYR=|%s|\n", newdb->rc_year);
					printf("\tPB=|%s|\n", tmp->km_isfdb);
					if (newdb->rc_storylen && newdb->rc_storylen[0])
						printf("\tSL=|%s|\n", newdb->rc_storylen);
					printf("}\n");
				} else if ( strcmp(isfdb->rc_year, "0") == 0) {
					printf("ISFDB (%s) needs year: %s\n", isfdb->rc_title, newdb->rc_year);
				}
			}
			newdb = newdb->rc_next;
		}

		/*
		 * Novels
		 */
		isfdb = isfdb_novels;
		while(isfdb) {
			isfdb->rc_checked = 0;
			isfdb = isfdb->rc_next;
		}
		newdb = newdb_novels;
		first = 1;
		while(newdb) {

			/*
			 * If this title is in the current key
			 */
			if (keymatch(newdb->rc_pubtags, tmp->km_newdb)) {
				isfdb = isfdb_novels;
				while(isfdb) {
					if (keymatch(isfdb->rc_pubtags, tmp->km_isfdb)) {
						if (strcmp(newdb->rc_title, isfdb->rc_title) == 0) {
							isfdb->rc_checked = 1;
							break;
						}
					}
					isfdb = isfdb->rc_next;
				}
				if (isfdb == NULL) {
					if (first) {
						printf("NOVELS MISSING IN ISFDB:\n");
						first = 0;
					}
					printf("%s {\n", newdb->rc_title);
					printf("\tAE=|%s|\n", newdb->rc_author);
					printf("\tYR=|%s|\n", newdb->rc_year);
					printf("\tPB=|%s|\n", tmp->km_isfdb);
					if (newdb->rc_storylen && newdb->rc_storylen[0])
						printf("\tSL=|%s|\n", newdb->rc_storylen);
					printf("}\n");
				} else if ( strcmp(isfdb->rc_year, "0") == 0) {
					printf("ISFDB (%s) needs year: %s\n", isfdb->rc_title, newdb->rc_year);
				}
			}
			newdb = newdb->rc_next;
		}


		/*
		 * Check for unmarked shortfiction
		 */
		isfdb = isfdb_shortfiction;
		first = 1;
		while(isfdb) {
			if (keymatch(isfdb->rc_pubtags, tmp->km_isfdb)) {
				if ( isfdb->rc_checked == 0) {
					if (first) {
						printf("SHORTFICTION MISSING IN NEWDB:\n");
						first = 0;
					}
					printf("%s {\n", isfdb->rc_title);
					printf("\tAE=|%s|\n", isfdb->rc_author);
					printf("\tYR=|%s|\n", isfdb->rc_year);
					printf("\tPB=|%s|\n", tmp->km_newdb);
					if (isfdb->rc_storylen && isfdb->rc_storylen[0])
						printf("\tSL=|%s|\n", isfdb->rc_storylen);
					printf("}\n");
				}
			}
			isfdb = isfdb->rc_next;
		}

		/*
		 * Check for unmarked essays
		 */
		isfdb = isfdb_essays;
		first = 1;
		while(isfdb) {
			if (keymatch(isfdb->rc_pubtags, tmp->km_isfdb)) {
				if ( isfdb->rc_checked == 0) {
					if (first) {
						printf("ESSAYS MISSING IN NEWDB:\n");
						first = 0;
					}
					printf("%s {\n", isfdb->rc_title);
					printf("\tAE=|%s|\n", isfdb->rc_author);
					printf("\tYR=|%s|\n", isfdb->rc_year);
					printf("\tPB=|%s|\n", tmp->km_newdb);
					if (isfdb->rc_storylen && isfdb->rc_storylen[0])
						printf("\tSL=|%s|\n", isfdb->rc_storylen);
					printf("}\n");
				}
			}
			isfdb = isfdb->rc_next;
		}

		/*
		 * Check for unmarked serials
		 */
		isfdb = isfdb_serials;
		first = 1;
		while(isfdb) {
			if (keymatch(isfdb->rc_pubtags, tmp->km_isfdb)) {
				if ( isfdb->rc_checked == 0) {
					if (first) {
						printf("SERIALS MISSING IN NEWDB:\n");
						first = 0;
					}
					printf("%s {\n", isfdb->rc_title);
					printf("\tAE=|%s|\n", isfdb->rc_author);
					printf("\tYR=|%s|\n", isfdb->rc_year);
					printf("\tPB=|%s|\n", tmp->km_newdb);
					if (isfdb->rc_storylen && isfdb->rc_storylen[0])
						printf("\tSL=|%s|\n", isfdb->rc_storylen);
					printf("}\n");
				}
			}
			isfdb = isfdb->rc_next;
		}

		/*
		 * Check for unmarked poems
		 */
		isfdb = isfdb_poems;
		first = 1;
		while(isfdb) {
			if (keymatch(isfdb->rc_pubtags, tmp->km_isfdb)) {
				if ( isfdb->rc_checked == 0) {
					if (first) {
						printf("POEMS MISSING IN NEWDB:\n");
						first = 0;
					}
					printf("%s {\n", isfdb->rc_title);
					printf("\tAE=|%s|\n", isfdb->rc_author);
					printf("\tYR=|%s|\n", isfdb->rc_year);
					printf("\tPB=|%s|\n", tmp->km_newdb);
					if (isfdb->rc_storylen && isfdb->rc_storylen[0])
						printf("\tSL=|%s|\n", isfdb->rc_storylen);
					printf("}\n");
				}
			}
			isfdb = isfdb->rc_next;
		}

		/*
		 * Check for unmarked novels
		 */
		isfdb = isfdb_novels;
		first = 1;
		while(isfdb) {
			if (keymatch(isfdb->rc_pubtags, tmp->km_isfdb)) {
				if ( isfdb->rc_checked == 0) {
					if (first) {
						printf("NOVELS MISSING IN NEWDB:\n");
						first = 0;
					}
					printf("%s {\n", isfdb->rc_title);
					printf("\tAE=|%s|\n", isfdb->rc_author);
					printf("\tYR=|%s|\n", isfdb->rc_year);
					printf("\tPB=|%s|\n", tmp->km_newdb);
					if (isfdb->rc_storylen && isfdb->rc_storylen[0])
						printf("\tSL=|%s|\n", isfdb->rc_storylen);
					printf("}\n");
				}
			}
			isfdb = isfdb->rc_next;
		}

		tmp = tmp->km_next;
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
		printf("usage: content <isfdb dir> <newdb dir>\n");
		exit(1);
	}

	sprintf(path, "%s/SHORTFICTION", argv[1]);
	search_file(path);
	isfdb_shortfiction = head;

	head = NULL;
	sprintf(path, "%s/ANTHOLOGIES", argv[1]);
	search_file(path);
	isfdb_anthologies = head;

	head = NULL;
	sprintf(path, "%s/COLLECTIONS", argv[1]);
	search_file(path);
	isfdb_collections = head;

	head = NULL;
	sprintf(path, "%s/ESSAYS", argv[1]);
	search_file(path);
	isfdb_essays = head;

	head = NULL;
	sprintf(path, "%s/SERIALS", argv[1]);
	search_file(path);
	isfdb_serials = head;

	head = NULL;
	sprintf(path, "%s/POEMS", argv[1]);
	search_file(path);
	isfdb_poems = head;

	head = NULL;
	sprintf(path, "%s/NOVELS", argv[1]);
	search_file(path);
	isfdb_novels = head;

	head = NULL;
	sprintf(path, "%s/SHORTFICTION", argv[2]);
	search_file(path);
	newdb_shortfiction = head;

	head = NULL;
	sprintf(path, "%s/ANTHOLOGIES", argv[2]);
	search_file(path);
	newdb_anthologies = head;

	head = NULL;
	sprintf(path, "%s/COLLECTIONS", argv[2]);
	search_file(path);
	newdb_collections = head;

	head = NULL;
	sprintf(path, "%s/ESSAYS", argv[2]);
	search_file(path);
	newdb_essays = head;

	head = NULL;
	sprintf(path, "%s/SERIALS", argv[2]);
	search_file(path);
	newdb_serials = head;

	head = NULL;
	sprintf(path, "%s/POEMS", argv[2]);
	search_file(path);
	newdb_poems = head;

	head = NULL;
	sprintf(path, "%s/NOVELS", argv[2]);
	search_file(path);
	newdb_novels = head;

	sprintf(path, "%s/KEYMAP", argv[2]);
	read_keymap(path);

	check_content();

	return(0);
}
