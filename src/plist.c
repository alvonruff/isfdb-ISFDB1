/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)plist.c	1.16	01/28/98 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef SUNOS
#include <sys/unistd.h>
#else
#include <unistd.h>
#endif
#include "sfdbase.h"

#define OPTARGS "t:a:y:?"

static char	tmppub[MEDIUMSIZE];
search_t        *title_list  = NULL;
search_t        *title_end  = NULL;
extern pub_t	*pub_list;

void
clear_markers(search_t *list)
{
	search_t        *tmp;
	award_t		*awtmp;

	tmp = list;
	while( tmp ) {
		tmp->se_marker = 0;
		if (tmp->se_awards) {
			awtmp = tmp->se_awards;
			while (awtmp) {
				awtmp->aw_marker = 0;
				awtmp = awtmp->aw_next;
			}
		}

		tmp = tmp->se_next;
	}
}


void
add_title(search_t *tmp)
{
	char		*ptr;

	tmp->se_next = title_list;
	title_list = tmp;
}


void
parse_works(char *filename, char *targetpub)
{
	FILE	*fp;
	FILE	*fp2;
	int	line_number = 1;
	int	dummy = 0;

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


void
original_flavor(pub_t *ptmp)
{
	search_t	*tmp;
	int		rightmargin = 50;
	int		omnibus = 0;

	tmp = title_list;
	while(tmp) {
		if ( strcmp(tmp->se_type, "o") == 0) {
			tmp = tmp->se_next;
			omnibus = 1;
			break;
		}
		tmp = tmp->se_next;
	}

	tmp = title_list;
	while(tmp) {
		if ( strcmp(tmp->se_type, "o") == 0) {
			tmp = tmp->se_next;
			continue;
		}
		if ( strcmp(tmp->se_type, "ar") == 0) {
			tmp = tmp->se_next;
			continue;
		}
		if ( (strcmp(tmp->se_type, "a") == 0) && !omnibus ) {
			tmp = tmp->se_next;
			continue;
		}
		if ( (strcmp(tmp->se_type, "c") == 0) && !omnibus ) {
			tmp = tmp->se_next;
			continue;
		}

		printf("%2s  ", tmp->se_type);
		print_title(4, tmp->se_title, rightmargin, T_PREINDENT, NULL);
		html_print_authors(tmp->se_author, rightmargin);
		tmp = tmp->se_next;
	}
}



int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int		result;
	char		path[256];
	char		title[256];
	search_t	*tmp;
	pub_t		*ptmp;
	char		*tmp2;

	printf("Content-type: text/html\n\n");

	if (argc != 2) {
		printf("Bad book input\n");	
		exit(1);
	}

#ifdef CHDIR
        result = chdir(CGIBIN);
        if (result != 0) {
                printf("CHDIR to %s failed\n", CGIBIN);
                exit(1);
        }
#endif

	parse_works("titles.xbt", argv[1] );
	load_pubs(F_EXACT, argv[1]);
	load_reviews(F_PLIST, argv[1]);
	load_interviews(F_PLIST, argv[1]);

	ptmp = pub_list;
	while (ptmp) {
		if ( strcmp(argv[1], ptmp->pu_abbreviation) == 0) {
			strcpy(title,  ptmp->pu_title);
			break;
		}
		ptmp = ptmp->pu_next;
	}

	printf("<html><head>\n");
	printf("<title>Content Listing</title></head>\n");
	printf("<body bgcolor=#ffffff>\n");
	printf("<h1>Content Listing</h1><hr>\n");
	print_menus3( argv[1] );
	printf("<hr>\n");

	if (ptmp) {
		if(ptmp->pu_coverpicture[0]) {
			printf("<table>\n");
			printf("<tr align=left>\n");
			printf("<td>\n");
			printf("<a href=\"%s\"><img src=\"%s\" height=150",
				ptmp->pu_coverpicture, ptmp->pu_coverpicture);
			printf("alt=\"picture\" border=2 align=middle></td></a>\n");
			printf("<td>\n");
		}
		printf("<pre>\n");
		if(ptmp->pu_isbn[0]) {
			if (ptmp->pu_isbn[0] != '#') {
				char newisbn[32];
				char *tmp1, *tmp2;

				tmp1 = ptmp->pu_isbn;
				tmp2 = newisbn;
				while (*tmp1) {
					if ((*tmp1 != '-') && (*tmp1 != ' ')) {
						*tmp2 = *tmp1;
						tmp2++;
					}
					tmp1++;
				}
				*tmp2 = 0;
				printf("Look for this book at: ");
				printf("[<a href=\"http://www.amazon.com/exec/obidos/ISBN=%s\">amazon</a>]", 
					newisbn);
				printf("[<a href=\"http://search.barnesandnoble.com/search?userid=&sourceid=&isbn=%s\">B&N</a>]", 
					newisbn);
				printf("[<a href=\"http://www.books.com/scripts/view.exe?isbn~%s\">books.com</a>]", 
					newisbn);
				printf("[<a href=\"http://futfan.com/cgi-bin/query?quick-isbn=%s\">futfan</a>]", 
					newisbn);
				printf("[<a href=\"http://www.bookpages.co.uk/Twist/twist.plx?form=BookDetails.htx&ISBN=%s\">bookpages</a>]", 
					newisbn);
				printf("\n\n");
			}
		}
		printf("<b>For:</b> <i>%s</i>\n\n", ptmp->pu_title);
		if(ptmp->pu_author[0]) {
			char *ptr;
			char *ptr2;

			ptr = ptmp->pu_author;
			printf("  Auth/Ed: ");
			html_print_authors_compressed(ptmp->pu_author, 11);
		}
		if(ptmp->pu_year[0]) {
			if (strcmp(ptmp->pu_year, "8888") == 0) {
				printf("  Year: Unpublished\n");
			} else if (strcmp(ptmp->pu_year, "9999") == 0) {
				printf("  Year: Forthcoming\n");
			} else if (strncmp(ptmp->pu_year, "9999", 4) == 0) {
				printf("  Year: Forthcoming %s\n", (char *)&(ptmp->pu_year[4]));
			} else {
				printf("  Year: %s\n", ptmp->pu_year);
			}
		}
		if(ptmp->pu_isbn[0]) {
			printf("  ISBN: %s\n", ptmp->pu_isbn);
		}
		if(ptmp->pu_publisher[0]) {
			if ( (strcmp(ptmp->pu_publisher, "Tor") == 0) ||
			     (strcmp(ptmp->pu_publisher, "Tor/Underwood") == 0) ||
			     (strcmp(ptmp->pu_publisher, "Tor/Orb") == 0) ||
			     (strcmp(ptmp->pu_publisher, "Tor/Forge") == 0) ||
			     (strcmp(ptmp->pu_publisher, "Tor Books") == 0)) {
				printf("  Pub: <a href=\"http://www.tor.com\">%s</a>\n", ptmp->pu_publisher);
			} else if ( (strcmp(ptmp->pu_publisher, "NESFA") == 0) ||
			     (strcmp(ptmp->pu_publisher, "NESFA Press") == 0)) {
				printf("  Pub: <a href=\"http://www.transarc.com/~jmann/nesfa.html\">%s</a>\n", 
					ptmp->pu_publisher);
			} else if ( (strcmp(ptmp->pu_publisher, "Bantam") == 0) ||
			     (strcmp(ptmp->pu_publisher, "Spectra/Bantam Books") == 0) ||
			     (strcmp(ptmp->pu_publisher, "Bantam/Spectra") == 0) ||
			     (strcmp(ptmp->pu_publisher, "Bantam/Turner") == 0) ||
			     (strcmp(ptmp->pu_publisher, "Bantam Spectra") == 0) ||
			     (strcmp(ptmp->pu_publisher, "Bantam Books") == 0)) {
				printf("  Pub: <a href=\"http://www.bdd.com/forum/bddforum.cgi/scifi/\">%s</a>\n", 
					ptmp->pu_publisher);
			} else if ( (strcmp(ptmp->pu_publisher, "Baen") == 0) ||
			     (strcmp(ptmp->pu_publisher, "Baen Books") == 0)) {
				printf("  Pub: <a href=\"http://www.baen.com/\">%s</a>\n", 
					ptmp->pu_publisher);
			} else {
				printf("  Pub: %s\n", ptmp->pu_publisher);
			}
		}
		if(ptmp->pu_price[0]) {
			unsigned char pound = 0xa3;

			if(ptmp->pu_price[0] == 'L') {
				printf("  Price: ");
				fwrite(&pound, 1, 1, stdout);
				printf("%s\n", (char *)&(ptmp->pu_price[1]));
			} else {
				printf("  Price: %s\n", ptmp->pu_price);
			}
		}
		if(ptmp->pu_pages[0])
			printf("  Pages: %s\n", ptmp->pu_pages);
		if(ptmp->pu_type[0])
			printf("  Type: %s\n", ptmp->pu_type);
		if(ptmp->pu_cover[0]) {
			if ( strchr(ptmp->pu_cover, '\"') ) {
				char *tmp;

				tmp = (char *)strchr(ptmp->pu_cover, '\"');
				tmp -= 5;
				*tmp = 0;
				tmp++;
				printf("  Cover: ");
				tmp2 = (char *)strchr(ptmp->pu_cover, '^');
				if (tmp2) {
					*tmp2 = 0;
					tmp2++;
					html_print1_author(ptmp->pu_cover, tmp2, 9, RIGHTMARGIN, 4);
				} else {
					html_print1_author(ptmp->pu_cover, NULL, 9, RIGHTMARGIN, 4);
				}
				printf(" %s\n", tmp);
			} else {
				printf("  Cover: ");
				tmp2 = (char *)strchr(ptmp->pu_cover, '^');
				if (tmp2) {
					*tmp2 = 0;
					tmp2++;
					html_print1_author(ptmp->pu_cover, tmp2, 9, RIGHTMARGIN, 4);
				} else {
					html_print1_author(ptmp->pu_cover, NULL, 9, RIGHTMARGIN, 4);
				}
				printf("\n");
			}
		}
		if(ptmp->pu_bcover[0]) {
			if ( strchr(ptmp->pu_bcover, '\"') ) {
				char *tmp;

				tmp = (char *)strchr(ptmp->pu_bcover, '\"');
				tmp -= 5;
				*tmp = 0;
				tmp++;
				printf("  Back Cover: ");
				tmp2 = (char *)strchr(ptmp->pu_bcover, '^');
				if (tmp2) {
					*tmp2 = 0;
					tmp2++;
					html_print1_author(ptmp->pu_bcover, tmp2, 9, RIGHTMARGIN, 4);
				} else {
					html_print1_author(ptmp->pu_bcover, NULL, 9, RIGHTMARGIN, 4);
				}
				printf(" %s\n", tmp);
			} else {
				printf("  Back Cover: ");
				tmp2 = (char *)strchr(ptmp->pu_bcover, '^');
				if (tmp2) {
					*tmp2 = 0;
					tmp2++;
					html_print1_author(ptmp->pu_bcover, tmp2, 9, RIGHTMARGIN, 4);
				} else {
					html_print1_author(ptmp->pu_bcover, NULL, 9, RIGHTMARGIN, 4);
				}
				printf("\n");
			}
		}
		if(ptmp->pu_notes[0]) {
			printf("  Notes:");
			read_and_printwrap(USE_SYNOPSIS, ptmp->pu_notes, 0, RIGHTMARGIN2, 0);
		}
		if(ptmp->pu_coverpicture[0]) {
			printf("</td>\n");
			printf("</table>\n");
			printf("<pre>\n");
		}
	} else {
		printf("<b>Error:</b> Cannot find the publication data for the tag %s\n\n", argv[1]);
	}

	printf("\n<b>Contains the following works:</b>\n");

	tmp = title_list;
	while(tmp) {
		if (tmp->se_type == NULL) {
			break;
		}
		if ( (tmp->se_storylen == NULL) &&
		     (strcmp(tmp->se_type, "p"))  &&
		     (strcmp(tmp->se_type, "se"))  &&
		     (strcmp(tmp->se_type, "es"))  &&
		     (strcmp(tmp->se_type, "r"))  &&
		     (strcmp(tmp->se_type, "e"))  &&
		     (strcmp(tmp->se_type, "i"))  &&
		     (strcmp(tmp->se_type, "ai"))  &&
		     (strcmp(tmp->se_type, "ar")) ) {
			original_flavor(ptmp);
			goto finish;
		}
		tmp = tmp->se_next;
	}

	tmp = title_list;
	clear_markers( tmp );
	print_zine_bycat( tmp );

finish:
	postamble("plist", argv[1]);
	printf("</pre>\n");
}
