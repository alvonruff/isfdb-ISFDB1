/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)cctitles.c	1.11	06/10/97 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "sfdbase.h"

extern int do_attribute(char *targetattr, attr_t *list, int options);

#define OPTARGS "a:t:s:g:c:"
#define DO_AUTHOR 1
#define DO_SERIES 2
#define DO_TAG    3

typedef struct titlerec {
	char		se_offset[12];
	char		*se_author;
	char		*se_series;
	char		*se_pubs;
	struct titlerec	*se_next;
} titlerec_t;

typedef struct target {
	char            *au_author;
	char            *au_offsets;
	int		 au_small;
	int		 au_length;
	struct target   *au_left;
	struct target   *au_right;
} target_t;

typedef struct target2 {
	char            *au_author;
	char            *au_pseudo;
	struct target2  *au_next;
} target2_t;


int		mode = 0;
int		notes_offset  = 0;
titlerec_t      *title_list   = NULL;
target_t        *author_list  = NULL;
target2_t       *author_list2 = NULL;
series_t        *series_list  = NULL;
tag_t           *tag_list     = NULL;
FILE		*notes_fp     = NULL;
FILE		*titles_fp    = NULL;
extern object_t	*Objlist;


void
print_author_entries(target_t *tmp)
{
	if (tmp->au_left) {
		print_author_entries(tmp->au_left);
	}
	printf("%s|%s\n", tmp->au_author, tmp->au_offsets);
	if (tmp->au_right) {
		print_author_entries(tmp->au_right);
	}
}

void
print_series_entries()
{
	series_t	*tmp;

	tmp = series_list;
	while(tmp) {
		printf("%s|%s\n", tmp->ss_series, tmp->ss_offsets);
		tmp = tmp->ss_next;
	}
}

void
print_tag_entries(tag_t *tmp)
{
	if (tmp->ta_left) {
		print_tag_entries(tmp->ta_left);
	}
	printf("%s|%s\n", tmp->ta_tag, tmp->ta_offsets);
	if (tmp->ta_right) {
		print_tag_entries(tmp->ta_right);
	}
}

void
add_author_entry(char *author, char *offset)
{
	target_t	*tmp;
	char		newoffset[16];
	int		value;
	target_t	*last = NULL;

	tmp = author_list;
	while(tmp) {
		last = tmp;
		value = strcmp(tmp->au_author, author);
		if ( value == 0 ) {
			sprintf(newoffset, "|%s", offset);
			if (tmp->au_small) {
				tmp->au_length += strlen(newoffset);
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
		} else if (value < 0) {
			tmp = tmp->au_left;
		} else {
			tmp = tmp->au_right;
		}
	}

	tmp = (target_t *)malloc( sizeof(target_t) );
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
	tmp->au_left   = NULL;
	tmp->au_right  = NULL;

	if (last == NULL) {
		author_list = tmp;
	} else if (value < 0) {
		last->au_left = tmp;
	} else {
		last->au_right = tmp;
	}
}


void
add_series_entry(char *series, char *offset)
{
	series_t	*tmp;
	char		newoffset[16];

	tmp = series_list;
	while(tmp) {
		if ( strcmp(tmp->ss_series, series) == 0 ) {
			sprintf(newoffset, "|%s", offset);
			if (tmp->ss_small) {
				tmp->ss_length += strlen(newoffset);
				if (tmp->ss_length > (SMALLSIZE-1)) {
					char *ptr;

					ptr = (char *)malloc(LARGESIZE);
					strcpy(ptr, tmp->ss_offsets);
					free(tmp->ss_offsets);
					tmp->ss_offsets = ptr;
					tmp->ss_small = 0;
				}
			}
			strcat(tmp->ss_offsets, newoffset);
			return;
		}
		tmp = tmp->ss_next;
	}

	tmp = (series_t *)malloc( sizeof(series_t) );
	if ( tmp == NULL ) {
		perror("out of memory");
		exit(1);
	}

	tmp->ss_series = (char *)malloc( strlen(series) + 1);
	strcpy(tmp->ss_series, series);

	tmp->ss_offsets = (char *)malloc(SMALLSIZE);
	strcpy(tmp->ss_offsets, offset);
	tmp->ss_length = strlen(offset);
	tmp->ss_small  = 1;
	tmp->ss_next = series_list;
	series_list = tmp;
}


void
add_tag_entry(char *tag, char *offset)
{
	tag_t	*tmp;
	char	newoffset[16];
	int 	value;
	tag_t	*last = NULL;

	tmp = tag_list;
	while(tmp) {
		last = tmp;
		value = strcmp(tmp->ta_tag, tag);
		if ( value == 0 ) {
			sprintf(newoffset, "|%s", offset);
			if (tmp->ta_small) {
				tmp->ta_length += strlen(newoffset);
				if (tmp->ta_length > (SMALLSIZE-1)) {
					char *ptr;

					ptr = (char *)malloc(LARGESIZE);
					strcpy(ptr, tmp->ta_offsets);
					free(tmp->ta_offsets);
					tmp->ta_offsets = ptr;
					tmp->ta_small = 0;
				}
			}
			strcat(tmp->ta_offsets, newoffset);
			return;
		} else if (value < 0) {
			tmp = tmp->ta_left;
		} else {
			tmp = tmp->ta_right;
		}
	}

	tmp = (tag_t *)malloc( sizeof(tag_t) );
	if ( tmp == NULL ) {
		perror("out of memory");
		exit(1);
	}
	tmp->ta_tag = (char *)malloc( strlen(tag) + 1);
	strcpy(tmp->ta_tag, tag);

	tmp->ta_offsets = (char *)malloc(SMALLSIZE);
	strcpy(tmp->ta_offsets, offset);
	tmp->ta_length = strlen(offset);
	tmp->ta_small  = 1;
	tmp->ta_left   = NULL;
	tmp->ta_right  = NULL;

	if (last == NULL) {
		tag_list = tmp;
	} else if (value < 0) {
		last->ta_left = tmp;
	} else {
		last->ta_right = tmp;
	}
}


void
add_authpseudo_entry(char *author, char *pseudo)
{
	target2_t	*tmp;
	char		newoffset[16];

	if (pseudo) {
		tmp = author_list2;
		while(tmp) {
			if ( strcmp(tmp->au_author, author) == 0 ) {
				if (tmp->au_pseudo) {
					if ( strcmp(tmp->au_pseudo, pseudo) == 0 ) {
						return;
					}
				} else {
					tmp->au_pseudo = (char *)malloc( strlen(pseudo) + 1);
					strcpy(tmp->au_pseudo, pseudo);
				}
			}
			tmp = tmp->au_next;
		}
	}

	tmp = (target2_t *)malloc( sizeof(target2_t) );
	if ( tmp == NULL ) {
		perror("out of memory");
		exit(1);
	}
	tmp->au_author = (char *)malloc( strlen(author) + 1);
	strcpy(tmp->au_author, author);
	if (pseudo) {
		tmp->au_pseudo = (char *)malloc( strlen(pseudo) + 1);
		strcpy(tmp->au_pseudo, pseudo);
	} else {
		tmp->au_pseudo = NULL;
	}

	tmp->au_next = author_list2;
	author_list2 = tmp;
}


void
create_author_list(titlerec_t *list)
{
	titlerec_t	*tmp;
	int		firsttime = 1;
	char		*author;
	char		*ptr;

	tmp = list;
	while( tmp ) {
		author = tmp->se_author;
		if ( strstr(author, "^") ) {
			auset_t *tmp2;

			tmp2 = (auset_t *)decompose(author);
			separate(tmp2);
			while( tmp2 ) {
				char	auth2[256];

				author = tmp2->au_actual;
				while ( strstr(author, "+") ) {
					ptr = (char *)strstr(author, "+");
					*ptr = 0;
					sprintf(auth2, "%s^%s", author, tmp2->au_pseudo);
					add_author_entry(auth2, tmp->se_offset);
					author = ++ptr;
				}
				sprintf(auth2, "%s^%s", author, tmp2->au_pseudo);
				add_author_entry(auth2, tmp->se_offset);
				tmp2 = tmp2->au_next;
			}
		} else {
			while ( strstr(author, "+") ) {
				ptr = (char *)strstr(author, "+");
				*ptr = 0;
				add_author_entry(author, tmp->se_offset);
				author = ++ptr;
			}
			add_author_entry(author, tmp->se_offset);
		}
		tmp = tmp->se_next;
	}
}


void
create_series_list(titlerec_t *list)
{
	titlerec_t	*tmp;

	tmp = list;
	while( tmp ) {
		if (tmp->se_series && tmp->se_series[0]) {
			add_series_entry(tmp->se_series, tmp->se_offset);
		}
		tmp = tmp->se_next;
	}
}

void
create_tag_list(titlerec_t *list)
{
	titlerec_t	*tmp;
	char		*pubs;
	char		*ptr;

	tmp = list;
	while( tmp ) {
		if ( tmp->se_pubs && tmp->se_pubs[0]) {
			pubs = tmp->se_pubs;
			ptr  = strstr(pubs, ",");
			while(ptr) {
				*ptr = 0;
				add_tag_entry(pubs, tmp->se_offset);
				pubs = ptr+1;
				ptr = strstr(pubs, ",");
			}
			add_tag_entry(pubs, tmp->se_offset);
		}
		tmp = tmp->se_next;
	}
}


void
create_authpseudo_list(titlerec_t *list)
{
	titlerec_t	*tmp;
	int		firsttime = 1;
	char		*author;
	char		*ptr;

	tmp = list;
	while( tmp ) {
		author = tmp->se_author;
		if ( strstr(author, "^") ) {
			auset_t *tmp2;

			tmp2 = (auset_t *)decompose(author);
			separate(tmp2);
			while( tmp2 ) {
				author = tmp2->au_actual;
				while ( strstr(author, "+") ) {
					ptr = (char *)strstr(author, "+");
					*ptr = 0;
					add_authpseudo_entry(author, tmp2->au_pseudo);
					author = ++ptr;
				}
				add_authpseudo_entry(author, tmp2->au_pseudo);
				tmp2 = tmp2->au_next;
			}
		} else {
			while ( strstr(author, "+") ) {
				ptr = (char *)strstr(author, "+");
				*ptr = 0;
				add_authpseudo_entry(author, NULL);
				author = ++ptr;
			}
			add_authpseudo_entry(author, NULL);
		}
		tmp = tmp->se_next;
	}
}


void
add_title(search_t *awt, int offset)
{
	titlerec_t	*tmp;

	tmp = (titlerec_t *)malloc( sizeof(titlerec_t) );
	if ( tmp == NULL ) {
		perror("out of memory");
		exit(1);
	}

	sprintf(tmp->se_offset, "%x", offset);
	if (mode == DO_AUTHOR) {
		tmp->se_author = (char *)malloc( strlen(awt->se_author) + 1);
		strcpy(tmp->se_author, awt->se_author);
	}
	if (mode == DO_SERIES) {
		if (awt->se_series && awt->se_series[0]) {
			tmp->se_series = (char *)malloc( strlen(awt->se_series) + 1);
			strcpy(tmp->se_series, awt->se_series);
		} else {
			tmp->se_series = NULL;
		}
	}
	if (mode == DO_TAG) {
		if (awt->se_pubs && awt->se_pubs[0]) {
			tmp->se_pubs = (char *)malloc( strlen(awt->se_pubs) + 1);
			strcpy(tmp->se_pubs, awt->se_pubs);
		} else {
			tmp->se_pubs = NULL;
		}
	}

	tmp->se_next = title_list;
	title_list = tmp;
}


void
parse_file(char *filename)
{
	FILE	*fp;
	int	line_number = 1;
	int	index;
	char	input;
	int	this_offset = 0;
	int	next_offset = 0;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		search_t *set;

		this_offset = next_offset;
		set = parse_title_entry(fp, (int *)&next_offset, PARSE_NOMALLOC);
		if ( set == NULL) {
			goto finish;
		}

		line_number++;
		add_title(set, this_offset);
	}

finish:
	fclose(fp);
}


void
search_file(char *filename, char *exten)
{
	object_t	*tmp;
	attr_t		*attr;
	extern int	line_number;

	line_number = 1;
	parse_pubs(filename);

	tmp = Objlist;
	while(tmp) {
		printf("%s|", tmp->ob_name);
		do_attribute("AE", tmp->ob_attrs, NOOPTIONS);
		if (do_attribute("YR", tmp->ob_attrs, NOOPTIONS)) {
			fprintf(stderr, "Error: No Year for [%s]\n", tmp->ob_name);
			exit(1);
		}
		do_attribute("SE", tmp->ob_attrs, NOOPTIONS);
		do_attribute("SS", tmp->ob_attrs, NOOPTIONS);
		do_attribute("PB", tmp->ob_attrs, NOOPTIONS);
		do_attribute("SL", tmp->ob_attrs, NOOPTIONS);
		do_attribute("NT", tmp->ob_attrs, DO_NOTES);
		do_attribute("SY", tmp->ob_attrs, DO_SYNOP);
		do_attribute("SN", tmp->ob_attrs, NOOPTIONS);
		do_attribute("TG", tmp->ob_attrs, NOOPTIONS);
		do_attribute("TR", tmp->ob_attrs, NOOPTIONS);
		printf("%s|\n", exten);

		tmp = tmp->ob_next;
	}

	Objlist = NULL;
}


void
checkpseudos()
{
	target2_t	*tmp;
	target2_t	*tmp2;

	printf("PASS 1 - Checking for inconsistant use of pseudonyms\n");
	printf("====================================================\n");

	/*
	 * Go through the list, finding each pseudonym
	 */
	tmp = author_list2;
	while(tmp) {
		if (tmp->au_pseudo && tmp->au_pseudo[0] ) {
			tmp2 = author_list2;
			while(tmp2) {
				if ( strcmp(tmp->au_pseudo, tmp2->au_author) == 0) {
					printf("Author |%s| should read |%s^%s|\n",
						tmp2->au_author, tmp->au_author, tmp->au_pseudo);
				}
				tmp2 = tmp2->au_next;
			}
		}
		tmp = tmp->au_next;
	}

}


float
similar(string1, string2)
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


void
checksimilar()
{
	target2_t	*tmp;
	target2_t	*tmp2;

	printf("\n\n");
	printf("PASS 2 - Checking for typographical errors\n");
	printf("====================================================\n");

	/*
	 * Go through the list, finding each pseudonym
	 */
	tmp = author_list2;
	while(tmp) {
		tmp2 = tmp->au_next;
		while(tmp2) {
			float result;

			if (strcmp(tmp->au_author, tmp2->au_author)) {
				result = similar(tmp->au_author, tmp2->au_author);
				if ( result > 1.7) {
					printf("Compare |%s| to |%s| (%f)\n",
						tmp->au_author, tmp2->au_author, result);
				}
			}
			tmp2 = tmp2->au_next;
		}
		tmp = tmp->au_next;
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
	titlerec_t	*tmp;
	char		path[256];
	FILE		*fp;

	close(1);
	fp = freopen(argv[2], "wb", stdout);


	while ( (option = getopt(argc, argv, OPTARGS)) != -1) {
		switch(option) {
		case 't':
			strcpy(path, BASE);
			strcat(path, "/dbase.ascii");
			result = chdir(path);
                        if (result != 0) {
                                printf("CHDIR to %s failed\n", path);
                                exit(1);
                        }
			notes_fp = fopen("NOTES", "wb+");
			search_file("NOVELS",       "n");
			search_file("ANTHOLOGIES",  "a");
			search_file("COLLECTIONS",  "c");
			search_file("SHORTFICTION", "sf");
			search_file("NONFICTION",   "nf");
			search_file("POEMS",	    "p");
			search_file("NONGENRE",	    "ng");
			search_file("ARTWORK",	    "ar");
			search_file("INTERIORS",    "ai");
			search_file("SERIALS",      "se");
			search_file("ESSAYS",       "es");
			search_file("OMNIBUS",      "o");
			search_file("EDITOR",       "e");
			search_file("TITLES.REVIEWS","r");
			search_file("TITLES.INTERV","i");
			fclose(notes_fp);
			break;
		case 'a':
			mode = DO_AUTHOR;
			parse_file("titles.dbase");
			create_author_list(title_list);
			print_author_entries(author_list);
			break;
		case 'c':
			strcpy(path, BASE);
			strcat(path, "/dbase.compiled");
			result = chdir(path);
                        if (result != 0) {
                                printf("CHDIR to %s failed\n", path);
                                exit(1);
                        }
			mode = DO_AUTHOR;
			parse_file("titles.dbase");
			create_authpseudo_list(title_list);
			checkpseudos();
			checksimilar();
			break;
		case 's':
			mode = DO_SERIES;
			parse_file("titles.dbase");
			create_series_list(title_list);
			print_series_entries();
			break;
		case 'g':
			mode = DO_TAG;
			parse_file("titles.dbase");
			create_tag_list(title_list);
			print_tag_entries(tag_list);
			break;
		}
		exit(0);
	}
}
