/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)plist.c	1.15	06/10/97 SFdbase";

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

#ifdef REMOVE
	if ( title_list == NULL) {
		title_list = title_end = tmp;
	} else {
		title_end->se_next = tmp;
		title_end = tmp;
	}
#else
	tmp->se_next = title_list;
	title_list = tmp;
#endif
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

	printf("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n");
	printf("<html>\n\n");
	printf("<head>\n");

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

	printf("<title>Asimov's Science Fiction</title>\n\n");
	printf("</HEAD>\n\n");
	printf("<BODY text=#000000 LINK=#0000FF VLINK=#822B82 ALINK=#00FF00 bgcolor=#FFFFFF>\n\n");
	printf("<CENTER>\n");
	printf("    <font size=4 face=\"Arial, Helvetica\"> <I>page design comments in italics</I> </font>\n");
	printf("<TABLE WIDTH=75%%>\n");


	if (ptmp) {
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
			while( strlen(ptr) > RIGHTMARGIN) {
				ptr2 = (char *)&ptr[RIGHTMARGIN];
				while(*ptr2 != ' ') {
					ptr2--;
				}
				*ptr2 = 0;
				printf("%s\n           ", ptr);
				ptr = ptr2+1;
			}
			printf("%s\n", ptr);
		}
		if(ptmp->pu_year[0])
			printf("  Year: %s\n", ptmp->pu_year);
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
	} else {
		printf("<b>Error:</b> Cannot find the publication data for the tag %s\n\n", argv[1]);
	}

	printf("\n<b>Contains the following works:</b>\n");

	tmp = title_list;
	clear_markers( tmp );
	print_zine_bycat2( tmp );

	printf("</td></tr>\n");
	printf("</table>\n");
	printf("</center></div>\n\n");
	printf("<p align=\"center\">\n");
	printf("<font size=\"2\" face=\"Arial, Helvetica\">\n");
	printf("If you find any errors, typos or other stuff worth mentioning,\n");
	printf("please send it to <A href=\"mailto:\">Asimov's</A>.<br>\n");
	printf("<font size=\"1\">Copyright ) 1998 Asimov's SF  All Rights Reserved Worldwide\n");
	printf("</p>\n\n");
	printf("</body>\n");
	printf("</HTML>\n");

finish:
	exit(0);
}
