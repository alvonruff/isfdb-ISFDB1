/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)Aw.c	1.15	01/05/00 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef SUNOS
#include <sys/unistd.h>
#endif
#include "sfdbase.h"

void get_award_info(char *exact_author, search_t *title_list, char *awardname);

static char	tmpauthor[MEDIUMSIZE];
static char	category[BIGSIZE];
entry		entries[MAX_ENTRIES];
int		newlevel;
int		rightmargin = 0;
int		nonefound   = 1;
int		locus       = 0;

extern search_t	*title_list;

static char *shortcats[] = {
	"n",
	"fn",
	"nv",
	"nt",
	"ss",
	"sf",
	"c",
	"a",
	"om",
	"nf",
	"ac",
	"at",
	"dp",
	"ed",
	"fz",
	"fw",
	"fa",
	"ar",
	"sz",
};

#define CATMAX 19
static char *longcats[] = {
	"Novel",
	"First Novel",
	"Novella",
	"Novelette",
	"Short Story",
	"Short Fiction",
	"Collection",
	"Anthology",
	"Other Media",
	"Non-Fiction",
	"Anthology/Collection",
	"Professional Artist",
	"Dramatic Presentation",
	"Professional Editor",
	"Fanzine",
	"Fan Writer",
	"Fan Artist",
	"Best Original Artwork",
	"Best Semi-Prozine",
};

#define PREFMAX 14
static char *prefcats[] = {
	"Novel",
	"SF Novel",
	"Fantasy Novel",
	"Book length Fantasy (old)",
	"Horror/Dark Fantasy Novel",
	"First Novel",
	"Novella",
	"Novelette",
	"Short Story",
	"Short Fiction",
	"Anthology",
	"Collection",
	"Related Non-Fiction Book",
	"Short Fiction (old)",
};


void
print_film_title(int indent, char *title, char *film, int rightmargin, int options)
{
	unsigned char percent = 0x25;
	char *tmp;
	int	loop;
	int 	position = 0;;

	if (! (options & T_PREINDENT)) {
		if (options & T_WINNER) {
			printf("o");
			position++;
			for(loop=0; loop<(indent-1); loop++) {
				printf(" ");
				position++;
			}
		} else {
			for(loop=0; loop<indent; loop++) {
				printf(" ");
				position++;
			}
		}
	}

        printf("<a href=\"http://us.imdb.com/M/title-exact?");
        tmp = film;
        while (*tmp) {
                if (*tmp == ' ') {
                        fwrite(&percent, 1, 1, stdout);
                        printf("20");
                } else {
                        printf("%c", *tmp);
                }
                tmp++;
        }
        printf("\">%s</a>", title);
        position += strlen(title);
	while(position < rightmargin) {
		printf(" ");
		position++;
	}
}


void
print_spaward(int level)
{
	switch(level) {
	case 71:
		printf("<i>No Winner -- Insufficient Votes</i>\n");
		break;
	case 72:
		printf("<i>Not on ballot -- Insufficient Nominations</i>\n");
		break;
	case 73:
		printf("<i>No Award Given This Year</i>\n");
		break;
	case 81:
		printf("<i>Withdrawn:</i>\n");
		break;
	case 82:
		printf("<i>Withdrawn - Nomination Declined:</i>\n");
		break;
	case 83:
		printf("<i>Withdrawn - Conflict of Interest:</i>\n");
		break;
	case 84:
		printf("<i>Withdrawn - Official Publication in a Previous Year:</i>\n");
		break;
	case 85:
		printf("<i>Withdrawn - Ineligible:</i>\n");
		break;
	case 91:
		printf("<i>Made First Ballot:</i>\n");
		break;
	case 92:
		printf("<i>Preliminary Nominee's:</i>\n");
		break;
	case 93:
		printf("<i>Honorable Mentions:</i>\n");
		break;
	case 98:
		printf("<i>Early Submissions:</i>\n");
		break;
	case 99:
		printf("<i>Nominations Below Cutoff:</i>\n");
		break;
	}
}

int
special_award(char *title)
{
	if ( strncmp(title, "Rank 71 =", 9) == 0) {
		return(1);
	} else if ( strncmp(title, "Rank 72 =", 9) == 0) {
		return(1);
	} else if ( strncmp(title, "Rank 73 =", 9) == 0) {
		return(1);
	} else if ( strncmp(title, "Rank 81 =", 9) == 0) {
		return(1);
	} else if ( strncmp(title, "Rank 82 =", 9) == 0) {
		return(1);
	} else if ( strncmp(title, "Rank 83 =", 9) == 0) {
		return(1);
	} else if ( strncmp(title, "Rank 84 =", 9) == 0) {
		return(1);
	} else if ( strncmp(title, "Rank 85 =", 9) == 0) {
		return(1);
	} else if ( strncmp(title, "Rank 91 =", 9) == 0) {
		return(1);
	} else if ( strncmp(title, "Rank 92 =", 9) == 0) {
		return(1);
	} else if ( strncmp(title, "Rank 93 =", 9) == 0) {
		return(1);
	} else if ( strncmp(title, "Rank 98 =", 9) == 0) {
		return(1);
	} else if ( strncmp(title, "Rank 99 =", 9) == 0) {
		return(1);
	} else {
		return(0);
	}
}

#define AW_USENOMS	0
#define AW_USEWINS	1
#define AW_USECAT	1
#define AW_NOCAT	0


void
print_award_set(int winner, int usecat, int catindex)
{
	search_t *tmp;
	int	 need_title = 1;
	int	 lastlevel = 0;

	tmp = title_list;
	while( tmp ) {
	
		if ( tmp->se_marker) {
			tmp = tmp->se_next;	
			continue;
		}
		if (winner) {
			if ( strcmp(tmp->se_year, "1") ) {
				tmp = tmp->se_next;	
				continue;
			}
		} else {
			if ( strcmp(tmp->se_year, "1") == 0) {
				tmp = tmp->se_next;	
				continue;
			}
		}
	
		sscanf(tmp->se_year, "%d", &newlevel);
		if (usecat) {
			if ( strcmp(shortcats[catindex], tmp->se_type) ) {
				tmp = tmp->se_next;	
				continue;
			}
			if (need_title) {
				printf("\n<b>%s</b>\n", longcats[catindex] );
				need_title = 0;
			}
		} else {
			if (!tmp->se_series) {
				tmp = tmp->se_next;
				continue;
			}
			if (strcmp(category, tmp->se_series)) {
				tmp = tmp->se_next;
				continue;
			}
		}
	
		tmp->se_marker = 1;
		if ( strcmp(tmp->se_title, "untitled") == 0 ) {
			strcpy(tmp->se_title, "----");
		} else if ( special_award(tmp->se_title) ) {
			tmp = tmp->se_next;
			continue;
		}
	
		if (newlevel > 69) {
			if (newlevel != lastlevel) {
				lastlevel = newlevel;
				print_spaward(newlevel);
			}
		}

		if (locus) {
			if (winner) {
				printf(" <b>1</b> ");
			} else {
				printf("%2s ", tmp->se_year);
			}
			if (tmp->se_superseries && tmp->se_superseries[0]) {
				print_film_title(3, tmp->se_title, tmp->se_superseries, rightmargin, T_PREINDENT);
			} else {
				print_title(3, tmp->se_title, rightmargin, T_PREINDENT, NULL);
			}
		} else {
			if (winner) {
				if (tmp->se_superseries && tmp->se_superseries[0]) {
					print_film_title(3, tmp->se_title, tmp->se_superseries, rightmargin, T_WINNER);
				} else {
					print_title(3, tmp->se_title, rightmargin, T_WINNER, NULL);
				}
			} else {
				if (tmp->se_superseries && tmp->se_superseries[0]) {
					print_film_title(3, tmp->se_title, tmp->se_superseries, rightmargin, T_NOOPT);
				} else {
					print_title(3, tmp->se_title, rightmargin, T_NOOPT, NULL);
				}
			}
		}
	
		if ( strcmp(tmp->se_author, "(********)") ) {
			html_print_authors(tmp->se_author, rightmargin);
		} else {
			printf("\n");
		}
	
		nonefound = 0;
		tmp = tmp->se_next;	
	}
}


void
print_awards(char *type, char *year)
{
	search_t        *tmp;
	int		loop;
	int		int_year;
	char		ascii_year[8];

	/*
	 * If this is a Locus poll, set the locus flag
	 */
	printf("<hr>");
	if ((strcmp(type, "Lc") == 0) ||
	    (strcmp(type, "An") == 0) ||
	    (strcmp(type, "Ar") == 0)) {
		locus = 1;
	}

	/*
	 * Find the longest title in this awards listing. If the
	 * longest title is less than 50 characters, pull in the
	 * right margin. While we are visiting each record, clear
	 * the print marker.
	 */
	tmp = title_list;
	while( tmp ) {
		tmp->se_marker = 0;
		if ( strlen(tmp->se_title) > rightmargin) {
			rightmargin = strlen(tmp->se_title);
		}
		tmp = tmp->se_next;	
	}

	/*
	 * Add 3 for the indent, 2 for the trailing pad, and
	 * 5 for the possible title type. Don't let the rightmargin
	 * exceed 50 characters.
	 */
	if (locus) {
		rightmargin += 5;
	} else {
		rightmargin += 10;
	}
	if (rightmargin > 50) {
		rightmargin = 50;
	}

	/*
	 * PASS 1: Look through the prefered list. This
	 * will allow fiction categories to be displayed
	 * before editor, film, etc.. awards.
	 */
	for(loop=0; loop<PREFMAX; loop++) {
		tmp = title_list;
		while( tmp ) {

			/* Skip records that have already been printed.  */
			if ( tmp->se_marker) {
				tmp = tmp->se_next;	
				continue;
			}

			/* If the record has a category and it matches a preference... */
			if (tmp->se_series && ( strcmp(prefcats[loop], tmp->se_series) == 0) ) {
				strcpy(category, tmp->se_series);
				printf("\n<b>%s</b>\n", category);
				print_award_set(AW_USEWINS, AW_NOCAT, 0);
				print_award_set(AW_USENOMS, AW_NOCAT, 0);
				break;
			}
			tmp = tmp->se_next;	
		}
	}

	/*
	 * PASS 2: Now go through the records again, and do
	 * the rest of the records with customized categories.
	 */
	tmp = title_list;
	while( tmp ) {

		/*
		 * If the award has a customized type, and it
		 * hasn't already been used, use it.
		 */
		if (tmp->se_series && (tmp->se_marker == 0)) {
			strcpy(category, tmp->se_series);
			printf("\n<b>%s</b>\n", category);
			print_award_set(AW_USEWINS, AW_NOCAT, loop);
			print_award_set(AW_USENOMS, AW_NOCAT, loop);
		}
		tmp = tmp->se_next;	
	}

	/*
	 * PASS 3: Now go through the records again, and do
	 * the rest of the records that don't use customized
	 * categories.
	 */
	for(loop=0; loop<CATMAX; loop++) {
		print_award_set(AW_USEWINS, AW_USECAT, loop);
		print_award_set(AW_USENOMS, AW_USECAT, loop);
	}

	if (nonefound) {
		printf("<b>No awards available for %s</b>\n", year);
	}

	printf("\n<hr>\n");
	sscanf(year, "%d", &int_year);
	sprintf(ascii_year, "%d", int_year-1);
	html_print_award_cite_msg(type, ascii_year, "Previous Year" );
	sprintf(ascii_year, "%d", int_year+1);
	html_print_award_cite_msg(type, ascii_year, "Next Year" );
	printf("[<a href=\"http://%s/sfdbase.html\">Main Menu</a>]", HTMLLOC);
	printf("[<a href=\"http://%s/search.html\">Search</a>]", HTMLLOC);
}


/*
 * add_award() allocates a data structure for the award
 * record, and puts it on the award linked-list in
 * ascending level order.
 */
static void
add_award(char *title,
	char *author,
	char *year,
	char *type,
	char *level,
	search_t *list,
	char *awardname,
	char *category,
	char *movie)
{
	search_t        *tmp;
	search_t        *find;

	/*
	 * Allocate a search record for the award
	 */
	tmp = (search_t *)malloc( sizeof(search_t) );
	if ( tmp == NULL ) {
		perror("out of memory");
		exit(1);
	}

	tmp->se_title  = (char *)malloc( strlen(title) + 1);
	tmp->se_author = (char *)malloc( strlen(author) + 1);
	tmp->se_type   = (char *)malloc( strlen(type) + 1);
	tmp->se_year   = (char *)malloc( strlen(year) + 1);
	strcpy(tmp->se_title,  title);
	strcpy(tmp->se_author, author);
	strcpy(tmp->se_type,   type);
	strcpy(tmp->se_year,   level);

	if (category) {
		tmp->se_series  = (char *)malloc( strlen(category) + 1);
		strcpy(tmp->se_series, category);
	} else {
		tmp->se_series  = NULL;
	}
	if (movie) {
		tmp->se_superseries  = (char *)malloc( strlen(movie) + 1);
		strcpy(tmp->se_superseries, movie);
	} else {
		tmp->se_superseries  = NULL;
	}
	sscanf(level, "%d", &(tmp->se_numyear) );


	/*
	 * Put the record on the award list. Records
	 * are put on the list such as the award level
	 * (overloaded into the se_numyear field)
	 * as in ascending order.
	 */
	if (title_list == NULL) {
		tmp->se_next = title_list;
		title_list = tmp;
	} else {

		/*
		 * The list is not doubly-linked, but
		 * we always need to put the new record
		 * before the target record. So we
		 * refer to se_next->se_numyear.
		 */
		find = title_list;
		if ( tmp->se_numyear <= find->se_numyear) {
			/* 
			 * Special-case where the record
			 * goes on the front of the list.
			 */
			tmp->se_next = title_list;
			title_list = tmp;
			return;
		} 

		/*
		 * Normal case, were the record goes anywhere
		 * but first or last.
		 */
		tmp->se_next = NULL;
		while( find->se_next) {
			if ( tmp->se_numyear <= find->se_next->se_numyear) {
				tmp->se_next = find->se_next;
				find->se_next = tmp;
				return;
			}
			find = find->se_next;
		}

		/*
		 * Put the record on the end of the list
		 */
		find->se_next = tmp;
		return;
	}
}


static void
parse_awards(char *exact_author, search_t *list, char *awardname)
{
	FILE	*fp;
	FILE	*fp2;
	int	line_number = 1;
	int	next_offset = 0;

	fp = fopen("awards.xby", "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		if ( parse_field(fp, tmpauthor, (int *)&next_offset, MEDIUMSIZE) == -1 ) {
			goto finish;
		}

		if (strcmp(tmpauthor, exact_author) == 0) {

			fp2 = fopen("awards.dbase", "rb");
			if (fp2 == NULL) {
				perror("Couldn't open dbase");
				exit(1);
			}

			while(1) {
				char offset[16];
				int eol;
				int int_offset;
				award_t *awt;

				parse_field_or_eol(fp, offset, &eol, 16);
				sscanf(offset, "%x", &int_offset);
				fseek(fp2, int_offset, SEEK_SET);
				awt = parse_award_entry(fp2, (int *)&next_offset, 1);
				if (awt == NULL) {
					break;
				}
				if ( strcmp(awt->aw_award, awardname) == 0) {
					add_award(awt->aw_title, awt->aw_author, awt->aw_year, 
						awt->aw_types, awt->aw_level, list, 
						awardname, awt->aw_typel, awt->aw_movie);
				}
				if ( eol ) {
					break;
				}
			}
			fclose(fp2);
			goto finish;

		} else if ( parse_to_eol(fp, (int *)&next_offset) == -1 ) {
			goto finish;
		}
		line_number++;
	}

finish:
	fclose(fp);
}


void
get_award_info(char *exact_author, search_t *title_list, char *awardname)
{
        int result;

#ifdef CHDIR
        result = chdir(CGIBIN);
        if (result != 0) {
                printf("CHDIR to %s failed\n", CGIBIN);
                exit(1);
        }

#endif
	parse_awards(exact_author, title_list, awardname);
}

