/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)dataentry.c	1.7	01/28/98 SFdbase";

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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

static char	argument[256];
static char	twocode[4];
static char	datacode[256];
char		buffer[1030];

#define SELECT_NOVEL		1
#define SELECT_SHORTFICTION	2
#define SELECT_COLLECTION	3
#define SELECT_ANTHOLOGY	4
#define SELECT_NONFICTION	5
#define SELECT_NONGENRE		6
#define SELECT_POEM		7
#define SELECT_ARTWORK		8
#define SELECT_SERIAL		9

search_t        *title_list = NULL;
search_t        *title_end  = NULL;
extern pub_t    *pub_list;

search_t *
parse_work(char *offset)
{
	FILE	*fp;
	int	dummy = 0;
	int	int_offset;
	search_t *set;

	fp = fopen("titles.dbase", "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}


	sscanf(offset, "%x", &int_offset);
	fseek(fp, int_offset, SEEK_SET);
	set = parse_title_entry(fp, (int *)&int_offset, PARSE_NOMALLOC);
	fclose(fp);
	return(set);
}


void
add_title(char *title, char *author, char *year, char *exten, char *series, char *superseries, char *slen)
{
	search_t        *tmp;
	char		*ptr;

	tmp = (search_t *)malloc( sizeof(search_t) );
	if ( tmp == NULL ) {
		perror("out of memory");
		exit(1);
	}

	tmp->se_author   = (char *)malloc( strlen(author) + 1);
	tmp->se_title    = (char *)malloc( strlen(title) + 1);
	tmp->se_year     = (char *)malloc( strlen(year) + 1);
	tmp->se_type     = (char *)malloc( strlen(exten) + 1);

	strcpy(tmp->se_author, author);
	strcpy(tmp->se_title, title);
	strcpy(tmp->se_year, year);
	strcpy(tmp->se_type, exten);

	if (slen) {
		tmp->se_storylen = (char *)malloc( strlen(slen) + 1);
		strcpy(tmp->se_storylen, slen);
	} else {
		tmp->se_storylen = NULL;
	}
	if ( series ) {
		tmp->se_series = (char *)malloc( strlen(series) + 1);
		strcpy(tmp->se_series, series);
	} else {
		tmp->se_series = NULL;
	}
	if ( superseries ) {
		tmp->se_superseries = (char *)malloc( strlen(superseries) + 1);
		strcpy(tmp->se_superseries, superseries);
	} else {
		tmp->se_superseries = NULL;
	}

	if ( title_list == NULL) {
		title_list = title_end = tmp;
	} else {
		title_end->se_next = tmp;
		title_end = tmp;
	}
}


void
parse_works(char *filename, char *pub)
{
	FILE	*fp;
	int	dummy = 0;
	char	*ps;
	char	*pe;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("Couldn't open dbase");
		exit(1);
	}

	while(1) {
		search_t *set;

		set = parse_title_entry(fp, (int *)&dummy, PARSE_NOMALLOC);
		if (set == NULL) {
			break;
		}

		ps = set->se_pubs;
		while(ps) {
			pe = (char *)strstr(ps, ",");
			if (pe) {
				*pe = 0;
			}
			if ( strcmp(ps, pub) == 0) {
				add_title(set->se_title, set->se_author, set->se_year, 
					set->se_type, set->se_series, set->se_superseries, 
					set->se_storylen);
				break;
			}
			if (pe) {
				ps = pe + 1;
			} else {
				ps = NULL;
			}
		}
	}
finish:
	fclose(fp);
}


void
print_options(int selected)
{
	int	loop;

	switch(selected) {
	case SELECT_NOVEL:
		printf("<option selected>Novel\n");
		break;
	case SELECT_SHORTFICTION:
		printf("<option selected>Shortfiction\n");
		break;
	case SELECT_COLLECTION:
		printf("<option selected>Collection\n");
		break;
	case SELECT_ANTHOLOGY:
		printf("<option selected>Anthology\n");
		break;
	case SELECT_NONFICTION:
		printf("<option selected>Nonfiction\n");
		break;
	case SELECT_NONGENRE:
		printf("<option selected>Nongenre\n");
		break;
	case SELECT_POEM:
		printf("<option selected>Poem\n");
		break;
	case SELECT_ARTWORK:
		printf("<option selected>Artwork\n");
		break;
	case SELECT_SERIAL:
		printf("<option selected>Serial\n");
		break;
	}

	for(loop=0; loop<10; loop++) {
		
		if (loop == selected)
			continue;

		switch(loop) {
		case SELECT_NOVEL:
			printf("<option>Novel\n");
			break;
		case SELECT_SHORTFICTION:
			printf("<option>Shortfiction\n");
			break;
		case SELECT_COLLECTION:
			printf("<option>Collection\n");
			break;
		case SELECT_ANTHOLOGY:
			printf("<option>Anthology\n");
			break;
		case SELECT_NONFICTION:
			printf("<option>Nonfiction\n");
			break;
		case SELECT_NONGENRE:
			printf("<option>Nongenre\n");
			break;
		case SELECT_POEM:
			printf("<option>Poem\n");
			break;
		case SELECT_ARTWORK:
			printf("<option>Artwork\n");
			break;
		case SELECT_SERIAL:
			printf("<option>Serial\n");
			break;
		}
	}
}

void
cinfo()
{
	int		option;
	int		index;
	int		result;
	search_t	*tmp;
	search_t	*set;
	audata_t	*auth;
	char		path[256];

	printf("<title>ISFDB Data Correction Form</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");

	printf("<h1>ISFDB Data Correction Form</h1>\n");
	printf("<p>\n");

	printf("This form is used to correct a publication already present\n");
	printf("in the ISFDB. Context-sensitive help is available on a per-entry\n");
	printf("basis by selecting the [H] hotlink which follows each entry label.\n");
	printf("Please look at the help information if you have never entered\n");
	printf("data into the ISFDB before.\n");

	printf("<form method=POST action=\"http:/%s/submit.cgi\">\n", HTFAKE);
	printf("<pre>\n");

	printf("<hr>\n");
	printf("<INPUT NAME=\"XX\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPXX\">HELP</a>] <b>Submitter's email address</b>\n", HTMLLOC);
	printf("<hr>\n");

	set = parse_work( argument );
	printf("<INPUT NAME=\"TI\" SIZE=45 VALUE=\"%s\">", set->se_title);
	printf(" [<a href=\"http://%s/help.html#HELPTI\">HELP</a>] <b>Title</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"AE\" SIZE=45 VALUE=\"%s\">", set->se_author);
	printf(" [<a href=\"http://%s/help.html#HELPAE\">HELP</a>] <b>Author</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"YR\" SIZE=45 VALUE=\"%s\">", set->se_year);
	printf(" [<a href=\"http://%s/help.html#HELPYR\">HELP</a>] <b>Year</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PB\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPB\">HELP</a>] <b>Publisher</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"IS\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPIS\">HELP</a>] <b>ISBN</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PP\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPP\">HELP</a>] <b>Pages</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PR\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPR\">HELP</a>] <b>Price</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"CV\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPCV\">HELP</a>] <b>Cover Artist</b>\n", HTMLLOC);

	if (set->se_series && set->se_series[0]) {
		printf("<INPUT NAME=\"SE\" SIZE=45 VALUE=\"%s\">", set->se_series);
		printf(" [<a href=\"http://%s/help.html#HELPSE\">HELP</a>] <b>Series</b>\n", HTMLLOC);
	} else {
		printf("<INPUT NAME=\"SE\" SIZE=45 >"); 
		printf(" [<a href=\"http://%s/help.html#HELPSE\">HELP</a>] <b>Series</b>\n", HTMLLOC);
	}

	if (set->se_seriesnum && set->se_seriesnum[0]) {
		printf("<INPUT NAME=\"SN\" SIZE=45 VALUE=\"%s\">", set->se_seriesnum);
		printf(" [<a href=\"http://%s/help.html#HELPSN\">HELP</a>] <b>Series Number</b>\n", HTMLLOC);
	} else {
		printf("<INPUT NAME=\"SN\" SIZE=45 >");
		printf(" [<a href=\"http://%s/help.html#HELPSN\">HELP</a>] <b>Series Number</b>\n", HTMLLOC);
	}

	if (set->se_superseries && set->se_superseries[0] ) {
		printf("<INPUT NAME=\"SS\" SIZE=45 VALUE=\"%s\">", set->se_superseries);
		printf(" [<a href=\"http://%s/help.html#HELPSS\">HELP</a>] <b>SuperSeries</b>\n", HTMLLOC);
	} else {
		printf("<INPUT NAME=\"SS\" SIZE=45 >");
		printf(" [<a href=\"http://%s/help.html#HELPSS\">HELP</a>] <b>SuperSeries</b>\n", HTMLLOC);
	}

	printf("<INPUT NAME=\"SL\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPSL\">HELP</a>] <b>Story Length</b>\n", HTMLLOC);

	printf("</pre>\n");

	printf("<select name=\"TYPE\">\n");
	if ( strcmp(set->se_type, "n") == 0) {
		print_options(SELECT_NOVEL);
	} else if ( strcmp(set->se_type, "a") == 0) {
		print_options(SELECT_ANTHOLOGY);
	} else if ( strcmp(set->se_type, "c") == 0) {
		print_options(SELECT_COLLECTION);
	} else if ( strcmp(set->se_type, "sf") == 0) {
		print_options(SELECT_SHORTFICTION);
	} else if ( strcmp(set->se_type, "nf") == 0) {
		print_options(SELECT_NONFICTION);
	} else if ( strcmp(set->se_type, "p") == 0) {
		print_options(SELECT_POEM);
	} else if ( strcmp(set->se_type, "ng") == 0) {
		print_options(SELECT_NONGENRE);
	} else if ( strcmp(set->se_type, "ar") == 0) {
		print_options(SELECT_ARTWORK);
	} else if ( strcmp(set->se_type, "se") == 0) {
		print_options(SELECT_SERIAL);
	}
	printf("</select>\n");

	printf("<select name=\"FORMAT\">\n");
	printf("<option selected>unknown\n");
	printf("<option>hc\n");
	printf("<option>pb\n");
	printf("<option>tp\n");
	printf("<option>ph\n");
	printf("</select> <b>Format</b>");
	printf(" [<a href=\"http://%s/help.html#HELPFORMAT\">HELP</a>]<br>\n", HTMLLOC);

        printf("<p><b>Notes</b> [<a href=\"http://%s/help.html#HELPGN\">HELP</a>]<br>\n", HTMLLOC);
	if(set->se_notes && set->se_notes[0]) {
		char *tmp;

		tmp = (char *)load_note( set->se_notes );
		printf("<TEXTAREA NAME=\"NT\" ROWS=10 COLS=60 VALUE=\"%s\"></TEXTAREA>", tmp);
	} else {
		printf("<TEXTAREA NAME=\"NT\" ROWS=10 COLS=60></TEXTAREA>");
	}

	printf("<br><p><b>Synopsis</b> [<a href=\"http://%s/help.html#HELPGN\">HELP</a>]<br>\n", HTMLLOC);
	if( set->se_synopsis && set->se_synopsis[0] ) {
		char *tmp;

		tmp = (char *)load_note( set->se_synopsis );
		printf("<TEXTAREA NAME=\"SY\" ROWS=10 COLS=60 VALUE=\"%s\"></TEXTAREA>", tmp);
	} else {
		printf("<TEXTAREA NAME=\"SY\" ROWS=10 COLS=60></TEXTAREA>");
	}

	strcpy(twocode,"C1");
}


void
ninfo1()
{
	printf("<title>ISFDB New Data Form</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<h1>ISFDB New Data Form</h1>\n");
	printf("<p>\n");

	printf("This form is used to add a publication not present\n");
	printf("in the ISFDB. Context-sensitive help is available on a per-entry\n");
	printf("basis by selecting the [H] hotlink which follows each entry label.\n");
	printf("Please look at the help information if you have never entered\n");
	printf("data into the ISFDB before.\n");

	printf("<form method=POST action=\"http:/%s/submit.cgi\">\n", HTFAKE);
	printf("<pre>\n");

	printf("<hr>\n");
	printf("<INPUT NAME=\"XX\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPXX\">HELP</a>] <b>Submitter's email address</b>\n", HTMLLOC);
	printf("<hr>\n");

	printf("<INPUT NAME=\"TI\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPTI\">HELP</a>] <b>Title</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"AE\" SIZE=45 VALUE=\"%s\">", argument );
	printf(" [<a href=\"http://%s/help.html#HELPAE\">HELP</a>] <b>Author</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"YR\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPYR\">HELP</a>] <b>Year</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PB\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPB\">HELP</a>] <b>Publisher</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"IS\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPIS\">HELP</a>] <b>ISBN</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PP\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPP\">HELP</a>] <b>Pages</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PR\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPR\">HELP</a>] <b>Price</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"CV\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPCV\">HELP</a>] <b>Cover Artist</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"SE\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPSE\">HELP</a>] <b>Series</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"SN\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPSN\">HELP</a>] <b>Series Number</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"SS\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPSS\">HELP</a>] <b>Super Series</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"SL\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPSL\">HELP</a>] <b>Story Length</b>\n", HTMLLOC);

	printf("</pre>\n");

	printf("The type selector below applies to the title you entered, <b>not</b> to some\n");
	printf("other book that the title might have appeared in.\n");
	printf("<p>\n");
	printf("<b>NOTICES: </b>");
	printf("<ol>\n");
	printf("<li>Only use the following types on this page: <i>Novel</i>, <i>Collection</i>, <i>Anthology</i>,\n");
	printf("<i>Nonfiction</i>, and <i>Nongenre</i>. The ISFDB is <b>not</b> accepting loose bits of\n");
	printf("data such as <i>Shortfiction</i>, <i>Poems</i>, <i>Artwork</i>, and <i>Serials</i>. If you want to enter\n");
	printf("a specific short story, enter the publication it appeared in, as well as all\n");
	printf("of the other stories that appeared in that publication. If you consider this\n");
	printf("to be too much work, then don't enter it. Loose short work data <b>will</b> be discarded.\n");
	printf("<li>Only use the <i>Collection</i> and <i>Anthology</i> types on this page if you do not know\n");
	printf("what works appeared in the collection or anthology. Otherwise go to the author's or\n");
	printf("editor's page and select [New Anth/Coll].\n");


	printf("</ol><br>\n");

	printf("<select name=\"TYPE\">\n");
	print_options(SELECT_NOVEL);
	printf("</select>\n");

	printf("<select name=\"FORMAT\">\n");
	printf("<option selected>unknown\n");
	printf("<option>hc\n");
	printf("<option>pb\n");
	printf("<option>tp\n");
	printf("<option>ph\n");
	printf("</select> <b>Format</b>");
	printf(" [<a href=\"http://%s/help.html#HELPFORMAT\">HELP</a>]<br>\n", HTMLLOC);

	printf("<p><b>Notes</b> [<a href=\"http://%s/help.html#HELPGN\">HELP</a>]<br>\n", HTMLLOC);
	printf("<TEXTAREA NAME=\"NT\" ROWS=10 COLS=60></TEXTAREA>");

	printf("<br><p><b>Synopsis</b> [<a href=\"http://%s/help.html#HELPGN\">HELP</a>]<br>\n", HTMLLOC);
	printf("<TEXTAREA NAME=\"SY\" ROWS=10 COLS=60></TEXTAREA>");
	strcpy(twocode,"N1");
}


void
ninfo2()
{
	printf("<title>ISFDB New Data Form</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<h1>ISFDB New Data Form</h1>\n");
	printf("<p>\n");

	printf("This form is used to add a publication not present\n");
	printf("in the ISFDB. Context-sensitive help is available on a per-entry\n");
	printf("basis by selecting the [H] hotlink which follows each entry label.\n");
	printf("Please look at the help information if you have never entered\n");
	printf("data into the ISFDB before.\n");

	printf("<form method=POST action=\"http:/%s/submit.cgi\">\n", HTFAKE);
	printf("<pre>\n");

	printf("<hr>\n");
	printf("<INPUT NAME=\"XX\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPXX\">HELP</a>] <b>Submitter's email address</b>\n", HTMLLOC);
	printf("<hr>\n");

	printf("<INPUT NAME=\"TI\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPTI\">HELP</a>] <b>Title</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"AE\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPAE\">HELP</a>] <b>Author</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"YR\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPYR\">HELP</a>] <b>Year</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PB\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPB\">HELP</a>] <b>Publisher</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"IS\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPIS\">HELP</a>] <b>ISBN</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PP\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPP\">HELP</a>] <b>Pages</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PR\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPR\">HELP</a>] <b>Price</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"CV\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPCV\">HELP</a>] <b>Cover Artist</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"SE\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPSE\">HELP</a>] <b>Series</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"SN\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPSN\">HELP</a>] <b>Series Number</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"SS\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPSS\">HELP</a>] <b>Super Series</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"SL\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPSL\">HELP</a>] <b>Story Length</b>\n", HTMLLOC);

	printf("</pre>\n");

	printf("<select name=\"TYPE\">\n");
	print_options(SELECT_NOVEL);
	printf("</select>\n");

	printf("<select name=\"FORMAT\">\n");
	printf("<option selected>unknown\n");
	printf("<option>hc\n");
	printf("<option>pb\n");
	printf("<option>tp\n");
	printf("<option>ph\n");
	printf("</select> <b>Format</b>");
	printf(" [<a href=\"http://%s/help.html#HELPFORMAT\">HELP</a>]<br>\n", HTMLLOC);

	printf("<p><b>Notes</b> [<a href=\"http://%s/help.html#HELPGN\">HELP</a>]<br>\n", HTMLLOC);
	printf("<TEXTAREA NAME=\"NT\" ROWS=10 COLS=60></TEXTAREA>");

	printf("<br><p><b>Synopsis</b> [<a href=\"http://%s/help.html#HELPGN\">HELP</a>]<br>\n", HTMLLOC);
	printf("<TEXTAREA NAME=\"SY\" ROWS=10 COLS=60></TEXTAREA>");
	strcpy(twocode,"N2");
}


void
cauthor()
{
	audata_t	*auth;

	printf("<title>ISFDB Author Information Correction Form</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<h1>ISFDB Author Information Correction</h1>\n");
	printf("<p>\n");

	printf("<h1>%s</h1><hr>\n", argument);
	printf("This form is used to correct an author already present\n");
	printf("in the ISFDB. Context-sensitive help is available on a per-entry\n");
	printf("basis by selecting the [H] hotlink which follows each entry label.\n");
	printf("Please look at the help information if you have never entered\n");
	printf("data into the ISFDB before. This form should only be used to update\n");
	printf("an author's biographic information. Never use this form to correct\n");
	printf("a title entry.\n");

	printf("<form method=POST action=\"http:/%s/submit.cgi\">\n", HTFAKE);
	printf("<pre>\n");

	printf("<hr>\n");
	printf("<INPUT NAME=\"XX\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPXX\">HELP</a>] <b>Submitter's email address</b>\n", HTMLLOC);
	printf("<hr>\n");

	auth = (audata_t *)get_author(argument);
	printf("<INPUT NAME=\"AE\" SIZE=45 VALUE=\"%s\">", argument );
	printf(" [<a href=\"http://%s/help.html#HELPWN\">HELP</a>] <b>Working Name</b>\n", HTMLLOC);
	printf("</pre>\n");

	printf("(Do <b>not</b> change the working name listed above. If you are trying to\n");
	printf("change the author of a title entry - then correct that title entry, not the\n");
	printf("author's biographic info. If you feel that the author should have a different\n");
	printf("working name, explain your position in the Notes area below. The working name is\n");
	printf("here so that we know which author you're talking about; if you change the\n");
	printf("working name we won't know which author we should apply changes to.)\n");

	printf("<pre>\n");
	if (auth && auth->au_legalname[0] )
		printf("<INPUT NAME=\"LN\" SIZE=45 VALUE=\"%s\">", auth->au_legalname );
	else
		printf("<INPUT NAME=\"LN\" SIZE=45 >");
	printf(" [<a href=\"http://%s/help.html#HELPLN\">HELP</a>] <b>Legal Name</b>\n", HTMLLOC);

	if (auth && auth->au_birthplace[0] )
		printf("<INPUT NAME=\"BP\" SIZE=45 VALUE=\"%s\">", auth->au_birthplace );
	else
		printf("<INPUT NAME=\"BP\" SIZE=45 >");
	printf(" [<a href=\"http://%s/help.html#HELPBP\">HELP</a>] <b>Birthplace</b>\n", HTMLLOC);

	if (auth && auth->au_birthdate[0] )
		printf("<INPUT NAME=\"BD\" SIZE=45 VALUE=\"%s\">", auth->au_birthdate );
	else
		printf("<INPUT NAME=\"BD\" SIZE=45 >");
	printf(" [<a href=\"http://%s/help.html#HELPBD\">HELP</a>] <b>Birthyear</b>\n", HTMLLOC);

	if (auth && auth->au_deathdate[0] )
		printf("<INPUT NAME=\"DD\" SIZE=45 VALUE=\"%s\">", auth->au_deathdate );
	else
		printf("<INPUT NAME=\"DD\" SIZE=45 >");
	printf(" [<a href=\"http://%s/help.html#HELPDD\">HELP</a>] <b>Deathyear</b>\n", HTMLLOC);

	if (auth && auth->au_pseudonyms[0] )
		printf("<INPUT NAME=\"PS\" SIZE=45 VALUE=\"%s\">", auth->au_pseudonyms );
	else
		printf("<INPUT NAME=\"PS\" SIZE=45 >");
	printf(" [<a href=\"http://%s/help.html#HELPPS\">HELP</a>] <b>Pseudonyms</b>\n", HTMLLOC);

	if (auth && auth->au_email[0] )
		printf("<INPUT NAME=\"EM\" SIZE=45 VALUE=\"%s\">", auth->au_email );
	else
		printf("<INPUT NAME=\"EM\" SIZE=45 >");
	printf(" [<a href=\"http://%s/help.html#HELPEM\">HELP</a>] <b>Email Address</b>\n", HTMLLOC);

	if (auth && auth->au_webpage[0] )
		printf("<INPUT NAME=\"WP\" SIZE=45 VALUE=\"%s\">", auth->au_webpage );
	else
		printf("<INPUT NAME=\"WP\" SIZE=45 >");
	printf(" [<a href=\"http://%s/help.html#HELPWP\">HELP</a>] <b>Web Page</b>\n", HTMLLOC);
	printf("</pre>\n");

	printf("<br>\n");
	printf("<b>Biographic Notes</b> [<a href=\"http://%s/help.html#HELPBN\">HELP</a>]<br>\n", HTMLLOC);

	if(auth && auth->au_biog && auth->au_biog[0] ) {
		char *tmp;

		tmp = (char *)load_note( auth->au_biog );
		printf("<TEXTAREA NAME=\"NT\" ROWS=10 COLS=60 VALUE=\"%s\"></TEXTAREA>", tmp);
	} else {
		printf("<TEXTAREA NAME=\"NT\" ROWS=10 COLS=60></TEXTAREA>");
	}

	strcpy(twocode,"C2");
	strcpy(datacode, argument);
}


void
nauthor()
{
	audata_t	*auth;

	printf("<title>ISFDB New Author Information Form</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<h1>ISFDB New Author Information </h1>\n");
	printf("<p>\n");
	printf("<hr>\n");

	printf("This form is used to enter an author not currently present\n");
	printf("in the ISFDB. Context-sensitive help is available on a per-entry\n");
	printf("basis by selecting the [H] hotlink which follows each entry label.\n");
	printf("Please look at the help information if you have never entered\n");
	printf("data into the ISFDB before.\n");

	printf("<form method=POST action=\"http:/%s/submit.cgi\">\n", HTFAKE);
	printf("<pre>\n");

	printf("<hr>\n");
	printf("<INPUT NAME=\"XX\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPXX\">HELP</a>] <b>Submitter's email address</b>\n", HTMLLOC);
	printf("<hr>\n");

	printf("<INPUT NAME=\"AE\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPWN\">HELP</a>] <b>Working Name</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"LN\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPLN\">HELP</a>] <b>Legal Name</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"BP\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPBP\">HELP</a>] <b>Birthplace</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"BD\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPBD\">HELP</a>] <b>Birthyear</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"DD\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPDD\">HELP</a>] <b>Deathyear</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PS\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPS\">HELP</a>] <b>Pseudonyms</b>\n", HTMLLOC);

	printf("</pre>\n");

	printf("<br>\n");
	printf("<b>Biographic Notes</b> [<a href=\"http://%s/help.html#HELPBN\">HELP</a>]<br>\n", HTMLLOC);

	printf("<TEXTAREA NAME=\"NT\" ROWS=10 COLS=60></TEXTAREA>");
	strcpy(twocode,"N3");
}


void
ncoll(int showauth)
{
	int	loop;
	char	tmpbuf[8];

	printf("<title>ISFDB New Data Form Collection/Anthology</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<h1>ISFDB New Data Form Collection/Anthology</h1>\n");
	printf("<p>\n");

	printf("This form is used to add a collection or anthology not present\n");
	printf("in the ISFDB. Context-sensitive help is available on a per-entry\n");
	printf("basis by selecting the [H] hotlink which follows each entry label.\n");
	printf("Please look at the help information if you have never entered\n");
	printf("data into the ISFDB before.\n");

	printf("<form method=POST action=\"http:/%s/submit.cgi\">\n", HTFAKE);
	printf("<pre>\n");

	printf("<hr>\n");
	printf("<INPUT NAME=\"XX\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPXX\">HELP</a>] <b>Submitter's email address</b>\n", HTMLLOC);
	printf("<hr>\n");

	printf("<INPUT NAME=\"TI\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPTI\">HELP</a>] <b>Title</b>\n", HTMLLOC);

	if (showauth) {
		printf("<INPUT NAME=\"AE\" SIZE=45 VALUE=\"%s\">", argument );
	} else {
		printf("<INPUT NAME=\"AE\" SIZE=45>");
	}
	printf(" [<a href=\"http://%s/help.html#HELPAE\">HELP</a>] <b>Author</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"YR\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPYR\">HELP</a>] <b>Year</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PB\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPB\">HELP</a>] <b>Publisher</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"IS\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPIS\">HELP</a>] <b>ISBN</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PP\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPP\">HELP</a>] <b>Pages</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PR\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPR\">HELP</a>] <b>Price</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"CV\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPCV\">HELP</a>] <b>Cover Artist</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"SE\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPSE\">HELP</a>] <b>Series</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"SN\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPSN\">HELP</a>] <b>Series Number</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"SS\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPSS\">HELP</a>] <b>Super Series</b>\n", HTMLLOC);

	printf("</pre>\n");

	printf("<hr>\n");
	printf("Enter each title in the book. If you are entering a collection\n");
	printf("you don't need to re-enter the author <i>unless</i> the story\n");
	printf("is a collaboration with another author. Enter the copyright\n");
	printf("year of the title only if it appears on the copyright page.\n");
	printf("It should go without saying that authors are required for anthology\n");
	printf("entries. Don't forget to select <i>anthology</i> below, if necessary.\n");
	printf("There is room for 30 entries here. If you need more, submit\n");
	printf("the first 30, return back to this page, and reuse the page to\n");
	printf("enter more titles.\n");
	printf("<pre>\n");
	printf("Title                  Author                 Year\n");
	printf("</pre>\n");

	for(loop=0; loop<30; loop++) {
		printf("<INPUT NAME=\"VT%02d\" SIZE=20>\n", loop);
		printf("<INPUT NAME=\"VA%02d\" SIZE=20>\n", loop);
		printf("<INPUT NAME=\"VY%02d\" SIZE=5>\n", loop);
		printf("<br>\n");
	}

	printf("<select name=\"TYPE\">\n");
	print_options(SELECT_COLLECTION);
	printf("</select>\n");

	printf("<select name=\"FORMAT\">\n");
	printf("<option selected>unknown\n");
	printf("<option>hc\n");
	printf("<option>pb\n");
	printf("<option>tp\n");
	printf("<option>ph\n");
	printf("</select> <b>Format</b>");
	printf(" [<a href=\"http://%s/help.html#HELPFORMAT\">HELP</a>]<br>\n", HTMLLOC);

	printf("<p><b>Notes</b> [<a href=\"http://%s/help.html#HELPGN\">HELP</a>]<br>\n", HTMLLOC);
	printf("<TEXTAREA NAME=\"NT\" ROWS=10 COLS=60></TEXTAREA>");
	strcpy(twocode,"N4");
}

void
nzine()
{
	int	loop;
	char	tmpbuf[8];

	printf("<title>ISFDB New Magazine Data Form</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<h1>ISFDB New Magazine Data Form</h1>\n");
	printf("<p>\n");

	printf("<form method=POST action=\"http:/%s/submit.cgi\">\n", HTFAKE);
	printf("<pre>\n");

	printf("<hr>\n");
	printf("<INPUT NAME=\"XX\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPXX\">HELP</a>] <b>Submitter's email address</b>\n", HTMLLOC);
	printf("<hr>\n");

	printf("<INPUT NAME=\"TI\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPTI\">HELP</a>] <b>Magazine, Issue</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"AE\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPAE\">HELP</a>] <b>Editor</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"YR\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPYR\">HELP</a>] <b>Year</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PP\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPP\">HELP</a>] <b>Pages</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"PR\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPPR\">HELP</a>] <b>Price</b>\n", HTMLLOC);

	printf("<INPUT NAME=\"CV\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPCV\">HELP</a>] <b>Cover Artist</b>\n", HTMLLOC);

	printf("</pre>\n");

	printf("<hr>\n");
	printf("Enter each story or essay in the magazine.\n");
	printf("There is room for 15 entries here. If there are more, submit\n");
	printf("the first 15, return back to this page, and reuse the page to\n");
	printf("enter more titles. <b>Note:</b> Only fill in the year field\n");
	printf("if the title is a reprint from a different year.\n");
	printf("<pre>\n");

	printf("Title                  ");
	printf("Author                 ");
	printf("Type   Year\n");
	printf("[<a href=\"http://%s/help.html#HELPTI\">HELP</a>]                 ", HTMLLOC);
	printf("[<a href=\"http://%s/help.html#HELPAE\">HELP</a>]                 ", HTMLLOC);
	printf("[<a href=\"http://%s/help.html#HELPST\">HELP</a>] ", HTMLLOC);
	printf("[<a href=\"http://%s/help.html#HELPYR\">HELP</a>]\n", HTMLLOC);
	printf("</pre>\n");

	for(loop=0; loop<15; loop++) {
		printf("<INPUT NAME=\"VT%02d\" SIZE=20>\n", loop);
		printf("<INPUT NAME=\"VA%02d\" SIZE=20>\n", loop);
		printf("<INPUT NAME=\"VL%02d\" SIZE=5>\n", loop);
		printf("<INPUT NAME=\"VY%02d\" SIZE=5>\n", loop);
		printf("<br>\n");
	}
	printf("<br><b>Notes</b><br>\n");
	printf("<TEXTAREA NAME=\"NT\" ROWS=5 COLS=60></TEXTAREA>");
	strcpy(twocode,"N5");
}

void
cpub()
{
	int		option;
	int		index;
	int		result;
	search_t	*tmp;
	audata_t	*auth;
	char		path[256];
	pub_t		*ptmp;
	int		counter = 0;

	parse_works("titles.dbase", argument );
	load_pubs(F_EXACT, argument );
	ptmp = pub_list;

	printf("<title>ISFDB Publication Correction Form</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<h1>ISFDB Publication Correction Form</h1>\n");
	printf("<p>\n");

	print_submission_notes();
	printf("<form method=POST action=\"http:/%s/submit.cgi\">\n", HTFAKE);
	printf("<pre>\n");

	printf("<hr>\n");
	printf("<INPUT NAME=\"XX\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPXX\">HELP</a>] <b>Submitter's email address</b>\n", HTMLLOC);
	printf("<hr>\n");

	if (ptmp->pu_title) {
		printf("<INPUT NAME=\"TI\" SIZE=45 VALUE=\"%s\"> <b>Title</b>\n", ptmp->pu_title);
	} else {
		printf("<INPUT NAME=\"TI\" SIZE=45> <b>Title</b>\n");
	}
	if (ptmp->pu_abbreviation) {
		printf("<INPUT NAME=\"AB\" SIZE=45 VALUE=\"%s\"> <b>Pub Tag (Do not change)</b>\n", 
			ptmp->pu_abbreviation);
	}
	if (ptmp->pu_author) {
		printf("<INPUT NAME=\"AE\" SIZE=45 VALUE=\"%s\"> <b>Author</b>\n", ptmp->pu_author);
	} else {
		printf("<INPUT NAME=\"AE\" SIZE=45> <b>Author</b>\n");
	}
	if (ptmp->pu_year) {
		printf("<INPUT NAME=\"YR\" SIZE=45 VALUE=\"%s\"> <b>Year</b>\n", ptmp->pu_year);
	} else {
		printf("<INPUT NAME=\"YR\" SIZE=45> <b>Year</b>\n");
	}
	if (ptmp->pu_isbn) {
		printf("<INPUT NAME=\"IS\" SIZE=45 VALUE=\"%s\"> <b>ISBN</b>\n", ptmp->pu_isbn);
	} else {
		printf("<INPUT NAME=\"IS\" SIZE=45> <b>ISBN</b>\n");
	}
	if (ptmp->pu_publisher) {
		printf("<INPUT NAME=\"PB\" SIZE=45 VALUE=\"%s\"> <b>Publisher</b>\n", ptmp->pu_publisher);
	} else {
		printf("<INPUT NAME=\"PB\" SIZE=45> <b>Publisher</b>\n");
	}
	if (ptmp->pu_price) {
		printf("<INPUT NAME=\"PR\" SIZE=45 VALUE=\"%s\"> <b>Price</b>\n", ptmp->pu_price);
	} else {
		printf("<INPUT NAME=\"PR\" SIZE=45> <b>Price</b>\n");
	}
	if (ptmp->pu_pages) {
		printf("<INPUT NAME=\"PP\" SIZE=45 VALUE=\"%s\"> <b>Pages</b>\n", ptmp->pu_pages);
	} else {
		printf("<INPUT NAME=\"PP\" SIZE=45> <b>Pages</b>\n");
	}
	if (ptmp->pu_cover) {
		printf("<INPUT NAME=\"CV\" SIZE=45 VALUE=\"%s\"> <b>Cover Artist</b>\n", ptmp->pu_cover);
	} else {
		printf("<INPUT NAME=\"CV\" SIZE=45> <b>Cover Artist</b>\n");
	}
	printf("</pre>\n");

	printf("<select name=\"TP\">\n");
	if ( strcmp(ptmp->pu_type, "hc") == 0) {
		printf("<option selected>hc\n");
		printf("<option>pb\n");
		printf("<option>tp\n");
		printf("<option>ph\n");
		printf("<option>unknown\n");
	} else if ( strcmp(ptmp->pu_type, "pb") == 0) {
		printf("<option selected>pb\n");
		printf("<option>hc\n");
		printf("<option>tp\n");
		printf("<option>ph\n");
		printf("<option>unknown\n");
	} else if ( strcmp(ptmp->pu_type, "tp") == 0) {
		printf("<option selected>tp\n");
		printf("<option>hc\n");
		printf("<option>pb\n");
		printf("<option>ph\n");
		printf("<option>unknown\n");
	} else if ( strcmp(ptmp->pu_type, "ph") == 0) {
		printf("<option selected>ph\n");
		printf("<option>hc\n");
		printf("<option>pb\n");
		printf("<option>tp\n");
		printf("<option>unknown\n");
	} else {
		printf("<option selected>unknown\n");
		printf("<option>hc\n");
		printf("<option>pb\n");
		printf("<option>tp\n");
		printf("<option>ph\n");
	}
	printf("</select> <b>Publication Format</b>\n");

	printf("<br><b>Notes</b><br>\n");
	if (ptmp->pu_notes) {
		printf("<TEXTAREA NAME=\"NT\" ROWS=10 COLS=60 VALUE=\"%s\"></TEXTAREA>",  ptmp->pu_notes);
	} else {
		printf("<TEXTAREA NAME=\"NT\" ROWS=10 COLS=60></TEXTAREA>");
	}
	printf("<p>\n");

	printf("<pre>\n");
	printf("<b>Content Listing:</b>\n");
	printf("--------------------------------------------------\n");
	printf("Title                  Author                 Year\n");
	printf("</pre>\n");

	tmp = title_list;
	while(tmp) {
		if ( strcmp(tmp->se_type, "c") == 0 ) {
			counter++;
			tmp = tmp->se_next;
			continue;
		}
		if ( tmp->se_title && strncmp(tmp->se_title, "Cover;", 6) == 0 ) {
			counter++;
			tmp = tmp->se_next;
			continue;
		}
		if (tmp->se_title) {
			printf("<INPUT NAME=\"VT%02d\" SIZE=20 VALUE=\"%s\">\n", counter, tmp->se_title);
		} else {
			printf("<INPUT NAME=\"VT%02d\" SIZE=20>\n", counter);
		}
		if (tmp->se_author) {
			printf("<INPUT NAME=\"VA%02d\" SIZE=20 VALUE=\"%s\">\n", counter, tmp->se_author);
		} else {
			printf("<INPUT NAME=\"VA%02d\" SIZE=20>\n", counter);
		}
		if (tmp->se_year) {
			printf("<INPUT NAME=\"VY%02d\" SIZE=5 VALUE=\"%s\">\n", counter, tmp->se_year);
		} else {
			printf("<INPUT NAME=\"VY%02d\" SIZE=5>\n", counter);
		}
		if (tmp->se_type) {
			printf("<INPUT NAME=\"VL%02d\" SIZE=5 VALUE=\"%s\">\n", counter, tmp->se_type);
		} else {
			printf("<INPUT NAME=\"VL%02d\" SIZE=5>\n", counter);
		}
		printf("<br>\n");
		counter++;
		tmp = tmp->se_next;
	}
	strcpy(twocode,"C3");
}


void
cpub2()
{
	int		option;
	int		index;
	int		result;
	search_t	*tmp;
	audata_t	*auth;
	char		path[256];
	pub_t		*ptmp;
	int		counter = 0;
	int		loop;

	parse_works("titles.dbase", argument );
	load_pubs(F_EXACT, argument );
	ptmp = pub_list;

	printf("<title>ISFDB Add Titles Form</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<h1>ISFDB Add Titles Form</h1>\n");
	printf("<p>\n");

	printf("This form is used to add titles to a publication already present\n");
	printf("in the ISFDB. Context-sensitive help is available on a per-entry\n");
	printf("basis by selecting the [H] hotlink which follows each entry label.\n");
	printf("Please look at the help information if you have never entered\n");
	printf("data into the ISFDB before.\n");

	printf("<form method=POST action=\"http:/%s/submit.cgi\">\n", HTFAKE);
	printf("<pre>\n");

	printf("<hr>\n");
	printf("<INPUT NAME=\"XX\" SIZE=45>");
	printf(" [<a href=\"http://%s/help.html#HELPXX\">HELP</a>] <b>Submitter's email address</b>\n", HTMLLOC);
	printf("<hr>\n");

	if (ptmp->pu_title) {
		printf("<INPUT NAME=\"TI\" SIZE=45 VALUE=\"%s\">", ptmp->pu_title);
		printf(" [<a href=\"http://%s/help.html#HELPTI\">HELP</a>] <b>Title</b>\n", HTMLLOC);
	} else {
		printf("<INPUT NAME=\"TI\" SIZE=45>");
		printf(" [<a href=\"http://%s/help.html#HELPTI\">HELP</a>] <b>Title</b>\n", HTMLLOC);
	}

	if (ptmp->pu_abbreviation) {
		printf("<INPUT NAME=\"AB\" SIZE=45 VALUE=\"%s\"> <b>Pub Tag</b>\n", ptmp->pu_abbreviation);
	}

	if (ptmp->pu_author) {
		printf("<INPUT NAME=\"AE\" SIZE=45 VALUE=\"%s\">", ptmp->pu_author);
		printf(" [<a href=\"http://%s/help.html#HELPAE\">HELP</a>] <b>Author</b>\n", HTMLLOC);
	} else {
		printf("<INPUT NAME=\"AE\" SIZE=45>");
		printf(" [<a href=\"http://%s/help.html#HELPAE\">HELP</a>] <b>Author</b>\n", HTMLLOC);
	}

	if (ptmp->pu_year) {
		printf("<INPUT NAME=\"YR\" SIZE=45 VALUE=\"%s\">", ptmp->pu_year);
		printf(" [<a href=\"http://%s/help.html#HELPYR\">HELP</a>] <b>Year</b>\n", HTMLLOC);
	} else {
		printf("<INPUT NAME=\"YR\" SIZE=45>");
		printf(" [<a href=\"http://%s/help.html#HELPYR\">HELP</a>] <b>Year</b>\n", HTMLLOC);
	}

	if (ptmp->pu_isbn) {
		printf("<INPUT NAME=\"IS\" SIZE=45 VALUE=\"%s\">", ptmp->pu_isbn);
		printf(" [<a href=\"http://%s/help.html#HELPIS\">HELP</a>] <b>ISBN</b>\n", HTMLLOC);
	} else {
		printf("<INPUT NAME=\"IS\" SIZE=45>");
		printf(" [<a href=\"http://%s/help.html#HELPIS\">HELP</a>] <b>ISBN</b>\n", HTMLLOC);
	}

	if (ptmp->pu_publisher) {
		printf("<INPUT NAME=\"PB\" SIZE=45 VALUE=\"%s\">", ptmp->pu_publisher);
		printf(" [<a href=\"http://%s/help.html#HELPPB\">HELP</a>] <b>Publisher</b>\n", HTMLLOC);
	} else {
		printf("<INPUT NAME=\"PB\" SIZE=45>");
		printf("<INPUT NAME=\"PB\" SIZE=45> <b>Publisher</b>\n");
	}

	if (ptmp->pu_price) {
		printf("<INPUT NAME=\"PR\" SIZE=45 VALUE=\"%s\">", ptmp->pu_price);
		printf(" [<a href=\"http://%s/help.html#HELPPR\">HELP</a>] <b>Price</b>\n", HTMLLOC);
	} else {
		printf("<INPUT NAME=\"PR\" SIZE=45>");
		printf(" [<a href=\"http://%s/help.html#HELPPR\">HELP</a>] <b>Price</b>\n", HTMLLOC);
	}

	if (ptmp->pu_pages) {
		printf("<INPUT NAME=\"PP\" SIZE=45 VALUE=\"%s\">", ptmp->pu_pages);
		printf(" [<a href=\"http://%s/help.html#HELPPP\">HELP</a>] <b>Pages</b>\n", HTMLLOC);
	} else {
		printf("<INPUT NAME=\"PP\" SIZE=45>");
		printf(" [<a href=\"http://%s/help.html#HELPPP\">HELP</a>] <b>Pages</b>\n", HTMLLOC);
	}

	if (ptmp->pu_cover) {
		printf("<INPUT NAME=\"CV\" SIZE=45 VALUE=\"%s\">", ptmp->pu_cover);
		printf(" [<a href=\"http://%s/help.html#HELPCV\">HELP</a>] <b>Cover Artist</b>\n", HTMLLOC);
	} else {
		printf("<INPUT NAME=\"CV\" SIZE=45>");
		printf(" [<a href=\"http://%s/help.html#HELPCV\">HELP</a>] <b>Cover Artist</b>\n", HTMLLOC);
	}
	printf("</pre>\n");

	printf("<select name=\"FORMAT\">\n");
	if ( strcmp(ptmp->pu_type, "hc") == 0) {
		printf("<option selected>hc\n");
		printf("<option>pb\n");
		printf("<option>tp\n");
		printf("<option>ph\n");
		printf("<option>unknown\n");
	} else if ( strcmp(ptmp->pu_type, "pb") == 0) {
		printf("<option selected>pb\n");
		printf("<option>hc\n");
		printf("<option>tp\n");
		printf("<option>ph\n");
		printf("<option>unknown\n");
	} else if ( strcmp(ptmp->pu_type, "tp") == 0) {
		printf("<option selected>tp\n");
		printf("<option>hc\n");
		printf("<option>pb\n");
		printf("<option>ph\n");
		printf("<option>unknown\n");
	} else if ( strcmp(ptmp->pu_type, "ph") == 0) {
		printf("<option selected>ph\n");
		printf("<option>hc\n");
		printf("<option>pb\n");
		printf("<option>tp\n");
		printf("<option>unknown\n");
	} else {
		printf("<option selected>unknown\n");
		printf("<option>hc\n");
		printf("<option>pb\n");
		printf("<option>tp\n");
		printf("<option>ph\n");
	}
	printf("</select> <b>Publication Format</b>");
	printf(" [<a href=\"http://%s/help.html#HELPFORMAT\">HELP</a>]<br>\n", HTMLLOC);

	printf("<p><b>Notes</b> [<a href=\"http://%s/help.html#HELPGN\">HELP</a>]<br>\n", HTMLLOC);
	if (ptmp->pu_notes) {
		printf("<TEXTAREA NAME=\"NT\" ROWS=10 COLS=60 VALUE=\"%s\"></TEXTAREA>",  ptmp->pu_notes);
	} else {
		printf("<TEXTAREA NAME=\"NT\" ROWS=10 COLS=60></TEXTAREA>");
	}
	printf("<p>\n");

	printf("The story type should\n");
	printf("be one of: nv=novella, nt=novelette, ss=short story, sf=short fiction\n");
	printf("(exact length unknown), se=serial, p=poem, es=essay.\n");

	printf("<pre>\n");
	printf("<b>Content Listing:</b>\n");
	printf("---------------------------------------------------------\n");
	printf("Title                  Author                 Year   Type\n");
	printf("</pre>\n");

	for(loop=counter; loop<30; loop++) {
		printf("<INPUT NAME=\"VT%02d\" SIZE=20>\n", loop);
		printf("<INPUT NAME=\"VA%02d\" SIZE=20>\n", loop);
		printf("<INPUT NAME=\"VY%02d\" SIZE=5>\n", loop);
		printf("<INPUT NAME=\"VL%02d\" SIZE=5>\n", loop);
		printf("<br>\n");
	}
	strcpy(twocode,"C4");
}

void
checkdata()
{
	int fd;
	int count;

	printf("<title>ISFDB Submission Check</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<h1>ISFDB Submission Check</h1>\n");
	printf("The following is a listing of very recent data submissions.\n");
	printf("Please note that data is removed from this file a number of\n");
	printf("times per day, even though the database is only updated once\n");
	printf("per day. This means that only the most recent submissions will\n");
	printf("show up here. It's purpose is to ensure that your browser is correctly\n");
	printf("submitting data.\n");
	printf("<hr>\n");
	printf("<pre>\n");

#ifdef CYGWIN
	fd = open("NEWDATA", O_RDONLY|O_BINARY);
#else
	fd = open("NEWDATA", O_RDONLY);
#endif
	if ( fd != -1) {
		while(1) {
			for(count=0; count<1026; count++)
				buffer[count] = 0;
			count = read(fd, buffer, BIGSIZE);
			if (count > 0) {
				printf("%s", buffer);
			} else {
				close(fd);
				postamble("dataentry", "XX");
				exit(0);
			}
		}
	}
}

int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int		index;
	int		result;
	char		path[256];
	char		style;
	struct stat	statbuf;

	printf("Content-type: text/html\n\n");
	if ( stat(".nosubs", &statbuf) != -1) {
		int	fd;
		char	input;

		printf("<html><head>\n");
		printf("<title>Data Submissions Offline</title></head>\n");
		printf("<body bgcolor=#ffff80>\n");
		printf("<h1>Data Submissions Are Offline</h1><hr>\n");

#ifdef CYGWIN
		fd = open(".nosubs", O_RDONLY|O_BINARY);
#else
		fd = open(".nosubs", O_RDONLY);
#endif
		if (fd != -1) {
			while(1) {
				if ( read(fd, &input, 1) != 1) {
					break;
				} else {
					printf("%c", input);
				}
			}
		}

		postamble("dataentry", argv[1]);
		exit(0);
	}

	if (argc != 2) {
		goto skipit;
	}

	style = argv[1][0];

	/*
	 * Convert to lower case
	 */
	index = 0;
	while( argv[1][index+2] != 0 ) {
		argument[index] = argv[1][index+2];
		if (argument[index] == '_')
			argument[index] = ' ';
		index++;
	}

	/*
	 * Remove backquotes
	 */
	while ( strstr(argument, "\\") ) {
		char *ptr;

		ptr = (char *)strstr(argument, "\\");
		while( *ptr ) {
			*ptr = *(ptr+1);
			ptr++;
		}
	}

skipit:
        result = chdir(CGIBIN);
        if (result != 0) {
                printf("CHDIR to %s failed\n", CGIBIN);
                exit(1);
        }


	printf("<html><head>\n");

	strcpy(datacode, "none");

	switch(style) {
	case '1':	ninfo1();
			break;
	case '2':	cinfo();
			break;
	case '3':	cauthor();
			break;
	case '4':	nauthor();
			break;
	case '5':	ninfo2();
			break;
	case '6':	ncoll(1);
			break;
	case '7':	nzine();
			break;
	case '8':	cpub2();
			break;
	case '9':	cpub();
			break;
	case 'a':	checkdata();
			break;
	}

	printf("<p>\n");
	printf("<input type=\"submit\" value=\"Submit Data\">\n");
	printf("<input type=\"reset\"  value=\"Clear Form\">\n");
	printf("<input type=\"hidden\" name=\"%s\" value=\"%s\" size=2>\n", twocode, twocode);
	printf("<input type=\"hidden\" name=\"datacode\" value=\"%s\" size=2>\n", datacode);
	printf("<p>\n");
	printf("</form>\n");
	postamble("dataentry", argv[1]);
}
