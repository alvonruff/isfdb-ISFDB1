/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)parse.c	1.11	06/10/97 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sfdbase.h"

static char	tmpspace1[BIGSIZE];
static char	tmpspace2[BIGSIZE];
static char	tmpspace3[BIGSIZE];
static char	tmpspace4[BIGSIZE];
static char	tmpspace5[BIGSIZE];
static char	tmpspace6[BIGSIZE];
static char	tmpspace7[BIGSIZE];
static char	tmpspace8[BIGSIZE];
static char	tmpspace9[BIGSIZE];
static char	tmpspace10[BIGSIZE];
static char	tmpspace11[BIGSIZE];
static char	tmpspace12[BIGSIZE];
static char	tmpspace13[BIGSIZE];
static char	tmpspace14[BIGSIZE];

char		configfile[MEDIUMSIZE];

int		perrno    = 0;
int		errcount  = 0;
char		*pstring  = NULL;
pub_t		*pub_list = NULL;
pub_t		*pub_end  = NULL;
inter_t         *it_list  = NULL;
inter_t         *it_end   = NULL;
review_t        *rv_list  = NULL;
review_t        *rv_end   = NULL;
audata_t	*at_list  = NULL;
audata_t	*at_end   = NULL;
award_t		*misc_awards = NULL;
award_t		nonmalloc;
search_t	nonmalloc2;


void
eat_comment(FILE *fp)
{
	int input;

	input = getc(fp);
	while (input == '#') {
		while( input != '\n')
			input = getc(fp);
		input = getc(fp);
	}
	ungetc(input, fp);
}

int
parse_field(FILE *fp, char *string, int *total, int maxsize)
{
	int	input = 0;
	int	index = 0;

	while( (input != '|') && (input != '\n') ) {
		input = getc(fp);
		if ( input == -1) {
			string[index] = 0;
			perrno = 1;
			errcount = index;
			pstring = string;
			return(-1);
		}
		(*total)++;
		string[index++] = input;
		if ( index > (maxsize-1) ) {
			string[(maxsize-1)] = 0;
			perrno = 2;
			errcount = index;
			pstring = string;
			return(-1);
		}
	}
	string[--index] = 0;
	return(0);
}

int
parse_field_or_eol(FILE *fp, char *string, int *eol, int maxsize)
{
	int	input = 0;
	int	index = 0;

	while( (input != '|') && (input != '\n') ) {
		input = getc(fp);
		if ( input == -1) {
			string[index] = 0;
			return(-1);
		}
		string[index++] = input;
		if ( index > (maxsize-1) ) {
			string[(maxsize-1)] = 0;
			return(-1);
		}
	}
	if (input == '\n') {
		*eol = 1;
	} else {
		*eol = 0;
	}
	string[--index] = 0;
	return(0);
}

int
parse_to_eol(FILE *fp, int *total)
{
	int	input = 0;

	while( input != '\n') {
		input = getc(fp);
		if ( input == -1) {
			return(-1);
		}
		(*total)++;
	}
	return(0);
}


search_t *
parse_title_entry(FILE	*fp, int *offset, int nomalloc)
{
	search_t	*tmp;
	int		len;
	static int	first = 1;

	if (first) {
		first = 0;
		nonmalloc2.se_title       = tmpspace1;
		nonmalloc2.se_author      = tmpspace2;
		nonmalloc2.se_year        = tmpspace3;
		nonmalloc2.se_series      = tmpspace4;
		nonmalloc2.se_superseries = tmpspace5;
		nonmalloc2.se_pubs        = tmpspace6;
		nonmalloc2.se_storylen    = tmpspace7;
		nonmalloc2.se_notes       = tmpspace8;
		nonmalloc2.se_synopsis    = tmpspace9;
		nonmalloc2.se_seriesnum   = tmpspace10;
		nonmalloc2.se_type        = tmpspace11;
		nonmalloc2.se_awtags      = tmpspace12;
		nonmalloc2.se_translator  = tmpspace13;
	}

	if ( parse_field(fp, tmpspace1, offset, BIGSIZE) == -1 ) {
		return(NULL);
	}
	if ( parse_field(fp, tmpspace2, offset, BIGSIZE) == -1 ) {
		print_fatal("parse title: error parsing author.\n");
		return(NULL);
	}
	if ( parse_field(fp, tmpspace3, offset, BIGSIZE) == -1 ) {
		print_fatal("parse title: error parsing year.\n");
		return(NULL);
	}
	if ( parse_field(fp, tmpspace4, offset, BIGSIZE) == -1 ) {
		print_fatal("parse title: error parsing series.\n");
		return(NULL);
	}
	if ( parse_field(fp, tmpspace5, offset, BIGSIZE) == -1 ) {
		print_fatal("parse title: error parsing superseries.\n");
		return(NULL);
	}
	if ( parse_field(fp, tmpspace6, offset, BIGSIZE) == -1 ) {
		print_fatal("parse title: error parsing pubs.\n");
		return(NULL);
	}
	if ( parse_field(fp, tmpspace7, offset, BIGSIZE) == -1 ) {
		print_fatal("parse title: error parsing storylen.\n");
		return(NULL);
	}
	if ( parse_field(fp, tmpspace8, offset, BIGSIZE) == -1 ) {
		print_fatal("parse title: error parsing notes.\n");
		return(NULL);
	}
	if ( parse_field(fp, tmpspace9, offset, BIGSIZE) == -1 ) {
		print_fatal("parse title: error parsing synopsis.\n");
		return(NULL);
	}
	if ( parse_field(fp, tmpspace10, offset, BIGSIZE) == -1 ) {
		print_fatal("parse title: error parsing series number.\n");
		return(NULL);
	}
	if ( parse_field(fp, tmpspace12, offset, BIGSIZE) == -1 ) {
		print_fatal("parse title: error parsing award tags.\n");
		return(NULL);
	}
	if ( parse_field(fp, tmpspace13, offset, BIGSIZE) == -1 ) {
		print_fatal("parse title: error parsing award tags.\n");
		return(NULL);
	}
	if ( parse_field(fp, tmpspace11, offset, BIGSIZE) == -1 ) {
		print_fatal("parse title: error parsing type.\n");
		return(NULL);
	}
	if ( parse_to_eol(fp, offset) == -1 ) {
		return(NULL);
	}

	if (nomalloc) {
		tmp = (search_t *)&nonmalloc2;
	} else {
		tmp = (search_t *)malloc( sizeof(search_t) );

		len = strlen(tmpspace1);
		if (len) {
			tmp->se_title = (char *)malloc(len+1);
			strcpy(tmp->se_title, tmpspace1);
		} else {
			tmp->se_title = NULL;
		}
		len = strlen(tmpspace2);
		if (len) {
			tmp->se_author = (char *)malloc(len+1);
			strcpy(tmp->se_author, tmpspace2);
		} else {
			tmp->se_author = NULL;
		}
		len = strlen(tmpspace3);
		if (len) {
			tmp->se_year = (char *)malloc(len+1);
			strcpy(tmp->se_year, tmpspace3);
		} else {
			tmp->se_year = NULL;
		}
		len = strlen(tmpspace4);
		if (len) {
			tmp->se_series = (char *)malloc(len+1);
			strcpy(tmp->se_series, tmpspace4);
		} else {
			tmp->se_series = NULL;
		}
		len = strlen(tmpspace5);
		if (len) {
			tmp->se_superseries = (char *)malloc(len+1);
			strcpy(tmp->se_superseries, tmpspace5);
		} else {
			tmp->se_superseries = NULL;
		}
		len = strlen(tmpspace6);
		if (len) {
			tmp->se_pubs = (char *)malloc(len+1);
			strcpy(tmp->se_pubs, tmpspace6);
		} else {
			tmp->se_pubs = NULL;
		}
		len = strlen(tmpspace7);
		if (len) {
			tmp->se_storylen = (char *)malloc(len+1);
			strcpy(tmp->se_storylen, tmpspace7);
		} else {
			tmp->se_storylen = NULL;
		}
		len = strlen(tmpspace8);
		if (len) {
			tmp->se_notes = (char *)malloc(len+1);
			strcpy(tmp->se_notes, tmpspace8);
		} else {
			tmp->se_notes = NULL;
		}
		len = strlen(tmpspace9);
		if (len) {
			tmp->se_synopsis = (char *)malloc(len+1);
			strcpy(tmp->se_synopsis, tmpspace9);
		} else {
			tmp->se_synopsis = NULL;
		}
		len = strlen(tmpspace10);
		if (len) {
			tmp->se_seriesnum = (char *)malloc(len+1);
			strcpy(tmp->se_seriesnum, tmpspace10);
		} else {
			tmp->se_seriesnum = NULL;
		}
		len = strlen(tmpspace11);
		if (len) {
			tmp->se_type = (char *)malloc(len+1);
			strcpy(tmp->se_type, tmpspace11);
		} else {
			tmp->se_type = NULL;
		}
		len = strlen(tmpspace12);
		if (len) {
			tmp->se_awtags = (char *)malloc(len+1);
			strcpy(tmp->se_awtags, tmpspace12);
		} else {
			tmp->se_awtags = NULL;
		}
		len = strlen(tmpspace13);
		if (len) {
			tmp->se_translator = (char *)malloc(len+1);
			strcpy(tmp->se_translator, tmpspace13);
		} else {
			tmp->se_translator = NULL;
		}
	}
	tmp->se_numyear   = 0;
	tmp->se_marker    = 0;
	tmp->se_pseudonym = NULL;
	tmp->se_offset    = NULL;
	tmp->se_awards    = NULL;
	tmp->se_next      = NULL;

	return(tmp);
}


award_t *
parse_award_entry(FILE	*fp, int *offset, int nomalloc)
{
	award_t		*tmp;
	static int	first = 1;
	int		len;

	if (first) {
		first = 0;
		nonmalloc.aw_title  = tmpspace1;
		nonmalloc.aw_author = tmpspace2;
		nonmalloc.aw_year   = tmpspace3;
		nonmalloc.aw_types  = tmpspace4;
		nonmalloc.aw_typel  = tmpspace5;
		nonmalloc.aw_level  = tmpspace6;
		nonmalloc.aw_movie  = tmpspace7;
		nonmalloc.aw_tag    = tmpspace8;
		nonmalloc.aw_award  = tmpspace9;
	}

	if ( parse_field(fp, tmpspace1, offset, BIGSIZE) == -1 ) {
		return(NULL);
	}
	if ( parse_field(fp, tmpspace2, offset, BIGSIZE) == -1 ) {
		return(NULL);
	}
	if ( parse_field(fp, tmpspace3, offset, BIGSIZE) == -1 ) {
		return(NULL);
	}
	if ( parse_field(fp, tmpspace4, offset, BIGSIZE) == -1 ) {
		return(NULL);
	}
	if ( parse_field(fp, tmpspace5, offset, BIGSIZE) == -1 ) {
		return(NULL);
	}
	if ( parse_field(fp, tmpspace6, offset, BIGSIZE) == -1 ) {
		return(NULL);
	}
	if ( parse_field(fp, tmpspace7, offset, BIGSIZE) == -1 ) {
		return(NULL);
	}
	if ( parse_field(fp, tmpspace8, offset, BIGSIZE) == -1 ) {
		return(NULL);
	}
	if ( parse_field(fp, tmpspace9, offset, BIGSIZE) == -1 ) {
		return(NULL);
	}
	if ( parse_to_eol(fp, offset) == -1 ) {
		return(NULL);
	}

	if (nomalloc) {
		tmp = (award_t *)&nonmalloc;
	} else {
		tmp = (award_t *)malloc( sizeof(award_t) );

		len = strlen(tmpspace1);
		if (len) {
			tmp->aw_title = (char *)malloc(len+1);
			strcpy(tmp->aw_title, tmpspace1);
		} else {
			tmp->aw_title = NULL;
		}
		len = strlen(tmpspace2);
		if (len) {
			tmp->aw_author = (char *)malloc(len+1);
			strcpy(tmp->aw_author, tmpspace2);
		} else {
			tmp->aw_author = NULL;
		}
		len = strlen(tmpspace3);
		if (len) {
			tmp->aw_year = (char *)malloc(len+1);
			strcpy(tmp->aw_year, tmpspace3);
		} else {
			tmp->aw_year = NULL;
		}
		len = strlen(tmpspace4);
		if (len) {
			tmp->aw_types = (char *)malloc(len+1);
			strcpy(tmp->aw_types, tmpspace4);
		} else {
			tmp->aw_types = NULL;
		}
		len = strlen(tmpspace5);
		if (len) {
			tmp->aw_typel = (char *)malloc(len+1);
			strcpy(tmp->aw_typel, tmpspace5);
		} else {
			tmp->aw_typel = NULL;
		}
		len = strlen(tmpspace6);
		if (len) {
			tmp->aw_level = (char *)malloc(len+1);
			strcpy(tmp->aw_level, tmpspace6);
		} else {
			tmp->aw_level = NULL;
		}
		len = strlen(tmpspace7);
		if (len) {
			tmp->aw_movie = (char *)malloc(len+1);
			strcpy(tmp->aw_movie, tmpspace7);
		} else {
			tmp->aw_movie = NULL;
		}
		len = strlen(tmpspace8);
		if (len) {
			tmp->aw_tag = (char *)malloc(len+1);
			strcpy(tmp->aw_tag, tmpspace8);
		} else {
			tmp->aw_tag = NULL;
		}
		len = strlen(tmpspace9);
		if (len) {
			tmp->aw_award = (char *)malloc(len+1);
			strcpy(tmp->aw_award, tmpspace9);
		} else {
			tmp->aw_award = NULL;
		}
	}

	tmp->aw_marker = 0;
	tmp->aw_next   = NULL;
	return(tmp);
}


audata_t *
get_author(char *author)
{
	FILE	*fp;
	int	line_number = 1;
	int	index;
	int	dummy;
	int	eol;
	char	input;
	audata_t	*tmp;

	fp = fopen("authors.dbase", "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		if ( parse_field(fp, tmpspace1, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( strcmp(tmpspace1, author) == 0 ) {
			tmp = (audata_t *)malloc( sizeof(audata_t) );

			if ( parse_field(fp, tmpspace2, (int *)&dummy, BIGSIZE) == -1 ) {
				goto finish;
			}
			if ( parse_field(fp, tmpspace3, (int *)&dummy, BIGSIZE) == -1 ) {
				goto finish;
			}
			if ( parse_field(fp, tmpspace4, (int *)&dummy, BIGSIZE) == -1 ) {
				goto finish;
			}
			if ( parse_field(fp, tmpspace5, (int *)&dummy, BIGSIZE) == -1 ) {
				goto finish;
			}
			if ( parse_field(fp, tmpspace6, (int *)&dummy, BIGSIZE) == -1 ) {
				goto finish;
			}
			if ( parse_field(fp, tmpspace7, (int *)&dummy, BIGSIZE) == -1 ) {
				goto finish;
			}
			if ( parse_field(fp, tmpspace8, (int *)&dummy, BIGSIZE) == -1 ) {
				goto finish;
			}
			parse_field_or_eol(fp, tmpspace9, &eol, BIGSIZE);

			tmp->au_legalname  = (char *)malloc( strlen(tmpspace2) + 1);
			tmp->au_birthplace = (char *)malloc( strlen(tmpspace3) + 1);
			tmp->au_birthdate  = (char *)malloc( strlen(tmpspace4) + 1);
			tmp->au_deathdate  = (char *)malloc( strlen(tmpspace5) + 1);
			tmp->au_pseudonyms = (char *)malloc( strlen(tmpspace6) + 1);
			tmp->au_email 	   = (char *)malloc( strlen(tmpspace7) + 1);
			tmp->au_webpage    = (char *)malloc( strlen(tmpspace8) + 1);
			tmp->au_biog       = (char *)malloc( strlen(tmpspace9) + 1);

			strcpy(tmp->au_legalname,  tmpspace2);
			strcpy(tmp->au_birthplace, tmpspace3);
			strcpy(tmp->au_birthdate,  tmpspace4);
			strcpy(tmp->au_deathdate,  tmpspace5);
			strcpy(tmp->au_pseudonyms, tmpspace6);
			strcpy(tmp->au_email,      tmpspace7);
			strcpy(tmp->au_webpage,    tmpspace8);
			strcpy(tmp->au_biog,       tmpspace9);

			fclose(fp);
			return(tmp);
		} else {
			if ( parse_to_eol(fp, (int *)&dummy) == -1 ) {
				goto finish;
			}
		}
	}

finish:
	fclose(fp);
	return(NULL);
}


int
load_pubs(int filteroption, char *filter)
{
	FILE	*fp;
	int	dummy;
	int	reuseit;
	int	savepub;
	pub_t	*tmp;

	fp = fopen("pubs.dbase", "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		tmp = (pub_t *)malloc( sizeof(pub_t) );
reuse:
		reuseit = 0;
		savepub = 0;
		if ( parse_field(fp, tmpspace1, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace2, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_title.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace3, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_author.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace4, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_year.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace5, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_isbn.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace6, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_publisher.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace7, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_price.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace8, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_pages.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace9, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_type.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace10, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_cover.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace13, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_coverpicture.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace11, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_bcover.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace14, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_bcoverpicture.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace12, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_notes.\n");
			goto finish;
		}

		switch( filteroption ) {
		default:
		case F_PLIST:	if ( strstr(filter, tmpspace1) ) {
					savepub = 1;
				}
				break;

		case F_EXACT:	if ( strcmp(filter, tmpspace1) == 0) {
					savepub = 1;
				}
				break;

		case F_YEAR:	if ( strcmp(filter, tmpspace4) == 0) {
					savepub = 1;
				}
				break;

		case F_FORTH:	if ( strncmp(filter, tmpspace4, 4) == 0) {
					savepub = 1;
				}
				break;

		case F_NOOPT:	savepub = 1;
				break;
		}

		if ( savepub ) {

			tmp->pu_abbreviation = (char *)malloc( strlen(tmpspace1) + 1);
			tmp->pu_title        = (char *)malloc( strlen(tmpspace2) + 1);
			tmp->pu_author       = (char *)malloc( strlen(tmpspace3) + 1);
			tmp->pu_year         = (char *)malloc( strlen(tmpspace4) + 1);
			tmp->pu_isbn         = (char *)malloc( strlen(tmpspace5) + 1);
			tmp->pu_publisher    = (char *)malloc( strlen(tmpspace6) + 1);
			tmp->pu_price        = (char *)malloc( strlen(tmpspace7) + 1);
			tmp->pu_pages        = (char *)malloc( strlen(tmpspace8) + 1);
			tmp->pu_type         = (char *)malloc( strlen(tmpspace9) + 1);
			tmp->pu_cover        = (char *)malloc( strlen(tmpspace10) + 1);
			tmp->pu_bcover       = (char *)malloc( strlen(tmpspace11) + 1);
			tmp->pu_notes        = (char *)malloc( strlen(tmpspace12) + 1);
			tmp->pu_coverpicture = (char *)malloc( strlen(tmpspace13) + 1);
			tmp->pu_bcoverpicture= (char *)malloc( strlen(tmpspace14) + 1);

			strcpy(tmp->pu_abbreviation, tmpspace1);
			strcpy(tmp->pu_title,        tmpspace2);
			strcpy(tmp->pu_author,       tmpspace3);
			strcpy(tmp->pu_year,         tmpspace4);
			strcpy(tmp->pu_isbn,         tmpspace5);
			strcpy(tmp->pu_publisher,    tmpspace6);
			strcpy(tmp->pu_price,        tmpspace7);
			strcpy(tmp->pu_pages,        tmpspace8);
			strcpy(tmp->pu_type,         tmpspace9);
			strcpy(tmp->pu_cover,        tmpspace10);
			strcpy(tmp->pu_bcover,       tmpspace11);
			strcpy(tmp->pu_notes,        tmpspace12);
			strcpy(tmp->pu_coverpicture, tmpspace13);
			strcpy(tmp->pu_bcoverpicture,tmpspace14);

			tmp->pu_next = NULL;
			if ( pub_list == NULL) {
				pub_list = pub_end = tmp;
			} else {
				pub_end->pu_next = tmp;
				pub_end = tmp;
			}
		} else {
			reuseit = 1;
		}

		if ( parse_to_eol(fp, (int *)&dummy) == -1 ) {
			break;
		}

		if (reuseit)
			goto reuse;
	}

finish:
	fclose(fp);
	return(0);
}


int
load_reviews(int filteroption, char *filter)
{
	FILE	*fp;
	int	dummy;
	int	reuseit;
	int	savereview;
	review_t	*tmp;

	fp = fopen("reviews.dbase", "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		tmp = (review_t *)malloc( sizeof(review_t) );
		tmp->rv_marker = 0;
reuse:
		reuseit = 0;
		savereview = 0;

		if ( parse_field(fp, tmpspace1, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace2, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_reviews: error parsing rv_revtitle.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace3, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_reviews: error parsing rv_author.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace4, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_reviews: error parsing rv_year.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace5, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_reviews: error parsing rv_reviewer.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace6, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_reviews: error parsing rv_pageno.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace7, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_reviews: error parsing rv_abbreviation.\n");
			goto finish;
		}
		if ( parse_field(fp, tmpspace8, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_reviews: error parsing rv_notes.\n");
			goto finish;
		}

		switch( filteroption ) {
		default:
		case F_PLIST:	if ( strstr(filter, tmpspace7) ) {
					savereview = 1;
				}
				break;

		case F_EXACT:	if ( strcmp(filter, tmpspace7) == 0) {
					savereview = 1;
				}
				break;

		case F_YEAR:	if ( strcmp(filter, tmpspace4) == 0) {
					savereview = 1;
				}
				break;

		case F_TAUTHOR:	if ( strstr(tmpspace3, filter) ) {
					savereview = 1;
				}
				break;

		case F_RAUTHOR:	if ( strstr(filter, tmpspace5) ) {
					savereview = 1;
				}
				break;

		case F_NOOPT:	savereview = 1;
				break;
		}

		if ( savereview ) {

			tmp->rv_title        = (char *)malloc( strlen(tmpspace1) + 1);
			tmp->rv_revtitle     = (char *)malloc( strlen(tmpspace2) + 1);
			tmp->rv_author       = (char *)malloc( strlen(tmpspace3) + 1);
			tmp->rv_year         = (char *)malloc( strlen(tmpspace4) + 1);
			tmp->rv_reviewer     = (char *)malloc( strlen(tmpspace5) + 1);
			tmp->rv_pageno       = (char *)malloc( strlen(tmpspace6) + 1);
			tmp->rv_abbreviation = (char *)malloc( strlen(tmpspace7) + 1);
			tmp->rv_notes        = (char *)malloc( strlen(tmpspace8) + 1);

			strcpy(tmp->rv_title,        tmpspace1);
			strcpy(tmp->rv_revtitle,     tmpspace2);
			strcpy(tmp->rv_author,       tmpspace3);
			strcpy(tmp->rv_year,         tmpspace4);
			strcpy(tmp->rv_reviewer,     tmpspace5);
			strcpy(tmp->rv_pageno,       tmpspace6);
			strcpy(tmp->rv_abbreviation, tmpspace7);
			strcpy(tmp->rv_notes,        tmpspace8);

			tmp->rv_next = NULL;
			if ( rv_list == NULL) {
				rv_list = rv_end = tmp;
			} else {
				rv_end->rv_next = tmp;
				rv_end = tmp;
			}
		} else {
			reuseit = 1;
		}

		if ( parse_to_eol(fp, (int *)&dummy) == -1 ) {
			break;
		}

		if (reuseit)
			goto reuse;
	}

finish:
	fclose(fp);
	return(0);
}


int
load_interviews(int filteroption, char *filter)
{
	FILE	*fp;
	int	dummy;
	int	reuseit;
	int	saveinterview;
	inter_t	*tmp;

	fp = fopen("interviews.dbase", "rb");
	if (fp == NULL) {
		perror("Couldn't open interview dbase");
		exit(1);
	}

	while(1) {
		tmp = (inter_t *)malloc( sizeof(inter_t) );
		tmp->it_marker = 0;
reuse:
		reuseit = 0;
		saveinterview = 0;

		if ( parse_field(fp, tmpspace1, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace2, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace3, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace5, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace4, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace7, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace6, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace8, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}

		switch( filteroption ) {
		default:
		case F_PLIST:	if ( strstr(filter, tmpspace6) ) {
					saveinterview = 1;
				}
				break;

		case F_EXACT:	if ( strcmp(filter, tmpspace6) == 0) {
					saveinterview = 1;
				}
				break;

		case F_YEAR:	if ( strcmp(filter, tmpspace5) == 0) {
					saveinterview = 1;
				}
				break;

		case F_TAUTHOR:	if ( strstr(tmpspace3, filter) ) {
					saveinterview = 1;
				}
				break;

		case F_RAUTHOR:	if ( strstr(filter, tmpspace4) ) {
					saveinterview = 1;
				}
				break;

		case F_NOOPT:	saveinterview = 1;
				break;
		}

		if ( saveinterview ) {

			tmp->it_title        = (char *)malloc( strlen(tmpspace1) + 1);
			tmp->it_inttitle     = (char *)malloc( strlen(tmpspace2) + 1);
			tmp->it_author       = (char *)malloc( strlen(tmpspace3) + 1);
			tmp->it_interviewer  = (char *)malloc( strlen(tmpspace4) + 1);
			tmp->it_year         = (char *)malloc( strlen(tmpspace5) + 1);
			tmp->it_abbreviation = (char *)malloc( strlen(tmpspace6) + 1);
			tmp->it_pageno       = (char *)malloc( strlen(tmpspace7) + 1);
			tmp->it_notes        = (char *)malloc( strlen(tmpspace8) + 1);

			strcpy(tmp->it_title,        tmpspace1);
			strcpy(tmp->it_inttitle,     tmpspace2);
			strcpy(tmp->it_author,       tmpspace3);
			strcpy(tmp->it_interviewer,  tmpspace4);
			strcpy(tmp->it_year,         tmpspace5);
			strcpy(tmp->it_abbreviation, tmpspace6);
			strcpy(tmp->it_pageno,       tmpspace7);
			strcpy(tmp->it_notes,        tmpspace8);

			tmp->it_next = NULL;
			if ( it_list == NULL) {
				it_list = it_end = tmp;
			} else {
				it_end->it_next = tmp;
				it_end = tmp;
			}
		} else {
			reuseit = 1;
		}

		if ( parse_to_eol(fp, (int *)&dummy) == -1 ) {
			break;
		}

		if (reuseit)
			goto reuse;
	}

finish:
	fclose(fp);
	return(0);
}


void
load_authors()
{
	FILE	*fp;
	int	line_number = 1;
	int	index;
	int	dummy;
	int	eol;
	char	input;
	audata_t	*tmp;

	fp = fopen("authors.dbase", "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		tmp = (audata_t *)malloc( sizeof(audata_t) );

		if ( parse_field(fp, tmpspace1, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace2, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace3, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace4, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace5, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace6, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace7, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_field(fp, tmpspace8, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}

		tmp->au_name       = (char *)malloc( strlen(tmpspace1) + 1);
		tmp->au_legalname  = (char *)malloc( strlen(tmpspace2) + 1);
		tmp->au_birthplace = (char *)malloc( strlen(tmpspace3) + 1);
		tmp->au_birthdate  = (char *)malloc( strlen(tmpspace4) + 1);
		tmp->au_deathdate  = (char *)malloc( strlen(tmpspace5) + 1);
		tmp->au_pseudonyms = (char *)malloc( strlen(tmpspace6) + 1);
		tmp->au_email      = (char *)malloc( strlen(tmpspace7) + 1);
		tmp->au_webpage    = (char *)malloc( strlen(tmpspace8) + 1);
		tmp->au_biog       = (char *)malloc( strlen(tmpspace9) + 1);

		strcpy(tmp->au_name,       tmpspace1);
		strcpy(tmp->au_legalname,  tmpspace2);
		strcpy(tmp->au_birthplace, tmpspace3);
		strcpy(tmp->au_birthdate,  tmpspace4);
		strcpy(tmp->au_deathdate,  tmpspace5);
		strcpy(tmp->au_pseudonyms, tmpspace6);
		strcpy(tmp->au_email,      tmpspace7);
		strcpy(tmp->au_webpage,    tmpspace8);
		strcpy(tmp->au_biog,       tmpspace9);

		tmp->au_next = NULL;
		if (at_list == NULL) {
			at_list = at_end = tmp;
		} else {
			at_end->au_next = tmp;
			at_end = tmp;
		}
		if ( parse_to_eol(fp, (int *)&dummy) == -1 ) {
			break;
		}
	}

finish:
	fclose(fp);
}


int
load_forthcoming(char *file)
{
	FILE	*fp;
	int	dummy;
	pub_t	*tmp;

	fp = fopen(file, "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		tmp = (pub_t *)malloc( sizeof(pub_t) );

		/* Eat database index number */
		if ( parse_field(fp, tmpspace1, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}

		/* TAG */
		if ( parse_field(fp, tmpspace1, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_abbreviation.\n");
			goto finish;
		}

		/* TITLE */
		if ( parse_field(fp, tmpspace2, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_title.\n");
			goto finish;
		}

		/* AUTHOR */
		if ( parse_field(fp, tmpspace3, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_author.\n");
			goto finish;
		}

		/* DATE */
		if ( parse_field(fp, tmpspace4, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_year.\n");
			goto finish;
		}

		/* PUBLISHER */
		if ( parse_field(fp, tmpspace6, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_publisher.\n");
			goto finish;
		}

		/* PAGE COUNT */
		if ( parse_field(fp, tmpspace8, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_pages.\n");
			goto finish;
		}

		/* PUBLICATION TYPE */
		if ( parse_field(fp, tmpspace9, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_type.\n");
			goto finish;
		}

		/* ISBN */
		if ( parse_field(fp, tmpspace5, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_isbn.\n");
			goto finish;
		}
		
		/* COVER ARTIST */
		if ( parse_field(fp, tmpspace10, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_cover.\n");
			goto finish;
		}

		/* TEXT TYPE */
		if ( parse_field(fp, tmpspace11, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_bcover.\n");
			goto finish;
		}

		/* PRICE */
		if ( parse_field(fp, tmpspace7, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_price.\n");
			goto finish;
		}

		/* NOTES */
		if ( parse_field(fp, tmpspace12, (int *)&dummy, BIGSIZE) == -1 ) {
			print_fatal("load_pubs: error parsing pu_notes.\n");
			goto finish;
		}

		tmp->pu_abbreviation = (char *)malloc( strlen(tmpspace1) + 1);
		tmp->pu_title        = (char *)malloc( strlen(tmpspace2) + 1);
		tmp->pu_author       = (char *)malloc( strlen(tmpspace3) + 1);
		tmp->pu_year         = (char *)malloc( strlen(tmpspace4) + 1);
		tmp->pu_isbn         = (char *)malloc( strlen(tmpspace5) + 1);
		tmp->pu_publisher    = (char *)malloc( strlen(tmpspace6) + 1);
		tmp->pu_price        = (char *)malloc( strlen(tmpspace7) + 1);
		tmp->pu_pages        = (char *)malloc( strlen(tmpspace8) + 1);
		tmp->pu_type         = (char *)malloc( strlen(tmpspace9) + 1);
		tmp->pu_cover        = (char *)malloc( strlen(tmpspace10) + 1);
		tmp->pu_bcover       = (char *)malloc( strlen(tmpspace11) + 1);
		tmp->pu_notes        = (char *)malloc( strlen(tmpspace12) + 1);

		strcpy(tmp->pu_abbreviation, tmpspace1);
		strcpy(tmp->pu_title,        tmpspace2);
		strcpy(tmp->pu_author,       tmpspace3);
		strcpy(tmp->pu_year,         tmpspace4);
		strcpy(tmp->pu_isbn,         tmpspace5);
		strcpy(tmp->pu_publisher,    tmpspace6);
		strcpy(tmp->pu_price,        tmpspace7);
		strcpy(tmp->pu_pages,        tmpspace8);
		strcpy(tmp->pu_type,         tmpspace9);
		strcpy(tmp->pu_cover,        tmpspace10);
		strcpy(tmp->pu_bcover,       tmpspace11);
		strcpy(tmp->pu_notes,        tmpspace12);

		tmp->pu_next = NULL;
		if ( pub_list == NULL) {
			pub_list = pub_end = tmp;
		} else {
			pub_end->pu_next = tmp;
			pub_end = tmp;
		}

		if ( parse_to_eol(fp, (int *)&dummy) == -1 ) {
			break;
		}
	}

finish:
	fclose(fp);
	return(0);
}
