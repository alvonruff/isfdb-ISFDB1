/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)search.c	1.13	01/28/98 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "sfdbase.h"

char	tmptitle[MEDIUMSIZE];
char	tmptitle2[MEDIUMSIZE];
char	tmpauthor[MEDIUMSIZE];
char	tmpauthor2[MEDIUMSIZE];
char	tmpyear[MEDIUMSIZE];
char	tmpseries[MEDIUMSIZE];
char	tmpseries2[MEDIUMSIZE];
char	tmpsuperseries[MEDIUMSIZE];
char	tmppubs[MEDIUMSIZE];
char	tmptype[MEDIUMSIZE];
char	tmpstorylen[MEDIUMSIZE];
char	tmpnotes[16];
char	tmpsynopsis[16];
char	tmpseriesnum[16];
char	filter_author[MEDIUMSIZE]	= {0};
char	filter_title[MEDIUMSIZE]	= {0};
char	filter_series[MEDIUMSIZE]	= {0};
char	filter_year[8]		= {0};
int	fiction_only		= 0;
int	reviews_only		= 0;
entry   entries[MAX_ENTRIES];

search_t	*title_list  = NULL;
search_t	*title_end   = NULL;
search_t	*author_list = NULL;

extern	author_t *au_head;
extern  review_t *rv_list;
extern  pub_t	 *pub_list;


void
add_author(char *title, char *author, char *year, char *pseudo )
{
	search_t	*tmp;
	int		len;

	/*
	 * First check to see if this author already
	 * has an entry
	 */
	tmp = author_list;
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
	tmp->se_year      = (char *)malloc( strlen(year) + 1);

	strcpy(tmp->se_title, title);
	strcpy(tmp->se_author, author);
	strcpy(tmp->se_year, year);

	len = strlen(pseudo);
	if (len) {
		tmp->se_pseudonym = (char *)malloc(len+1);
		strcpy(tmp->se_pseudonym, pseudo);
	} else {
		tmp->se_pseudonym = NULL;
	}

	tmp->se_next = author_list;
	author_list = tmp;
}


void
add_title(search_t *awt)
{
	search_t	*tmp;

	if (fiction_only) {
		if (strcmp(awt->se_type, "a") == 0)
			return;
		if (strcmp(awt->se_type, "c") == 0)
			return;
		if (strcmp(awt->se_type, "nf") == 0)
			return;
		if (strcmp(awt->se_type, "ng") == 0)
			return;
		if (strcmp(awt->se_type, "ar") == 0)
			return;
		if (strcmp(awt->se_type, "ai") == 0)
			return;
		if (strcmp(awt->se_type, "se") == 0)
			return;
		if (strcmp(awt->se_type, "es") == 0)
			return;
		if (strcmp(awt->se_type, "r") == 0)
			return;
		if (strcmp(awt->se_type, "i") == 0)
			return;
	}

	if (reviews_only) {
		if (strcmp(awt->se_type, "r"))
			return;
	}

	tmp = (search_t *)malloc( sizeof(search_t) );
	if ( tmp == NULL ) {
		perror("out of memory");
		exit(1);
	}

	tmp->se_title  = (char *)malloc( strlen(awt->se_title) + 1);
	tmp->se_author = (char *)malloc( strlen(awt->se_author) + 1);
	tmp->se_year   = (char *)malloc( strlen(awt->se_year) + 1);

	strcpy(tmp->se_title,  awt->se_title);
	strcpy(tmp->se_author, awt->se_author);
	strcpy(tmp->se_year,   awt->se_year);

	if (awt->se_type) {
		tmp->se_type = (char *)malloc( strlen(awt->se_type) + 1);
		strcpy(tmp->se_type, awt->se_type);
	} else {
		tmp->se_type = NULL;
	}
	if (awt->se_series) {
		tmp->se_series = (char *)malloc( strlen(awt->se_series) + 1);
		strcpy(tmp->se_series, awt->se_series);
	} else {
		tmp->se_series = NULL;
	}
	if (awt->se_pubs) {
		tmp->se_pubs = (char *)malloc( strlen(awt->se_pubs) + 1);
		strcpy(tmp->se_pubs, awt->se_pubs);
	} else {
		tmp->se_pubs = NULL;
	}

	tmp->se_next = NULL;
        if ( title_list == NULL) {
		title_list = title_end = tmp;
	} else {
		title_end->se_next = tmp;
		title_end = tmp;
	}
}

void
add_series(char *series)
{
	search_t	*tmp;

	/*
	 * First check to see if this title already
	 * has an entry.
	 */
	tmp = title_list;
	while(tmp) {
		if ( tmp->se_series && (strcmp(series, tmp->se_series) == 0) ) {
			return;
		}
		tmp = tmp->se_next;
	}

	tmp = (search_t *)malloc( sizeof(search_t) );
	if ( tmp == NULL ) {
		perror("out of memory");
		exit(1);
	}

	tmp->se_series = (char *)malloc( strlen(series) + 1);
	strcpy(tmp->se_series, series);

        if ( title_list == NULL) {
		title_list = title_end = tmp;
	} else {
		title_end->se_next = tmp;
		title_end = tmp;
	}
}

void
search_file3(char *filename)
{
	FILE	*fp;
	int	dummy;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("Couldn't open shortfiction.dbase");
		exit(1);
	}

	while(1) {
		if ( parse_field(fp, tmpseries, (int *)&dummy, MEDIUMSIZE) == -1) {
			goto finish;
		}
		if ( parse_to_eol(fp, (int *)&dummy) == -1 ) {
			goto finish;
		}
		lowercase(tmpseries2, tmpseries);
		if ( strstr(tmpseries2, filter_series) ) {
			add_series(tmpseries);
		}
	}

finish:

	fclose(fp);
}

void
search_file2(char *filename)
{
	FILE	*fp;
	int	dummy;
	auset_t	*tmp;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("Couldn't open shortfiction.dbase");
		exit(1);
	}

	while(1) {
		if ( parse_field(fp, tmpauthor, (int *)&dummy, MEDIUMSIZE) == -1) {
			goto finish;
		}
		if ( parse_to_eol(fp, (int *)&dummy) == -1 ) {
			goto finish;
		}
		tmp = (auset_t *)decompose(tmpauthor);
		separate(tmp);
		while( tmp ) {
			lowercase(tmptitle2, tmp->au_actual);
			if ( strstr(tmptitle2, filter_author) ) {
				add_author(tmptitle, tmp->au_actual, tmpyear, tmp->au_pseudo);
			}
			if (tmp->au_pseudo && tmp->au_pseudo[0]) {
				lowercase(tmptitle2, tmp->au_pseudo);
				if ( strstr(tmptitle2, filter_author) ) {
					add_author(tmptitle, tmp->au_actual, tmpyear, tmp->au_pseudo);
				}
			}
			tmp = tmp->au_next;
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
		perror("Couldn't open shortfiction.dbase");
		exit(1);
	}

	while(1) {
		search_t *set;

		set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
		if ( set == NULL) {
			goto finish;
		}
		line_number++;

		if (filter_year[0]) {
			if ( strcmp(set->se_year, filter_year) == 0) {
				add_title(set);
				continue;
			}
		} 
		if (filter_title[0]) {
			lowercase(tmptitle2, set->se_title);
			if (strstr(tmptitle2, filter_title)) {
				add_title(set);
				continue;
			}
		} 
		if (filter_series[0]) {
			lowercase(tmpseries2, set->se_series);
			if (strstr(tmpseries2, filter_series)) {
				add_title(set);
				continue;
			}
		} 
		if (filter_author[0]) {
			auset_t	*tmp;

			tmp = (auset_t *)decompose(tmpauthor);
			separate(tmp);
			while( tmp ) {
				lowercase(tmptitle2, tmp->au_actual);
				if ( strstr(tmptitle2, filter_author) ) {
					add_author(set->se_title, tmp->au_actual, set->se_year, tmp->au_pseudo);
				}
				if (tmp->au_pseudo && tmp->au_pseudo[0]) {
					lowercase(tmptitle2, tmp->au_pseudo);
					if ( strstr(tmptitle2, filter_author) ) {
						add_author(set->se_title, tmp->au_actual, set->se_year, tmp->au_pseudo);
					}
				}
				tmp = tmp->au_next;
			}
		} 
	}

finish:

	fclose(fp);
}

void
request(char *arg)
{
	FILE *fp;

	printf("<html><head>\n");
	printf("<title>Research Request For: %s</title></head>\n", arg);
	printf("<body bgcolor=#ffffff>\n");
	printf("<h1>Research Request For: %s</h1>", arg);
#ifdef LINUX
	fp = popen("/bin/mail -s NewData avonruff@msilink.com", "w");
#else
	fp = popen("/bin/mailx -s NewData avonruff@msilink.com", "w");
#endif
	if (fp) {
		fprintf(fp, "Please research [%s]\n", arg);
		fclose(fp);
		printf("Your request to research [%s] has been emailed.\n", arg);
	}
}


int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int	result;
	int	loop;
	int	content_length;
	char	path[256];
	char	arg[256];
	char	type[256];
	char	*ptr;
	int	max;
	char	*startptr;
	char	*endptr;

	printf("Content-type: text/html\n\n");

#ifdef POST_METHOD
	if(strcmp(getenv("REQUEST_METHOD"), "POST") ) {
		printf("This script should be referenced with a METHOD of POST.\n");
		printf("<A HREF=\"http://www.ncsa.uiuc.edu/SDG/Software/Mosaic/Docs/fill-out-forms/overview.html\">forms overview</A>.%c",10);
		exit(1);
	}
	
	if(strcmp(getenv("CONTENT_TYPE"), "application/x-www-form-urlencoded")) {
		printf("This script can only be used to decode form results. \n");
		exit(1);
	}

	content_length = atoi(getenv("CONTENT_LENGTH"));
	for(loop=0; content_length && (!feof(stdin)); loop++) {
		max = loop;
		entries[loop].val = (char *)fmakeword(stdin, '&', &content_length);
		plustospace(entries[loop].val);
		unescape_url(entries[loop].val);
		entries[loop].name = (char *)makeword(entries[loop].val, '=');
	}

	for(loop=0; loop <= max; loop++) {
		if (strncmp(entries[loop].name, "arg", 3) == 0) {
			if (entries[loop].val[0]) {
				strcpy(arg, entries[loop].val);
			} else {
				arg[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "type", 4) == 0) {
			if (entries[loop].val[0]) {
				strcpy(type, entries[loop].val);
			} else {
				type[0] = 0;
			}
		} else {
		}
	}
#else
	if (argc != 2) {
		/* Error */
	}

	/*
	 * Replace any '+' characters with a space
	 */
	ptr = argv[1];
	while( *ptr ) {
		if (*ptr == '+') {
			*ptr = ' ';
		} else if (strncmp(ptr, "%27", 3) == 0) {
			char *ptr2;
			char *ptr3;

			ptr2 = ptr;
			*ptr2 = '\'';
			ptr2++;
			ptr3 = ptr+3;
			while(*ptr3) {
				*ptr2 = *ptr3;
				ptr2++;
				ptr3++;
			}
		} else if (strncmp(ptr, "%20", 3) == 0) {
			char *ptr2;
			char *ptr3;

			ptr2 = ptr;
			*ptr2 = ' ';
			ptr2++;
			ptr3 = ptr+3;
			while(*ptr3) {
				*ptr2 = *ptr3;
				ptr2++;
				ptr3++;
			}
		}
		ptr++;
	}

	startptr = (char *)strstr(argv[1], "arg=");
	if (startptr) {
		startptr += 4;
		endptr = (char *)strstr(startptr, "&");
		if (endptr) {
			*endptr = 0;
			strcpy(arg, startptr);
			startptr = endptr + 1;
		}

	} else {
		/* Error */
	}

	startptr = (char *)strstr(startptr, "type=");
	if (startptr) {
		startptr += 5;
		strcpy(type, startptr);
	} else {
		/* Error */
	}

#endif

	/*
	 * Reduce request to lower case
	 */
	ptr = arg;
	while( *ptr ) {
		*ptr = tolower( *ptr );
		ptr++;
	}
	
	if ( strncmp(type, "Name", 4) == 0) {
		(void)strcpy(filter_author, arg);
	} else if ( strncmp(type, "Title", 5) == 0) {
		(void)strcpy(filter_title, arg);
	} else if ( strncmp(type, "Series", 6) == 0) {
		(void)strcpy(filter_series, arg);
	} else if ( strncmp(type, "Year", 4) == 0) {
		(void)strcpy(filter_year, arg);
	} else if ( strncmp(type, "Index", 5) == 0) {
		(void)strcpy(filter_year, arg);
		fiction_only = 1;
	} else if ( strncmp(type, "RTindex", 7) == 0) {
		(void)strcpy(filter_year, arg);
		reviews_only = 1;
	} else if ( strncmp(type, "RAindex", 7) == 0) {
		(void)strcpy(filter_year, arg);
		reviews_only = 1;
	} else if ( strncmp(type, "Request", 7) == 0) {
		request(arg);
	}

#ifdef CHDIR
        result = chdir(CGIBIN);
        if (result != 0) {
                printf("CHDIR to %s failed\n", CGIBIN);
                exit(1);
        }
#endif

	if (filter_title[0]) {
		search_t	*tmp;
		char		message[256];

		search_file("titles.dbase");
		printf("<html><head>\n");
		printf("<title>Search For: %s</title></head>\n", filter_title);
		printf("<body bgcolor=#ffffff>\n");
		printf("<h1>Search For Title: %s</h1>", filter_title);

		tmp = title_list;
		print_total_found(tmp);
		printf("<hr>\n");
		printf("<pre>\n");

		print_titles_bycat( title_list );
	} else if (filter_series[0]) {
		search_t	*tmp;
		char		message[256];

		search_file3("titles.xbs");
		printf("<html><head>\n");
		printf("<title>Search For: %s</title></head>\n", filter_series);
		printf("<body bgcolor=#ffffff>\n");
		printf("<h1>Search For Series: %s</h1>", filter_series);

		tmp = title_list;
		print_total_found(tmp);
		printf("<hr>\n");
		printf("<pre>\n");

		print_search_series(tmp);
	} else if (filter_author[0]) {
		search_t *tmp;
		char	*ptr;
		char	newauthor[256];
		author_t *au;

		search_file2("titles.xba");
		tmp = author_list;
		if ( total_found(tmp) == 1) {
			if ( ( strstr(tmp->se_author, "^") == NULL) &&
			     ( strstr(tmp->se_author, "+") == NULL) ) {
#ifdef CHDIR
        			strcpy(path, BASE);
				strcat(path, "/bin");
				result = chdir(path);
                                if (result != 0) {
				        printf("CHDIR to %s failed\n", path);
				        exit(1);
                                }
#endif

				ptr = tmp->se_author;
				while(*ptr) {
					if (*ptr == ' ')
						*ptr = '_';

					ptr++;
				}
				execl("./exact_author.cgi", "exact_author", bquote(tmp->se_author), 0);
				printf("EXEC failed\n");
				exit(1);
			}
		}

		printf("<html><head>\n");
		printf("<title>Search For: %s</title></head>\n", filter_author);
		printf("<body bgcolor=#ffffff>\n");
		printf("<h1>Search For Author: %s</h1>", filter_author);

		tmp = author_list;
		print_total_found(tmp);
		printf("<hr>\n");
		printf("<pre>\n");

		/*
		 * Now reform the list by putting each one in an
		 * author structure, and sorting by lastname.
		 */
		while(tmp) {
			au = au_head;
			while(au) {
				if (strcmp(au->au_author, tmp->se_author) == 0) {
					break;
				}
				au = au->au_next;
			}
			if (!au) {
				char *ctmp;

				au = (author_t *)malloc(sizeof(author_t));
				au->au_list = tmp;
				au->au_end  = NULL;

				au->au_author = (char *)malloc( strlen(tmp->se_author) + 1);
				strcpy(au->au_author, tmp->se_author);

				ctmp = (char *)lastname(tmp->se_author);
				au->au_lastname = (char *)malloc( strlen(ctmp) + 1);
				strcpy(au->au_lastname, ctmp);

				insert_author(au);
			}
			tmp = tmp->se_next;
		}

		au = au_head;
		while(au) {
			html_print1_author(au->au_list->se_author, au->au_list->se_pseudonym, 
				0, RIGHTMARGIN, 0);
			printf("\n");
			au = au->au_next;
		}

	} else if ( (filter_year[0])  && (strncmp(type, "Year", 4) == 0) ) {
		search_t	*tmp;
		char		message[256];
		int		firsttime;

		search_file("titles.dbase");
		printf("<html><head>\n");
		printf("<title>Search For Year: %s</title></head>\n", filter_year);
		printf("<body bgcolor=#ffffff>\n");
		printf("<h1>Search For Year: %s</h1>", filter_year);

		tmp = title_list;
		print_total_found(tmp);
		printf("<hr>\n");
		printf("<pre>\n");

		print_titles_bycat( title_list );

	} else if ( (filter_year[0])  && (strncmp(type, "Index", 5) == 0) ) {
		search_t	*tmp;
		char		message[256];
		int		firsttime;

		search_file("titles.dbase");
		printf("<html><head>\n");
		printf("<title>Speculative Fiction Index For Year: %s</title></head>\n", filter_year);
		printf("<body bgcolor=#ffffff>\n");
		printf("<h1>Speculative Fiction Index For Year: %s</h1>", filter_year);

		tmp = title_list;
		print_totals_found(tmp);
		printf("<hr>\n");
		printf("<pre>\n");

		load_pubs(F_YEAR, filter_year);
		print_titles_author( title_list );

	} else if ( (filter_year[0])  && (strncmp(type, "RTindex", 7) == 0) ) {
		review_t	*tmp;
		pub_t		*ptmp;
		char		message[256];
		int		firsttime;

		printf("<html><head>\n");
		printf("<title>Speculative Fiction Review Index For Year: %s</title></head>\n", filter_year);
		printf("<body bgcolor=#ffffff>\n");
		printf("<h1>Speculative Fiction Review Index For Year: %s</h1>", filter_year);
		printf("<hr>\n");
		printf("<pre>\n");

		load_reviews(F_YEAR, filter_year);
		load_pubs(F_YEAR, filter_year);

		tmp = rv_list;
		tmptitle[0] = 0;
		tmpauthor[0] = 0;
		while(tmp) {
			if ( strcmp(tmp->rv_title, tmptitle) ||
			     strcmp(tmp->rv_author, tmp->rv_author) ) {
				strcpy(tmptitle, tmp->rv_title);
				strcpy(tmpauthor, tmp->rv_author);
				printf("%s,  by ", tmp->rv_title );
				html_print_authors_compressed(tmp->rv_author, 0);
			}
			ptmp = pub_list;
			while (ptmp) {
				if (strcmp(ptmp->pu_abbreviation, tmp->rv_abbreviation) == 0) {
					printf("\t%s (%s)\n", ptmp->pu_title, tmp->rv_reviewer);
					break;
				}
				ptmp = ptmp->pu_next;
			}
			tmp = tmp->rv_next;
		}
	} else if ( (filter_year[0])  && (strncmp(type, "RAindex", 7) == 0) ) {
		review_t	*tmp;
		pub_t		*ptmp;
		author_t	*au;
		char		message[256];
		int		firsttime;

		printf("<html><head>\n");
		printf("<title>Speculative Fiction Review Index For Year: %s</title></head>\n", filter_year);
		printf("<body bgcolor=#ffffff>\n");
		printf("<h1>Speculative Fiction Review Index For Year: %s</h1>", filter_year);
		printf("<hr>\n");
		printf("<pre>\n");

		load_reviews(F_YEAR, filter_year);
		load_pubs(F_YEAR, filter_year);

		tmp = rv_list;
		au_head = NULL;
		while(tmp) {
			au = au_head;
			while(au) {
				if (strcmp(au->au_author, tmp->rv_author) == 0) {
					break;
				}
				au = au->au_next;
			}
			if (!au) {
				char *ctmp;

				au = (author_t *)malloc(sizeof(author_t));
				au->au_list = NULL;
				au->au_end  = NULL;

				au->au_author = (char *)malloc( strlen(tmp->rv_author) + 1);
				strcpy(au->au_author, tmp->rv_author);

				ctmp = (char *)lastname(tmp->rv_author);
				au->au_lastname = (char *)malloc( strlen(ctmp) + 1);
				strcpy(au->au_lastname, ctmp);

				insert_author(au);
			}
			tmp = tmp->rv_next;
		}

		au = au_head;
		while(au) {
			int match = 0;
			tmptitle[0] = 0;
			strcpy(tmpauthor, au->au_author);
			html_print_authors_compressed(au->au_author, 0);
			tmp = rv_list;
			while(tmp) {
				if (strcmp(tmpauthor, tmp->rv_author) == 0) {
					match = 1;
					if (strcmp(tmptitle, tmp->rv_title) ) {
						strcpy(tmptitle, tmp->rv_title);
						printf("    %s\n", tmp->rv_title );
					}
					ptmp = pub_list;
					while (ptmp) {
						if (strcmp(ptmp->pu_abbreviation, tmp->rv_abbreviation) == 0) {
							printf("        %s (%s)\n", ptmp->pu_title, tmp->rv_reviewer);
						}
						ptmp = ptmp->pu_next;
					}
				}
				tmp = tmp->rv_next;
			}
			if (match == 0) {
				printf("No match for [%s]\n", au->au_author);
			}
			au = au->au_next;
		}
	}

finish:
	postamble("search", arg);
	printf("</pre>\n");
	exit(0);
}
