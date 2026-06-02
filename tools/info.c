/*
 *     (C) COPYRIGHT 1995-2001   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)info.c	1.11	06/10/97 SFdbase";

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include "sfdbase.h"

extern void sortpub_by_year2(pub_t **oldlist);
extern void load_authors();

#define CURRENT_YEAR	2001
#define START_YEAR	1925
#define END_YEAR	(START_YEAR+80)
#define SCREEN_WIDTH	((END_YEAR-START_YEAR)+10)
#define SCREEN_HEIGHT	24

static char	tmpspace1[BIGSIZE];
static char	tmpspace2[BIGSIZE];
static char	tmpspace3[BIGSIZE];
static char	tmpspace4[BIGSIZE];
static char	tmpspace5[BIGSIZE];
static char	tmpspace6[BIGSIZE];
static char	tmpspace7[BIGSIZE];
static char	tmpspace8[BIGSIZE];
static char	tmpauthor[MEDIUMSIZE];
search_t        *title_list  = NULL;
search_t        *title_end   = NULL;

int		totnovels = 0;
int		totshort = 0;
int		totshort2 = 0;
int		totanth = 0;
int		totart = 0;
int		totartint = 0;
int		totpoem = 0;
int		totnf = 0;
int		totcoll = 0;
int		totserial = 0;
int		totnong = 0;
int		totessays = 0;
int		totreviews = 0;
int		totomnibus = 0;
int		totintr = 0;
int		toteditor = 0;

int		totanalog = 0;
int		tothugo = 0;
int		totcampbell = 0;
int		totchesley = 0;
int		totclarke = 0;
int		totstoker = 0;
int		totsidewise = 0;
int		totsturgeon = 0;
int		totlocus = 0;
int		totnebula = 0;
int		totwf = 0;
int		totbf = 0;
int		totdt = 0;
int		totax = 0;
int		totar = 0;
int		totaurealis = 0;
int		totgandalf = 0;
int		totbalrog = 0;
int		totbsfa = 0;
int		totpk = 0;
int		totap = 0;
int		totpromo = 0;
int		totsfbc = 0;
int		totsfcron = 0;
int		totau = 0;
int		tottiptree = 0;
int		totmytho = 0;
int		tothomer = 0;
int		totjwca = 0;
int		tothallf = 0;
int		totcompton = 0;
int		totdeathr = 0;
int		totihg = 0;
int		totskylark = 0;
int		totgaughan = 0;
int		totimaginaire = 0;
int		totlambda = 0;
int		totretrohugo = 0;
int		totrhysling = 0;
int		totifa = 0;
int		year_stat[3000];
int		year_stat2[3000];
float		year_stat3[3000];
int		divisor;
char		vscreen[SCREEN_WIDTH][SCREEN_HEIGHT];

typedef struct target {
        char            *au_author;
        char            *au_email;
        char            *au_webpage;
        struct target   *au_left;
        struct target   *au_right;
} target_t;
target_t        *author_list = NULL;

search_t        *author_list2 = NULL;
award_t         *aw_list = NULL;
award_t         *aw_end = NULL;
extern  author_t *au_head;
extern  pub_t	 *pub_list;
extern	int	dbaseloc;
extern  audata_t *at_list;



void
init_vscreen(int minx, int maxx, int xinc, int miny, int maxy)
{
	int x,y;

	/*
	 * Fill the virtual screen with spaces
	 */
	for(x=0; x<SCREEN_WIDTH; x++) {
		for(y=0; y<SCREEN_HEIGHT; y++) {
			vscreen[x][y] = ' ';
		}
	}

	/*
	 * Plot the vertical hash lines
	 */
	for(x=5; x<=SCREEN_WIDTH; x+=5) {
		for(y=0; y<(SCREEN_HEIGHT-3); y++) {
			vscreen[x][y] = '|';
		}
	}

	/*
	 * Label the left column
	 */
	divisor = 1 + (maxy - miny)/(SCREEN_HEIGHT-4);
	for(y=miny; y<=maxy; y+=divisor) {
		char tmpbuf[6];
		int loop;

		sprintf(tmpbuf, "%d", y);
		for(loop=0; loop<strlen(tmpbuf); loop++) {
			vscreen[loop][(SCREEN_HEIGHT-4)-(y/divisor)] = tmpbuf[loop];
		}
	}

	for(x=minx; x<maxx; x+=xinc) {
		char tmpbuf[6];
		int loop;

		sprintf(tmpbuf, "%d", x);
		for(loop=0; loop<strlen(tmpbuf); loop++) {
			if (x > 999) {
				vscreen[x-minx+3+loop][(SCREEN_HEIGHT-2)] = tmpbuf[loop];
			} else if (x > 99) {
				vscreen[x-minx+4+loop][(SCREEN_HEIGHT-2)] = tmpbuf[loop];
			} else if (x > 9) {
				vscreen[x-minx+5+loop][(SCREEN_HEIGHT-2)] = tmpbuf[loop];
			} else {
				vscreen[x-minx+6+loop][(SCREEN_HEIGHT-2)] = tmpbuf[loop];
			}
		}
	}
}

void
plot_column(int x, int y)
{
	while(y) {
		vscreen[x][(SCREEN_HEIGHT-4)-(y/divisor)] = 'X';
		y--;
	}
}


void
print_vscreen()
{
	int x,y;

	for(y=0; y<SCREEN_HEIGHT; y++) {
		for(x=0; x<SCREEN_WIDTH; x++) {
			printf("%c", vscreen[x][y]);
		}
		printf("\n");
	}
}

void
add_author_entry(char *author, char *email, char *webpage)
{
	target_t        *tmp;
	char		*ptr;
	int		value;
	target_t        *last = NULL;


	/*
	 * Remove any pseudonyms
	 */
	ptr = (char *)strstr(author, "^");
	if (ptr) {
		*ptr = 0;
	}

	tmp = author_list;
	while(tmp) {
		last = tmp;
		value = strcmp(tmp->au_author, author);
		if ( value == 0 ) {
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
	if (email) {
		tmp->au_email = (char *)malloc( strlen(email) + 1);
		strcpy(tmp->au_email, author);
	}
	if (webpage) {
		tmp->au_webpage = (char *)malloc( strlen(webpage) + 1);
		strcpy(tmp->au_webpage, author);
	}
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
parse_authors(char *filename)
{
	FILE	*fp;
	int	line_number = 1;
	int	index;
	char	input;
	char	*author;
	char	*ptr;
	int	dummy = 0;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		if ( parse_field(fp, tmpauthor, (int *)&dummy, MEDIUMSIZE) == -1 ) {
			goto finish;
		}
		author = tmpauthor;
		if ( strstr(author, "^") ) {
			auset_t *tmp2;

			tmp2 = (auset_t *)decompose(author);
			separate(tmp2);
			while( tmp2 ) {
				author = tmp2->au_actual;
				while ( strstr(author, "+") ) {
					ptr = (char *)strstr(author, "+");
					*ptr = 0;
					add_author_entry(author, NULL, NULL);
					author = ++ptr;
				}
				add_author_entry(author, NULL, NULL);
				tmp2 = tmp2->au_next;
			}
		} else {
			while ( strstr(author, "+") ) {
				ptr = (char *)strstr(author, "+");
				*ptr = 0;
				add_author_entry(author, NULL, NULL);
				author = ++ptr;
			}
			add_author_entry(author, NULL, NULL);
		}
		if ( parse_to_eol(fp, (int *)&dummy) == -1 ) {
			goto finish;
		}
	}
finish:
	fclose(fp);
}


void
search_file(char *filename)
{
	FILE	*fp;
	int	line_number = 1;
	int	dummy;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		search_t *set;

		set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
		if (set == NULL) {
			goto finish;
		}
		line_number++;

		if (strcmp(set->se_type, "n") == 0) {
			totnovels++;
		} else if (strcmp(set->se_type, "sf") == 0) {
			char *ptr;

			totshort++;
			ptr = strstr(set->se_pubs, ",");
			while(ptr) {
				totshort2++;
				ptr++;
				ptr = strstr(ptr, ",");
			}
			totshort2++;
		} else if (strcmp(set->se_type, "a") == 0) {
			totanth++;
		} else if (strcmp(set->se_type, "ar") == 0) {
			totart++;
		} else if (strcmp(set->se_type, "ai") == 0) {
			totartint++;
		} else if (strcmp(set->se_type, "p") == 0) {
			totpoem++;
		} else if (strcmp(set->se_type, "nf") == 0) {
			totnf++;
		} else if (strcmp(set->se_type, "c") == 0) {
			totcoll++;
		} else if (strcmp(set->se_type, "se") == 0) {
			totserial++;
		} else if (strcmp(set->se_type, "ng") == 0) {
			totnong++;
		} else if (strcmp(set->se_type, "es") == 0) {
			totessays++;
		} else if (strcmp(set->se_type, "r") == 0) {
			totreviews++;
		} else if (strcmp(set->se_type, "i") == 0) {
			totintr++;
		} else if (strcmp(set->se_type, "o") == 0) {
			totomnibus++;
		} else if (strcmp(set->se_type, "e") == 0) {
			toteditor++;
		} else {
			printf("%s\n", set->se_type);
		}
	}

finish:

	fclose(fp);
}


void
search_file2(char *filename, char *type)
{
	FILE	*fp;
	int	line_number = 1;
	int	dummy;
	int	year;
	int	loop;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	for(loop=0; loop<3000; loop++) {
		year_stat[loop] = 0;
	}

	while(1) {
		search_t *set;

		set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
		if (set == NULL) {
			goto finish;
		}

		if (set->se_year) {
			sscanf(set->se_year, "%d", &year);
			if ( strcmp(type, set->se_type) == 0) {
				if (year>0 && year<END_YEAR) {
					year_stat[year]++;
				}
			}
		}
	}

finish:
	fclose(fp);
}


void
search_file3(char *filename, char *type, int treprints)
{
	FILE	*fp;
	int	line_number = 1;
	int	dummy;
	int	year;
	int	loop;
	int	reprints;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	for(loop=0; loop<3000; loop++) {
		year_stat[loop] = 0;
	}

	while(1) {
		search_t *set;

		set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
		if (set == NULL) {
			goto finish;
		}

		reprints = 0;
		if (set->se_pubs && strstr(set->se_pubs, ",")) {
			char *ptr;

			ptr = set->se_pubs;
			while(1) {
				ptr = (char *)strstr(ptr, ",");
				if (ptr) {
					reprints++;
					ptr++;
				} else {
					break;
				}
			}
		}

		if (set->se_year) {
			sscanf(set->se_year, "%d", &year);
			if ( strcmp(type, set->se_type) == 0) {
				if (year>1899 && year<END_YEAR) {
					year_stat2[year]++;
					if (reprints >= treprints) {
						year_stat[year]++;
					}
				}
			}
		}
	}

finish:
	fclose(fp);
}


void
add_author(char *title, char *author, char *year, char *pseudo )
{
	search_t	*tmp;

	/*
	 * First check to see if this author already
	 * has an entry
	 */
	tmp = author_list2;
	while(tmp) {

		/*
		 * The old entry may have been as a pseudonynm.
		 * If the new entry doesn't have an attached
		 * pseudonym, zero out the pseudonym field in the
		 * old entry.
		 */
		if ( strcmp(author, tmp->se_author) == 0 ) {
			if (tmp->se_pseudonym && (pseudo[0] == 0)) {
				tmp->se_pseudonym[0] = 0;
			}
			return;
		}
		tmp = tmp->se_next;
	}

	/*
	 * No author entry found. malloc a new entry and copy in
	 * the data. Attach the the entry to the author list.
	 */
	tmp = (search_t *)malloc( sizeof(search_t) );
	if ( tmp == NULL ) {
		perror("out of memory");
		exit(1);
	}

	tmp->se_title     = (char *)malloc( strlen(title) + 1);
	tmp->se_author    = (char *)malloc( strlen(author) + 1);
	tmp->se_pseudonym = (char *)malloc( strlen(pseudo) + 1);
	tmp->se_year      = (char *)malloc( strlen(year) + 1);
	tmp->se_series      = NULL;
	tmp->se_superseries = NULL;
	tmp->se_seriesnum   = NULL;
	tmp->se_type        = NULL;
	tmp->se_offset      = NULL;
	tmp->se_storylen    = NULL;
	tmp->se_notes       = NULL;
	tmp->se_synopsis    = NULL;

	strcpy(tmp->se_title, title);
	strcpy(tmp->se_author, author);
	strcpy(tmp->se_pseudonym, pseudo);
	strcpy(tmp->se_year, year);

	tmp->se_next = author_list2;
	author_list2 = tmp;
}


void
parse_awards(int save)
{
	FILE	*fp;
	unsigned int next_offset;

	fp = fopen("awards.dbase", "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		char offset[16];
		int eol;
		int int_offset;
		award_t *awt;

		if (save) {
			awt = parse_award_entry(fp, (int *)&next_offset, 0);
		} else {
			awt = parse_award_entry(fp, (int *)&next_offset, 1);
		}
		if (awt == NULL) {
			goto finish;
		}

		if (save) {
			if ( (strcmp(awt->aw_types, "n")   == 0) ||
			     (strcmp(awt->aw_types, "lf")  == 0) ||
			     (strcmp(awt->aw_types, "nsf")  == 0) ||
			     (strcmp(awt->aw_types, "nft")  == 0) ||
			     (strcmp(awt->aw_types, "nho")  == 0) ||
			     (strcmp(awt->aw_types, "nv") == 0) ||
			     (strcmp(awt->aw_types, "nt") == 0) ||
			     (strcmp(awt->aw_types, "ss") == 0) ||
			     (strcmp(awt->aw_types, "sf") == 0)) {
				awt->aw_marker = 0;
				awt->aw_next = NULL;
				if (aw_list == NULL ) {
					aw_list = aw_end = awt;
				} else {
					aw_end->aw_next = awt;
					aw_end = awt;
				}
			}
		} else {
			if( strcmp(awt->aw_award, "Hu") ==0) {
				tothugo++;
			} else if( strcmp(awt->aw_award, "Ca") ==0) {
				totcampbell++;
			} else if( strcmp(awt->aw_award, "Cl") ==0) {
				totclarke++;
			} else if( strcmp(awt->aw_award, "Cy") ==0) {
				totchesley++;
			} else if( strcmp(awt->aw_award, "An") ==0) {
				totanalog++;
			} else if( strcmp(awt->aw_award, "St") ==0) {
				totstoker++;
			} else if( strcmp(awt->aw_award, "Sw") ==0) {
				totsidewise++;
			} else if( strcmp(awt->aw_award, "Su") ==0) {
				totsturgeon++;
			} else if( strcmp(awt->aw_award, "Lc") ==0) {
				totlocus++;
			} else if( strcmp(awt->aw_award, "Ne") ==0) {
				totnebula++;
			} else if( strcmp(awt->aw_award, "Wf") ==0) {
				totwf++;
			} else if( strcmp(awt->aw_award, "Bf") ==0) {
				totbf++;
			} else if( strcmp(awt->aw_award, "As") ==0) {
				totaurealis++;
			} else if( strcmp(awt->aw_award, "Dt") ==0) {
				totdt++;
			} else if( strcmp(awt->aw_award, "Ax") ==0) {
				totax++;
			} else if( strcmp(awt->aw_award, "Ar") ==0) {
				totar++;
			} else if( strcmp(awt->aw_award, "Ga") ==0) {
				totgandalf++;
			} else if( strcmp(awt->aw_award, "Bl") ==0) {
				totbalrog++;
			} else if( strcmp(awt->aw_award, "Bs") ==0) {
				totbsfa++;
			} else if( strcmp(awt->aw_award, "Pk") ==0) {
				totpk++;
			} else if( strcmp(awt->aw_award, "Ap") ==0) {
				totap++;
			} else if( strcmp(awt->aw_award, "Au") ==0) {
				totau++;
			} else if( strcmp(awt->aw_award, "Tp") ==0) {
				tottiptree++;
			} else if( strcmp(awt->aw_award, "My") ==0) {
				totmytho++;
			} else if( strcmp(awt->aw_award, "Lm") ==0) {
				totlambda++;
			} else if( strcmp(awt->aw_award, "Hm") ==0) {
				tothomer++;
			} else if( strcmp(awt->aw_award, "Jc") ==0) {
				totjwca++;
			} else if( strcmp(awt->aw_award, "Hf") ==0) {
				tothallf++;
			} else if( strcmp(awt->aw_award, "Pr") ==0) {
				totpromo++;
			} else if( strcmp(awt->aw_award, "Sf") ==0) {
				totsfbc++;
			} else if( strcmp(awt->aw_award, "Sc") ==0) {
				totsfcron++;
			} else if( strcmp(awt->aw_award, "Sw") ==0) {
				totsidewise++;
			} else if( strcmp(awt->aw_award, "Rh") ==0) {
				totretrohugo++;
			} else if( strcmp(awt->aw_award, "If") ==0) {
				totifa++;
			} else if( strcmp(awt->aw_award, "Cc") ==0) {
				totcompton++;
			} else if( strcmp(awt->aw_award, "Sk") ==0) {
				totskylark++;
			} else if( strcmp(awt->aw_award, "Gg") ==0) {
				totgaughan++;
			} else if( strcmp(awt->aw_award, "Im") ==0) {
				totimaginaire++;
			} else if( strcmp(awt->aw_award, "Ih") ==0) {
				totihg++;
			} else if( strcmp(awt->aw_award, "Dr") ==0) {
				totdeathr++;
			} else if( strcmp(awt->aw_award, "Ry") ==0) {
				totrhysling++;
			} else {
				printf("%s\n", awt->aw_award);
			}
		}
	}

finish:
	fclose(fp);
}


void
parse_awards2(int way, int byscore)
{
	FILE		*fp;
	unsigned int 	next_offset;
	search_t	*tmp;
	int		loop;
	char		*ptr;
	int		tmpscore;

	fp = fopen("awards.dbase", "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		char offset[16];
		int eol;
		int int_offset;
		int doit;
		award_t *awt;

		awt = parse_award_entry(fp, (int *)&next_offset, 1);
		if (awt == NULL) {
			goto finish;
		}

		if ( (strcmp(awt->aw_types, "n")   == 0) ||
		     (strcmp(awt->aw_types, "lf")  == 0) ||
		     (strcmp(awt->aw_types, "nsf") == 0) ||
		     (strcmp(awt->aw_types, "nft") == 0) ||
		     (strcmp(awt->aw_types, "nho")  == 0)) {
			if (way < 9)
				doit = 1;
			else
				doit = 0;
		} else if
		   ( (strcmp(awt->aw_types, "nv") == 0) ||
		     (strcmp(awt->aw_types, "nt") == 0) ||
		     (strcmp(awt->aw_types, "ss") == 0) ||
		     (strcmp(awt->aw_types, "sf") == 0)) {
			if (way > 8)
				doit = 1;
			else
				doit = 0;
		} else {
			doit = 0;
		}


		if (doit) {
			int score;
			int popular;
			int year;
			int ratio;
			int age;

			/*
			 * First see if this title is already in the
			 * list.
			 */
			tmp = author_list2;
			while(tmp) {
				if ( strcmp(awt->aw_author, tmp->se_author) == 0 ) {
					if ( strcmp(awt->aw_title, tmp->se_title) == 0 ) {
						break;
					}
				}
				tmp = tmp->se_next;
			}

			if (tmp == NULL) {
				tmp = (search_t *)malloc( sizeof(search_t) );
				if (tmp) {
					tmp->se_title  = (char *)malloc( strlen(awt->aw_title) + 1);
					tmp->se_author = (char *)malloc( strlen(awt->aw_author) + 1);
					tmp->se_type   = (char *)malloc( strlen(awt->aw_year) + 1);
					strcpy(tmp->se_author, awt->aw_author);
					strcpy(tmp->se_title,  awt->aw_title);
					strcpy(tmp->se_type,   awt->aw_year);
					tmp->se_series      = NULL;
					tmp->se_superseries = NULL;
					tmp->se_seriesnum   = NULL;
					tmp->se_offset      = NULL;
					tmp->se_storylen    = NULL;
					tmp->se_notes       = NULL;
					tmp->se_synopsis    = NULL;
				}
				tmp->se_numyear = 0;
				tmp->se_next = author_list2;
				author_list2 = tmp;
			}

			sscanf(awt->aw_level, "%d", &score);
			sscanf(awt->aw_year, "%d", &year);
			if (score == 1) {
				tmpscore = 50;
			} else if (score == 2) {
				tmpscore = 35;
			} else if (strcmp(awt->aw_award, "Lc") && (score == 9)) {
				tmpscore = 35;
			} else if (score < 33) {
				tmpscore = (33 - score);
			} else {
				tmpscore = 1;
			}

			/*
			 * Prometheus awards tend to nominate a work year
			 * after year until it wins. Toss out those nominations.
			 */
			if ( (strcmp(awt->aw_award, "Pr") == 0) && (score != 1)) {
				tmpscore = 0;
			}

			if ( (strcmp(awt->aw_award, "Lc") == 0) ||
			     (strcmp(awt->aw_award, "Au") == 0) ||
			     (strcmp(awt->aw_award, "Hm") == 0) ||
			     (strcmp(awt->aw_award, "Hu") == 0) ||
			     (strcmp(awt->aw_award, "Rh") == 0) ||
			     (strcmp(awt->aw_award, "Sf") == 0) ||
			     (strcmp(awt->aw_award, "Bf") == 0) || 
			     (strcmp(awt->aw_award, "Dt") == 0) || 
			     (strcmp(awt->aw_award, "Ar") == 0) || 
			     (strcmp(awt->aw_award, "Ga") == 0) || 
			     (strcmp(awt->aw_award, "Bs") == 0) || 
			     (strcmp(awt->aw_award, "Wf") == 0)) {
				popular = 1;
			} else {
				popular = 0;
			}

			age   = CURRENT_YEAR - year;
			if (((way == 7) || (way == 10)) && !popular) {
				tmpscore *= 5; 
				tmpscore += age;
			} else if (((way == 8) || (way == 11)) && popular) {
				tmpscore *= 5;
				if (age > 10) {
					tmpscore -= age;
					if (tmpscore < 1)
						tmpscore = 1;
				}
			}
			tmp->se_numyear += tmpscore;
		}
	}

finish:
	if (way == 6) {
		int dummy;

		fclose(fp);
		fp = fopen("titles.dbase", "rb");
		if (fp == NULL) {
			perror("Couldn't open titles.dbase");
			exit(1);
		}

		/*
		 * Blast away all of the year data. Groping through the title
		 * database should put in the corrected year; if not, they'll
		 * fall off the edge at 0.
		 */
		tmp = author_list2;
		while(tmp) {
			strcpy(tmp->se_type, "0");
			tmp = tmp->se_next;
		}
		while(1) {
			search_t *set;

			set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
			if (set == NULL) {
				goto finish2;
			}

			if (strcmp(set->se_type, "n") == 0) {
				tmp = author_list2;
				while(tmp) {
					if ( strcmp(set->se_author, tmp->se_author) == 0 ) {
						if ( strcmp(set->se_title, tmp->se_title) == 0 ) {
							strcpy(tmp->se_type, set->se_year);
							break;
						}
					}
					tmp = tmp->se_next;
				}
			}
		}
	} else if (way == 9) {
		int dummy;

		fclose(fp);
		fp = fopen("titles.dbase", "rb");
		if (fp == NULL) {
			perror("Couldn't open titles.dbase");
			exit(1);
		}

		/*
		 * Blast away all of the year data. Groping through the title
		 * database should put in the corrected year; if not, they'll
		 * fall off the edge at 0.
		 */
		tmp = author_list2;
		while(tmp) {
			strcpy(tmp->se_type, "0");
			tmp = tmp->se_next;
		}
		while(1) {
			search_t *set;

			set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
			if (set == NULL) {
				goto finish2;
			}

			if (strcmp(set->se_type, "sf") == 0) {
				tmp = author_list2;
				while(tmp) {
					if ( strcmp(set->se_author, tmp->se_author) == 0 ) {
						if ( strcmp(set->se_title, tmp->se_title) == 0 ) {
							strcpy(tmp->se_type, set->se_year);
							break;
						}
					}
					tmp = tmp->se_next;
				}
			}
		}
	}


finish2:
	fclose(fp);
	sort_by_year( &author_list2 );
	if (byscore) {
		for(loop=1; loop<=100; loop++) {

			tmp = author_list2;
			while(tmp) {
				if (tmp->se_next->se_next == NULL)
					break;
				tmp = tmp->se_next;
			}

			printf("%3d %4d ", loop, tmp->se_next->se_numyear);
			ptr = (char *)strstr(tmp->se_next->se_title, "^");
			if (ptr) {
				*ptr = 0;
			}
			print_title(9, tmp->se_next->se_title, RIGHTMARGIN, T_PREINDENT, NULL);
			html_print_authors(tmp->se_next->se_author, RIGHTMARGIN);
			tmp->se_next = NULL;
		}
	} else {
		for(loop=END_YEAR; loop>1940; loop--) {
			search_t *this_year;
			search_t *tmp2;
			char	 stryear[8];

			this_year = tmp2 = NULL;
			tmp = author_list2;
			sprintf(stryear, "%d", loop);
			while(tmp) {
				if ( (strcmp(stryear, tmp->se_type) == 0) &&
				     (tmp->se_numyear > 49)) {
					if (tmp2) {
						tmp2->se_next = tmp->se_next;
					} else {
						author_list2 = tmp->se_next;
					}
					if (this_year) {
						tmp->se_next = this_year;
					} else {
						tmp->se_next = NULL;
					}
					this_year = tmp;
					if (tmp2) {
						tmp = tmp2->se_next;
					} else {
						tmp = author_list2;
					}
				} else {
					tmp2 = tmp;
					tmp = tmp->se_next;
				}
			}

			tmp = this_year;
			if (tmp) {
				printf("\n\n<b>%d</b><hr>\n", loop);
			}
			while(tmp) {
				printf("%4d ", tmp->se_numyear);
				ptr = (char *)strstr(tmp->se_title, "^");
				if (ptr) {
					*ptr = 0;
				}
				print_title(5, tmp->se_title, RIGHTMARGIN, T_PREINDENT, NULL);
				html_print_authors(tmp->se_author, RIGHTMARGIN);
				tmp = tmp->se_next;
			}
		}
	}


}


void
parse_awards3(int way)
{
	FILE		*fp;
	unsigned int 	next_offset;
	search_t	*tmp;
	int		loop;
	char		*ptr;
	int		tmpscore;

	fp = fopen("awards.dbase", "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		char offset[16];
		int eol;
		int int_offset;
		int score;
		int popular;
		int year;
		int ratio;
		int age;
		award_t *awt;

		awt = parse_award_entry(fp, (int *)&next_offset, 1);
		if (awt == NULL) {
			goto finish;
		}

		if ( (strcmp(awt->aw_types, "n")   == 0) ||
		     (strcmp(awt->aw_types, "lf")  == 0) ||
		     (strcmp(awt->aw_types, "nsf") == 0) ||
		     (strcmp(awt->aw_types, "nft") == 0) ||
		     (strcmp(awt->aw_types, "nho") == 0) ||
		     (strcmp(awt->aw_types, "nv")  == 0) ||
		     (strcmp(awt->aw_types, "nt")  == 0) ||
		     (strcmp(awt->aw_types, "ss")  == 0) ||
		     (strcmp(awt->aw_types, "sf")  == 0)) {
			/* nothing */
		} else {
			continue;
		}


		tmp = author_list2;
		while(tmp) {
			if ( strcmp(awt->aw_author, tmp->se_author) == 0 ) {
				break;
			}
			tmp = tmp->se_next;
		}

		if (tmp == NULL) {
			tmp = (search_t *)malloc( sizeof(search_t) );
			if (tmp) {
				tmp->se_author = (char *)malloc( strlen(awt->aw_author) + 1);
				strcpy(tmp->se_author, awt->aw_author);
				tmp->se_series      = NULL;
				tmp->se_superseries = NULL;
				tmp->se_seriesnum   = NULL;
				tmp->se_type        = NULL;
				tmp->se_offset      = NULL;
				tmp->se_storylen    = NULL;
				tmp->se_notes       = NULL;
				tmp->se_synopsis    = NULL;
			}
			tmp->se_numyear = 0;
			tmp->se_next = author_list2;
			author_list2 = tmp;
		}

		sscanf(awt->aw_level, "%d", &score);
		sscanf(awt->aw_year, "%d", &year);
		if (score == 1) {
			tmpscore = 50;
		} else if (score == 2) {
			tmpscore = 35;
		} else if (score < 33) {
			tmpscore = (33 - score);
		} else {
			tmpscore = 1;
		}

		if ( (strcmp(awt->aw_award, "Lc") == 0) ||
		     (strcmp(awt->aw_award, "Au") == 0) ||
		     (strcmp(awt->aw_award, "Hm") == 0) ||
		     (strcmp(awt->aw_award, "Hu") == 0) ||
		     (strcmp(awt->aw_award, "Rh") == 0) ||
		     (strcmp(awt->aw_award, "Sf") == 0) ||
		     (strcmp(awt->aw_award, "Bf") == 0) ||
		     (strcmp(awt->aw_award, "Dt") == 0) ||
		     (strcmp(awt->aw_award, "Ar") == 0) ||
		     (strcmp(awt->aw_award, "Ga") == 0) ||
		     (strcmp(awt->aw_award, "Bs") == 0) ||
		     (strcmp(awt->aw_award, "Wf") == 0)) {
			popular = 1;
		} else {
			popular = 0;
		}

		age   = CURRENT_YEAR - year;
		if ((way == 13) && !popular) {
			tmpscore *= 5; 
			tmpscore += age;
		} else if ((way == 14) && popular) {
			tmpscore *= 5;
			if (age > 10) {
				tmpscore -= age;
				if (tmpscore < 1)
					tmpscore = 1;
			}
		}

		tmp->se_numyear += tmpscore;
	}

finish:
	fclose(fp);

	sort_by_yearold( &author_list2 );

	for(loop=1; loop<=100; loop++) {
		tmp = author_list2;
		while(tmp) {
			if (tmp->se_next->se_next == NULL) {
				printf("%3d %4d ", loop, tmp->se_next->se_numyear);
				ptr = (char *)strstr(tmp->se_next->se_author, "^");
				if (ptr) {
					*ptr = 0;
				}
				html_print1_author(tmp->se_next->se_author, 0, 0, RIGHTMARGIN, 0);
				printf("\n");
				tmp->se_next = NULL;
				break;
			} else {
				tmp = tmp->se_next;
			}
		}
	}


}


void
parse_awards4(int way)
{
	FILE		*fp;
	unsigned int 	next_offset;
	search_t	*tmp;
	pub_t		*ptmp, *ptmp2;
	int		loop;
	char		*ptr;
	int		tmpscore;

	fp = fopen("awards.dbase", "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	/*
	 * First: load up the awards
	 */
	while(1) {
		char offset[16];
		int eol;
		int int_offset;
		int doit;
		award_t *awt;

		awt = parse_award_entry(fp, (int *)&next_offset, 1);
		if (awt == NULL) {
			goto finish;
		}

		if ( (strcmp(awt->aw_types, "nv") == 0) ||
		     (strcmp(awt->aw_types, "nt") == 0) ||
		     (strcmp(awt->aw_types, "ss") == 0) ||
		     (strcmp(awt->aw_types, "sf") == 0)) {
			doit = 1;
		} else {
			doit = 0;
		}


		if (doit) {
			int score;
			int popular;
			int year;
			int ratio;
			int age;

			/*
			 * First see if this title is already in the
			 * list.
			 */
			tmp = author_list2;
			while(tmp) {
				if ( strcmp(awt->aw_author, tmp->se_author) == 0 ) {
					if ( strcmp(awt->aw_title, tmp->se_title) == 0 ) {
						break;
					}
				}
				tmp = tmp->se_next;
			}

			if (tmp == NULL) {
				tmp = (search_t *)malloc( sizeof(search_t) );
				if (tmp) {
					tmp->se_author = (char *)malloc( strlen(awt->aw_author) + 1);
					tmp->se_title  = (char *)malloc( strlen(awt->aw_title) + 1);
					tmp->se_type   = (char *)malloc( strlen(awt->aw_year) + 1);
					strcpy(tmp->se_author, awt->aw_author);
					strcpy(tmp->se_title,  awt->aw_title);
					strcpy(tmp->se_type,   awt->aw_year);
					tmp->se_series      = NULL;
					tmp->se_superseries = NULL;
					tmp->se_seriesnum   = NULL;
					tmp->se_offset      = NULL;
					tmp->se_storylen    = NULL;
					tmp->se_notes       = NULL;
					tmp->se_synopsis    = NULL;
				}
				tmp->se_numyear = 0;
				tmp->se_next = author_list2;
				author_list2 = tmp;
			}

			sscanf(awt->aw_level, "%d", &score);
			sscanf(awt->aw_year, "%d", &year);
			if (score == 1) {
				tmpscore = 50;
			} else if (score == 2) {
				tmpscore = 35;
			} else if (score < 33) {
				tmpscore = (33 - score);
			} else {
				tmpscore = 1;
			}

			if ( (strcmp(awt->aw_award, "Lc") == 0) ||
			     (strcmp(awt->aw_award, "Au") == 0) ||
			     (strcmp(awt->aw_award, "Hm") == 0) ||
			     (strcmp(awt->aw_award, "Hu") == 0) ||
			     (strcmp(awt->aw_award, "Rh") == 0) ||
			     (strcmp(awt->aw_award, "Sf") == 0) ||
			     (strcmp(awt->aw_award, "Bf") == 0) || 
			     (strcmp(awt->aw_award, "Dt") == 0) || 
			     (strcmp(awt->aw_award, "Ar") == 0) || 
			     (strcmp(awt->aw_award, "Ga") == 0) || 
			     (strcmp(awt->aw_award, "Bs") == 0) || 
			     (strcmp(awt->aw_award, "Wf") == 0)) {
				popular = 1;
			} else {
				popular = 0;
			}

			age   = CURRENT_YEAR - year;
			if ((way == 10) && !popular) {
				tmpscore *= 5; 
				tmpscore += age;
			} else if ((way == 11) && popular) {
				tmpscore *= 5;
				if (age > 10) {
					tmpscore -= age;
					if (tmpscore < 1)
						tmpscore = 1;
				}
			}
			tmp->se_numyear += tmpscore;
		}
	}

finish:
	/*
	 * Now go find the pub tags for each award story
	 */
	fclose(fp);
	fp = fopen("titles.dbase", "rb");
	if (fp == NULL) {
		perror("Couldn't open titles.dbase");
		exit(1);
	}

	while(1) {
		int dummy;
		search_t *set;

		set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
		if (set == NULL) {
			goto finish2;
		}

		if (strcmp(set->se_type, "sf") == 0) {
			tmp = author_list2;
			while(tmp) {
				if ( strcmp(set->se_author, tmp->se_author) == 0 ) {
					if ( strcmp(set->se_title, tmp->se_title) == 0 ) {
						if (set->se_pubs) {
							if (tmp->se_pubs) {
								free(tmp->se_pubs);
							}
							tmp->se_pubs = (char *)malloc(strlen(set->se_pubs)+1);
							strcpy(tmp->se_pubs, set->se_pubs);
						}
						break;
					}
				}
				tmp = tmp->se_next;
			}
		}
	}


	/*
	 * Use the pu_numyear field to accumulate the scores
	 */
finish2:
	fclose(fp);
	load_pubs(F_NOOPT, NULL);
	ptmp = pub_list;
	while(ptmp) {
		ptmp->pu_numyear = 0;
		ptmp = ptmp->pu_next;
	}

	/*
	 * For each award story
	 */
	tmp = author_list2;
	while(tmp) {
		char *current_tag;
		char *next_tag;

		if (tmp->se_pubs) {
			current_tag = tmp->se_pubs;
		} else {
			current_tag = NULL;
		}

		/*
		 * For each pub tag
		 */
		while(current_tag) {
			next_tag = (char *)strstr(current_tag, ",");
			if ( next_tag ) {
				*next_tag = 0;
				next_tag++;
			}

			ptmp = pub_list;
			while(ptmp) {
				if ( strcmp(ptmp->pu_abbreviation, current_tag) == 0) {
					ptmp->pu_numyear += tmp->se_numyear;
					break;
				}
				ptmp = ptmp->pu_next;
			}
			current_tag = next_tag;
		}
		tmp = tmp->se_next;
	}

	/*
	 * Remove publication duplicates
	 */
	ptmp = pub_list;
	while(ptmp) {
		ptmp2 = ptmp->pu_next;
		while(ptmp2) {
			if ( (strcmp(ptmp->pu_title, ptmp2->pu_title) == 0) &&
			     (strcmp(ptmp->pu_author, ptmp2->pu_author) == 0)) {
				ptmp2->pu_numyear = 0;
			}
			ptmp2 = ptmp2->pu_next;
		}
		ptmp = ptmp->pu_next;
	}

	sortpub_by_year2( &pub_list );
	ptmp = pub_list;
	for(loop=1; loop<=100; loop++) {
		printf("%3d %4d ", loop, ptmp->pu_numyear );
		printf("<a href=\"http://DBASELOC/plist.cgi?%s\">%s</a> (%s)\n",
			ptmp->pu_abbreviation, ptmp->pu_title, ptmp->pu_year);
		printf("             %s\n", ptmp->pu_author);
		ptmp = ptmp->pu_next;
	}
}


int
count_author_entries(target_t *tmp)
{
	int lcount = 0;
	int rcount = 0;

	if (tmp->au_left) {
		lcount = count_author_entries(tmp->au_left);
	}
	if (tmp->au_right) {
		rcount = count_author_entries(tmp->au_right);
	}
	return( 1 + lcount + rcount );
}


void
do_info1()
{
	int		authors;
	int		grandtotal;

	printf("<html>\n");
	printf("<head>\n");
	printf("<title>ISFDB Statistics</title>\n");
	printf("</head>\n");
	printf("<body bgcolor=\"#ffffff\" text=\"#000000\" >\n");

	printf("<h1>ISFDB Statistics</h1>\n");
	printf("<hr>\n");

	parse_authors("titles.xba");
	search_file("titles.dbase");
	parse_awards(0);

	authors = count_author_entries(author_list);

	grandtotal = totnovels + totshort + totcoll + totanth +
		     totnf + totpoem + totnong + totart + totserial +
		     totessays + totreviews + totintr + totartint + totomnibus;
	     
	printf("<pre>\n");
	printf("<b>%6d</b> Authors\n", authors);
	printf("<b>%6d</b> Titles\n", grandtotal - totart - 
		totserial - totreviews - totintr - totartint);
	printf("</pre>\n");
	printf("<hr>\n");

	printf("<dl>\n");
	printf("<dt><b>%d</b> Work Entries:", grandtotal);
	printf("<ul>\n");
	printf("<li>Novels:        <b>%d</b>\n", totnovels);
	printf("<li>Short Fiction: <b>%d</b> (Total citations)\n", totshort2);
	printf("<li>Short Fiction: <b>%d</b> (Unique titles)\n", totshort);
	printf("<li>Collections:   <b>%d</b>\n", totcoll);
	printf("<li>Anthologies:   <b>%d</b>\n", totanth);
	printf("<li>Non-Fiction:   <b>%d</b>\n", totnf);
	printf("<li>Poems:         <b>%d</b>\n", totpoem);
	printf("<li>Non-Genre:     <b>%d</b>\n", totnong);
	printf("<li>Cover Artwork: <b>%d</b>\n", totart);
	printf("<li>Omnibus:       <b>%d</b>\n", totomnibus);
	printf("<li>Int. Artwork:  <b>%d</b>\n", totartint);
	printf("<li>Serials:       <b>%d</b>\n", totserial);
	printf("<li>Essays:        <b>%d</b>\n", totessays);
	printf("<li>Reviews:       <b>%d</b>\n", totreviews);
	printf("<li>Interviews:    <b>%d</b>\n", totintr);
	printf("</ul>\n"); 
	printf("<hr>\n");

	grandtotal = totcampbell + totclarke + tothugo + 
		     totlocus + totnebula + totpk + totstoker + 
		     totwf + totau + tottiptree + totlambda + tothomer +
		     totmytho + totsturgeon + tothallf + totpromo + 
		     totsfbc + totsfcron + totbf + totdt + totbsfa + totifa
		     + totgandalf + totbalrog + totaurealis + totar + totax
		     + totsidewise + totjwca + totcompton + totskylark + 
		     totgaughan + totihg + totdeathr + totap + totimaginaire + 
		     totchesley + totanalog;
	printf("<dt><b>%d</b> Award Entries:\n", grandtotal); 
	printf("<ul>\n"); 
	printf("<li>Analog:        <b>%d</b>\n", totanalog); 
	printf("<li>Apollo:        <b>%d</b>\n", totap); 
	printf("<li>Asimov's Poll: <b>%d</b>\n", totar); 
	printf("<li>Asimov's Ugrad <b>%d</b>\n", totax); 
	printf("<li>Aurealis:      <b>%d</b>\n", totaurealis); 
	printf("<li>Aurora:        <b>%d</b>\n", totau); 
	printf("<li>Balrog:        <b>%d</b>\n", totbalrog); 
	printf("<li>BFA:           <b>%d</b>\n", totbf); 
	printf("<li>BSFA:          <b>%d</b>\n", totbsfa); 
	printf("<li>Campbell:      <b>%d</b>\n", totcampbell); 
	printf("<li>Chesley:       <b>%d</b>\n", totchesley); 
	printf("<li>Clarke:        <b>%d</b>\n", totclarke); 
	printf("<li>Compton Crook: <b>%d</b>\n", totcompton); 
	printf("<li>Deathrealm:    <b>%d</b>\n", totdeathr); 
	printf("<li>Ditmar:        <b>%d</b>\n", totdt); 
	printf("<li>Gandalf:       <b>%d</b>\n", totgandalf); 
	printf("<li>Gaughan:       <b>%d</b>\n", totgaughan); 
	printf("<li>HOMer:         <b>%d</b>\n", tothomer); 
	printf("<li>Hugos:         <b>%d</b>\n", tothugo); 
	printf("<li>IFA:           <b>%d</b>\n", totifa); 
	printf("<li>IHG:           <b>%d</b>\n", totihg); 
	printf("<li>Imaginaire:    <b>%d</b>\n", totimaginaire); 
	printf("<li>JWCA:          <b>%d</b>\n", totjwca); 
	printf("<li>Lambda:        <b>%d</b>\n", totlambda); 
	printf("<li>Locus Poll:    <b>%d</b>\n", totlocus); 
	printf("<li>Mythopoeic:    <b>%d</b>\n", totmytho); 
	printf("<li>Nebulas:       <b>%d</b>\n", totnebula); 
	printf("<li>P.K. Dick:     <b>%d</b>\n", totpk); 
	printf("<li>Prometheus:    <b>%d</b>\n", totpromo); 
	printf("<li>Retro Hugo:    <b>%d</b>\n", totretrohugo); 
	printf("<li>Rhysling:      <b>%d</b>\n", totrhysling); 
	printf("<li>SFBC:          <b>%d</b>\n", totsfbc); 
	printf("<li>SF Chronicle:  <b>%d</b>\n", totsfcron); 
	printf("<li>Skylark:       <b>%d</b>\n", totskylark); 
	printf("<li>Sidewise:      <b>%d</b>\n", totsidewise); 
	printf("<li>Stoker:        <b>%d</b>\n", totstoker); 
	printf("<li>Sturgeon:      <b>%d</b>\n", totsturgeon); 
	printf("<li>Tiptree:       <b>%d</b>\n", tottiptree); 
	printf("<li>WFA:           <b>%d</b>\n", totwf); 
	printf("</ul>\n"); 

	printf("</dl>\n"); 
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
do_info3()
{
	int loop;
	int max;
	int column;

	/*
	 * Novels
	 */
	max = 0;
	search_file2("titles.dbase", "n");
	for(loop=0; loop<3000; loop++) {
		if (year_stat[loop] > max)
			max = year_stat[loop];
	}
	init_vscreen(START_YEAR, END_YEAR, 5, 0, max);
	for(column=5,loop=START_YEAR; loop<END_YEAR; loop++,column++) {
		plot_column(column, year_stat[loop]);
	}
	printf("<pre>\n");
	printf("                          Database Entries By Year (Novels)\n\n");
	print_vscreen();

	/*
	 * Collections
	 */
	max = 0;
	search_file2("titles.dbase", "c");
	for(loop=0; loop<3000; loop++) {
		if (year_stat[loop] > max)
			max = year_stat[loop];
	}
	init_vscreen(START_YEAR, END_YEAR, 5, 0, max);
	for(column=5,loop=START_YEAR; loop<END_YEAR; loop++,column++) {
		plot_column(column, year_stat[loop]);
	}
	printf("\n\n<hr>\n");
	printf("                          Database Entries By Year (Collections)\n\n");
	print_vscreen();

	/*
	 * Anthologies
	 */
	max = 0;
	search_file2("titles.dbase", "a");
	for(loop=0; loop<3000; loop++) {
		if (year_stat[loop] > max)
			max = year_stat[loop];
	}
	init_vscreen(START_YEAR, END_YEAR, 5, 0, max);
	for(column=5,loop=START_YEAR; loop<END_YEAR; loop++,column++) {
		plot_column(column, year_stat[loop]);
	}
	printf("\n\n<hr>\n");
	printf("                          Database Entries By Year (Anthologies)\n\n");
	print_vscreen();

	/*
	/*
	/*
	 * Short Fiction
	 */
	max = 0;
	search_file2("titles.dbase", "sf");
	for(loop=0; loop<3000; loop++) {
		if (year_stat[loop] > max)
			max = year_stat[loop];
	}
	init_vscreen(START_YEAR, END_YEAR, 5, 0, max);
	for(column=5,loop=START_YEAR; loop<END_YEAR; loop++,column++) {
		plot_column(column, year_stat[loop]);
	}
	printf("\n\n<hr>\n");
	printf("                          Database Entries By Year (Short Fiction)\n\n");
	print_vscreen();

	/*
	 * Essays
	 */
	max = 0;
	search_file2("titles.dbase", "es");
	for(loop=0; loop<3000; loop++) {
		if (year_stat[loop] > max)
			max = year_stat[loop];
	}
	init_vscreen(START_YEAR, END_YEAR, 5, 0, max);
	for(column=5,loop=START_YEAR; loop<END_YEAR; loop++,column++) {
		plot_column(column, year_stat[loop]);
	}
	printf("\n\n<hr>\n");
	printf("                          Database Entries By Year (Essays)\n\n");
	print_vscreen();

	/*
	 * Reviews
	 */
	max = 0;
	search_file2("titles.dbase", "r");
	for(loop=0; loop<3000; loop++) {
		if (year_stat[loop] > max)
			max = year_stat[loop];
	}
	init_vscreen(START_YEAR, END_YEAR, 5, 0, max);
	for(column=5,loop=START_YEAR; loop<END_YEAR; loop++,column++) {
		plot_column(column, year_stat[loop]);
	}
	printf("\n\n<hr>\n");
	printf("                          Database Entries By Year (Reviews)\n\n");
	print_vscreen();
}


void
do_info4(int mode)
{
	FILE	*fp;
	FILE	*fp2;
	int	line_number = 1;
	int	index;
	int	dummy;
	int	doit;
	int	eol;
	char	input;
	char	file1[24];
	char	file2[24];
	char	command[128];
	audata_t	*tmp;

	fp = fopen("authors.dbase", "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	sprintf(file1, "/tmp/isfdb.i%d", getpid() );
	sprintf(file2, "/tmp/isfdb.o%d", getpid() );

	fp2 = fopen(file1, "wb+");
	if (fp2 == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		tmpspace3[0] = 0;
		tmpspace4[0] = 0;
		eol = 0;
		if ( parse_field(fp, tmpauthor, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
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
		parse_field_or_eol(fp, tmpspace5, &eol, BIGSIZE);
		if (!eol) {
			if ( parse_to_eol(fp, (int *)&dummy) == -1 ) {
				goto finish;
			}
		}

		/*
		 * tmpspace3 = Birthdate
		 * tmpspace4 = Deathdate
		 */
		doit = 0;
		switch (mode) {
		case 0:
			/* Birthdate, but no Deathdate */
			if ( tmpspace3[0] && !(tmpspace4[0]) ) {
				doit = 1;
			}
			break;
		case 1:
			if (tmpspace4[0] == 'u') {
				continue;
			}
			/* Birthdate, and Deathdate */
			if ( tmpspace3[0] && tmpspace4[0] ) {
				doit = 1;
			}
			break;
		case 2:
			if (tmpspace4[0] == 'u') {
				continue;
			}
			/* Birthdate, and Deathdate */
			if ( tmpspace3[0] && tmpspace4[0] ) {
				doit = 1;
			}
			break;
		case 3:
			/* Birthdate, but no Deathdate */
			if ( tmpspace3[0] && !(tmpspace4[0]) ) {
				doit = 1;
			}
			break;
		}

		if (doit) {
			char    underscored_author[256];
			char	*ptr1;
			int	byear;
			int	dyear;
			int	age;

			if (tmpspace3[0] != '1') {
				continue;
			}

			sscanf(tmpspace3, "%d", &byear);
			switch(mode) {
			case 0:
				if ((CURRENT_YEAR - byear) < 65) {
					continue;
				}
				break;
			case 1:
				sscanf(tmpspace4, "%d", &dyear);
				age = dyear - byear;
				if (age < 80) {
					continue;
				}
				break;
			case 2:
				sscanf(tmpspace4, "%d", &dyear);
				age = dyear - byear;
				if (age > 49) {
					continue;
				}
				break;
			case 3:
				if ((CURRENT_YEAR - byear) > 35) {
					continue;
				}
				break;
			}

			strcpy(underscored_author, tmpauthor);
			ptr1 = underscored_author;
			while (*ptr1) {
				if (*ptr1 == ' ')
					*ptr1 = '_';
				ptr1++;
			}
			switch(mode) {
			case 0:
				fprintf(fp2, "%s - <a href=\"http://DBASELOC/exact_author.cgi?%s\">%s</a>\n",
					tmpspace3, escape_string(underscored_author), tmpauthor);
				break;
			case 1:
				fprintf(fp2, "%3d - <a href=\"http://DBASELOC/exact_author.cgi?%s\">%s</a>\n",
					age, escape_string(underscored_author), tmpauthor);
				break;
			case 2:
				fprintf(fp2, "%2d - <a href=\"http://DBASELOC/exact_author.cgi?%s\">%s</a>\n",
					age, escape_string(underscored_author), tmpauthor);
				break;
			case 3:
				fprintf(fp2, "%s - <a href=\"http://DBASELOC/exact_author.cgi?%s\">%s</a>\n",
					tmpspace3, escape_string(underscored_author), tmpauthor);
				break;
			}
		}
	}

finish:
	fclose(fp);
	fclose(fp2);

	switch(mode) {
	case 0:
		sprintf(command, "/bin/sort %s > %s", file1, file2);
		break;
	case 1:
		sprintf(command, "/bin/sort -r %s > %s", file1, file2);
		break;
	case 2:
		sprintf(command, "/bin/sort %s > %s", file1, file2);
		break;
	case 3:
		sprintf(command, "/bin/sort -r %s > %s", file1, file2);
		break;
	}
	system(command);
	sprintf(command, "/bin/cat %s", file2);
	system(command);

	unlink(file1);
	unlink(file2);
}


void
push_authors(target_t *tmp)
{
	author_t	*au;
	char		*ctmp;

	if (tmp->au_left) {
		push_authors(tmp->au_left);
	}

	au = (author_t *)malloc(sizeof(author_t));
	au->au_end  = NULL;

	au->au_author = (char *)malloc( strlen(tmp->au_author) + 1);
	strcpy(au->au_author, tmp->au_author);

	ctmp = (char *)lastname(tmp->au_author);
	au->au_lastname = (char *)malloc( strlen(ctmp) + 1);
	strcpy(au->au_lastname, ctmp);
	insert_author(au);

	if (tmp->au_right) {
		push_authors(tmp->au_right);
	}
}

void
do_info5()
{
	search_t	*tmp;
	author_t	*au;
	author_t	*aulast;
	int		position;
	int		loop1;
	int		loop2;
	int		newmargin;
	char		underscored_author[256];
	char		*ptr1;
	char		*bqt;
	char		lastheader = 'z';
	char		oldfilename[16];
	FILE		*dirfp = NULL;

	parse_authors("titles.xba");
	push_authors(author_list);
	au = au_head;

	position  = 0;
	newmargin = 0;

	while(au) {

		strcpy(underscored_author, au->au_author);
		ptr1 = underscored_author;
		while (*ptr1) {
			if (*ptr1 == ' ')
				*ptr1 = '_';
			ptr1++;
		}

		bqt = (char *)bquote(underscored_author);
		if ( strlen(bqt) == 0) {
			au = au->au_next;
			continue;
		}

		if (lastheader != tolower(au->au_lastname[0]) ) {
			char filename[16];

			lastheader = tolower(au->au_lastname[0]);
			sprintf(filename, "DIR_%c_HTML", toupper(lastheader) );
			if (dirfp) {
				printf("<td><a href=\"http://HTMLBASE/%s\">%s</a></td>\n",
					oldfilename, aulast->au_author);
				printf("</tr>\n");
				if (position) {
					while(position != 3) {
						fprintf(dirfp, "<td>-</td>\n");
						position++;
					}
					fprintf(dirfp, "</tr>\n");
				}
				fprintf(dirfp, "</table>\n");
				fprintf(dirfp, "\n<hr>\n");
				fprintf(dirfp, "<a href=\"http://HTMLBASE/sfdbase.html\">\n");
				fprintf(dirfp, "<img src=\"isfdb2.gif\" border=2 align=middle></a>\n");
				fprintf(dirfp, "<i>COPYRIGHT</i>\n");
				fprintf(dirfp, "</center>\n");
				fprintf(dirfp, "</body>\n");
				fprintf(dirfp, "</html>\n");
				fclose(dirfp);
			}
			dirfp = fopen(filename, "wb+");

			sprintf(filename, "DIR_%c.html", toupper(lastheader) );
			printf("<tr>\n");
			printf("<td><a href=\"http://HTMLBASE/%s\">%s</a></td>\n",
				filename, au->au_author);
			strcpy(oldfilename, filename);

			fprintf(dirfp, "<html><head><title>ISFDB Author Directory (%c)</title></head>\n", 
				toupper(lastheader));
			fprintf(dirfp, "<body bgcolor=\"#ffffff\" text=\"#000000\" link=\"#155BF0\" vlink=\"#309372\">\n");
			fprintf(dirfp, "<center>\n");
			fprintf(dirfp, "<h1>ISFDB Author Directory (%c)</h1>\n<hr>\n", 
				toupper(lastheader));
			fprintf(dirfp, "<table border=1 cellpadding=2 cellspacing=1 BGCOLOR=\"#FFFFD0\">\n");
			position  = 0;
			newmargin = 0;
		}

		if (position == 0) {
			fprintf(dirfp, "<tr>\n");
		}
		fprintf(dirfp, "<td><a href=\"http://DBASELOC/exact_author.cgi?%s\">%s</a></td>\n",
			escape_string(bqt), au->au_author);
		position++;
		if (position == 3) {
			fprintf(dirfp, "</tr>\n");
			position  = 0;
		}
		aulast = au;
		au = au->au_next;
	}
	if (position) {
		while(position != 3) {
			fprintf(dirfp, "<td>-</td>\n");
			position++;
		}
		fprintf(dirfp, "</tr>\n");
	}
	if (aulast) {
		printf("<td><a href=\"http://HTMLBASE/%s\">%s</a></td>\n",
			oldfilename, aulast->au_author);
		position++;
		printf("</tr>\n");
	}
	fprintf(dirfp, "</table>\n");
	fprintf(dirfp, "\n<hr>\n");
	fprintf(dirfp, "<a href=\"http://HTMLBASE/sfdbase.html\">\n");
	fprintf(dirfp, "<img src=\"isfdb2.gif\" border=2 align=middle></a>\n");
	fprintf(dirfp, "<i>COPYRIGHT</i>\n");
	fprintf(dirfp, "</center>\n");
	fprintf(dirfp, "</body>\n");
	fprintf(dirfp, "</html>\n");
	fclose(dirfp);
}

void
do_info6(int way)
{
	dbaseloc = 1;
	parse_awards2(way, 1);
}

void
do_info7(int way)
{
	dbaseloc = 1;
	parse_awards3(way);
}

void
do_info8(int way)
{
	dbaseloc = 1;
	parse_awards2(way, 0);
}

void
do_info9(int way)
{
	dbaseloc = 1;
	parse_awards4(way);
}

void
do_info10()
{
	int loop, loop2;
	int max, column;

	for(loop2=1; loop2<6; loop2++) {
		/*
		 * At least 1 reprint
		 */
		max = 0;
		search_file3("titles.dbase", "sf", loop2);
		for(loop=0; loop<3000; loop++) {
			if (year_stat[loop] == 0) {
				year_stat3[loop] = 0.0;
			} else {
				year_stat3[loop] = 100.0 * ((float)year_stat[loop] / (float)year_stat2[loop]);
				if ( year_stat3[loop] > (float)max) {
					max = (int)year_stat3[loop];
				}
			}
		}

		init_vscreen(START_YEAR, END_YEAR, 5, 0, max);
		for(column=5,loop=START_YEAR; loop<END_YEAR; loop++,column++) {
			plot_column(column, (int)year_stat3[loop]);
		}
		printf("\n\n");
		if (loop2 == 1) {
			printf("                          Percent Short Stories with at least %d reprint\n\n", loop2);
		} else {
			printf("                          Percent Short Stories with at least %d reprints\n\n", loop2);
		}
		print_vscreen(); 
	}

}


void
do_info11()
{
	search_t	*tmp;
	author_t	*au;
	author_t	*aulast;
	char		underscored_author[256];
	char		*ptr1;
	char		*bqt;
	char		lastheader = 'z';
	FILE	*fp;
	int	dummy;

	fp = fopen("authors.dbase", "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		tmpspace7[0] = 0;
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
		if ( parse_field(fp, tmpspace7, (int *)&dummy, BIGSIZE) == -1 ) {
			goto finish;
		}
		if ( parse_to_eol(fp, (int *)&dummy) == -1 ) {
			goto finish;
		}

		if (tmpspace7[0] == 0) {
			continue;
		} else {
			add_author_entry(tmpspace1, tmpspace7, NULL);
		}

	}

finish:
	push_authors(author_list);
	au = au_head;
	while(au) {
		strcpy(underscored_author, au->au_author);
		ptr1 = underscored_author;
		while (*ptr1) {
			if (*ptr1 == ' ')
				*ptr1 = '_';
			ptr1++;
		}

		bqt = (char *)bquote(underscored_author);
		if ( strlen(bqt) == 0) {
			au = au->au_next;
			continue;
		}

		if (lastheader != tolower(au->au_lastname[0]) ) {
			printf("\n\n");
			lastheader = tolower(au->au_lastname[0]);
		}

		printf("<a href=\"http://DBASELOC/exact_author.cgi?%s\">%s</a>\n", 
			escape_string(bqt), au->au_author);

		aulast = au;
		au = au->au_next;
	}

	if (aulast)
		printf("%s</a>\n", aulast->au_author);
}


void
do_iaindex()
{
	author_t        *au;
	award_t		*tmp;
	award_t		*tmp2;
	int		first, titlemode;

	parse_awards(1);

	au_head = NULL;
	tmp = aw_list;
	while(tmp) {
		au = au_head;
		while(au) {
			if (strcmp(au->au_author, tmp->aw_author) == 0) {
				break;
			}
			au = au->au_next;
		}
		if (!au) {
			char *ctmp;

			au = (author_t *)malloc(sizeof(author_t));
			au->au_list = NULL;
			au->au_end  = NULL;

			au->au_author = (char *)malloc( strlen(tmp->aw_author) + 1);
			strcpy(au->au_author, tmp->aw_author);

			ctmp = (char *)lastname(tmp->aw_author);
			au->au_lastname = (char *)malloc( strlen(ctmp) + 1);
			strcpy(au->au_lastname, ctmp);

			insert_author(au);
		}
		tmp = tmp->aw_next;
	}

	au = au_head;
	while(au) {
		strcpy(tmpauthor, au->au_author);
		printf("\n%s\n", au->au_author);
		tmp = aw_list;
		while(tmp) {
			if (tmp->aw_marker) {
				tmp = tmp->aw_next;
				continue;
			}

			if (strcmp(tmpauthor, tmp->aw_author) == 0) {
				if ( strcmp("untitled", tmp->aw_title) == 0) {
					printf("    %s: ", tmp->aw_typel );
					titlemode = 0;
				} else if ( strstr(tmp->aw_title, "^")) {
					char *ptr;

					ptr = (char *)strstr(tmp->aw_title, "^");
					*ptr = 0; ptr++;
					printf("    %s [vt %s]: ", tmp->aw_title, ptr );
					ptr--; *ptr = '^';

				} else {
					printf("    %s: ", tmp->aw_title );
					titlemode = 1;
				}

				first = 1;
				tmp2 = tmp;
				while(tmp2) {
					if (strcmp(tmpauthor, tmp2->aw_author)) {
						tmp2 = tmp2->aw_next;
						continue;
					}
					if (titlemode && (strcmp(tmp2->aw_title, tmp->aw_title) == 0)) {
						if (first) {
							first = 0;
						} else {
							printf(", ");
						}
						printf("%s%s%s[%s]", 
						tmp2->aw_award, tmp2->aw_year, 
						tmp2->aw_types, tmp2->aw_level);
						tmp2->aw_marker = 1;
					}
					if (!titlemode && (strcmp(tmp2->aw_typel, tmp->aw_typel) == 0)) {
						if (first) {
							printf("        ");
							first = 0;
						} else {
							printf(", ");
						}
						printf("%s%s%s[%s]", 
						tmp2->aw_award, tmp2->aw_year, 
						tmp2->aw_types, tmp2->aw_level);
						tmp2->aw_marker = 1;
					}
					tmp2 = tmp2->aw_next;
				}
				printf("\n");
			}
			tmp = tmp->aw_next;
		}
		au = au->au_next;
	}
}


void
do_author_stat(char *author, char *year, int *tnumber, int *max, char *title)
{
	audata_t *tmp;
	int age = -1;

	if (strcmp(year, "0") == 0) {
		return;
	}
	if (strcmp(year, "8888") == 0) {
		return;
	}

	tmp = at_list;
	while(tmp) {
		if (strcmp(author, tmp->au_name) == 0) {
			int bdate, tdate, ddate;

			if (tmp->au_birthdate[0]) {
				if ( strcmp(tmp->au_birthdate, "unknown") == 0) {
					break;
				}
				sscanf(tmp->au_birthdate, "%d", &bdate);
				if (strncmp(year, "9999", 4) == 0) {
					tdate = CURRENT_YEAR;
				} else {
					sscanf(year, "%d", &tdate);
				}
				age = tdate - bdate;
				if ((tdate == 0) || (tdate > END_YEAR)) {
					break;
				}
				if (tmp->au_deathdate[0]) {
					sscanf(tmp->au_deathdate, "%d", &ddate);
					if (age > (ddate - bdate)) {
						/*
						 * This says: if the author died,
						 * how much input did they really
						 * have in the novel? Toss it.
						 */
						age = 0;
					}
				}
				if ((age >= 15) && (age <= 90)) {
					year_stat[age]++;
					(*tnumber)++;
					if (year_stat[age] > *max) {
						*max = year_stat[age];
					}
#ifdef DEBUGAGE
				}
				if ((age < 25) || (age > 75)) {
					printf("%02d [%s,%s]\n", age, title, author);
#endif
				}
			}
			break;
		}
		tmp = tmp->au_next;
	}
#ifdef REMOVE
	if (age != -1) {
		if (age < 20)
			printf("age=%d for %s (%s)\n", age, author, title);
		if (age > 75)
			printf("age=%d for %s (%s)\n", age, author, title);
	}
#endif
}

void
do_author_stat2(char *author, char *year, char *title)
{
	audata_t *tmp;

	tmp = at_list;
	while(tmp) {
		if (strcmp(author, tmp->au_name) == 0) {
			int bdate, tdate, age;

			if (tmp->au_birthdate[0]) {
				if ( strcmp(tmp->au_birthdate, "unknown") == 0) {
					break;
				}
				sscanf(tmp->au_birthdate, "%d", &bdate);
				sscanf(year, "%d", &tdate);
				age = tdate - bdate;
				if ((tdate <= 0) || (tdate > END_YEAR)) {
					break;
				}
				if ((age >= 15) && (age <= 90)) {
					if (*(int *)tmp->au_deathdate > age) {
						*(int *)tmp->au_deathdate = age;
					}
#ifdef DEBUGAGE
				}
				if ((age < 25) || (age > 75)) {
					printf("%02d [%s,%s]\n", age, title, author);
#endif
				}
			}
			break;
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
	search_t	*tmp;
	audata_t	*auth;
	char		path[256];
	char		file[128];
	int		fd;
	struct stat	buf;

	if (argc < 2) {
		printf("Bad info request\n");	
		exit(1);
	}

	if ( strcmp( argv[1], "1") == 0) {
		do_info1();
	} else if ( strcmp( argv[1], "3") == 0) {
		printf("<html>\n");
		printf("<head>\n");
		printf("<title>ISFDB Statistics by Year</title>\n");
		printf("</head>\n");
		printf("<body bgcolor=\"#ffffff\" text=\"#000000\" >\n");

		printf("<h1>ISFDB Statistics by Year</h1>\n");
		printf("<hr>\n");
		do_info3();
	} else if ( strcmp( argv[1], "4") == 0) {
		printf("<html><head>\n");
		printf("<title>Oldest Living Authors/Editors/Artists</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>Oldest Living Authors/Editors/Artists</h1><hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info4(0);
	} else if ( strcmp( argv[1], "5") == 0) {
		printf("<html><head>\n");
		printf("<title>Author Directory</title></head>\n");
		printf("<body bgcolor=\"#ffffff\" text=\"#000000\" link=\"#155BF0\" vlink=\"#309372\">\n");
		printf("<center>\n");
		printf("<h1>ISFDB Author Directory</h1>\n");
		printf("</center>\n");
		printf("<hr>\n");
		printf("<p>\n");
		printf("<center>\n");
		printf("<table border=1 cellpadding=2 cellspacing=1 BGCOLOR=\"#FFFFD0\">\n");
		printf("<tr>\n");
		printf("<td><b>From</b></td>\n");
		printf("<td><b>To</b></td>\n");
		printf("</tr>\n");
		fflush(stdout);
		do_info5();
		printf("</table>\n");
		printf("</center>\n");
	} else if ( strcmp( argv[1], "6") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Top 100 Books (Balanced List)</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Top 100 Books (Balanced List)</h1>\n<hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info6(6);
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "7") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Top 100 Books (Critical List)</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Top 100 Books (Critical List)</h1>\n<hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info6(7);
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "8") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Top 100 Books (Popular List)</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Top 100 Books (Popular List)</h1>\n<hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info6(8);
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "9") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Top 100 Short Stories (Balanced List)</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Top 100 Short Stories (Balanced List)</h1>\n<hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info6(9);
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "10") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Top 100 Short Stories (Critical List)</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Top 100 Short Stories (Critical List)</h1>\n<hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info6(10);
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "11") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Top 100 Short Stories (Popular List)</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Top 100 Short Stories (Popular List)</h1>\n<hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info6(11);
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "12") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Top 100 Authors (Balanced List)</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Top 100 Authors (Balanced List)</h1>\n<hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info7(12);
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "13") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Top 100 Authors (Critical List)</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Top 100 Authors (Critical List)</h1>\n<hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info7(13);
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "14") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Top 100 Authors (Popular List)</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Top 100 Authors (Popular List)</h1>\n<hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info7(14);
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "15") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Top Books by Year</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Top Books by Year</h1>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info8(6);
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "16") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Top Short Stories by Year</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Top Short Stories by Year</h1>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info8(9);
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "17") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Award Index (By Author)</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Award Index (By Author)</h1>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_iaindex();
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "18") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Top 100 Anth/Coll/Mag (Balanced List)</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Top 100 Anth/Coll/Mag (Balanced List)</h1>\n<hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info9(9);
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "19") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Top 100 Anth/Coll/Mag (Critical List)</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Top 100 Anth/Coll/Mag (Critical List)</h1>\n<hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info9(10);
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "20") == 0) {
		printf("<html><head>\n");
		printf("<title>ISFDB Top 100 Anth/Coll/Mag (Popular List)</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>ISFDB Top 100 Anth/Coll/Mag (Popular List)</h1>\n<hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info9(11);
		printf("</pre>\n");
	} else if ( strcmp( argv[1], "21") == 0) {
		audata_t *tmp;
		FILE	*fp;
		int	loop, dummy, max, tnumber, bnumber;
		char	*author;
		char *ptr;

		for(loop=0; loop<3000; loop++) {
			year_stat[loop] = 0;
		}

		max = tnumber = bnumber = 0;
		load_authors();

		fp = fopen("titles.dbase", "rb");
		if (fp == NULL) {
			perror("Couldn't open titles.dbase");
			exit(1);
		}

		while(1) {
			search_t *set;

			set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
			if (set == NULL) {
				break;
			}

			if (strcmp(set->se_type, argv[2])) {
				continue;
			}

			bnumber++;
			author = set->se_author;
			if ( strstr(author, "^") ) {
				auset_t *tmp2;

				tmp2 = (auset_t *)decompose(author);
				separate(tmp2);
				while( tmp2 ) {
					author = tmp2->au_actual;
					while ( strstr(author, "+") ) {
						ptr = (char *)strstr(author, "+");
						*ptr = 0;
						do_author_stat(author, set->se_year, &tnumber, &max, set->se_title);
						author = ++ptr;
					}
					do_author_stat(author, set->se_year, &tnumber, &max, set->se_title);
					tmp2 = tmp2->au_next;
				}
			} else {
				while ( strstr(author, "+") ) {
					ptr = (char *)strstr(author, "+");
					*ptr = 0;
					do_author_stat(author, set->se_year, &tnumber, &max, set->se_title);
					author = ++ptr;
				}
				do_author_stat(author, set->se_year, &tnumber, &max, set->se_title);
			}
		}
		init_vscreen(15, 85, 5, 0, max);
		for(loop=15; loop<100; loop++) {
			plot_column(loop-10, year_stat[loop]);
		}

		printf("<html><head> <title>ISFDB Age List</title></head>\n");
		printf("<body bgcolor=\"ffffff\">\n");
		printf("<h1>ISFDB Age Graph</h1>\n");
		printf("<pre>\n");

		if ( strcmp(argv[2], "n") == 0) {
			printf("      Novels vs. Age (%d%% of titles used)\n\n", (100 * tnumber)/bnumber);
		} else if ( strcmp(argv[2], "sf") == 0) {
			printf("      Shortfiction vs. Age (%d%% of titles used)\n\n", (100 * tnumber)/bnumber);
		} else if ( strcmp(argv[2], "es") == 0) {
			printf("      Essays vs. Age (%d%% of titles used)\n\n", (100 * tnumber)/bnumber);
		}

		print_vscreen();
		printf("</pre></body></html>\n");
		exit(0);

	} else if ( strcmp( argv[1], "22") == 0) {
		audata_t *tmp;
		FILE	*fp;
		int	loop, dummy, max, number;
		int	novelmode, tnumber;
		int	winners_only;
		award_t	*atmp;
		char	*author;
		char *ptr;

		for(loop=0; loop<3000; loop++) {
			year_stat[loop] = 0;
		}

		max = number = 0;
		load_authors();
		parse_awards(1);

		if (strcmp(argv[2], "n") == 0) {
			novelmode = 1;
		} else {
			novelmode = 0;
		}

		if (strcmp(argv[3], "1") == 0) {
			winners_only = 1;
		} else {
			winners_only = 0;
		}

		fp = fopen("titles.dbase", "rb");
		if (fp == NULL) {
			perror("Couldn't open titles.dbase");
			exit(1);
		}

		while(1) {
			search_t *set;

			/*
			 * Parse the next title entry
			 */
			set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
			if (set == NULL) {
				break;
			}

			/*
			 * Weed out undesired titles
			 */
			if (novelmode) {
				if (strcmp(set->se_type, "n"))
					continue;
			} else {
				if (strcmp(set->se_type, "sf"))
					continue;
			}

			/*
			 * Find a matching title/author for it.
			 */
			atmp = aw_list;
			while(atmp) {
				if (strcmp(atmp->aw_title, set->se_title) ||
				    strcmp(atmp->aw_author, set->se_author) ) {
					atmp = atmp->aw_next;
					continue;
				}

				/*
				 * If the type is wrong, keep looking
				 */
				if (strcmp(atmp->aw_types, argv[2])) {
					atmp = atmp->aw_next;
					continue;
				}

				if (winners_only) {
					if (strcmp(atmp->aw_level, "1")) {
						atmp = atmp->aw_next;
						continue;
					}
				}
				break;
			}

			/*
			 * If there was no award, skip the entry
			 */
			if (atmp == NULL) {
				continue;
			}

			author = atmp->aw_author;
			if ( strstr(author, "^") ) {
				auset_t *tmp2;

				tmp2 = (auset_t *)decompose(author);
				separate(tmp2);
				while( tmp2 ) {
					author = tmp2->au_actual;
					while ( strstr(author, "+") ) {
						ptr = (char *)strstr(author, "+");
						*ptr = 0;
						do_author_stat(author, atmp->aw_year, &tnumber, &max, set->se_title);
						author = ++ptr;
					}
					do_author_stat(author, atmp->aw_year, &tnumber, &max, set->se_title);
					tmp2 = tmp2->au_next;
				}
			} else {
				while ( strstr(author, "+") ) {
					ptr = (char *)strstr(author, "+");
					*ptr = 0;
					do_author_stat(author, atmp->aw_year, &tnumber, &max, set->se_title);
					author = ++ptr;
				}
				do_author_stat(author, atmp->aw_year, &tnumber, &max, set->se_title);
			}
			atmp = atmp->aw_next;
		}
		init_vscreen(15, 90, 5, 0, max);
		for(loop=15; loop<90; loop++) {
			plot_column(loop-10, year_stat[loop]);
		}

		printf("<html><head> <title>ISFDB Age List</title></head>\n");
		printf("<body bgcolor=\"ffffff\">\n");
		printf("<h1>ISFDB Age Graph</h1>\n");
		printf("<pre>\n");

		if (winners_only) {
			if ( strcmp(argv[2], "n") == 0) {
				printf("      Award-winning Novels vs. Age\n\n");
			} else if ( strcmp(argv[2], "nv") == 0) {
				printf("      Award-winning Novellas vs. Age\n\n");
			} else if ( strcmp(argv[2], "nt") == 0) {
				printf("      Award-winning Novelettes vs. Age\n\n");
			} else if ( strcmp(argv[2], "ss") == 0) {
				printf("      Award-winning Short Stories vs. Age\n\n");
			}
		} else {
			if ( strcmp(argv[2], "n") == 0) {
				printf("      Award-nominated Novels vs. Age\n\n");
			} else if ( strcmp(argv[2], "nv") == 0) {
				printf("      Award-nominated Novellas vs. Age\n\n");
			} else if ( strcmp(argv[2], "nt") == 0) {
				printf("      Award-nominated Novelettes vs. Age\n\n");
			} else if ( strcmp(argv[2], "ss") == 0) {
				printf("      Award-nominated Short Stories vs. Age\n\n");
			}
		}
		print_vscreen();
		printf("</pre></body></html>\n");
		exit(0);

	} else if ( strcmp( argv[1], "23") == 0) {
		audata_t *tmp;
		FILE	*fp;
		int	age, loop, dummy, max, tnumber, bnumber;
		char	*author;
		char *ptr;

		for(loop=0; loop<100; loop++) {
			year_stat[loop] = 0;
		}

		max = bnumber = 0;
		load_authors();

		fp = fopen("titles.dbase", "rb");
		if (fp == NULL) {
			perror("Couldn't open titles.dbase");
			exit(1);
		}

		/*
		 * Allocate space to store the minimum age, and
		 * initialize the minimum age to 100.
		 */
		tmp = at_list;
		while(tmp) {
			tmp->au_deathdate = (char *)malloc( sizeof(int) );
			*(int *)tmp->au_deathdate = 100;
			tmp = tmp->au_next;
		}

		while(1) {
			search_t *set;

			set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
			if (set == NULL) {
				break;
			}

			if (strcmp(set->se_type, "n")) {
				continue;
			}

			bnumber++;
			author = set->se_author;
			if ( strstr(author, "^") ) {
				auset_t *tmp2;

				tmp2 = (auset_t *)decompose(author);
				separate(tmp2);
				while( tmp2 ) {
					author = tmp2->au_actual;
					while ( strstr(author, "+") ) {
						ptr = (char *)strstr(author, "+");
						*ptr = 0;
						do_author_stat2(author, set->se_year, set->se_title);
						author = ++ptr;
					}
					do_author_stat2(author, set->se_year, set->se_title);
					tmp2 = tmp2->au_next;
				}
			} else {
				while ( strstr(author, "+") ) {
					ptr = (char *)strstr(author, "+");
					*ptr = 0;
					do_author_stat2(author, set->se_year, set->se_title);
					author = ++ptr;
				}
				do_author_stat2(author, set->se_year, set->se_title);
			}
		}

		tmp = at_list;
		while(tmp) {
			age = *(int *)tmp->au_deathdate;
			if (age != 100) {
				year_stat[age]++;
				if (year_stat[age] > max) {
					max = year_stat[age];
				}
			}
			tmp = tmp->au_next;
		}


		init_vscreen(15, 85, 5, 0, max);
		for(loop=15; loop<100; loop++) {
			plot_column(loop-10, year_stat[loop]);
		}


		printf("<html><head> <title>ISFDB Age List</title></head>\n");
		printf("<body bgcolor=\"ffffff\">\n");
		printf("<h1>ISFDB Age Graph</h1>\n");
		printf("<pre>\n");

		printf("      First Novels vs. Age\n\n");
		print_vscreen();
		printf("</pre></body></html>\n");
		exit(0);

	} else if ( strcmp( argv[1], "24") == 0) {
		audata_t *tmp;
		award_t	*atmp;
		FILE	*fp;
		int	loop, dummy, max;

		for(loop=0; loop<100; loop++) {
			year_stat[loop] = 0;
		}

		max = 0;
		load_authors();

		fp = fopen("titles.dbase", "rb");
		if (fp == NULL) {
			perror("Couldn't open titles.dbase");
			exit(1);
		}

		while(1) {
			search_t *set;

			set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
			if (set == NULL) {
				break;
			}

			if (strcmp(set->se_type, "n")) {
				continue;
			}

			tmp = at_list;
			while(tmp) {
				if (strcmp(set->se_author, tmp->au_name) == 0) {
					int bdate, tdate, age;

					if (tmp->au_birthdate[0]) {
						sscanf(tmp->au_birthdate, "%d", &bdate);
						sscanf(set->se_year, "%d", &tdate);
						age = tdate - bdate;
						if ((tdate == 0) || (tdate > END_YEAR)) {
							break;
						}
						if ((age >= 15) && (age <= 90)) {
							year_stat[age]++;
							if (year_stat[age] > max) {
								max = year_stat[age];
							}
						}
					}
					break;
				}
				tmp = tmp->au_next;
			}
		}
		fclose(fp);


		for(loop=0; loop<100; loop++) {
			year_stat2[loop] = 0;
		}

		fp = fopen("titles.dbase", "rb");
		if (fp == NULL) {
			perror("Couldn't open titles.dbase");
			exit(1);
		}

		max = 0;
		parse_awards(1);

		while(1) {
			search_t *set;

			/*
			 * Parse the next title entry
			 */
			set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
			if (set == NULL) {
				break;
			}

			/*
			 * Only take novels
			 */
			if (strcmp(set->se_type, "n"))
				continue;

			/*
			 * Find a matching title/author for it.
			 */
			atmp = aw_list;
			while(atmp) {
				if (strcmp(atmp->aw_title, set->se_title) ||
				    strcmp(atmp->aw_author, set->se_author) ) {
					atmp = atmp->aw_next;
					continue;
				}

				/*
				 * Only take novels
				 */
				if (strcmp(atmp->aw_types, "n")) {
					atmp = atmp->aw_next;
					continue;
				}

				break;
			}

			/*
			 * If there was no award, skip the entry
			 */
			if (atmp == NULL) {
				continue;
			}

			/*
			 * Now look for an author entry
			 */
			tmp = at_list;
			while(tmp) {
				if (strcmp(atmp->aw_author, tmp->au_name) == 0) {
					int bdate, tdate, age;

					if (tmp->au_birthdate) {
						sscanf(tmp->au_birthdate, "%d", &bdate);
						sscanf(atmp->aw_year, "%d", &tdate);
						age = tdate - bdate;
						if ((tdate == 0) || (tdate > END_YEAR)) {
							break;
						}
						if ((age >= 15) && (age <= 90)) {
							year_stat2[age]++;
							if (year_stat2[age] > max) {
								max = year_stat2[age];
							}
						}
					}
					break;
				}
				tmp = tmp->au_next;
			}
			atmp = atmp->aw_next;
		}

		max = 0;
		for(loop=15; loop<75; loop++) {
			if (year_stat[loop] == 0) {
				year_stat3[loop] = 0.0;
			} else {
				year_stat3[loop] = 100.0 * ((float)year_stat2[loop] / (float)year_stat[loop]);
				if ( year_stat3[loop] > (float)max) {
					max = (int)year_stat3[loop];
				}
			}
		}

		init_vscreen(15, 75, 5, 0, max);
		for(loop=15; loop<75; loop++) {
			plot_column(loop-10, (int)year_stat3[loop]);
		}


		printf("<html><head> <title>ISFDB Age List</title></head>\n");
		printf("<body bgcolor=\"ffffff\">\n");
		printf("<h1>ISFDB Age Graph</h1>\n");
		printf("<pre>\n");

		printf("      Quality Ratio (100x Magnification) vs. Age\n\n");
		print_vscreen();

		printf("</pre></body></html>\n");

		exit(0);
	} else if ( strcmp( argv[1], "25") == 0) {
		do_info10();
	} else if ( strcmp( argv[1], "26") == 0) {
		do_info11();
	} else if ( strcmp( argv[1], "27") == 0) {
		audata_t *tmp;
		FILE	*fp;
		int	age, loop, dummy, max, tnumber, bnumber;
		char	*author;
		char *ptr;

		for(loop=0; loop<100; loop++) {
			year_stat[loop] = 0;
		}

		max = bnumber = 0;
		load_authors();

		fp = fopen("titles.dbase", "rb");
		if (fp == NULL) {
			perror("Couldn't open titles.dbase");
			exit(1);
		}

		/*
		 * Allocate space to store the minimum age, and
		 * initialize the minimum age to 100.
		 */
		tmp = at_list;
		while(tmp) {
			tmp->au_deathdate = (char *)malloc( sizeof(int) );
			*(int *)tmp->au_deathdate = 100;
			tmp = tmp->au_next;
		}

		while(1) {
			search_t *set;

			set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
			if (set == NULL) {
				break;
			}

			if (strcmp(set->se_type, "sf")) {
				continue;
			}

			bnumber++;
			author = set->se_author;
			if ( strstr(author, "^") ) {
				auset_t *tmp2;

				tmp2 = (auset_t *)decompose(author);
				separate(tmp2);
				while( tmp2 ) {
					author = tmp2->au_actual;
					while ( strstr(author, "+") ) {
						ptr = (char *)strstr(author, "+");
						*ptr = 0;
						do_author_stat2(author, set->se_year, set->se_title);
						author = ++ptr;
					}
					do_author_stat2(author, set->se_year, set->se_title);
					tmp2 = tmp2->au_next;
				}
			} else {
				while ( strstr(author, "+") ) {
					ptr = (char *)strstr(author, "+");
					*ptr = 0;
					do_author_stat2(author, set->se_year, set->se_title);
					author = ++ptr;
				}
				do_author_stat2(author, set->se_year, set->se_title);
			}
		}

		tmp = at_list;
		while(tmp) {
			age = *(int *)tmp->au_deathdate;
			if (age != 100) {
				year_stat[age]++;
				if (year_stat[age] > max) {
					max = year_stat[age];
				}
			}
			tmp = tmp->au_next;
		}


		init_vscreen(15, 85, 5, 0, max);
		for(loop=15; loop<100; loop++) {
			plot_column(loop-10, year_stat[loop]);
		}


		printf("<html><head> <title>ISFDB Age List</title></head>\n");
		printf("<body bgcolor=\"ffffff\">\n");
		printf("<h1>ISFDB Age Graph</h1>\n");
		printf("<pre>\n");

		printf("      First Short Fiction vs. Age\n\n");
		print_vscreen();
		printf("</pre></body></html>\n");
		exit(0);

	} else if ( strcmp( argv[1], "28") == 0) {
		printf("<html><head>\n");
		printf("<title>Oldest Non-Living Authors/Editors/Artists</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>Oldest Non-Living Authors/Editors/Artists</h1><hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info4(1);
	} else if ( strcmp( argv[1], "29") == 0) {
		printf("<html><head>\n");
		printf("<title>Youngest Non-Living Authors/Editors/Artists</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>Youngest Non-Living Authors/Editors/Artists</h1><hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info4(2);
	} else if ( strcmp( argv[1], "30") == 0) {
		printf("<html><head>\n");
		printf("<title>Youngest Living Authors/Editors/Artists</title></head>\n");
		printf("<body bgcolor=ffffff>\n");
		printf("<h1>Youngest Living Authors/Editors/Artists</h1><hr>\n");
		printf("<pre>\n");
		fflush(stdout);
		do_info4(3);
	}

	printf("<hr>\n");
	printf("<center>\n");
	printf("<a href=\"http://HTMLBASE/sfdbase.html\">\n");
	printf("<img src=\"isfdb2.gif\" border=2 align=middle></a>\n");
	printf("<i>COPYRIGHT</i>\n");
	printf("</center>\n");
	printf("<hr>\n");
	printf("</pre>\n");
	printf("</body>\n");
	printf("</html>\n");
	exit(0);
}
