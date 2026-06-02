/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)submit.c	1.11	01/28/98 SFdbase";

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef SUNOS
#include <sys/unistd.h>
#endif
#include "sfdbase.h"

#define CURRENT_YEAR 1998

char	errmsg[256];
char	datacode[256];
char	tmptitle[256];
char	tmpauthor[256];
char	tmptag[256] = {0};
char	tmpyear[256];
char	tmpseries[256];
char	tmpseriesnum[256];
char	tmpsuperseries[256];
char	tmpcover[256];
char	tmptype[256];
char	tmpformat[256];
char	tmppubs[256];
char	tmpisbn[256];
char	tmppages[256];
char	tmpprice[256];
char	tmpstorylen[256];
char	tmplegalname[256];
char	tmpbirthplace[256];
char	tmpbirthdate[256];
char	tmpdeathdate[256];
char	tmppseudonyms[256];
char	tmpemail[256];
char	tmpwebpage[256];
char	tmpsubmitter[256];
char	tmpnotes[8192];
char	tmpsynopsis[8192];
entry	entries[MAX_ENTRIES];
int	style = 0;

char	vtitle[100][256];
char	vauthor[100][256];
char	vlength[100][64];
char	vyear[100][64];


#define STYLE_C1	1
#define STYLE_N1	2
#define STYLE_N2	3
#define STYLE_C2	4
#define STYLE_N3	5
#define STYLE_N4	6
#define STYLE_N5	7	/* New magazine */
#define STYLE_C4	8	/* Update pub titles */
#define STYLE_C3	9	/* Correct publication */

void author_data(int style, FILE *fp);

void
work_data(int style, FILE *fp)
{
	int autotag;
	int shortwarning=0;

	switch(style) {
	case STYLE_C1:	fprintf(fp, "# ENTRY - [Old Author Work Correction]\n");
			break;
	case STYLE_N1:	fprintf(fp, "# ENTRY - [Old Author New Work Data]\n");
			break;
	case STYLE_N2:	fprintf(fp, "# ENTRY - [New Author New Work Data]\n");
			break;
	}

	autotag = getquerieint();

	fprintf(fp, "# BEGIN - %s ==========================\n", tmptype);
	fprintf(fp, "%s {\n", tmptitle);
	fprintf(fp, "\tAB=|AUTOTAG%d|\n", autotag);
	if (tmpauthor[0])
		fprintf(fp, "\tAE=|%s|\n", tmpauthor);
	if (tmpyear[0]) {
		fprintf(fp, "\tYR=|%s|\n", tmpyear);
	} else {
		fprintf(fp, "\tYR=|0|\n");
	}
	if (tmpseries[0])
		fprintf(fp, "\tSE=|%s|\n", tmpseries);
	if (tmpseriesnum[0])
		fprintf(fp, "\tSN=|%s|\n", tmpseriesnum);
	if (tmpsuperseries[0])
		fprintf(fp, "\tSS=|%s|\n", tmpsuperseries);
	if (tmpcover[0])
		fprintf(fp, "\tCV=|%s|\n", tmpcover);
	if (tmppubs[0])
		fprintf(fp, "\tPB=|%s|\n", tmppubs);
	if (tmpisbn[0]) {
		fprintf(fp, "\tIS=|%s|\n", tmpisbn);
		shortwarning = 1;
	}
	if (tmppages[0])
		fprintf(fp, "\tPP=|%s|\n", tmppages);
	if (tmpprice[0])
		fprintf(fp, "\tPR=|%s|\n", tmpprice);
	if (tmpformat[0])
		fprintf(fp, "\tTP=|%s|\n", tmpformat);
	if (tmpstorylen[0])
		fprintf(fp, "\tSL=|%s|\n", tmpstorylen);
	if (tmpnotes[0])
		fprintf(fp, "\tNT=|%s|\n", tmpnotes);
	if (tmpsynopsis[0])
		fprintf(fp, "\tSY=|%s|\n", tmpsynopsis);
	fprintf(fp, "}\n");
	fprintf(fp, "# END\n");
	fprintf(fp, "# =====================================\n");
	fflush(fp);
	fclose(fp);

	printf("<title>ISFDB Data Submission</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<pre>\n");
	printf("The following database request has been submitted locally:\n\n");
	printf("%s {\n", tmptitle);
	printf("\tAB=|AUTOTAG%d|\n", autotag);
	if (tmpauthor[0])
		printf("\tAE=|%s|\n", tmpauthor);
	if (tmpyear[0]) {
		printf("\tYR=|%s|\n", tmpyear);
	} else {
		printf("\tYR=|0|\n");
	}
	if (tmpseries[0])
		printf("\tSE=|%s|\n", tmpseries);
	if (tmpseriesnum[0])
		printf("\tSN=|%s|\n", tmpseriesnum);
	if (tmpsuperseries[0])
		printf("\tSS=|%s|\n", tmpsuperseries);
	if (tmpcover[0])
		printf("\tCV=|%s|\n", tmpcover);
	if (tmppubs[0])
		printf("\tPB=|%s|\n", tmppubs);
	if (tmpisbn[0]) {
		printf("\tIS=|%s|\n", tmpisbn);
		shortwarning = 1;
	}
	if (tmppages[0])
		printf("\tPP=|%s|\n", tmppages);
	if (tmpprice[0])
		printf("\tPR=|%s|\n", tmpprice);
	if (tmpformat[0])
		printf("\tTP=|%s|\n", tmpformat);
	if (tmpstorylen[0])
		printf("\tSL=|%s|\n", tmpstorylen);
	if (tmpnotes[0])
		printf("\tNT=|%s|\n", tmpnotes);
	if (tmpsynopsis[0])
		printf("\tSY=|%s|\n", tmpsynopsis);
	printf("}\n");

	if (shortwarning && (strcmp(tmptype, "Shortfiction") == 0)) {
		printf("\n\n*** WARNING: Short stories do not usually have an ISBN unless\n");
		printf("they are a chapter book (A short book containing exactly one story).\n");
		printf("Your submission suggests that I can go to Barnes & Noble and special\n");
		printf("order a book called: [%s] using ISBN=%s\n", tmptitle, tmpisbn);
		printf("\nIf this is not true, then please either enter an anthology/collection\n");
		printf("submission and include your story in it\n");
	}
}

void
dataerror(char *msg, FILE *fp)
{
	printf("<title>ISFDB Data Submission Error</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<pre>\n");
	printf("*** Your data submission request has failed. Error detected:\n");
	printf("\n\n<b>%s</b>\n", msg);
	printf("</pre>\n");
	fclose(fp);
	exit(0);
}


void
author_data(int style, FILE *fp)
{
	char digits[] = {'0','1','2','3','4','5','6','7','8','9'};
	int loop1, loop2;
	int birthdate = 0;
	int deathdate = 0;
	audata_t *auth;
	int	legalname_mod = 0; 
	int	birthplace_mod = 0; 
	int	birthdate_mod = 0; 
	int	deathdate_mod = 0; 
	int	pseudonyms_mod = 0; 
	int	notes_mod = 0; 
	int	email_mod = 0; 
	int	webpage_mod = 0; 
	int nomods = 1;

	auth = (audata_t *)get_author(tmpauthor);

	/*
	 * Check [1] - Changes to the author name are not allowed
	 */
	if (datacode[0]) {
		if (strcmp(tmpauthor, datacode)) {
			dataerror("Author name cannot be changed. Submit name change via notes section.", fp);
		}
	}


	/*
	 * Check [2] - See if the legal name was changed.
	 */
	if (tmplegalname[0]) {
		legalname_mod = 1;
		if (auth->au_legalname[0] && (strcmp(tmplegalname, auth->au_legalname) == 0)) {
			legalname_mod = 0;
		}
	}


	/*
	 * Check [3] - See if the birthplace was changed.
	 */
	if (tmpbirthplace[0]) {
		birthplace_mod = 1;
		if (auth->au_birthplace[0] && (strcmp(tmpbirthplace, auth->au_birthplace) == 0)) {
			birthplace_mod = 0;
		}
	}


	/*
	 * Check [4] - See if the birthdate was changed. If it was, check to make
	 * sure that the data is 4-digit numerical.
	 */
	if (tmpbirthdate[0]) {
		birthdate_mod = 1;
		if (auth->au_birthdate[0] && (strcmp(tmpbirthdate, auth->au_birthdate) == 0)) {
			birthdate_mod = 0;
		} else {
			for (loop1=0; loop1<4; loop1++) {
				if (tmpbirthdate[loop1]) {
					for (loop2=0; loop2<10; loop2++) {
						if (tmpbirthdate[loop1] == digits[loop2]) {
							break;
						}
					}
					if (loop2 == 10) {
						sprintf(errmsg, "Birthdate contains an illegal character: %c\n", 
							tmpbirthdate[loop1]);
						dataerror(errmsg, fp);
					}
				} else {
					dataerror("birthdate must contain 4 digits", fp);
				}
			}
			sscanf(tmpbirthdate, "%d", &birthdate);
		}
	}
	if (tmpbirthdate[4]) {
		dataerror("birthdate must contain only 4 digits", fp);
	}

	/*
	 * Check [5] - See if the deathdate was changed. If it was, check to make
	 * sure that the data is 4-digit numerical. Then make sure that the author's
	 * lifespan is sensical.
	 */
	if (tmpdeathdate[0]) {
		deathdate_mod = 1;
		if (auth->au_deathdate[0] && (strcmp(tmpdeathdate, auth->au_deathdate) == 0)) {
			deathdate_mod = 0;
		} else {
			for (loop1=0; loop1<4; loop1++) {
				if (tmpdeathdate[loop1]) {
					for (loop2=0; loop2<10; loop2++) {
						if (tmpdeathdate[loop1] == digits[loop2]) {
							break;
						}
					}
					if (loop2 == 10) {
						sprintf(errmsg, "Birthdate contains an illegal character: %c\n", 
							tmpdeathdate[loop1]);
						dataerror(errmsg, fp);
					}
				} else {
					dataerror("deathdate must contain 4 digits", fp);
				}
			}
			sscanf(tmpdeathdate, "%d", &deathdate);
			if (deathdate > CURRENT_YEAR) {
				sprintf(errmsg, "Psychic deathdate (%d) not allowed\n", deathdate);
				dataerror(errmsg, fp);
			}
		}
	}
	if (tmpdeathdate[4]) {
		dataerror("deathdate must contain only 4 digits", fp);
	}

	if (birthdate && deathdate) {
		int lifespan;

		lifespan = deathdate - birthdate;
		if (lifespan < 0) {
			dataerror("deathdate precedes birthdate", fp);
		}

		if (lifespan < 10) {
			sprintf(errmsg, "Lifespan only %d years\n", lifespan);
			dataerror(errmsg, fp);
		}
		if (lifespan > 120) {
			sprintf(errmsg, "Unusually long lifespan of %d years\n", lifespan);
			dataerror(errmsg, fp);
		}
	}


	/*
	 * Check [6] - See if the pseudonym list was changed.
	 */
	if (tmppseudonyms[0]) {
		pseudonyms_mod = 1;
		if (auth->au_pseudonyms[0] && (strcmp(tmppseudonyms, auth->au_pseudonyms) == 0)) {
			pseudonyms_mod = 0;
		}
	}

	/*
	 * Check [7] - See if the email list was changed.
	 */
	if (tmpemail[0]) {
		email_mod = 1;
		if (auth->au_email[0] && (strcmp(tmpemail, auth->au_email) == 0)) {
			email_mod = 0;
		}
	}

	if (tmpwebpage[0]) {
		webpage_mod = 1;
		if (auth->au_webpage[0] && (strcmp(tmpwebpage, auth->au_webpage) == 0)) {
			webpage_mod = 0;
		}
	}

	/*
	 * Check [7] - See if the notes were changed.
	 */
	if (tmpnotes[0]) {
		notes_mod = 1;
		if (auth->au_biog[0] && (strcmp(tmpnotes, auth->au_biog) == 0)) {
			notes_mod = 0;
		}
	}

	if (legalname_mod || birthplace_mod || birthdate_mod || deathdate_mod || pseudonyms_mod || notes_mod || email_mod || webpage_mod) {
		switch(style) {
		case STYLE_C2:	fprintf(fp, "# ENTRY - [Old Author Data Correction]\n");
				break;
		case STYLE_N3:	fprintf(fp, "# ENTRY - [New Author New Data]\n");
				break;
		}
	} else {
		dataerror("No changes were submitted", fp);
	}


	fprintf(fp, "# BEGIN - Author ==========================\n");
	fprintf(fp, "%s {\n", tmpauthor);
	if (tmplegalname[0] && legalname_mod)
		fprintf(fp, "\tLN=|%s|\n", tmplegalname);
	if (tmpbirthplace[0] && birthplace_mod)
		fprintf(fp, "\tBP=|%s|\n", tmpbirthplace);
	if (tmpbirthdate[0] && birthdate_mod)
		fprintf(fp, "\tBD=|%s|\n", tmpbirthdate);
	if (tmpdeathdate[0] && deathdate_mod)
		fprintf(fp, "\tDD=|%s|\n", tmpdeathdate);
	if (tmppseudonyms[0] && pseudonyms_mod)
		fprintf(fp, "\tPS=|%s|\n", tmppseudonyms);
	if (tmpemail[0] && email_mod)
		fprintf(fp, "\tEM=|%s|\n", tmpemail);
	if (tmpwebpage[0] && webpage_mod)
		fprintf(fp, "\tEM=|%s|\n", tmpwebpage);
	if (tmpnotes[0] && notes_mod)
		fprintf(fp, "\tNT=|%s|\n", tmpnotes);
	fprintf(fp, "}\n");
	fprintf(fp, "# END\n");
	fprintf(fp, "# =====================================\n");
	fflush(fp);
	fclose(fp);


	printf("<title>ISFDB Data Submission</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<pre>\n");
	printf("The following database request has been submitted locally:\n\n");
	printf("%s {\n", tmpauthor);
	if (tmplegalname[0] && legalname_mod) {
		printf("\tLN=|%s|\n", tmplegalname);
	}
	if (tmpbirthplace[0] && birthplace_mod) {
		printf("\tBP=|%s|\n", tmpbirthplace);
	}
	if (tmpbirthdate[0] && birthdate_mod) {
		printf("\tBD=|%s|\n", tmpbirthdate);
	}
	if (tmpdeathdate[0] && deathdate_mod) {
		printf("\tDD=|%s|\n", tmpdeathdate);
	}
	if (tmppseudonyms[0] && pseudonyms_mod) {
		printf("\tPS=|%s|\n", tmppseudonyms);
	}
	if (tmpemail[0] && email_mod) {
		printf("\tEM=|%s|\n", tmpemail);
	}
	if (tmpwebpage[0] && webpage_mod) {
		printf("\tWP=|%s|\n", tmpwebpage);
	}
	if (tmpnotes[0] && notes_mod) {
		printf("\tNT=|%s|\n", tmpnotes);
	}
	printf("}\n");
}

void
zine_data(int style, FILE *fp)
{
	int loop;
	int autotag;

	switch(style) {
	case STYLE_N5:	fprintf(fp, "# ENTRY - [New Zine Data]\n");
			break;
	}

	autotag = getquerieint();

	fprintf(fp, "# BEGIN - Zine ==========================\n");
	fprintf(fp, "%s {\n", tmptitle);
	fprintf(fp, "\tAB=|AUTOTAG%d|\n", autotag);
	if (tmpauthor[0])
		fprintf(fp, "\tAE=|%s|\n", tmpauthor);
	if (tmpyear[0]) {
		fprintf(fp, "\tYR=|%s|\n", tmpyear);
	} else {
		fprintf(fp, "\tYR=|0|\n");
	}
	if (tmppages[0])
		fprintf(fp, "\tPP=|%s|\n", tmppages);
	if (tmpprice[0])
		fprintf(fp, "\tPR=|%s|\n", tmpprice);
	if (tmpcover[0])
		fprintf(fp, "\tCV=|%s|\n", tmpcover);
	if (tmpnotes[0])
		fprintf(fp, "\tNT=|%s|\n", tmpnotes);
	fprintf(fp, "}\n");
	for(loop=0; loop<30; loop++) {
		if (vtitle[loop][0]) {
			fprintf(fp, "%s {\n", vtitle[loop]);
			if (vauthor[loop][0]) {
				fprintf(fp, "\tAE=|%s|\n", vauthor[loop]);
			}
			if (vlength[loop][0]) {
				fprintf(fp, "\tSL=|%s|\n", vlength[loop]);
			}
			if (vyear[loop][0]) {
				fprintf(fp, "\tYR=|%s|\n", vyear[loop]);
			} else if (tmpyear[0]) {
				fprintf(fp, "\tYR=|%s|\n", tmpyear);
			} else {
				fprintf(fp, "\tYR=|0|\n");
			}
			fprintf(fp, "\tPB=|AUTOTAG%d|\n", autotag);
			fprintf(fp, "}\n");
		}
	}
	fprintf(fp, "# END\n");
	fprintf(fp, "# =====================================\n");
	fflush(fp);
	fclose(fp);

	printf("<title>ISFDB Data Submission</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<pre>\n");
	printf("The following database request has been submitted locally:\n\n");
	printf("%s {\n", tmptitle);
	printf("\tAB=|AUTOTAG%d|\n", autotag);
	if (tmpauthor[0])
		printf("\tAE=|%s|\n", tmpauthor);
	if (tmpyear[0]) {
		printf("\tYR=|%s|\n", tmpyear);
	} else {
		printf("\tYR=|0|\n");
	}
	if (tmppages[0])
		printf("\tPP=|%s|\n", tmppages);
	if (tmpprice[0])
		printf("\tPR=|%s|\n", tmpprice);
	if (tmpcover[0])
		printf("\tCV=|%s|\n", tmpcover);
	if (tmpnotes[0])
		printf("\tNT=|%s|\n", tmpnotes);
	printf("}\n");
	for(loop=0; loop<30; loop++) {
		if (vtitle[loop][0]) {
			printf("%s {\n", vtitle[loop]);
			if (vauthor[loop][0]) {
				printf("\tAE=|%s|\n", vauthor[loop]);
			}
			if (vlength[loop][0]) {
				printf("\tSL=|%s|\n", vlength[loop]);
			}
			if (vyear[loop][0]) {
				printf("\tYR=|%s|\n", vyear[loop]);
			} else if (tmpyear[0]) {
				printf("\tYR=|%s|\n", tmpyear);
			} else {
				printf("\tYR=|0|\n");
			}
			printf("\tPB=|AUTOTAG%d|\n", autotag);
			printf("}\n");
		}
	}
}

void
coll_data(int style, FILE *fp)
{
	int loop;
	int collection = 0;
	int autotag;

	switch(style) {
	case STYLE_N4:	if (strcmp(tmptype, "Collection") == 0) {
				fprintf(fp, "# ENTRY - [Collection Data]\n");
				collection = 1;
			} else {
				fprintf(fp, "# ENTRY - [Anthology Data]\n");
			}
			break;
	}

	autotag = getquerieint();
	fprintf(fp, "# BEGIN - %s ==========================\n", tmptype);
	fprintf(fp, "%s {\n", tmptitle);
	fprintf(fp, "\tAB=|AUTOTAG%d|\n", autotag);
	if (tmpauthor[0])
		fprintf(fp, "\tAE=|%s|\n", tmpauthor);
	if (tmpyear[0]) {
		fprintf(fp, "\tYR=|%s|\n", tmpyear);
	} else {
		fprintf(fp, "\tYR=|0|\n");
	}
	if (tmppubs[0])
		fprintf(fp, "\tPB=|%s|\n", tmppubs);
	if (tmpisbn[0])
		fprintf(fp, "\tIS=|%s|\n", tmpisbn);
	if (tmppages[0])
		fprintf(fp, "\tPP=|%s|\n", tmppages);
	if (tmpprice[0])
		fprintf(fp, "\tPR=|%s|\n", tmpprice);
	if (tmpcover[0])
		fprintf(fp, "\tCV=|%s|\n", tmpcover);
	if (tmpseries[0])
		fprintf(fp, "\tSE=|%s|\n", tmpseries);
	if (tmpseriesnum[0])
		fprintf(fp, "\tSN=|%s|\n", tmpseriesnum);
	if (tmpsuperseries[0])
		fprintf(fp, "\tSS=|%s|\n", tmpsuperseries);
	if (tmpformat[0])
		fprintf(fp, "\tTP=|%s|\n", tmpformat);
	if (tmpnotes[0])
		fprintf(fp, "\tNT=|%s|\n", tmpnotes);
	if (tmpsynopsis[0])
		fprintf(fp, "\tSY=|%s|\n", tmpsynopsis);
	fprintf(fp, "}\n");
	for(loop=0; loop<30; loop++) {
		if (vtitle[loop][0]) {
			fprintf(fp, "%s {\n", vtitle[loop]);
			if (vauthor[loop][0]) {
				fprintf(fp, "\tAE=|%s|\n", vauthor[loop]);
			} else if (collection) {
				fprintf(fp, "\tAE=|%s|\n", tmpauthor);
			}
			if (vyear[loop][0]) {
				fprintf(fp, "\tYR=|%s|\n", vyear[loop]);
			} else {
				fprintf(fp, "\tYR=|0|\n");
			}
			fprintf(fp, "\tPB=|AUTOTAG%d|\n", autotag);
			fprintf(fp, "}\n");
		}
	}
	fprintf(fp, "# END\n");
	fprintf(fp, "# =====================================\n");
	fflush(fp);
	fclose(fp);

	printf("<title>ISFDB Data Submission</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<pre>\n");
	printf("The following database request has been submitted locally:\n\n");
	printf("%s {\n", tmptitle);
	printf("\tAB=|AUTOTAG%d|\n", autotag);
	if (tmpauthor[0])
		printf("\tAE=|%s|\n", tmpauthor);
	if (tmpyear[0]) {
		printf("\tYR=|%s|\n", tmpyear);
	} else {
		printf("\tYR=|0|\n");
	}
	if (tmppubs[0])
		printf("\tPB=|%s|\n", tmppubs);
	if (tmpisbn[0])
		printf("\tIS=|%s|\n", tmpisbn);
	if (tmppages[0])
		printf("\tPP=|%s|\n", tmppages);
	if (tmpprice[0])
		printf("\tPR=|%s|\n", tmpprice);
	if (tmpcover[0])
		printf("\tCV=|%s|\n", tmpcover);
	if (tmpseries[0])
		printf("\tSE=|%s|\n", tmpseries);
	if (tmpseriesnum[0])
		printf("\tSN=|%s|\n", tmpseriesnum);
	if (tmpsuperseries[0])
		printf("\tSS=|%s|\n", tmpsuperseries);
	if (tmpformat[0])
		printf("\tTP=|%s|\n", tmpformat);
	if (tmpnotes[0])
		printf("\tNT=|%s|\n", tmpnotes);
	if (tmpsynopsis[0])
		printf("\tSY=|%s|\n", tmpsynopsis);
	printf("}\n");
	for(loop=0; loop<30; loop++) {
		if (vtitle[loop][0]) {
			printf("%s {\n", vtitle[loop]);
			if (vauthor[loop][0]) {
				printf("\tAE=|%s|\n", vauthor[loop]);
			} else if (collection) {
				printf("\tAE=|%s|\n", tmpauthor);
			}
			if (vyear[loop][0]) {
				printf("\tYR=|%s|\n", vyear[loop]);
			} else {
				printf("\tYR=|0|\n");
			}
			printf("\tPB=|AUTOTAG%d|\n", autotag);
			printf("}\n");
		}
	}
}


void
pub_data(int style, FILE *fp)
{
	int loop;

	switch(style) {
	case STYLE_C3:	fprintf(fp, "# ENTRY - [Old Pub Correction]\n");
			break;
	case STYLE_C4:	fprintf(fp, "# ENTRY - [Old Pub Add Titles]\n");
			break;
	}

	fprintf(fp, "# BEGIN - Pub ==========================\n");
	fprintf(fp, "%s {\n", tmptitle);
	if (tmptag[0])
		fprintf(fp, "\tAB=|%s|\n", tmptag);
	if (tmpauthor[0])
		fprintf(fp, "\tAE=|%s|\n", tmpauthor);
	if (tmpyear[0]) {
		fprintf(fp, "\tYR=|%s|\n", tmpyear);
	} else {
		fprintf(fp, "\tYR=|0|\n");
	}
	if (tmpcover[0])
		fprintf(fp, "\tCV=|%s|\n", tmpcover);
	if (tmppubs[0])
		fprintf(fp, "\tPB=|%s|\n", tmppubs);
	if (tmpisbn[0])
		fprintf(fp, "\tIS=|%s|\n", tmpisbn);
	if (tmppages[0])
		fprintf(fp, "\tPP=|%s|\n", tmppages);
	if (tmpprice[0])
		fprintf(fp, "\tPR=|%s|\n", tmpprice);
	if (tmpformat[0])
		fprintf(fp, "\tTP=|%s|\n", tmpformat);
	if (tmpstorylen[0])
		fprintf(fp, "\tSL=|%s|\n", tmpstorylen);
	if (tmpnotes[0])
		fprintf(fp, "\tNT=|%s|\n", tmpnotes);
	fprintf(fp, "}\n");
	for(loop=0; loop<30; loop++) {
		if (vtitle[loop][0]) {
			fprintf(fp, "%s {\n", vtitle[loop]);
			if (vauthor[loop][0]) {
				fprintf(fp, "\tAE=|%s|\n", vauthor[loop]);
			}
			if (vyear[loop][0]) {
				fprintf(fp, "\tYR=|%s|\n", vyear[loop]);
			} else {
				fprintf(fp, "\tYR=|0|\n");
			}
			if (vlength[loop][0]) {
				fprintf(fp, "\tSL=|%s|\n", vlength[loop]);
			}
			fprintf(fp, "\tPB=|%s|\n", tmptag);
			fprintf(fp, "}\n");
		}
	}
	fprintf(fp, "# END\n");
	fprintf(fp, "# =====================================\n");
	fflush(fp);
	fclose(fp);

	printf("<title>ISFDB Data Submission</title></head>\n");
	printf("<body bgcolor=#ffff80>\n");
	printf("<pre>\n");
	printf("The following database request has been submitted locally:\n\n");
	printf("%s {\n", tmptitle);
	if (tmptag[0])
		printf("\tAB=|%s|\n", tmptag);
	if (tmpauthor[0])
		printf("\tAE=|%s|\n", tmpauthor);
	if (tmpyear[0]) {
		printf("\tYR=|%s|\n", tmpyear);
	} else {
		printf("\tYR=|0|\n");
	}
	if (tmpcover[0])
		printf("\tCV=|%s|\n", tmpcover);
	if (tmppubs[0])
		printf("\tPB=|%s|\n", tmppubs);
	if (tmpisbn[0])
		printf("\tIS=|%s|\n", tmpisbn);
	if (tmppages[0])
		printf("\tPP=|%s|\n", tmppages);
	if (tmpprice[0])
		printf("\tPR=|%s|\n", tmpprice);
	if (tmpformat[0])
		printf("\tTP=|%s|\n", tmpformat);
	if (tmpstorylen[0])
		printf("\tSL=|%s|\n", tmpstorylen);
	if (tmpnotes[0])
		printf("\tNT=|%s|\n", tmpnotes);
	printf("}\n");
	for(loop=0; loop<30; loop++) {
		if (vtitle[loop][0]) {
			printf("%s {\n", vtitle[loop]);
			if (vauthor[loop][0]) {
				printf("\tAE=|%s|\n", vauthor[loop]);
			}
			if (vyear[loop][0]) {
				printf("\tYR=|%s|\n", vyear[loop]);
			} else {
				printf("\tYR=|0|\n");
			}
			if (vlength[loop][0]) {
				printf("\tSL=|%s|\n", vlength[loop]);
			}
			printf("\tPB=|%s|\n", tmptag);
			printf("}\n");
		}
	}
}

void
submit_error(int style, FILE *fp)
{
	fprintf(fp, "============================\n");
	fprintf(fp, "Bogus submission style = %d\n", style);
	fprintf(fp, "============================\n");
	fprintf(fp, ".\n");
	fflush(fp);
	fclose(fp);

	printf("<pre>\n");
	printf("<h1>Internal Error %d</h1>\n", style);
	printf("The appropriate authorities have been notified.\n");
}


int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int		loop;
	int		max;
	int		content_length;
	char		path[256];
	FILE		*fp;
	time_t		ltime;
	struct tm	*ltm;
	struct stat     statbuf;
#ifdef DEBUG
	FILE		*debugfp;
#endif

	printf("Content-type: text/html\n\n");
#ifdef NOT_NEEDED
	if(strcmp(getenv("REQUEST_METHOD"), "POST") ) {
		printf("This script should be referenced with a METHOD of POST.\n");
		printf("<A HREF=\"http://www.ncsa.uiuc.edu/SDG/Software/Mosaic/Docs/fill-out-forms/overview.html\">forms overview</A>.%c",10);
		exit(1);
	}
	
	if(strcmp(getenv("CONTENT_TYPE"), "application/x-www-form-urlencoded")) {
		printf("This script can only be used to decode form results. \n");
		exit(1);
	}
#endif

	if ( stat(".nosubs", &statbuf) != -1) {
		int	fd;
		char	input;

		printf("<html><head>\n");
		printf("<title>Data Submissions Offline</title></head><body>\n");
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

		postamble("submit", argv[1]);
		exit(0);
	}

	content_length = atoi(getenv("CONTENT_LENGTH"));
	for(loop=0; content_length && (!feof(stdin)); loop++) {
		max = loop;
		entries[loop].val = (char *)fmakeword(stdin, '&', &content_length);
		plustospace(entries[loop].val);
		unescape_url(entries[loop].val);
		entries[loop].name = (char *)makeword(entries[loop].val, '=');
	}

#ifdef DEBUG
	debugfp = fopen("SUBDEBUG", "ab");
	if (debugfp) {
		fprintf(debugfp,"---------------------------------------------------\n");
	}
#endif
	for(loop=0; loop <= max; loop++) {
#ifdef DEBUG
	if (debugfp) {
		fprintf(debugfp,"NAME=[%s] VAL=[%s]\n", entries[loop].name, entries[loop].val );
	}
#endif
		if (strncmp(entries[loop].name, "TI", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmptitle, entries[loop].val);
			} else {
				tmptitle[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "AE", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpauthor, entries[loop].val);
			} else {
				tmpauthor[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "AB", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmptag, entries[loop].val);
			} else {
				tmptag[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "YR", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpyear, entries[loop].val);
			} else {
				tmpyear[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "PB", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmppubs, entries[loop].val);
			} else {
				tmppubs[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "IS", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpisbn, entries[loop].val);
			} else {
				tmpisbn[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "PP", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmppages, entries[loop].val);
			} else {
				tmppages[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "PR", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpprice, entries[loop].val);
			} else {
				tmpprice[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "CV", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpcover, entries[loop].val);
			} else {
				tmpcover[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "SE", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpseries, entries[loop].val);
			} else {
				tmpseries[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "SS", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpsuperseries, entries[loop].val);
			} else {
				tmpsuperseries[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "SN", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpseriesnum, entries[loop].val);
			} else {
				tmpseriesnum[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "SL", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpstorylen, entries[loop].val);
			} else {
				tmpstorylen[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "TYPE", 4) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmptype, entries[loop].val);
			} else {
				tmptype[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "FORMAT", 6) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpformat, entries[loop].val);
				if ( strcmp(tmpformat, "unknown") == 0) {
					tmpformat[0] = 0;
				}
			} else {
				tmpformat[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "TP", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpformat, entries[loop].val);
				if ( strcmp(tmpformat, "unknown") == 0) {
					tmpformat[0] = 0;
				}
			} else {
				tmpformat[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "NT", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpnotes, entries[loop].val);
			} else {
				tmpnotes[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "SY", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpsynopsis, entries[loop].val);
			} else {
				tmpsynopsis[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "LN", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmplegalname, entries[loop].val);
			} else {
				tmplegalname[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "BP", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpbirthplace, entries[loop].val);
			} else {
				tmpbirthplace[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "BD", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpbirthdate, entries[loop].val);
			} else {
				tmpbirthdate[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "DD", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpdeathdate, entries[loop].val);
			} else {
				tmpdeathdate[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "PS", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmppseudonyms, entries[loop].val);
			} else {
				tmppseudonyms[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "EM", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpemail, entries[loop].val);
			} else {
				tmpemail[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "WP", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpwebpage, entries[loop].val);
			} else {
				tmpwebpage[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "XX", 2) == 0) {
			if (entries[loop].val[0]) {
				strcpy(tmpsubmitter, entries[loop].val);
			} else {
				tmpsubmitter[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "datacode", 8) == 0) {
			if (entries[loop].val[0]) {
				strcpy(datacode, entries[loop].val);
			} else {
				datacode[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "C1", 2) == 0) {
			style = STYLE_C1;
		} else if (strncmp(entries[loop].name, "C2", 2) == 0) {
			style = STYLE_C2;
		} else if (strncmp(entries[loop].name, "N1", 2) == 0) {
			style = STYLE_N1;
		} else if (strncmp(entries[loop].name, "N2", 2) == 0) {
			style = STYLE_N2;
		} else if (strncmp(entries[loop].name, "N3", 2) == 0) {
			style = STYLE_N3;
		} else if (strncmp(entries[loop].name, "N4", 2) == 0) {
			style = STYLE_N4;
		} else if (strncmp(entries[loop].name, "N5", 2) == 0) {
			style = STYLE_N5;
		} else if (strncmp(entries[loop].name, "C3", 2) == 0) {
			style = STYLE_C3;
		} else if (strncmp(entries[loop].name, "C4", 2) == 0) {
			style = STYLE_C4;
		} else if (strncmp(entries[loop].name, "VT", 2) == 0) {
			int offset;
			char *tmp;

			tmp = (char *)&(entries[loop].name[2]);
			sscanf(tmp,"%d", &offset);
			if (entries[loop].val[0]) {
				strcpy(vtitle[offset], entries[loop].val);
			} else {
				vtitle[offset][0] = 0;
			}
		} else if (strncmp(entries[loop].name, "VA", 2) == 0) {
			int offset;
			char *tmp;

			tmp = (char *)&(entries[loop].name[2]);
			sscanf(tmp,"%d", &offset);
			if (entries[loop].val[0]) {
				strcpy(vauthor[offset], entries[loop].val);
			} else {
				vauthor[offset][0] = 0;
			}
		} else if (strncmp(entries[loop].name, "VL", 2) == 0) {
			int offset;
			char *tmp;

			tmp = (char *)&(entries[loop].name[2]);
			sscanf(tmp,"%d", &offset);
			if (entries[loop].val[0]) {
				strcpy(vlength[offset], entries[loop].val);
			} else {
				vlength[offset][0] = 0;
			}
		} else if (strncmp(entries[loop].name, "VY", 2) == 0) {
			int offset;
			char *tmp;

			tmp = (char *)&(entries[loop].name[2]);
			sscanf(tmp,"%d", &offset);
			if (entries[loop].val[0]) {
				strcpy(vyear[offset], entries[loop].val);
			} else {
				vyear[offset][0] = 0;
			}
		}
	}
#ifdef DEBUG
	if (debugfp) {
		fclose(debugfp);
	}
#endif

	fp = fopen("NEWDATA", "ab");
	if (fp == NULL) {
		printf("Can't access the request file:\n\n   %s\n", path);
		goto finish;
	}

	time( &ltime );
	ltm = gmtime( &ltime );
	fprintf(fp, "# Timestamp (GMT) = %s", asctime(ltm) );
#ifdef NOT_NEEDED
	fprintf(fp, "# [HTTP_USER_AGENT=%s]\n", getenv("HTTP_USER_AGENT") );
	fprintf(fp, "# [REMOTE_HOST=%s, REMOTE_ADDR=%s]\n", getenv("REMOTE_HOST"), getenv("REMOTE_ADDR") );
#endif
	if (tmpsubmitter[0]) {
		fprintf(fp, "# [email=%s]\n", tmpsubmitter);
	} else {
		fprintf(fp, "# [email=NOT SUBMITTED]\n");
	}

	switch(style) {
	case STYLE_C1:	work_data(style, fp);
			break;
	case STYLE_N1:	work_data(style, fp);
			break;
	case STYLE_N2:	work_data(style, fp);
			break;
	case STYLE_C2:	author_data(style, fp);
			break;
	case STYLE_N3:	author_data(style, fp);
			break;
	case STYLE_N4:	coll_data(style, fp);
			break;
	case STYLE_N5:	zine_data(style, fp);
			break;
	case STYLE_C3:	pub_data(style, fp);
			break;
	case STYLE_C4:	pub_data(style, fp);
			break;
	default:	submit_error(style, fp);
			break;
	}
finish:
	postamble("submit", argv[1]);
	printf("</pre>\n");
	fclose(fp);
}
