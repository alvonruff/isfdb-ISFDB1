/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)print.c	1.22	06/10/97 SFdbase";

#include <sys/types.h>
#include <sys/file.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#ifdef AIX
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#ifdef SUNOS
#include <sys/unistd.h>
#else
#include <unistd.h>
#endif
#include "sfdbase.h"

void print_pub_citations(char *citations, int correction);

#define CORRECTIONS_OKAY

#define MAXTITLECATS 13
struct titlecat {
	char	*longcat;
	char	*shortcat;
} titlecats[MAXTITLECATS] = {
	{"Novels",		"n"},
	{"Anthologies",		"a"},
	{"Collections",		"c"},
	{"Nonfiction",		"nf"},
	{"Shortfiction",	"sf"},
	{"Poems",		"p"},
	{"Non-Genre",		"ng"},
	{"Cover Artwork",	"ar"},
	{"Serials/Complete Novels", "se"},
	{"Essays/Articles",	"es"},
	{"Reviews",		"r"},
	{"Interviews",		"i"},
	{"Interior Artwork",	"ai"},
};

#define MAXZINEABBRS 34
struct zineabbr {
	char	*abbr;
	char	*title;
} zineabbrs[MAXZINEABBRS] = {
	{"ABOR",   "Aboriginal"},
	{"ABSMAG", "Absolute Magnitude"},
	{"AMAZQTR",   "Amazing Quarterly"},
	{"AMAZ",   "Amazing"},
	{"ANLG",   "Analog"},
	{"ASF",    "Asimov's"},
	{"AST",    "Astounding"},
	{"AUREAL", "Aurealis"},
	{"CAVAL",  "Cavalier"},
	{"CNTRY",  "Century"},
	{"CRANK",  "Crank!"},
	{"EIDO",   "Eidolon"},
	{"FANTBK", "Fantasy Book"},
	{"FSF",    "F&SF"},
	{"GAL",    "Galaxy"},
	{"HARSH",  "Harsh Mistress SF"},
	{"INTZ",   "Interzone"},
	{"LOUIS",  "Louis L'Amour Western Magazine"},
	{"MGRAF",  "Midnight Graffiti"},
	{"MZBFANT","MZB's Fantasy"},
	{"NWYRKR", "The New Yorker"},
	{"OMNI",   "Omni"},
	{"OOMNI",  "Omni Online"},
	{"ONSPEC", "On Spec"},
	{"PIRWRI", "Pirate Writings"},
	{"PLBY",   "Playboy"},
	{"PULPH",  "Pulphouse"},
	{"REALMS", "Realms of Fantasy"},
	{"SFAGE",  "SF Age"},
	{"STRPLA", "Strange Plasma"},
	{"TIME",   "Time"},
	{"TMRW",   "Tomorrow"},
	{"WEIRDT", "Weird Tales"},
	{"WIRED",  "Wired"},
};

#define MAXDATEABBRS 25
struct dateabbr {
	char	*abbr;
	char	*title;
} dateabbrs[MAXDATEABBRS] = {
	{"JANFEB", "Jan-Feb"},
	{"MARAPR", "Mar-Apr"},
	{"MAYJUN", "May-Jun"},
	{"JULAUG", "Jul-Aug"},
	{"SEPOCT", "Sep-Oct"},
	{"OCTNOV", "Oct-Nov"},
	{"NOVDEC", "Nov-Dec"},
	{"SPRING", "Spring"},
	{"AUTUMN", "Autumn"},
	{"FALL",   "Fall"},
	{"MDEC",   "Mid-Dec"},
	{"JAN",    "January"},
	{"FEB",    "February"},
	{"MAR",    "March"},
	{"APR",    "April"},
	{"MAY",    "May"},
	{"JUN",    "June"},
	{"JUL",    "July"},
	{"AUG",    "August"},
	{"SEP",    "September"},
	{"OCT",    "October"},
	{"NOV",    "November"},
	{"DEC",    "December"},
	{"WIN",    "Winter"},
	{"SUM",    "Summer"},
};



author_t	*au_head = NULL;
author_t        *au_end = NULL;
char		tmpbuf[BIGSIZE];
char		hotlink[BIGSIZE];
char		tmpbio[8192];
char		*_seriesnum = NULL;
int		dobuy = 0;
int		dbaseloc = 0;
extern int	perrno;
extern char	*pstring;
extern int	errcount;
extern pub_t	*pub_list;
extern pub_t	*pub_end;
extern inter_t	*it_list;
extern inter_t	*it_end;
extern review_t	*rv_list;
extern review_t	*rv_end;
extern award_t  *misc_awards;
extern char 	*escape_string(unsigned char *);

char *
bquote(char *string)
{
	char *tmp;
	char *ptr1;
	char *ptr2;

	tmp = (char *)malloc( strlen(string) + 50);
	ptr1 = string;
	ptr2 = tmp;
	while (*ptr1) {
		if (*ptr1 == '\'') {
			*ptr2 = '\\';
			ptr2++;
		}
		*ptr2 = *ptr1;
		ptr1++;
		ptr2++;
	}
	*ptr2 = 0;
	return(tmp);

}

void
lowercase(unsigned char *target, unsigned char *input)
{
	while (*input) {
		if (*input & 0x80) {
			switch(*input) {

			case 0xc0: case 0xc1: case 0xc2: case 0xc3: case 0xc4: case 0xc5: 
			case 0xe0: case 0xe1: case 0xe2: case 0xe3: case 0xe4: case 0xe5: 
				*target = 'a';
				break;

			case 0xc8: case 0xc9: case 0xca: case 0xcb:
			case 0xe8: case 0xe9: case 0xea: case 0xeb:
				*target = 'e';
				break;

			case 0xcc: case 0xcd: case 0xce: case 0xcf:
			case 0xec: case 0xed: case 0xee: case 0xef:
				*target = 'i';
				break;

			case 0xd2: case 0xd3: case 0xd4: case 0xd5: case 0xd6:
			case 0xf2: case 0xf3: case 0xf4: case 0xf5: case 0xf6:
				*target = 'o';
				break;

			case 0xd9: case 0xda: case 0xdb: case 0xdc:
			case 0xf9: case 0xfa: case 0xfb: case 0xfc:
				*target = 'u';
				break;

			default:
				*target = *input;
				break;
			}
		} else {
			*target = tolower(*input);
		}
		target++;
		input++;
	}
	*target=0;
}

void
separate(auset_t *input)
{
	auset_t	*tmp;
	char	*ptr1;
	char	*ptr2;
	char	tmpname[256];

	tmp = input;
	while ( tmp ) {
		if ( strstr(tmp->au_actual, "^") ) {
			ptr1 = (char *)strstr(tmp->au_actual, "^");
			*ptr1 = 0;
			ptr1++;
			if (tmp->au_pseudo) {
				free(tmp->au_pseudo);
			}
			tmp->au_pseudo = (char *)malloc( strlen(ptr1) + 1);
			strcpy(tmp->au_pseudo, ptr1);

			/*
			 * Strip parenthesis from the pseudonym
			 */
			ptr1 = tmp->au_pseudo;
			ptr2 = tmpname;
			while ( *ptr1 ) {
				if ( (*ptr1 != '(') && (*ptr1 != ')') ) {
					*ptr2 = *ptr1;
					ptr2++;
				}
				ptr1++;
			}
			*ptr2 = 0;
			strcpy(tmp->au_pseudo, tmpname);
		} else {
			if (tmp->au_pseudo == NULL) {
				tmp->au_pseudo = (char *)malloc(2);
			}
			tmp->au_pseudo[0] = 0;
		}

		/*
		 * Strip parenthesis from the actual authors
		 */
		ptr1 = tmp->au_actual;
		ptr2 = tmpname;
		while ( *ptr1 ) {
			if ( (*ptr1 != '(') && (*ptr1 != ')') ) {
				*ptr2 = *ptr1;
				ptr2++;
			}
			ptr1++;
		}
		*ptr2 = 0;
		strcpy(tmp->au_actual, tmpname);
		tmp = tmp->au_next;
	}
}


/*
 * The decompose() function decomposes a pseudonym "equation" 
 * into individual terms. Pseudonym calculus is a pretty 
 * weird concept, but to put it plainly, decompose() takes 
 * an equation like:
 *
 *    (a^b)+c+(d^f)+e
 *
 * And breaks it into individual author sets:
 *
 *    [(a^b)]  [c]  [(d^f)]  [e]
 *
 * While the separate() function then turns it into:
 *
 *      a     c     d     e    <- actual authors
 *     --- + --- + --- + ---
 *      b           f          <- pseudonyms
 *
 * The authors (as published) can be found by removing the
 * elements to the left of each ^ sign:
 *
 * (Neal Stephenson+George Jewsbury)^Stephen Bury = 
 *	    Stephen Bury
 *
 * (Alexei Panshin^Louis J. A. Adams)+Joe L. Hensley = 
 *          Louis J. A. Adams + Joe L. Hensley
 *
 * The actual authors can be found by removing the
 * elements to the right of each ^ sign:
 *
 * (Neal Stephenson+George Jewsbury)^Stephen Bury = 
 *	    Neal Stephenson + George Jewsbury
 *
 * (Alexei Panshin^Louis J. A. Adams)+Joe L. Hensley = 
 *          Alexei Panshin + Joe L. Hensley
 */
auset_t *
decompose(char *string)
{
	char	*ps;
	char	*pe;
	int	inparen;
	auset_t	*tmp = NULL;
	auset_t	*tmp2;

	if (string == NULL) {
		return(NULL);
	}

	ps = string;
	pe = string;
	inparen = 0;
	while(1) {
		if (*pe == 0) {
			tmp2 = (auset_t *)malloc( sizeof(auset_t) );
			tmp2->au_actual = (char *)malloc( strlen(ps) + 1);
			tmp2->au_pseudo = NULL;
			strcpy(tmp2->au_actual, ps);
			tmp2->au_next = tmp;
			tmp = tmp2;
			return(tmp);
		} else if (*pe == '(') {
			inparen++;
			pe++;
		} else if (*pe == ')') {
			inparen--;
			pe++;
		} else if ((*pe == '+') && !inparen) {
			*pe = 0;
			tmp2 = (auset_t *)malloc( sizeof(auset_t) );
			tmp2->au_actual = (char *)malloc( strlen(ps) + 1);
			tmp2->au_pseudo = NULL;
			strcpy(tmp2->au_actual, ps);
			tmp2->au_next = tmp;
			tmp = tmp2;
			pe++;
			ps = pe;
		} else {
			pe++;
		}
	}
}


/*
 * This function is identical to strlen(), but doesn't
 * include embedded html directives. (Calculates the
 * screen length of an html string).
 */
int
htmlstrlen(char *string)
{
	int	len = 0;
	char	*tmp;

	tmp = string;
	while(*tmp) {
		if (*tmp == '<') {
			while(*tmp && (*tmp != '>'))
				tmp++;
			tmp++;
		} else {
			len++;
			tmp++;
		}
	}
	return(len);
}


int
posit_check(int position, int rightmargin, int indent)
{
	int loop;

#ifdef DEBUG
	printf("[%d,%d]", position, rightmargin);
#endif

	if ( position > rightmargin) {
		printf("\n");
		for(loop=0; loop<indent; loop++)
			printf(" ");
		printf("  ");
		position = indent + 2;
	} 
	return(position);
}


void
read_and_printwrap(int filetype, char *coffset, int position, int rightmargin, int indent)
{
	FILE	*fp;
	int	loop;
	int	offset;
	int	result;
	int	skipping = 0;
	int	doing_italics = 0;
	char	input;
	char	*link, *tmp;
	char	*hotspot;
	unsigned char percent = 0x25;

	fp = fopen("notes", "rb");
	if ( fp != NULL) {

		if (filetype == USE_NOTES) {
			printf(" - ");
			position += 3;
		} else {
			position = 0;
			for(loop=0; loop<(indent+2); loop++) {
				printf(" ");
				position++;
			}
		}

		sscanf(coffset, "%x", &offset);
		result = fseek(fp, offset, SEEK_SET);
		if (result < 0) {
			return;
		}
		for(loop=0; loop<6; loop++)
			input = getc(fp);

		input = getc(fp);
		while( input != '|' ) {

			/*
			 * Check for hotlink directives
			 */
			if (input == '%') {
				char next;

				next = getc(fp);
				if (next == '%') {
					int posit=0;
					while(1) {
						hotlink[posit] = getc(fp);
						if ( (hotlink[posit] == '%') &&
						     (hotlink[posit-1] == '%')) {
							hotlink[posit-1] = 0;
							break;
						}
						posit++;
						if (posit > 1023) {
							printf("\n<h1>Lord Help Me - Internal Error!</h1>\n");
							return;
						}
					}

					link = (char *)&hotlink[2];
					switch( hotlink[0] ) {

					case 'A':
						position = html_print1_author(link,
							0, position, rightmargin, indent);
						break;

					case 'F':
						printf("<a href=\"http://us.imdb.com/M/title-exact?");
						tmp = link;
						while (*tmp) {
							if (*tmp == ' ') {
								fwrite(&percent, 1, 1, stdout);
								printf("20");
							} else {
								printf("%c", *tmp);
							}
							tmp++;
						}
						printf("\">%s</a>", link);
						position += strlen(link);
						break;

					case 'M':
						printf("<br><b>Email:</b> ");
						printf("<a href=\"mailto:%s\">%s</a> ", link, link);
						break;

					case 'H':
						hotspot = link;
						link = (char *)strstr(hotspot, "##");
						*link = 0;
						link += 2;
						printf("<a href=\"%s\">%s</a>", link, hotspot );
						break;

					default:
						printf("[??%s??]", hotlink);
						break;
					}
				} else {
					ungetc(next, fp);
					printf("%c", input);
				}
			} else {
				printf("%c", input);
			}

			/*
			 * If we word wrap during italics, identation will be wrong
			 * since the italics may have a different font width than
			 * the regular font. This code figures out when we are doing
			 * italics.
			 */
			if (input == '<') {
				skipping = 1;
				input = getc(fp);
				if ((input == 'i') || (input == 'I')) {
					doing_italics = 1;
				} else if (input == '/') {
					char closing;

					closing = getc(fp);
					if ((closing == 'i') || (closing == 'I'))
						doing_italics = 0;
					ungetc(closing, fp);
				}
				ungetc(input, fp);
			} else if (input == '>') {
				skipping = 0;
			}

			/*
			 * Don't count the length of html directives
			 * since the don't appear on screen.
			 */
			if (!skipping)
				position++;

			if ( (position > rightmargin) && (input == ' ')) {
				if (doing_italics)
					printf("</i>");
				printf("\n");
				position = 0;
				for(loop=0; loop<(indent+2); loop++) {
					printf(" ");
					position++;
				}
				if (doing_italics)
					printf("<i>");
			}
			input = getc(fp);
		}
		fclose(fp);
	}

	if (filetype == USE_SYNOPSIS) {
		printf("\n");
	}
}


int
printwrap(char *buffer, int position, int rightmargin, int indent)
{
	int	length;
	int	loop;

	length = htmlstrlen(buffer);
	position = posit_check(position+length, rightmargin, indent);
	printf("%s", buffer);
	return( position );
}


int
html_print_award_cite_msg(char *award, char* year, char *msg)
{
	sprintf(tmpbuf, "[<a href=\"http:/%s/AwYear.cgi?%s%s\">%s</a>]",
		HTFAKE, award, year, msg);
	printf("%s", tmpbuf);
	return( htmlstrlen(tmpbuf) );
}


int
html_print_award_cite(char *award, char* year)
{
	sprintf(tmpbuf, "[<a href=\"http:/%s/AwYear.cgi?%s%s\">%s%s</a>]",
		HTFAKE, award, year, award, year);
	printf("%s", tmpbuf);
	return( htmlstrlen(tmpbuf) );
}


int
html_print_nom_cite(char *award, char* year)
{
	sprintf(tmpbuf, "[<a href=\"http:/%s/AwYear.cgi?%s%s\">%s%s n</a>]",
		HTFAKE, award, year, award, year);
	printf("%s", tmpbuf);
	return( htmlstrlen(tmpbuf) );
}


int
html_print_x_cite(char *award, char* year)
{
	sprintf(tmpbuf, "[<a href=\"http:/%s/AwYear.cgi?%s%s\">%s%s x</a>]",
		HTFAKE, award, year, award, year);
	printf("%s", tmpbuf);
	return( htmlstrlen(tmpbuf) );
}

/*
 * html_print1_author() is used to output an author hotlink. It 
 * should be handed and author/pseudo string pair following a
 * decompose/separate operation in the AE field. 
 *
 * o position should be set to the current cursor position.
 * o rightmargin should be set to the offset where wordwrap 
 *   is desired.
 * o ident should be set to the offset where output should
 *   continue following a wordwrap.
 *
 */
int
html_print1_author(
	char	*author,
	char	*pseudo,
	int	position,
	int	rightmargin,
	int	indent)
{
	char	*ptr1;
	char	*ptr2;
	char	*plus1;
	char	*plus2;
	char	underscored_author[256];
	int	needparen = 0;
	int	firsttime = 1;

	/*
	 * Make a copy of the author string, altering all of the spaces
	 * to underscores. This version of the author is used as an
	 * argument to exact_author embedded in the html.
	 */
	strcpy(underscored_author, author);
	ptr1 = underscored_author;
	while (*ptr1) {
		if (*ptr1 == ' ')
			*ptr1 = '_';
		ptr1++;
	}

	/*
	 * If this was a collaboration as a pseudonym, start a
	 * left paren, so we get: (a and b) (as c).
	 */
	ptr1 = author;
	ptr2 = underscored_author;
	if ( strstr(ptr1, "+") && pseudo && pseudo[0]) {
		position = posit_check(position, rightmargin, indent);
		printf("<b>(</b>");
		needparen = 1;
		position++;
	}

	/*
	 * If this was a collaboration, output each author involved
	 * with the work.
	 */
	while (strstr(ptr1, "+")) {

		/*
		 * Get the underscored and non-underscored versions
		 * of the author string in sync.
		 */
		plus1 = (char *)strstr(ptr1, "+");
		plus2 = (char *)strstr(ptr2, "+");
		*plus1 = 0;
		*plus2 = 0;
		plus1++;
		plus2++;

		/*
		 * Output the author hotlink
		 */
		if (firsttime) {
			firsttime = 0;
		} else {
			position = posit_check(position, rightmargin, indent);
		}
		if (dbaseloc) {
			printf("<a href=\"http://DBASELOC/exact_author.cgi?%s\">%s</a>",
				escape_string((unsigned char *)bquote(ptr2)), ptr1);
		} else {
			printf("<a href=\"http:/%s/exact_author.cgi?%s\">%s</a>",
				HTFAKE, escape_string((unsigned char *)bquote(ptr2)), ptr1);
		}
		position += htmlstrlen(ptr1);

		/*
		 * Then the conjunction
		 */
		printf(" <b>and</b> ");
		position += 5;

		ptr1 = plus1;
		ptr2 = plus2;
	}

	/*
	 * Output the last collaborator or the single author name.
	 */
	position = posit_check(position, rightmargin, indent);
	if (dbaseloc) {
		printf("<a href=\"http://DBASELOC/exact_author.cgi?%s\">%s</a>",
			escape_string((unsigned char *)bquote(ptr2)), ptr1);
	} else {
		printf("<a href=\"http:/%s/exact_author.cgi?%s\">%s</a>",
			HTFAKE, escape_string((unsigned char *)bquote(ptr2)), ptr1);
	}
	position += htmlstrlen(ptr1);

	/*
	 * Close with a right paren if needed.
	 */
	if ( needparen ) {
		printf("<b>)</b> ");
		position += 2;
	} else {
		printf(" ");
		position++;
	}

	/*
	 * If a pseudonym was used, then go ahead and display that as well.
	 */
	if ( pseudo && pseudo[0] ) {
		position = posit_check(position, rightmargin, indent);
		printf("(as <i>%s</i>) ", pseudo);
		position += htmlstrlen(pseudo) + 6;
	}
	return(position);
}


void
html_print_authors(char *authors, int rightmargin)
{
	auset_t	*tmp;
	int	loop;

	tmp = (auset_t *)decompose(authors);
	separate(tmp);
	while(tmp) {
		html_print1_author(tmp->au_actual, tmp->au_pseudo, RIGHTMARGIN2, 
			RIGHTMARGIN2, rightmargin);
		if (tmp->au_next) {
			printf(" <b>and</b>\n");
			for(loop=0; loop<rightmargin; loop++)
				printf(" ");
		}
		tmp = tmp->au_next;
	}
	printf("\n");
}

void
html_print_authors_compressed(char *authors, int leftmargin)
{
	auset_t	*tmp;
	int	loop;
	int	position = 0;

	tmp = (auset_t *)decompose(authors);
	separate(tmp);
	while(tmp) {
		position += html_print1_author(tmp->au_actual, tmp->au_pseudo, 0, RIGHTMARGIN2, 4);
		if (tmp->au_next) {
			printf(" <b>and</b> ");
			if (position > RIGHTMARGIN) {
				printf("\n");
				position = 0;
				for(loop=0; loop<leftmargin; loop++) {
					printf(" ");
				}
			}
		}
		tmp = tmp->au_next;
	}
	printf("\n");
}


int
html_print_all_authors(char *authors, int indent, int position, int rightmargin)
{
	auset_t		*tmp;
	auset_t		*tmp2;
	int		loop;
	int		needwith = 1;
	int		needand = 0;
	int		nextlen = 0;

	tmp = (auset_t *)decompose(authors);
	separate(tmp);
	while(tmp) {
		if (needand) {
			position = posit_check(position, rightmargin, indent);
			sprintf(tmpbuf, "<b>and</b> ");
			position = printwrap(tmpbuf, position, rightmargin, indent);
		}
		position += html_print1_author(tmp->au_actual, tmp->au_pseudo, 
			    position, rightmargin, indent);
		needand = 1;
		tmp = tmp->au_next;
	}
	return( position );
}


int
html_print_other_authors(char *authors, char *exact_author, int indent, int position, int rightmargin)
{
	auset_t		*tmp;
	auset_t		*tmp2;
	int		loop;
	int		needwith = 1;
	int		needand = 0;
	int		nextlen = 0;

	tmp = (auset_t *)decompose(authors);
	separate(tmp);
	tmp2 = tmp;
	while(tmp) {
		if( strcmp(tmp->au_actual, exact_author) == 0) {
			if (tmp->au_pseudo[0]) {
				position = posit_check(position, rightmargin, indent);
				sprintf(tmpbuf, "[as <b>%s</b>]", tmp->au_pseudo);
				position = printwrap(tmpbuf, position, rightmargin, indent);
			}
		}
		tmp = tmp->au_next;
	}

	tmp = tmp2;
	while(tmp) {
		if( strcmp(tmp->au_actual, exact_author) ) {
			if (needand) {
				position = posit_check(position, rightmargin, indent);
				sprintf(tmpbuf, "<b>and</b> ");
				position = printwrap(tmpbuf, position, rightmargin, indent);
			}
			if (needwith) {
				needwith = 0;
				needand = 1;
				position = posit_check(position, rightmargin, indent);
				if (dobuy) {
					sprintf(tmpbuf, " <b>By</b> ");
				} else {
					sprintf(tmpbuf, " <b>with</b> ");
				}
				position = printwrap(tmpbuf, position, rightmargin, indent);
			}

			position = posit_check(position, rightmargin, indent);
			if ( strstr(tmp->au_actual, "+") && strstr(tmp->au_actual, exact_author)) {
				char newactual[80];
				char *auposit;
				char *plusposit;
				char *curposit;
				int  needplus = 0;

				/*
				 * The author has collaborated as a pseudonym.
				 * Strip out the author's name.
				 */
				newactual[0] = 0;
				auposit = (char *)strstr(tmp->au_actual, exact_author);
				curposit = tmp->au_actual;
				plusposit = (char *)strstr(curposit, "+");
				while( plusposit ) {
					*plusposit = 0;
					if ( curposit != auposit ) {
						if (needplus)
							strcat(newactual, "+");
						strcat(newactual, curposit);
						needplus = 1;
					}
					curposit = plusposit+1;
					plusposit = (char *)strstr(curposit, "+");
				}
				if ( curposit != auposit ) {
					if (needplus)
						strcat(newactual, "+");
					strcat(newactual, curposit);
				}
				position += html_print1_author(newactual, tmp->au_pseudo, 
					    position, rightmargin, indent);
			} else {
				position += html_print1_author(tmp->au_actual, tmp->au_pseudo, 
					    position, rightmargin, indent);
			}
		}
		tmp = tmp->au_next;
	}
	return( position );
}


/*
 * This is the central title printing function. It combines several
 * different printing styles into one function, which explains its
 * needless complexity. It's easier understood if followed using a
 * single option type. Recursion is used to handle variant titles.
 */
int
print_title(int indent, char *title, int rightedge, int options, char *offset)
{
	int	titlelen;
	int	loop;
	int	loop2;
	char	*tmptitle;

	/*
	 * If a variant title is present, then recursively call print_title() on
	 * each subtitle.
	 */
	if (strstr(title, "^")) {
		char	*vptr;
		int	result;

		/*
		 * First: output the original title
		 */
		vptr = (char *)strstr(title, "^");
		*vptr = 0;
		print_title(indent, title, rightedge, options, offset);

		/*
		 * Next: output each variant title, passing along the
		 * T_VARIANT option, so that the title is annotated
		 * as: [vt title]. Separate each title with a newline.
		 */
		title = vptr+1;
		while( vptr = (char *)strstr(title, "^") ) {
			printf("\n");
			*vptr = 0;
			print_title(indent+2, title, rightedge, options|T_VARIANT, offset);
			title = vptr+1;
		}

		/*
		 * Last: output the last variant title. We only care about the
		 * cursor position on the last title, so that's the one that is
		 * returned to the caller.
		 */
		printf("\n");
		result = print_title(indent+2, title, rightedge, options|T_VARIANT, offset);
		return(result);
	}

	/*
	 * options:
	 *
	 * T_PREINDENT = we aren't allowed to prepend any 
	 *               characters to the title.
	 * T_WINNER    = prepend an 'o' to the title.
	 * 
	 * otherwise pad the title with the spaces indicated
	 * by ident.
	 */
	titlelen = strlen(title) + indent + 2;
	tmptitle = title;
	if (! (options & T_PREINDENT)) {
		if (options & T_WINNER) {
			printf("o");
			for(loop=0; loop<(indent-1); loop++) {
				printf(" ");
			}
		} else {
			for(loop=0; loop<indent; loop++) {
				printf(" ");
			}
		}
	}

	/*
	 * If this is a variant title, start a variant 
	 * annotation. Note that this can be used with
	 * the T_QUOTE or T_NOPAD options, so don't
	 * else it.
	 */
	if ( options & T_VARIANT) {
		if (options & T_PREINDENT) {
			printf("  ");
			titlelen += 2;
		}
		if ( options & T_NOPAD) {
			printf("[<b>vt</b> ");
		} else {
			printf("[vt ");
		}
		titlelen += 4;
	}

	if (_seriesnum && _seriesnum[0]) {
		printf("%s ", _seriesnum);
		_seriesnum = NULL;
	}

	/*
	 * The T_NOPAD is used to italize novels, while
	 * the T_QUOTE is used to quote short fiction.
	 */
	if ( options & T_QUOTE) {
		printf("\"");
	} else if ( options & T_NOPAD) {
		printf("<i>");
	}


again:
	if (titlelen > rightedge) {

		/*
		 * If we know in advance that the title is going to
		 * spill off the right margin, then go ahead and
		 * calculate where to break the title.
		 */
		loop = rightedge - indent - 2;
		while(loop) {
			if ( tmptitle[loop] == ' ') {
				break;
			} else {
				loop--;
			}
		}

		if (loop) {

			/*
			 * We found a break point. If an offset is present,
			 * then make the title a hotlink to a publication
			 * citation.
			 */
			if (offset) {
				printf("<a href=\"http:/%s/pwork.cgi?%s\">",
					HTFAKE, offset );
			}

			/*
			 * Output the characters up to the title
			 * break point.
			 */
			for(loop2=0; loop2<loop; loop2++) {
				printf("%c", tmptitle[loop2]);
			}

			/*
			 * If an offset is present, then finish off the
			 * publication hotlink.
			 */
			if (offset) {
				printf("</a>");
			}

			/*
			 * Put a kabosh an the italics, and generate a newline.
			 * (The space character is the wrong width if italicized)
			 */
			if ( (options & T_NOPAD) && !(options & T_QUOTE) ) {
				printf("</i>");
			}
			printf("\n");

			/*
			 * Now ident the appropriate amount. Move in 2
			 * more spaces to denote a wrap.
			 */
			for(loop2=0; loop2<indent; loop2++) {
				printf(" ");
			}
			printf("  ");


			/*
			 * Start italicizing again
			 */
			if ( (options & T_NOPAD) && !(options & T_QUOTE) ) {
				printf("<i>");
			}

			/*
			 * Figure out where we left off and start 
			 * again.
			 */
			tmptitle = (char *)&(tmptitle[loop+1]);
			titlelen = strlen(tmptitle) + indent + 4;
			goto again;

		} else {

			/*
			 * Sorry, but the title couldn't be broken up.
			 * If an offset is present, then make the title 
			 * a hotlink to a publication citation.
			 */
			if (offset) {
				printf("<a href=\"http:/%s/pwork.cgi?%s\">",
					HTFAKE, offset );
			}
			printf("%s", tmptitle);
			if (offset) {
				printf("</a>");
			}
			printf("\n");

			/*
			 * All we can do is rezero the output cursor.
			 */
			titlelen = 0;
		}
	} else {

		/*
		 * The title (or last remainder of a broken title) 
		 * fits without being broke up.
		 */
		if (offset) {
			printf("<a href=\"http:/%s/pwork.cgi?%s\">",
				HTFAKE, offset );
		}
		printf("%s", tmptitle);
		if (offset) {
			printf("</a>");
		}
		if ( !(options & T_NOPAD) &&
		     !(options & T_VARIANT) ) {
			printf("  ");
		}
	}

	/*
	 * The title is all out now. Finish up the option
	 * handling. First, finish variant title citations.
	 */
	if ( options & T_VARIANT) {
		/*
		 * We can't output any trailing characters 
		 * if T_NOPAD is set, so just return in
		 * that case.
		 */
		if ( options & T_NOPAD) {
			if ( options & T_QUOTE) {
				printf("\"");
			} else {
				printf("</i>");
			}
			printf("]");
			return( titlelen-1);
		} else {
			printf("]");
			printf("  ");
			titlelen++;
			if ( options & T_PREINDENT) {
				for(loop=0; loop<indent; loop++) {
					printf(" ");
				}
			}
		}
	}

	if ( options & T_NOPAD) {
		if ( options & T_QUOTE) {
			printf("\"");
		} else {
			printf("</i>");
		}
		return( titlelen-2 );
	} else {
		while(titlelen < rightedge) {
			printf(" ");
			titlelen++;
		}
	}
}


void
print1_title(int 	indent, 
	     search_t	*entry,
	     char 	*exact_author,
	     int	corrections,
	     int	special,
	     char	*seriesnum)
{
	int		position;
	int		nextlen;
	int		loop;
	int		rightmargin = RIGHTMARGIN2;

	if (seriesnum && seriesnum[0]) {
		_seriesnum = seriesnum;
	}

	/*
	 * TITLE - Output the title portion of the citation. This includes the
	 * displaying of variant titles, if present.
	 */
	if (corrections) {
#ifdef CORRECTIONS_OKAY
		printf("[<a href=\"http:/%s/dataentry.cgi?2,%s\">c</a>]", HTFAKE, entry->se_offset );
		position = print_title(indent-3, entry->se_title, rightmargin, T_NOPAD, NULL);
#else
		position = print_title(indent, entry->se_title, rightmargin, T_NOPAD, NULL);
#endif
	} else if (entry->se_pubs) {
		position = print_title(indent, entry->se_title, rightmargin, T_NOPAD, entry->se_offset);
	} else {
		position = print_title(indent, entry->se_title, rightmargin, T_NOPAD, NULL);
	}
	position = posit_check(position, rightmargin, indent);


	/*
	 * DATE - Output the year of the citation. May require decoding if the
	 * date is forthcoming (9999...), unknown (0), or unpublished (8888).
	 */
	if ( strcmp(entry->se_year, "0") == 0) {
		sprintf(tmpbuf, " (<b>unknown</b>)");
	} else if ( strncmp(entry->se_year, "9999", 4) == 0) {
		if ( strlen(entry->se_year) > 4) {
			sprintf(tmpbuf, " (<b>forthcoming %s</b>)", (char *)&(entry->se_year[4]));
		} else {
			sprintf(tmpbuf, " (<b>forthcoming</b>)");
		}
	} else if ( strncmp(entry->se_year, "8888", 4) == 0) {
		sprintf(tmpbuf, " (<b>unpublished</b>)");
	} else {
		sprintf(tmpbuf, " (<b>%s</b>)", entry->se_year);
	}
	position = printwrap(tmpbuf, position, rightmargin, indent);
	position = posit_check(position, rightmargin, indent);


	/*
	 * Under "special" circumstances, shortfiction and collections may not
	 * be displayed in the appropriate section (as when they wind up in the
	 * series section). Tag these citations as special.
	 */
	if (special) {
		if ( strcmp(entry->se_type, "sf") == 0) {
			printf(" [<b>SF</b>]");
			position += 5;
		} else if ( strcmp(entry->se_type, "c") == 0) {
			printf(" [<b>C</b>]");
			position += 4;
		} else if ( strcmp(entry->se_type, "o") == 0) {
			if ( entry->se_storylen ) {
				printf(" [<b>O");
				printf("%s", entry->se_storylen);
				printf("</b>]");
				position += strlen(entry->se_storylen)+4;
			} else {
				printf(" [<b>O</b>]");
				position += 4;
			}
		} else if ( strcmp(entry->se_type, "ng") == 0) {
			printf(" [<b>NG</b>]");
			position += 5;
		} else if ( strcmp(entry->se_type, "nf") == 0) {
			printf(" [<b>NF</b>]");
			position += 5;
		}
	}
	position = posit_check(position, rightmargin, indent);


	/*
	 * Denote novelizations
	 */
	if ( strcmp(entry->se_type, "n") == 0) {
		if ( entry->se_storylen && 
		     (strcmp(entry->se_storylen, "nvz") == 0)) {
			printf(" [<b>nvz</b>]");
			position += 6;
		} else if ( entry->se_storylen && 
		     (strcmp(entry->se_storylen, "jvn") == 0)) {
			printf(" [<b>jvn</b>]");
			position += 6;
		}
		position = posit_check(position, rightmargin, indent);
	} else if ( strcmp(entry->se_type, "r") == 0) {
		if (entry->se_storylen) {
			printf(" <b>by</b> ");
			position += 4;
			position = html_print_all_authors(entry->se_storylen, indent, position, rightmargin);
		}
	}


	/*
	 * AUTHORS - Spit out the authors attached to the work. This routine
	 * handles all of the pseudonym and collaboration work.
	 */
	position = html_print_other_authors(entry->se_author, exact_author, indent, position, rightmargin);

	/*
	 * TRANSLATORS - Output any translators.
	 */
	if (entry->se_translator) {
		char	*mult_trans;
		char	*target;

		target = entry->se_translator;
		mult_trans = (char *)strstr(entry->se_translator, ";");
		if (mult_trans) {
			*mult_trans = 0;
		}

		while (target) {
			char *field = target;
			char *delim;

			printf("[");
			position++;

			delim = (char *)strstr(field, ",");
			if (delim) {
				*delim = 0;
				printf("<b>%s Translation - </b>", field);
				position += strlen(field) + 15;
				field = ++delim;
			}

			delim = (char *)strstr(field, ",");
			if (delim) {
				*delim = 0;
				printf("<b>%s:</b> ", field);
				position += 6;
				field = ++delim;
				position = html_print1_author(field, NULL, position, rightmargin, indent);
			}

			printf("] ");
			position += 2;

			if (mult_trans) {
				mult_trans++;
				target = mult_trans;
				mult_trans = (char *)strstr(mult_trans, ";");
				if (mult_trans)
					*mult_trans = 0;
			} else {
				break;
			}
		}
	}

	/*
	 * AWARDS - List any awards nominations or wins. Note that since this is the last
	 * item printed in the citation, wordwrap checks should be done before printing,
	 * and not after, to prevent the introduction of blank lines into the output.
	 */
	if (!corrections && entry->se_awards) {
		award_t *tmp;
		int	level;

		tmp = entry->se_awards;
		while(tmp) {
			sscanf(tmp->aw_level, "%d", &level);
			position = posit_check(position, rightmargin, indent);
			if (level == 1) {
				position += html_print_award_cite(tmp->aw_award, tmp->aw_year);
			} else if (level < 70) {
				position += html_print_nom_cite(tmp->aw_award, tmp->aw_year);
			} else {
				position += html_print_x_cite(tmp->aw_award, tmp->aw_year);
			}
			tmp = tmp->aw_next;
		}
	}

	/*
	 * SYNOPSIS - List any synopsis associated with the entry.
	 */
	if (entry->se_synopsis) {
		char underscored_title[256];
		char *ptr1;
		char *ptr2;

		ptr1 = entry->se_title;
		ptr2 = underscored_title;
		while (*ptr1) {
			if (*ptr1 == ' ') {
				*ptr2 = '_';
			} else {
				*ptr2 = *ptr1;
			}
			ptr1++; ptr2++;
		}
		*ptr2 = 0;

		position = posit_check(position, 75, indent);
		printf("[<a href=\"http:/%s/psynop.cgi?%s,%s\"><b>Synopsis</b></a>]",
		HTFAKE, entry->se_synopsis, underscored_title );
		position += 8;
	}

	/*
	 * REVIEWS -
	 */
	if (!corrections && rv_list) {
		review_t *tmp;

		tmp = rv_list;
		while(tmp) {
			if ( strcmp(entry->se_type, "sf") == 0) {
				tmp = tmp->rv_next;
				continue;
			}
			if ( strcmp(entry->se_title, tmp->rv_title) == 0) {
				position = posit_check(position, rightmargin, indent);
				printf("[<a href=\"http:/%s/rwork.cgi?%s\"><b>Reviews</b></a>]", 
					HTFAKE, entry->se_offset );
				break;
			} else if ( strstr(entry->se_title, "^") && strstr(entry->se_title, tmp->rv_title)) {
				position = posit_check(position, rightmargin, indent);
				printf("[<a href=\"http:/%s/rwork.cgi?%s\"><b>Reviews</b></a>]", 
					HTFAKE, entry->se_offset );
				break;
			}
			tmp = tmp->rv_next;
		}
	}

	/*
	 * NOTES - List any notes associated with the entry.
	 */
	if (entry->se_notes) {
		read_and_printwrap(USE_NOTES, entry->se_notes, position, RIGHTMARGIN2, indent);
	}

	printf("\n");

}


void
print_awardname(char *name)
{
	if (strcmp(name, "Hu") == 0) {
		printf("Hugo Award");
	} else if (strcmp(name, "Ne") == 0) {
		printf("Nebula Award");
	} else if (strcmp(name, "Cl") == 0) {
		printf("Arthur C. Clarke Award");
	} else if (strcmp(name, "Ca") == 0) {
		printf("John W. Campbell Memorial Award");
	} else if (strcmp(name, "Cy") == 0) {
		printf("Chesley Award");
	} else if (strcmp(name, "An") == 0) {
		printf("Analog Award");
	} else if (strcmp(name, "Lc") == 0) {
		printf("Locus Poll Award");
	} else if (strcmp(name, "St") == 0) {
		printf("Bram Stoker Award");
	} else if (strcmp(name, "Wf") == 0) {
		printf("World Fantasy Award");
	} else if (strcmp(name, "Bf") == 0) {
		printf("British Fantasy Award");
	} else if (strcmp(name, "Bs") == 0) {
		printf("British Science Fiction Award");
	} else if (strcmp(name, "Ds") == 0) {
		printf("British Science Fiction Award");
	} else if (strcmp(name, "Dt") == 0) {
		printf("Ditmar Award");
	} else if (strcmp(name, "Ax") == 0) {
		printf("Asimov's Undergraduate Award");
	} else if (strcmp(name, "Ar") == 0) {
		printf("Asimov's Readers' Poll");
	} else if (strcmp(name, "As") == 0) {
		printf("Aurealis Award");
	} else if (strcmp(name, "Bl") == 0) {
		printf("Balrog Award");
	} else if (strcmp(name, "Ga") == 0) {
		printf("Gandalf Award");
	} else if (strcmp(name, "Pk") == 0) {
		printf("Philip K. Dick Award");
	} else if (strcmp(name, "Tp") == 0) {
		printf("James Tiptree, Jr. Award");
	} else if (strcmp(name, "Lm") == 0) {
		printf("Lambda Award");
	} else if (strcmp(name, "Au") == 0) {
		printf("Aurora Award");
	} else if (strcmp(name, "Hm") == 0) {
		printf("HOMer Award");
	} else if (strcmp(name, "Jc") == 0) {
		printf("John W. Campbell Award");
	} else if (strcmp(name, "My") == 0) {
		printf("Mythopoeic Award");
	} else if (strcmp(name, "Su") == 0) {
		printf("Sturgeon Award");
	} else if (strcmp(name, "Hf") == 0) {
		printf("Hall of Fame Award");
	} else if (strcmp(name, "Pr") == 0) {
		printf("Prometheus Award");
	} else if (strcmp(name, "Sf") == 0) {
		printf("SFBC Award");
	} else if (strcmp(name, "Sc") == 0) {
		printf("SF Chronicle Award");
	} else if (strcmp(name, "Sw") == 0) {
		printf("Sidewise Award");
	} else if (strcmp(name, "Rh") == 0) {
		printf("Retro Hugo Award");
	} else if (strcmp(name, "If") == 0) {
		printf("International Fantasy Award");
	} else if (strcmp(name, "Cc") == 0) {
		printf("Compton Crook Award");
	} else if (strcmp(name, "Sk") == 0) {
		printf("Skylark Award");
	} else if (strcmp(name, "Gg") == 0) {
		printf("Gaughan Award");
	} else if (strcmp(name, "Im") == 0) {
		printf("Imaginaire Award");
	} else if (strcmp(name, "Ih") == 0) {
		printf("Internation Horror Guild Award");
	} else if (strcmp(name, "Dr") == 0) {
		printf("Deathrealm Award");
	} else if (strcmp(name, "Ap") == 0) {
		printf("Apollo Award");
	} else if (strcmp(name, "Ry") == 0) {
		printf("Rhysling Award");
	} else {
		printf("???", name);
	}
}

int
print_full_awardname(char *name)
{
	if (strcmp(name, "Hu") == 0) {
		printf("\n[Hu]  = Hugo Award: ");
		return(20);
	} else if (strcmp(name, "Ne") == 0) {
		printf("\n[Ne]  = Nebula Award: ");
		return(22);
	} else if (strcmp(name, "Cl") == 0) {
		printf("\n[Cl]  = Arthur C. Clarke Award: ");
		return(32);
	} else if (strcmp(name, "Ca") == 0) {
		printf("\n[Ca]  = John W. Campbell Memorial Award: ");
		return(41);
	} else if (strcmp(name, "Lc") == 0) {
		printf("\n[Lc]  = Locus Poll Award: ");
		return(26);
	} else if (strcmp(name, "St") == 0) {
		printf("\n[St]  = Bram Stoker Award: ");
		return(27);
	} else if (strcmp(name, "Wf") == 0) {
		printf("\n[Wf]  = World Fantasy Award: ");
		return(29);
	} else if (strcmp(name, "Bf") == 0) {
		printf("\n[Bf]  = British Fantasy Award: ");
		return(31);
	} else if (strcmp(name, "Bs") == 0) {
		printf("\n[Bs]  = British Science Fiction Award: ");
		return(39);
	} else if (strcmp(name, "Dt") == 0) {
		printf("\n[Dt]  = Ditmar Award: ");
		return(22);
	} else if (strcmp(name, "Ax") == 0) {
		printf("\n[Ax]  = Asimov's Undergraduate Award: ");
		return(38);
	} else if (strcmp(name, "Ar") == 0) {
		printf("\n[Ar]  = Asimov's Readers' Poll: ");
		return(32);
	} else if (strcmp(name, "As") == 0) {
		printf("\n[As]  = Aurealis Award: ");
		return(24);
	} else if (strcmp(name, "Bl") == 0) {
		printf("\n[Bl]  = Balrog Award: ");
		return(22);
	} else if (strcmp(name, "Ga") == 0) {
		printf("\n[Ga]  = Gandalf Award: ");
		return(23);
	} else if (strcmp(name, "Pk") == 0) {
		printf("\n[Pk]  = Philip K. Dick Award: ");
		return(30);
	} else if (strcmp(name, "Au") == 0) {
		printf("\n[Au]  = Aurora Award: ");
		return(22);
	} else if (strcmp(name, "Tp") == 0) {
		printf("\n[Tp]  = James Tiptree, Jr. Award: ");
		return(34);
	} else if (strcmp(name, "Lm") == 0) {
		printf("\n[Lm]  = Lambda Award: ");
		return(22);
	} else if (strcmp(name, "Hm") == 0) {
		printf("\n[Hm]  = HOMer Award: ");
		return(21);
	} else if (strcmp(name, "Jc") == 0) {
		printf("\n[Jc]  = John W. Campbell Award: ");
		return(32);
	} else if (strcmp(name, "My") == 0) {
		printf("\n[My]  = Mythopoeic Award: ");
		return(26);
	} else if (strcmp(name, "Su") == 0) {
		printf("\n[Su]  = Sturgeon Award: ");
		return(24);
	} else if (strcmp(name, "Hf") == 0) {
		printf("\n[Hf]  = Hall of Fame Award: ");
		return(28);
	} else if (strcmp(name, "Pr") == 0) {
		printf("\n[Pr]  = Prometheus Award: ");
		return(26);
	} else if (strcmp(name, "Sf") == 0) {
		printf("\n[Sf]  = SFBC Award: ");
		return(20);
	} else if (strcmp(name, "Sc") == 0) {
		printf("\n[Sc]  = SF Chronicle Award: ");
		return(28);
	} else if (strcmp(name, "Rh") == 0) {
		printf("\n[Rh]  = Retro Hugo Award: ");
		return(26);
	} else if (strcmp(name, "Sw") == 0) {
		printf("\n[Sw]  = Sidewise Award: ");
		return(24);
	} else if (strcmp(name, "If") == 0) {
		printf("\n[If]  = International Fantasy Award: ");
		return(37);
	} else if (strcmp(name, "Cc") == 0) {
		printf("\n[Cc]  = Compton Crook Award: ");
		return(32);
	} else if (strcmp(name, "Sk") == 0) {
		printf("\n[Sk]  = Skylark Award: ");
		return(23);
	} else if (strcmp(name, "Gg") == 0) {
		printf("\n[Gg]  = Gaughan Award: ");
		return(23);
	} else if (strcmp(name, "Im") == 0) {
		printf("\n[Im]  = Imaginaire Award: ");
		return(26);
	} else if (strcmp(name, "Ih") == 0) {
		printf("\n[Ih]  = Internation Horror Guild Award: ");
		return(40);
	} else if (strcmp(name, "Dr") == 0) {
		printf("\n[Dr]  = Deathrealm Award: ");
		return(26);
	} else if (strcmp(name, "Ap") == 0) {
		printf("\n[Ap]  = Apollo Award: ");
		return(22);
	} else if (strcmp(name, "Ry") == 0) {
		printf("\n[Ry]  = Rhysling Award: ");
		return(24);
	} else if (strcmp(name, "Cy") == 0) {
		printf("\n[Cy]  = Chesley Award: ");
		return(23);
	} else if (strcmp(name, "An") == 0) {
		printf("\n[An]  = Analog Award: ");
		return(22);
	} else {
		printf("\n[%s]  = ???: ", name);
		return(13);
	}
}


void
print_misc_awards(char *awtype, char *title)
{
	award_t		*awtmp;
	char		current_award[32];
	char		current_year[8];
	int		distance = 0;
	int		foundaward = 1;
	int		need_title = 1;
	int		need_awname = 0;

	while( foundaward ) {
		foundaward = 0;
		current_award[0] = 0;
		strcpy(current_year, "0");
		awtmp = misc_awards;
		while (awtmp) {
			if (awtmp->aw_marker) {
				awtmp = awtmp->aw_next;
				continue;
			}
			if ( current_award[0] == 0) {
				strcpy(current_award, awtmp->aw_award);
				need_awname = 1;
			}
			if ( awtype && strcmp(awtmp->aw_types, awtype)) {
				awtmp = awtmp->aw_next;
				continue;
			}
			if ( strcmp(current_award, awtmp->aw_award) == 0) {
				if (strcmp(awtmp->aw_level, "1") == 0) {
					strcpy(current_year, awtmp->aw_year);
					if (need_title) {
						need_title = 0;
						printf("\n<b>%s</b>", title);
					}
					if (need_awname) {
						need_awname = 0;
						distance = print_full_awardname(awtmp->aw_award);
					}
					printf("[<a href=\"http:/%s/AwYear.cgi?%s%s\">%s</a>]",
						HTFAKE, awtmp->aw_award, 
						awtmp->aw_year, awtmp->aw_year);
					distance += 6;
					if (distance > 75) {
						printf("\n            ");
						distance = 12;
					}
				}
				foundaward = 1;
				awtmp->aw_marker = 1;
			}
			awtmp = awtmp->aw_next;
		}
	}
}


void
print_preamble( search_t *list )
{
	search_t	*tmp;
	award_t		*awtmp;
	char		current_award[32];
	char		current_year[8];
	int		collection = 0;
	int		nonfiction = 0;
	int		anthology = 0;
	int		foundaward = 1;
	int		need_awname = 0;
	int		need_title = 1;
	int		distance = 0;

	while( foundaward ) {
		foundaward = 0;
		tmp = list;
		current_award[0] = 0;
		strcpy(current_year, "0");
		while( tmp ) {
			if ( tmp->se_awards) {
				awtmp = tmp->se_awards;
				while (awtmp) {
					if (awtmp->aw_marker) {
						awtmp = awtmp->aw_next;
						continue;
					}
					if ( current_award[0] == 0) {
						strcpy(current_award, awtmp->aw_award);
						need_awname = 1;
					}
					if ( strcmp(current_award, awtmp->aw_award) == 0) {
						if (strcmp(awtmp->aw_level, "1") == 0) {
							strcpy(current_year, awtmp->aw_year);
							if (need_title) {
								need_title = 0;
								printf("\n<b>Awards:</b>");
							}
							if (need_awname) {
								need_awname = 0;
								distance = print_full_awardname(awtmp->aw_award);
							}
							printf("[<a href=\"http:/%s/AwYear.cgi?%s%s\">%s</a>]",
								HTFAKE, awtmp->aw_award, 
								awtmp->aw_year, awtmp->aw_year);
							distance += 6;
							if (distance > 75) {
								printf("\n            ");
								distance = 12;
							}
						}
						foundaward = 1;
						awtmp->aw_marker = 1;
					}
					awtmp = awtmp->aw_next;
				}
			}
			tmp = tmp->se_next;
		}
	}

	if (!misc_awards) {
		printf("\n");
		return;
	}

	print_misc_awards("ed", "Editing Awards:");
	print_misc_awards("au", "Achievment Awards:");
	print_misc_awards("ar", "Professional Artist Awards:");
	print_misc_awards("aw", "Best Original Art Work:");
	print_misc_awards("fz", "Fan Editing Awards:");
	print_misc_awards("fw", "Fan Writer Awards:");
	print_misc_awards("fa", "Fan Artist Awards:");
	print_misc_awards(NULL, "Awards with No Bibliographic Records:");
	printf("\n");
}


void
print_serials(search_t *list, search_t *target, char *exact_author, int corrections, int indent)
{
	search_t	*serials;
	int		needtitle = 1;

	serials = list;
	while (serials) {
		char *ptr;
		char oldchar;
		int printit = 0;

		if ( serials->se_marker ) {
			serials = serials->se_next;
			continue;
		}
		if ( strcmp(serials->se_type, "se") ) {
			serials = serials->se_next;
			continue;
		}

		ptr = serials->se_title;
		ptr += strlen(serials->se_title) - 1;
		while((ptr != (char *)serials) && (*ptr != '(') ) {
			ptr--;
		}
		ptr--; 
		oldchar = *ptr;
		*ptr = 0;

		if (strcmp(target->se_title, serials->se_title) == 0) {
			printit = 1;
		}
		if (serials->se_storylen && (strcmp(serials->se_storylen, target->se_title) == 0)) {
			printit = 1;
		}

		*ptr = oldchar;
		if (printit) {
			if (needtitle) {
				int loop;

				needtitle = 0;
				for(loop=0; loop<indent+4; loop++)
					printf(" ");
				printf("<b>Magazine Appearances:</b>\n");
			}
			print1_title(indent+4, serials, exact_author, corrections, 0, 0);
			serials->se_marker = 1;
		}

		serials = serials->se_next;
	}
}

void
print_entries(search_t *list, char *exact_author, char *target, char *title, int corrections)
{
	search_t	*tmp;
	int		firsttime = 1;

	tmp = list;
	while( tmp ) {
		if ( tmp->se_marker ) {
			tmp = tmp->se_next;
			continue;
		}
		if ( strcmp(tmp->se_type, target) ) {
			tmp = tmp->se_next;
			continue;
		}
		if ( strstr(tmp->se_author, exact_author) ) {
			if (firsttime) {
				firsttime = 0;
				printf("\n<b>%s</b>\n", title);
			}
			tmp->se_marker = 1;
			print1_title(4, tmp, exact_author, corrections, 0, 0);
			print_serials(list, tmp, exact_author, corrections, 4);
		}
		tmp = tmp->se_next;
	}
}


void
print_pub_entries(search_t *list, char *exact_author, char *target, char *title, int corrections)
{
	search_t	*tmp;
	int		firsttime = 1;

	tmp = list;
	while( tmp ) {
		if ( tmp->se_marker ) {
			tmp = tmp->se_next;
			continue;
		}
		if ( strcmp(tmp->se_type, target) ) {
			tmp = tmp->se_next;
			continue;
		}
		if ( strstr(tmp->se_author, exact_author) ) {
			if (firsttime) {
				firsttime = 0;
				printf("<h2>%s</h2>\n", title);
			}
			tmp->se_marker = 1;
			printf("<i>%s</i>", tmp->se_title);
			if ( (strcmp(tmp->se_type, "a") == 0) && tmp->se_series) {
				printf(" [%s]", tmp->se_series);
			}
			printf(" (%s)\n", tmp->se_year);
			print_pub_citations(tmp->se_pubs, 0);
		}
		tmp = tmp->se_next;
	}
}


/*
 * dotype: 
 *  0 = fiction
 *  1 = anthologies
 *  2 = nonfiction
 *  3 = essays
 */
void
print_series(search_t *list, char *exact_author, int corrections, int dotype )
{
	search_t	*tmp;
	char		*current_series;
	char		*super_series;
	int		foundseries = 1;
	int		noseriesyet = 1;
	int		nosuperseriesyet = 1;
	char		*ptr1;
	char		underscored_series[256];

	super_series = NULL;
	while( foundseries ) {
		foundseries = 0;
		tmp = list;
		current_series = NULL;
		while (tmp) {
			if ( tmp->se_marker ) {
				tmp = tmp->se_next;
				continue;
			}
			if (dotype == 1 ) {
				if ( strcmp(tmp->se_type, "a") ) {
					tmp = tmp->se_next;
					continue;
				}
			} else if (dotype == 2 ) {
				if ( strcmp(tmp->se_type, "nf") ) {
					tmp = tmp->se_next;
					continue;
				}
			} else if (dotype == 3 ) {
				if ( strcmp(tmp->se_type, "es") ) {
					tmp = tmp->se_next;
					continue;
				}
			} else {
				if ( strcmp(tmp->se_type, "a") == 0 ) {
					tmp = tmp->se_next;
					continue;
				}
				if ( strcmp(tmp->se_type, "nf") == 0 ) {
					tmp = tmp->se_next;
					continue;
				}
				if ( strcmp(tmp->se_type, "es") == 0 ) {
					tmp = tmp->se_next;
					continue;
				}
			}

			/*
			 * If the entry has a series, but we aren't tracking one,
			 * start tracking one.
			 */
			if (tmp->se_series && (current_series == NULL)) {
				/*
				 * If we are tracking a superseries, but this
				 * entry doesn't have one, skip it
				 */
				if (super_series && (tmp->se_superseries == NULL)) {
					tmp = tmp->se_next;
					continue;
				}

				/*
				 * If we are tracking a superseries, but this
				 * entry doesn't match, skip it
				 */
				if (super_series && strcmp(tmp->se_superseries, super_series)) {
					tmp = tmp->se_next;
					continue;
				}

				if ((super_series == NULL) && tmp->se_superseries) {
					super_series = tmp->se_superseries;
				}
				current_series = tmp->se_series;
				if (noseriesyet) {
					noseriesyet = 0;
					if (dotype == 1) {
						printf("\n<b>Anthology Series</b>");
					} else if (dotype == 2) {
						printf("\n<b>Non-Fiction Series</b>");
					} else if (dotype == 3) {
						printf("\n<b>Essay/Article Series</b>");
					} else {
						printf("\n<b>Series</b>");
					}
				}
				if (super_series && nosuperseriesyet) {
					nosuperseriesyet = 0;
					printf("\n    %s\n", super_series);
				}

				strcpy(underscored_series, current_series);
				ptr1 = underscored_series;
				while (*ptr1) {
					if (*ptr1 == ' ')
						*ptr1 = '_';
					ptr1++;
				}

				if (super_series) {
					printf("        ");
				} else {
					printf("\n    ");
				}
				printf("<a href=\"http:/%s/pseries.cgi?%s\">%s</a>\n",
					HTFAKE, underscored_series, current_series );

			}

			if (tmp->se_series && strcmp(tmp->se_series, current_series) == 0) {

				/*
				 * Some error checking
				 */
				if( tmp->se_superseries && (super_series == NULL)) {
					printf("\n\n<h1>*** Database Error ***</h1>\n");
					printf("The title: <i>%s</i>\n", tmp->se_title);
					printf("is in the series: <i>%s</i>\n", tmp->se_series);
					printf("and in the superseries: <i>%s</i>\n", tmp->se_superseries);
					printf("But some other title in that series is not contained in that superseries.\n\n");
					tmp = tmp->se_next;
					continue;
				}

				/*
				 * If the entry has a superseries, but it doesn't match the
				 * current one, skip it.
				 */
				if( tmp->se_superseries && strcmp(tmp->se_superseries, super_series)) {
					tmp = tmp->se_next;
					continue;
				}
				if (super_series) {
					print1_title(12, tmp, exact_author,
						corrections, 1, tmp->se_seriesnum);
					print_serials(list, tmp, exact_author, corrections, 12);
				} else {
					print1_title(8, tmp, exact_author,
						corrections, 1, tmp->se_seriesnum);
					print_serials(list, tmp, exact_author, corrections, 8);
				}
				foundseries = 1;
				tmp->se_marker = 1;
			}
			tmp = tmp->se_next;
		}

		/*
		 * If we were tracking a superseries, and have exhausted all
		 * subseries, kill the superseries, and pass through the
		 * list again.
		 */
		if (super_series && (foundseries == 0)) {
			super_series = NULL;
			foundseries = 1;
			nosuperseriesyet = 1;
		}
	}
}


void
print_search_series(search_t *list)
{
	search_t	*tmp;
	char		*ptr1;
	char	underscored_series[256];

	tmp = list;
	while (tmp) {

		if (tmp->se_series) {
			strcpy(underscored_series, tmp->se_series);
			ptr1 = underscored_series;
			while (*ptr1) {
				if (*ptr1 == ' ')
					*ptr1 = '_';
				ptr1++;
			}

			printf("<a href=\"http:/%s/pseries.cgi?%s\">%s</a>\n",
				HTFAKE, underscored_series, tmp->se_series );
		}
		tmp = tmp->se_next;
	}
}


int
getquerieint()
{
	int queries;
	int fd;
        int result;

#ifdef CYGWIN
	fd = open(".queries", O_RDWR|O_BINARY );
#else
	fd = open(".queries", O_RDWR);
#endif
	if ( fd == -1) {
#ifdef CYGWIN
		fd = open(".queries", O_RDWR|O_CREAT|O_BINARY, 0666);
#else
		fd = open(".queries", O_RDWR|O_CREAT, 0666);
#endif
		queries = 0;
	} else {
		result = read(fd, (char *)&queries, 4);
                if (result < 0) {
		        queries = 0;
                }
	}
	close(fd);
	return(queries);
}

void
postamble(char *binary, char *argument)
{
	int		queries;
	int		fd;
	FILE		*fp;
	time_t          ltime;
	struct tm       *ltm;
	char		*host;
        int             result;


#ifdef CYGWIN
	fd = open(".queries", O_RDWR|O_BINARY );
#else
	fd = open(".queries", O_RDWR);
#endif
	if ( fd == -1) {
#ifdef CYGWIN
		fd = open(".queries", O_RDWR|O_CREAT|O_BINARY, 0666);
#else
		fd = open(".queries", O_RDWR|O_CREAT, 0666);
#endif
		queries = 0;
	} else {
		result = read(fd, (char *)&queries, 4);
                if (result < 0) {
		        queries = 0;
                }
	}
	queries++;
	if ( fd != -1) {
		lseek(fd, 0, 0);
		write(fd, (char *)&queries, 4);
		close(fd);
	}

	printf("\n<hr>\n");
	printf("<b>ISFDB Engine</b> - <i>Version 1.12 (08/14/99)</i> - [<i>Queries served</i>: <b>%d</b>]\n", queries);
	printf("Copyright (c) 2001 Al von Ruff\n<hr>");

#ifdef REMOVE
	fp = fopen(".log", "ab");
	if ( fp ) {
		time( &ltime );
		ltm = gmtime( &ltime );
		host = getenv("REMOTE_HOST");
		if (host == NULL) {
			fprintf(fp, "%s [%s]\t%s\n", asctime(ltm), binary, argument);
		} else {
			fprintf(fp, "%s [%s]\t%s\t%s\n", asctime(ltm), host, binary, argument);
		}
		fclose(fp);	
	}
#endif
}

void
print_menus( char *author, search_t *list, int alpha )
{
	int	printpubmenu = 0;
	char	*ptr;

	while( list ) {
		if (list->se_pubs) {
			printpubmenu = 1;
			break;
		}
		list = list->se_next;
	}

	ptr = escape_string((unsigned char *)author);
	author = ptr;

	printf("\n");
	if(printpubmenu) {
		printf("[<a href=\"http:/%s/pbiblio.cgi?%s\">Pub Biblio</a>]", HTFAKE, author );
	}
	if (alpha) {
		printf(" [<a href=\"http:/%s/a_exact_author.cgi?%s\">Alpha</a>]", HTFAKE, author );
	} else {
		printf(" [<a href=\"http:/%s/exact_author.cgi?%s\">Chron</a>]", HTFAKE, author );
	}
	printf(" [<a href=\"http://%s/sfdbase.html\">Main Menu</a>]", HTMLLOC);
	printf(" [<a href=\"http://%s/search.html\">Search</a>]", HTMLLOC);
	printf(" [<a href=\"http:/%s/dataentry.cgi?3,%s\">Correct Author Info</a>]", HTFAKE, author );
#ifdef CORRECTIONS_OKAY
	printf("<br>\n");
	printf(" [<a href=\"http:/%s/cinfo.cgi?%s\">Correct Summary Data</a>]",
		HTFAKE, author );
	printf(" [<a href=\"http:/%s/dataentry.cgi?6,%s\">New Anth/Coll</a>]",
		HTFAKE, author );
	printf(" [<a href=\"http:/%s/dataentry.cgi?1,%s\">New Data</a>]",
		HTFAKE, author );
#endif
#ifdef NEXT
	printf("<hr>\n");
	printf("Add:    ");
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">novel</a>]", HTFAKE, author );
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">coll</a>]", HTFAKE, author );
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">anth</a>]", HTFAKE, author );
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">short</a>]", HTFAKE, author );
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">essay</a>]", HTFAKE, author );
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">nonfic</a>]", HTFAKE, author );
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">nongenre</a>]", HTFAKE, author );
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">poem</a>]", HTFAKE, author );
#endif
#ifdef LATER
	printf("\nChange: ");
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">novel</a>]", HTFAKE, author );
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">coll</a>]", HTFAKE, author );
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">anth</a>]", HTFAKE, author );
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">short</a>]", HTFAKE, author );
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">essay</a>]", HTFAKE, author );
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">nonfic</a>]", HTFAKE, author );
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">nongenre</a>]", HTFAKE, author );
	printf("[<a href=\"http:/%s/dataentry.cgi?3,%s\">poem</a>]", HTFAKE, author );
	printf("<hr></pre>");
#endif
}

void
print_menus2(char *pub )
{
	printf("\n");
	printf(" [<a href=\"http://%s/sfdbase.html\">Main Menu</a>]", HTMLLOC);
	printf(" [<a href=\"http://%s/search.html\">Search</a>]", HTMLLOC);
	printf(" [<a href=\"http:/%s/pinfo.cgi?%s\">Correct Pub</a>]",
		HTFAKE, pub );
	printf(" [<a href=\"http:/%s/pinfo.cgi?%s\">Add Pub</a>]",
		HTFAKE, pub );
	printf("\n");
}

void
print_menus3(char *pub )
{
	printf("\n");
	printf(" [<a href=\"http://%s/sfdbase.html\">Main Menu</a>]", HTMLLOC);
	printf(" [<a href=\"http://%s/search.html\">Search</a>]", HTMLLOC);
#ifdef CORRECTIONS_OKAY
	printf(" [<a href=\"http:/%s/dataentry.cgi?9,%s\">Correct This Pub</a>]",
		HTFAKE, pub );
	printf(" [<a href=\"http:/%s/dataentry.cgi?8,%s\">Add Titles To This Pub</a>]",
		HTFAKE, pub );
#endif
	printf("\n");
}

void
print_submission_notes()
{

	printf("A couple of notes concerning entries on this page:\n");
	printf("<ol>\n");

	printf("<li>The \"^\" operator is used to denote pseudonyms. In plain\n");
	printf("English, it means \"as\". So an entry for Richard Bachman will show\n");
	printf("up as Stephen King^Richard Bachman.\n");

	printf("<li>The \"+\" operator is used to denote collaborations. In plain\n");
	printf("English, it means \"with\". So an entry for Larry Niven working\n");
	printf("with Jerry Pournelle will show up as Larry Niven+Jerry Pournelle.\n");

	printf("<li>Complex pseudonym constructions can be made with the use of\n");
	printf("paranthesis. For instance, the collaborative pseudonym Lewis Padgett\n");
	printf("can be denoted as: (C. L. Moore+Henry Kuttner)^Lewis Padgett\n");

	printf("<li>The year 0 denotes an unknown publication date.\n");
	printf("The year 8888 can be used to denote infamously <i>unpublished</i> books.\n");
	printf("The year 9999 denotes a forthcoming publication date. This entry\n");
	printf("can also take an optional MonYr extension to show the projected\n");
	printf("release date. \"9999Apr96\" denotes a forthcoming book expected\n");
	printf("to be released in Apr 96. Note that the date must be two digits, while\n");
	printf("the 3 character month code must be one of: Jan, Feb, Mar, Apr, May,\n");
	printf("Jun, Jul, Aug, Sep, Oct, Nov, Dec.\n");
	printf("<li>The series field should be left blank if the work does not belong\n");
	printf("to a series, or if the series is unknown.\n");
	printf("<li>The format types are: hc=Hardcover, pb=Paperback, tp=Trade, and ph=Pamphlet.\n");
	printf("</ol>\n");
	printf("<p>\n");
}

int
total_found(search_t *ptr)
{
	int found = 0;

	while(ptr) {
		found++;
		ptr = ptr->se_next;
	}
	return(found);
}

void
print_total_found(search_t *ptr)
{
	int found = 0;

	while(ptr) {
		found++;
		ptr = ptr->se_next;
	}
	if (found == 1) {
		printf("Found <b>%d</b> match\n", found);
	} else {
		printf("Found <b>%d</b> matches\n", found);
	}
}

void
print_totals_found(search_t *ptr)
{
	search_t *tmp;
	int 	 found;

	tmp = ptr;
	found = 0;
	while(tmp) {
		if ( strcmp(tmp->se_type, "n") == 0 )
			found++;
		tmp = tmp->se_next;
	}
	if (found == 1) {
		printf("[<b>%d</b> novel]", found);
	} else {
		printf("[<b>%d</b> novels]", found);
	}

	tmp = ptr;
	found = 0;
	while(tmp) {
		if ( strcmp(tmp->se_type, "sf") == 0 )
			found++;
		tmp = tmp->se_next;
	}
	printf(" [<b>%d</b> shortfiction]", found);

	tmp = ptr;
	found = 0;
	while(tmp) {
		if ( strcmp(tmp->se_type, "p") == 0 )
			found++;
		tmp = tmp->se_next;
	}
	if (found == 1) {
		printf(" [<b>%d</b> poem]", found);
	} else {
		printf(" [<b>%d</b> poems]", found);
	}
}

void
print1_pub_cite(char *citation)
{
	pub_t *tmp;
	int	needcomma = 0;
	int	length = 0;

	tmp = pub_list;
	while(tmp) {
		if ( strcmp(citation, tmp->pu_abbreviation) == 0) {
			printf("<a href=\"http:/%s/plist.cgi?%s\">%s</a>",
				HTFAKE, tmp->pu_abbreviation, tmp->pu_title);
			if(tmp->pu_author[0]) {
				printf(", %s", tmp->pu_author);
			}
			if(tmp->pu_year[0]) {
				if (tmp->pu_year[0] == '0') {
					printf(", (year unknown)");
				} else {
					printf(", %s", tmp->pu_year);
				}
			}
			if(tmp->pu_publisher[0]) {
				printf(", %s", tmp->pu_publisher);
			}
			if(tmp->pu_isbn[0]) {
				printf(", %s", tmp->pu_isbn);
			}
			if(tmp->pu_price[0]) {
				unsigned char pound = 0xa3;

				printf(", ");
				if (tmp->pu_price[0] == 'L') {
					fwrite(&pound, 1, 1, stdout);
					printf("%s", (char *)&(tmp->pu_price[1]));
				} else {
					printf("%s", tmp->pu_price);
				}
			}
			if(tmp->pu_type[0]) {
				printf(", %s", tmp->pu_type);
			}
			break;
		}
		tmp = tmp->pu_next;
	}
}

void
print_pub_citations(char *citations, int correction)
{
	char	*ps;
	char	*pe;
	int	inst = 1;
	pub_t	*tmp1,*tmp2;
	pub_t	*pub_list2 = NULL;

	if (citations == NULL) {
		printf("<br>");
		return;
	}

	ps = citations;
	while(ps) {
		pe = (char *)strstr(ps, ",");
		if (pe) {
			*pe = 0;
		}

		tmp1 = pub_list;
		tmp2 = NULL;
		while(tmp1) {
			if ( strcmp(ps, tmp1->pu_abbreviation) == 0) {
				if (tmp2) {
					tmp2->pu_next = tmp1->pu_next;
					tmp1->pu_next =  pub_list2;
					pub_list2 = tmp1;
					tmp1 = tmp2->pu_next;
				} else {
					pub_list = tmp1->pu_next;
					tmp1->pu_next =  pub_list2;
					pub_list2 = tmp1;
					tmp1 = pub_list;
				}
			} else {
				tmp2 = tmp1;
				tmp1 = tmp1->pu_next;
			}
		}

		if (pe) {
			ps = pe + 1;
		} else {
			ps = NULL;
		}
	}

	/*
	 * Use tmp2 to point to the remnants of the old publication
	 * list, then set pub_list to point at the subset we created.
	 * Then sort the subset.
	 */
	tmp2 = pub_list;
	pub_list = pub_list2;
	sortpub_by_year( &pub_list );

	/*
	 * Print out the publications
	 */
	tmp1 = pub_list;
	printf("<ol>\n");
	while(tmp1) {
#ifdef CORRECTIONS_OKAY
		if (correction) {
#else
		if (0) {
#endif
			printf("<li>[<a href=\"http:/%s/dataentry.cgi?9,%s\">c</a>]", HTFAKE, tmp1->pu_abbreviation );
		} else {
			printf("<li>"); 
		}
		print1_pub_cite( tmp1->pu_abbreviation );
		tmp1 = tmp1->pu_next;
	}
	printf("\n</ol>\n");

	/*
	 * Now glue the old pub list back onto the subset.
	 */
	if (pub_list) {
		tmp1 = pub_list;
		while(tmp1->pu_next) {
			tmp1 = tmp1->pu_next;
		}
		tmp1->pu_next = tmp2;
	} else {
		pub_list = tmp2;
	}
}


void
print_titles_bycat(search_t *list)
{
	search_t	*tmp;
	int		firsttime;
	int		loop;

	for(loop=0; loop<MAXTITLECATS; loop++) {
		firsttime = 1;
		tmp = list;
		while(tmp) {
			if ( strcmp(tmp->se_type, titlecats[loop].shortcat) == 0) {
				if (firsttime) {
					firsttime = 0;
					printf("\n<b>%s</b>\n", titlecats[loop].longcat);
				}
				print_title(0, tmp->se_title, RIGHTMARGIN, T_NOOPT, NULL);
				html_print_authors(tmp->se_author, RIGHTMARGIN);
			}
			tmp = tmp->se_next;
		}
	}
}


void
print_zine_bycat(search_t *list)
{
	search_t	*tmp;
	review_t	*rtmp;
	inter_t		*itmp;
	int		firsttime;
	int		rightmargin = 0;
	int		foundreview;
	int		foundinterview;
	char		current_reviewer[64];

	tmp = list;
	while ( tmp ) {
		if ( strcmp(tmp->se_type, "ar") == 0) {
			tmp = tmp->se_next;
			continue;
		}
		if ( strlen(tmp->se_title) > rightmargin) {
			rightmargin = strlen(tmp->se_title);
		}
		tmp = tmp->se_next;
	}
	rightmargin += 5;
	if (rightmargin > 50) {
		rightmargin = 50;
	}

	/*
	 * Print Serials
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( strcmp(tmp->se_type, "se") == 0) {
			if (firsttime) {
				firsttime = 0;
				printf("\n<b>Serials/Complete Novels</b>\n");
			}
			print_title(0, tmp->se_title, rightmargin, T_NOOPT, NULL);
			html_print_authors(tmp->se_author, rightmargin);
		}
		tmp = tmp->se_next;
	}

	/*
	 * Print Novellas
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( tmp->se_storylen && (strcmp(tmp->se_storylen, "nv") == 0) ) {
			if (firsttime) {
				firsttime = 0;
				printf("\n<b>Novellas</b>\n");
			}
			print_title(0, tmp->se_title, rightmargin, T_NOOPT, NULL);
			html_print_authors(tmp->se_author, rightmargin);
		}
		tmp = tmp->se_next;
	}

	/*
	 * Print Novelettes
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( tmp->se_storylen && (strcmp(tmp->se_storylen, "nt") == 0) ) {
			if (firsttime) {
				firsttime = 0;
				printf("\n<b>Novelettes</b>\n");
			}
			print_title(0, tmp->se_title, rightmargin, T_NOOPT, NULL);
			html_print_authors(tmp->se_author, rightmargin);
		}
		tmp = tmp->se_next;
	}

	/*
	 * Print Short Stories
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( tmp->se_storylen && (strcmp(tmp->se_storylen, "ss") == 0) ) {
			if (firsttime) {
				firsttime = 0;
				printf("\n<b>Short Stories</b>\n");
			}
			print_title(0, tmp->se_title, rightmargin, T_NOOPT, NULL);
			html_print_authors(tmp->se_author, rightmargin);
		}
		tmp = tmp->se_next;
	}

	/*
	 * Print Generic Short Fiction
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( tmp->se_storylen && (strcmp(tmp->se_storylen, "sf") == 0) ) {
			if (firsttime) {
				firsttime = 0;
				printf("\n<b>Short Fiction</b>\n");
			}
			print_title(0, tmp->se_title, rightmargin, T_NOOPT, NULL);
			html_print_authors(tmp->se_author, rightmargin);
		}
		tmp = tmp->se_next;
	}

	/*
	 * Print Essays
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( strcmp(tmp->se_type, "es") == 0) {
			if (firsttime) {
				firsttime = 0;
				printf("\n<b>Essays/Articles</b>\n");
			}
			print_title(0, tmp->se_title, rightmargin, T_NOOPT, NULL);
			html_print_authors(tmp->se_author, rightmargin);
		}
		tmp = tmp->se_next;
	}

	/*
	 * Print Poems
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( strcmp(tmp->se_type, "p") == 0) {
			if (firsttime) {
				firsttime = 0;
				printf("\n<b>Poems</b>\n");
			}
			print_title(0, tmp->se_title, rightmargin, T_NOOPT, NULL);
			html_print_authors(tmp->se_author, rightmargin);
		}
		tmp = tmp->se_next;
	}

	/*
	 * Print Essays
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( strcmp(tmp->se_type, "ai") == 0) {
			if (firsttime) {
				firsttime = 0;
				printf("\n<b>Interior Artwork</b>\n");
			}
			print_title(0, tmp->se_title, rightmargin, T_NOOPT, NULL);
			html_print_authors(tmp->se_author, rightmargin);
		}
		tmp = tmp->se_next;
	}

	/*
	 * Print Reviews
	 */
	foundreview = 1;
	while( foundreview ) {
		foundreview = 0;
		rtmp = rv_list;
		current_reviewer[0] = 0;
		while( rtmp ) {
			if (rtmp->rv_marker) {
				rtmp = rtmp->rv_next;
				continue;
			}
			if ( current_reviewer[0] == 0) {
				strcpy(current_reviewer, rtmp->rv_reviewer);
				dobuy = 1;
				printf("\n<b>Reviews</b>");
				html_print_other_authors(rtmp->rv_reviewer, "xXxX", 0, 0, rightmargin);
				printf("\n");
				dobuy = 0;
				print_title(0, rtmp->rv_title, rightmargin, T_NOOPT, NULL);
				html_print_authors(rtmp->rv_author, rightmargin);
				foundreview = 1;
				rtmp->rv_marker = 1;
			} else if (strcmp(current_reviewer, rtmp->rv_reviewer) == 0) {
				print_title(0, rtmp->rv_title, rightmargin, T_NOOPT, NULL);
				html_print_authors(rtmp->rv_author, rightmargin);
				foundreview = 1;
				rtmp->rv_marker = 1;
			}
			rtmp = rtmp->rv_next;
		}
	}

	/*
	 * Interviews
	 */
	foundinterview = 1;
	while( foundinterview ) {
		foundinterview = 0;
		itmp = it_list;
		current_reviewer[0] = 0;
		while( itmp ) {
			if (itmp->it_marker) {
				itmp = itmp->it_next;
				continue;
			}
			if ( current_reviewer[0] == 0) {
				strcpy(current_reviewer, itmp->it_interviewer);
				dobuy = 1;
				printf("\n<b>Interviews</b>");
				html_print_other_authors(itmp->it_interviewer, "xXxX", 0, 0, rightmargin);
				printf("\n");
				dobuy = 0;
				print_title(0, itmp->it_title, rightmargin, T_NOOPT, NULL);
				html_print_authors(itmp->it_author, rightmargin);
				foundinterview = 1;
				itmp->it_marker = 1;
			} else if (strcmp(current_reviewer, itmp->it_interviewer) == 0) {
				print_title(0, itmp->it_title, rightmargin, T_NOOPT, NULL);
				html_print_authors(itmp->it_author, rightmargin);
				foundinterview = 1;
				itmp->it_marker = 1;
			}
			itmp = itmp->it_next;
		}
	}
}


void
print_fatal(char *msg)
{
	printf("<html><head>\n");
	printf("<title>ISFDB Fatal Error</title></head><body>\n");
	printf("<h1>ISFDB Fatal Error</h1>\n");
	printf("<pre>\n");
	printf("\n%s\n", msg);

	printf("perrno=[%d] ", perrno);
	switch( perrno ) {
	case 0: printf("(No Parse Error)\n");
		break;
	case 1: printf("(Truncated Line)\n");
		printf("Chars parsed=[%d]\n", errcount);
		printf("Field parsed=[%s]\n", pstring);
		break;
	case 2: printf("(Field Too Large)\n");
		printf("Chars parsed=[%d]\n", errcount);
		printf("Field parsed=[%s]\n", pstring);
		break;
	}
	postamble("Fatal Error", "XXX");
	printf("</pre>\n");
	exit(1);
}

/*
 * STRCMP() does a a caseless comparison, so "de Lint" doesn't get
 * alphabetized after "Zelazny"
 */
int
STRCMP(char *str1, char *str2)
{
	char buf1[256];
	char buf2[256];
	char *tmp1;
	char *tmp2;

	tmp1 = str1;
	tmp2 = buf1;
	while (*tmp1) {
		*tmp2 = toupper(*tmp1);
		tmp1++; tmp2++;
	}
	*tmp2 = 0;
	tmp1 = str2;
	tmp2 = buf2;
	while (*tmp1) {
		*tmp2 = toupper(*tmp1);
		tmp1++; tmp2++;
	}
	*tmp2 = 0;
	return(strcmp(buf1, buf2) );
}


void
insert_author(author_t *au)
{
	author_t *au2;

	if (au_head) {
		au2 = au_head;
		while(au2) {
			int strcmp1 = STRCMP(au->au_lastname, au2->au_lastname);
			int strcmp2 = strcmp(au->au_author, au2->au_author);

			if ( ((strcmp1 <  0) && (au2->au_prev)) ||
			     ((strcmp1 == 0) && (strcmp2 < 0) && (au2->au_prev)) ) {
				au->au_prev  = au2->au_prev;
				au2->au_prev = au;
				au->au_prev->au_next = au;
				au->au_next = au2;
				return;
			} else if ( ((strcmp1 <  0) && !(au2->au_prev)) ||
				    ((strcmp1 == 0) && (strcmp2 < 0)  && !(au2->au_prev)) ) {
				au->au_prev  = NULL;
				au2->au_prev = au;
				au->au_next  = au2;
				au_head      = au;
				return;
			}
			au2 = au2->au_next;
		}
		au_end->au_next = au;
		au->au_prev = au_end;
		au->au_next = NULL;
		au_end = au;
	} else {
		au->au_next = NULL;
		au->au_prev = NULL;
		au_head = au;
		au_end  = au;
	}
}

char *
lastname(char *fullname)
{
	char	*tmp;

	tmp = fullname + strlen(fullname) - 1;

#ifdef OLDWAY
	while ((unsigned int)tmp > (unsigned int)fullname) {
#else
	while (tmp > fullname) {
#endif
		if (*tmp == ' ') {

			/*
			 * This section does special handling for lastnames which consist
			 * of multiple words.
			 */
			if ( strncmp(tmp, " Le ", 4) && strstr(fullname, " Le ") ) {
				tmp--;
				continue;
			} else if ( strncmp(tmp, " de ", 4) && strstr(fullname, " de ") ) {
				tmp--;
				continue;
			} else if ( strncmp(tmp, " De ", 4) && strstr(fullname, " De ") ) {
				tmp--;
				continue;
			} else if ( strncmp(tmp, " Di ", 4) && strstr(fullname, " Di ") ) {
				tmp--;
				continue;
			} else if ( strncmp(tmp, " del ", 5) && strstr(fullname, " del ") ) {
				tmp--;
				continue;
			} else if ( strncmp(tmp, " Del ", 5) && strstr(fullname, " Del ") ) {
				tmp--;
				continue;
			} else if ( strncmp(tmp, " Von ", 5) && strstr(fullname, " Von ") ) {
				tmp--;
				continue;
			} else if ( strncmp(tmp, " von ", 5) && strstr(fullname, " von ") ) {
				tmp--;
				continue;
			} else if ( strncmp(tmp, " Van ", 5) && strstr(fullname, " Van ") ) {
				tmp--;
				continue;
			} else if ( strncmp(tmp, " van ", 5) && strstr(fullname, " van ") ) {
				tmp--;
				continue;
			} else if ( strncmp(tmp, " St. ", 5) && strstr(fullname, " St. ") ) {
				tmp--;
				continue;

			/*
			 * This section does special handling for stuff which follows
			 * the last name.
			 */
			} else if (strncmp(tmp, " Jr.", 4) == 0) {
				tmp--;
				continue;
			} else if (strcmp(tmp, " M.D.") == 0) {
				tmp--;
				continue;
			} else if (strcmp(tmp, " Ph.D.") == 0) {
				tmp--;
				continue;
			} else if (strcmp(tmp, " M.S.") == 0) {
				tmp--;
				continue;
			} else if (strcmp(tmp, " II") == 0) {
				tmp--;
				continue;
			} else if (strcmp(tmp, " III") == 0) {
				tmp--;
				continue;
			} else if (strcmp(tmp, " aîné") == 0) {
				tmp--;
				continue;
			} else {
				tmp++;
				break;
			}
		}
		tmp--;
	}
	return(tmp);
}


int
print_zine(char *pub, int needsemi)
{
	int  loop;
	char *tmp;
	char origpub[128];

	strcpy(origpub, pub);
	for(loop=0; loop<MAXZINEABBRS; loop++) {
		if (strncmp(pub, zineabbrs[loop].abbr, strlen(zineabbrs[loop].abbr)) == 0) {
			goto foundit;
		}
	}
	return(0);

foundit:
	if (needsemi) {
		printf("; <i>%s</i>, ", zineabbrs[loop].title);
	} else {
		printf("<i>%s</i>, ", zineabbrs[loop].title);
	}
	pub += strlen(zineabbrs[loop].abbr);

	/*
	 * Drop the year
	 */
	tmp = pub;
	while (*tmp) {
		tmp++;
	}
	tmp -= 2;
	*tmp = 0;

	for(loop=0; loop<MAXDATEABBRS; loop++) {
		if (strncmp(pub, dateabbrs[loop].abbr, strlen(dateabbrs[loop].abbr)) == 0) {
			printf("<a href=\"http:/%s/plist.cgi?%s\">%s</a> ",
				HTFAKE, origpub, dateabbrs[loop].title);
			return(1);
		}
	}

	if ((pub[0] >= '0') && (pub[0] <= '9'))
		printf("#");

	printf("<a href=\"http:/%s/plist.cgi?%s\">%s</a> ", HTFAKE, origpub, pub);
	return(1);
}


int
do_one_pub(char *pub, char *year, int needsemi, int novel)
{
	pub_t	*tmp;
	int	notfound = 1;

	tmp = pub_list;
	while(tmp) {
		if ( strcmp(pub, tmp->pu_abbreviation) == 0) {
			if( strcmp(year, tmp->pu_year) ) {
				return(0);
			} else {
				notfound = 0;
				break;
			}
		}
		tmp = tmp->pu_next;
	}

	if (notfound) {
		return(0);
	}

	if (novel) {
		if (tmp->pu_publisher[0]) {
			if (tmp->pu_type[0]) {
				printf("(%s, %s)", tmp->pu_publisher, tmp->pu_type);
			} else {
				printf("(%s)", tmp->pu_publisher);
			}
			return(1);
		} else {
			return(0);
		}
	}

	if (print_zine(pub, needsemi))
		return(1);

	if (needsemi)
		printf("; ");

	if (tmp->pu_publisher[0]) {
		if (tmp->pu_type[0]) {
			printf("[<a href=\"http:/%s/plist.cgi?%s\">%s</a>] (%s, %s)",
				HTFAKE, tmp->pu_abbreviation, tmp->pu_title,
				tmp->pu_publisher, tmp->pu_type);
		} else {
			printf("[<a href=\"http:/%s/plist.cgi?%s\">%s</a>] (%s)",
				HTFAKE, tmp->pu_abbreviation, tmp->pu_title,
				tmp->pu_publisher );
		}
	} else {
		printf("[<a href=\"http:/%s/plist.cgi?%s\">%s</a>]",
			HTFAKE, tmp->pu_abbreviation, tmp->pu_title);
	}
	return(0);
}

void
print_pubs(char *pubs, char *year, int novel)
{
	char *tmp;
	char lastpub[32];
	int  doit = 1;
	int  needsemi = 0;

	lastpub[0] = 0;
	while(1) {
		tmp = (char *)strstr(pubs, ",");
		if (tmp) {
			*tmp = 0;
			if (lastpub[0]) {
				if (strlen(lastpub) == strlen(pubs)) {
					if ( strncmp(lastpub, pubs, strlen(pubs)-1) == 0) {
						doit = 0;
					}
				}
			}
			strcpy(lastpub, pubs);
			if (doit) {
				needsemi = do_one_pub(pubs, year, needsemi, novel);
			}
			pubs = tmp+1;
		} else {
			if (lastpub[0]) {
				if (strlen(lastpub) == strlen(pubs)) {
					if ( strncmp(lastpub, pubs, strlen(pubs)-1) == 0) {
						doit = 0;
					}
				}
			}
			if (doit) {
				needsemi = do_one_pub(pubs, year, needsemi, novel);
			}
			break;
		}
		doit = 1;
	}
	printf("\n");
}


void
print_titles_author(search_t *list)
{
	search_t	*tmp;
	search_t	*tmp2;
	author_t	*au;

	tmp = list;
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
			au->au_list = NULL;
			au->au_end  = NULL;

			au->au_author = (char *)malloc( strlen(tmp->se_author) + 1);
			strcpy(au->au_author, tmp->se_author);

			ctmp = (char *)lastname(tmp->se_author);
			au->au_lastname = (char *)malloc( strlen(ctmp) + 1);
			strcpy(au->au_lastname, ctmp);

			insert_author(au);

		}
		tmp2 = tmp;
		tmp  = tmp->se_next;

		/*
		 * Put the title on the author's list. Put at the
		 * end of the list to preserve the sorted order.
		 */
		tmp2->se_next = NULL;
		if (au->au_list) {
			au->au_end->se_next = tmp2;
			au->au_end = tmp2;
		} else {
			au->au_end  = tmp2;
			au->au_list = tmp2;
		}
	}

	au = au_head;
	while(au) {
		html_print_authors_compressed(au->au_author, 0);

		/*
		 * Print out the novels
		 */
		tmp = au->au_list;
		while(tmp) {
			if ( strcmp(tmp->se_type, "n") == 0) {
				print_title(4, tmp->se_title, RIGHTMARGIN, T_NOPAD, NULL);
				if (tmp->se_pubs) {
					printf(", ");
					print_pubs(tmp->se_pubs, tmp->se_year, 1);
				} else {
					printf("\n");
				}
			}
			tmp = tmp->se_next;
		}

		/*
		 * Print out the shortfiction
		 */
		tmp = au->au_list;
		while(tmp) {
			if ( strcmp(tmp->se_type, "sf") == 0) {
				print_title(4, tmp->se_title, RIGHTMARGIN, T_NOPAD|T_QUOTE, NULL);
				if (tmp->se_pubs) {
					printf(", ");
					print_pubs(tmp->se_pubs, tmp->se_year, 0);
				} else {
					printf("\n");
				}
			}
			tmp = tmp->se_next;
		}

		/*
		 * Print out the poems
		 */
		tmp = au->au_list;
		while(tmp) {
			if ( strcmp(tmp->se_type, "p") == 0) {
				print_title(4, tmp->se_title, RIGHTMARGIN, T_NOPAD|T_QUOTE, NULL);
				printf(" (poem)");
				if (tmp->se_pubs) {
					printf(", ");
					print_pubs(tmp->se_pubs, tmp->se_year, 0);
				} else {
					printf("\n");
				}
			}
			tmp = tmp->se_next;
		}

		au = au->au_next;
	}
}

char *
load_note(char *coffset)
{
	FILE	*fp;
	int	loop;
	int	offset;
	char	input;
	int	counter;
	int	result;

	fp = fopen("notes", "rb");
	if ( fp != NULL) {

		sscanf(coffset, "%x", &offset);
		result = fseek(fp, offset, SEEK_SET);
		if (result < 0) {
		        return( NULL );
		}
		for(loop=0; loop<6; loop++) {
			input = getc(fp);
		}

		offset = counter = 0;
		input = getc(fp);
		while( input != '|' ) {
			if ( (counter > 40) && ( input == ' ')) {
				tmpbio[offset++] = '\n';
				counter = 0;
				input = getc(fp);
				continue;
			}
			if ( input =='\"') {
				tmpbio[offset++] = '\'';
				tmpbio[offset++] = '\"';
			} else {
				tmpbio[offset++] = input;
			}
			input = getc(fp);
			counter++;
		}
		tmpbio[offset] = 0;
		return( tmpbio );
	} else {
		return( NULL );
	}
}

void
print_zine_bycat2(search_t *list)
{
	search_t	*tmp;
	review_t	*rtmp;
	inter_t		*itmp;
	int		firsttime;
	int		rightmargin = 0;
	int		foundreview;
	int		foundinterview;
	char		current_reviewer[64];

	printf("<td WIDTH=2%></td>\n");
	printf("<td WIDTH=69% VALIGN=Top>\n");

	/*
	 * Print Serials
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( strcmp(tmp->se_type, "se") == 0) {
			if (firsttime) {
				firsttime = 0;
				printf("<table border=0 width=100% VALIGN=Top BGCOLOR=#EEEEEE CELLPADDING=3>\n");
				printf("   <TR><TD colspan=3 ALIGN=left BGCOLOR=\"#FFBBBB\"><font size=3");
				printf(" face=\"Arial, Helvetica\"><B>Serial</B></TD></TR>\n");
				printf("   <TR>\n");
			} else {
				printf("   </TR>\n");
				printf("   <TR>\n");
			}
			printf("   <TD width=29%% valign=top ALIGN=right><FONT SIZE=\"2\" face=\"Arial, Helvetica\"");
			printf(" COLOR=#800000>%s</TD><td width=2%%></td>\n", tmp->se_author);
			printf("   <TD width=69%% valign=top ALIGN=left><FONT SIZE=\"2\" face=\"Arial, Helvetica\"");
			printf(" COLOR=#800000>%s</TD>\n", tmp->se_title);
		}
		tmp = tmp->se_next;
	}
	if (firsttime == 0) {
		printf("   </TR>\n");
		printf("   </table>\n\n");
	}

	/*
	 * Print Novellas
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( tmp->se_storylen && (strcmp(tmp->se_storylen, "nv") == 0) ) {
			if (firsttime) {
				firsttime = 0;
				printf("<table border=0 width=100%% VALIGN=Top BGCOLOR=#EEEEEE CELLPADDING=3>\n");
				printf("   <TR><TD colspan=3 ALIGN=left BGCOLOR=\"#FFBBBB\"><font size=3");
				printf(" face=\"Arial, Helvetica\"><B>Novellas</B></TD></TR>\n");
				printf("   <TR>\n");
			} else {
				printf("   </TR>\n");
				printf("   <TR>\n");
			}
			printf("   <TD width=29%% valign=top ALIGN=right><FONT SIZE=\"2\" face=\"Arial, Helvetica\"");
			printf(" COLOR=#800000>%s</TD><td width=2%%></td>\n", tmp->se_author);
			printf("   <TD width=69%% valign=top ALIGN=left><FONT SIZE=\"2\" face=\"Arial, Helvetica\"");
			printf(" COLOR=#800000>%s</TD>\n", tmp->se_title);
		}
		tmp = tmp->se_next;
	}
	if (firsttime == 0) {
		printf("   </TR>\n");
		printf("   </table>\n\n");
	}

	/*
	 * Print Novelettes
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( tmp->se_storylen && (strcmp(tmp->se_storylen, "nt") == 0) ) {
			if (firsttime) {
				firsttime = 0;
				printf("<table border=0 width=100%% VALIGN=Top BGCOLOR=#EEEEEE CELLPADDING=3>\n");
				printf("   <TR><TD colspan=3 ALIGN=left BGCOLOR=\"#FFBBBB\"><font size=3");
				printf(" face=\"Arial, Helvetica\"><B>Novelettes</B></TD></TR>\n");
				printf("   <TR>\n");
			} else {
				printf("   </TR>\n");
				printf("   <TR>\n");
			}
			printf("   <TD width=29%% valign=top ALIGN=right><FONT SIZE=\"2\" face=\"Arial, Helvetica\"");
			printf(" COLOR=#800000>%s</TD><td width=2%%></td>\n", tmp->se_author);
			printf("   <TD width=69%% valign=top ALIGN=left><FONT SIZE=\"2\" face=\"Arial, Helvetica\"");
			printf(" COLOR=#800000>%s</TD>\n", tmp->se_title);
		}
		tmp = tmp->se_next;
	}
	if (firsttime == 0) {
		printf("   </TR>\n");
		printf("   </table>\n\n");
	}

	/*
	 * Print Short Stories
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( tmp->se_storylen && (strcmp(tmp->se_storylen, "ss") == 0) ) {
			if (firsttime) {
				firsttime = 0;
				printf("<table border=0 width=100%% VALIGN=Top BGCOLOR=#EEEEEE CELLPADDING=3>\n");
				printf("   <TR><TD colspan=3 ALIGN=left BGCOLOR=\"#FFBBBB\"><font size=3");
				printf(" face=\"Arial, Helvetica\"><B>Short Stories</B></TD></TR>\n");
				printf("   <TR>\n");
			} else {
				printf("   </TR>\n");
				printf("   <TR>\n");
			}
			printf("   <TD width=29%% valign=top ALIGN=right><FONT SIZE=\"2\" face=\"Arial, Helvetica\"");
			printf(" COLOR=#800000>%s</TD><td width=2%%></td>\n", tmp->se_author);
			printf("   <TD width=69%% valign=top ALIGN=left><FONT SIZE=\"2\" face=\"Arial, Helvetica\"");
			printf(" COLOR=#800000>%s</TD>\n", tmp->se_title);
		}
		tmp = tmp->se_next;
	}
	if (firsttime == 0) {
		printf("   </TR>\n");
		printf("   </table>\n\n");
	}

	/*
	 * Print Generic Short Fiction
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( tmp->se_storylen && (strcmp(tmp->se_storylen, "sf") == 0) ) {
			if (firsttime) {
				firsttime = 0;
				printf("<table border=0 width=100%% VALIGN=Top BGCOLOR=#EEEEEE CELLPADDING=3>\n");
				printf("   <TR><TD colspan=3 ALIGN=left BGCOLOR=\"#FFBBBB\"><font size=3");
				printf(" face=\"Arial, Helvetica\"><B>Short Fiction</B></TD></TR>\n");
				printf("   <TR>\n");
			} else {
				printf("   </TR>\n");
				printf("   <TR>\n");
			}
			printf("   <TD width=29%% valign=top ALIGN=right><FONT SIZE=\"2\" face=\"Arial, Helvetica\"");
			printf(" COLOR=#800000>%s</TD><td width=2%%></td>\n", tmp->se_author);
			printf("   <TD width=69%% valign=top ALIGN=left><FONT SIZE=\"2\" face=\"Arial, Helvetica\"");
			printf(" COLOR=#800000>%s</TD>\n", tmp->se_title);
		}
		tmp = tmp->se_next;
	}
	if (firsttime == 0) {
		printf("   </TR>\n");
		printf("   </table>\n\n");
	}

	/*
	 * Print Poems
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( strcmp(tmp->se_type, "p") == 0) {
			if (firsttime) {
				firsttime = 0;
				printf("<table border=0 width=100%% VALIGN=Top BGCOLOR=#EEEEEE CELLPADDING=3>\n");
				printf("   <TR><TD colspan=3 ALIGN=left BGCOLOR=\"#FFBBBB\"><font size=3");
				printf(" face=\"Arial, Helvetica\"><B>Poetry</B></TD></TR>\n");
				printf("   <TR>\n");
			} else {
				printf("   </TR>\n");
				printf("   <TR>\n");
			}
			printf("   <TD width=29%% valign=top ALIGN=right><FONT SIZE=\"2\" face=\"Arial, Helvetica\"");
			printf(" COLOR=#800000>%s</TD><td width=2%%></td>\n", tmp->se_author);
			printf("   <TD width=69%% valign=top ALIGN=left><FONT SIZE=\"2\" face=\"Arial, Helvetica\"");
			printf(" COLOR=#800000>%s</TD>\n", tmp->se_title);
		}
		tmp = tmp->se_next;
	}
	if (firsttime == 0) {
		printf("   </TR>\n");
		printf("   </table>\n\n");
	}

	/*
	 * Print Essays
	 */
	firsttime = 1;
	tmp = list;
	while(tmp) {
		if ( strcmp(tmp->se_type, "es") == 0) {
			if (firsttime) {
				firsttime = 0;
				printf("<table border=0 width=100%% VALIGN=Top BGCOLOR=#EEEEEE CELLPADDING=3>\n");
				printf("   <TR><TD colspan=3 ALIGN=left BGCOLOR=\"#FFBBBB\"><font size=3");
				printf(" face=\"Arial, Helvetica\"><B>Departments</B></TD></TR>\n");
				printf("   <TR>\n");
			} else {
				printf("   </TR>\n");
				printf("   <TR>\n");
			}
			printf("   <TD width=29%% valign=top ALIGN=right><FONT SIZE=\"2\" face=\"Arial, Helvetica\"");
			printf(" COLOR=#800000>%s</TD><td width=2%%></td>\n", tmp->se_author);
			printf("   <TD width=69%% valign=top ALIGN=left><FONT SIZE=\"2\" face=\"Arial, Helvetica\"");
			printf(" COLOR=#800000>%s</TD>\n", tmp->se_title);
		}
		tmp = tmp->se_next;
	}
	if (firsttime == 0) {
		printf("   </TR>\n");
		printf("   </table>\n\n");
	}

#ifdef REMOVE
	/*
	 * Interviews
	 */
	foundinterview = 1;
	while( foundinterview ) {
		foundinterview = 0;
		itmp = it_list;
		current_reviewer[0] = 0;
		while( itmp ) {
			if (itmp->it_marker) {
				itmp = itmp->it_next;
				continue;
			}
			if ( current_reviewer[0] == 0) {
				strcpy(current_reviewer, itmp->it_interviewer);
				dobuy = 1;
				printf("\n<b>Interviews</b>");
				html_print_other_authors(itmp->it_interviewer, "xXxX", 0, 0, rightmargin);
				printf("\n");
				dobuy = 0;
				print_title(0, itmp->it_title, rightmargin, T_NOOPT, NULL);
				html_print_authors(itmp->it_author, rightmargin);
				foundinterview = 1;
				itmp->it_marker = 1;
			} else if (strcmp(current_reviewer, itmp->it_interviewer) == 0) {
				print_title(0, itmp->it_title, rightmargin, T_NOOPT, NULL);
				html_print_authors(itmp->it_author, rightmargin);
				foundinterview = 1;
				itmp->it_marker = 1;
			}
			itmp = itmp->it_next;
		}
	}
#endif
}
