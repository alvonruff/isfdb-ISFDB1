/*
 *     (C) COPYRIGHT 1997-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)ccdbase.c	1.13	05/01/98 SFdbase";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char base[256];

void
doerror(int errorcode)
{
	if (errorcode) {
		fprintf(stderr, "*** Error code %d\n", errorcode);
		exit(1);
	}
}


int
main(int argc, char *argv[] )
{
	int result;
	int nostats = 0;

	if (argc == 3) {
		if (strcmp(argv[1], "-n") == 0) {
			sprintf(base, "%s/bin", argv[2]);
			nostats = 1;
		} else {
			fprintf(stderr, "ccdbase [-n] <base directory>\n");
			exit(1);
		}
	} else if (argc == 2) {
		sprintf(base, "%s/bin", argv[1]);
	} else {
		fprintf(stderr, "ccdbase [-n] <base directory>\n");
		exit(1);
	}

	result = chdir(base);
	if (result == -1) {
		fprintf(stderr, "Cannot cd to %s\n", base);
		exit(1);
	}

	/*
	 * STEP 1 - Create the Artist Database
	 */
	printf("Creating: Artist Database\n");
	result = system("./ccpubs -a ../dbase.ascii/ARTWORK");
	doerror(result);

	/*
	 * STEP 2 - Create the Review Database
	 */
	printf("Creating: Review Database\n");
	result = system("./ccreviews OUT");
	doerror(result);
	printf("Sorting:  Review Database\n");
	result = system("./booksort OUT ../dbase.compiled/reviews.dbase");
	doerror(result);

	/*
	 * STEP 3 - Compile and Sort the Interviews Database
	 */
	printf("Creating: Interview Database\n");
	result = system("./ccinterv OUT");
	doerror(result);
	printf("Sorting:  Interview Database\n");
	result = system("./booksort OUT ../dbase.compiled/interviews.dbase");
	doerror(result);

	/*
	 * STEP 4 - Compile and Sort the Titles Database
	 */
	printf("Creating: Titles Database\n");
	result = system("./cctitles -t OUT");
	doerror(result);
	printf("Sorting:  Titles Database\n");
	result = system("./booksort OUT titles.dbase");
	doerror(result);
	printf("Creating: Titles indexed by author\n");
	result = system("./cctitles -a titles.xba");
	doerror(result);
	printf("Creating: Titles indexed by series\n");
	result = system("./cctitles -s OUT");
	doerror(result);
	result = system("./booksort OUT titles.xbs");
	doerror(result);
	printf("Creating: Titles indexed by tag\n");
	result = system("./cctitles -g titles.xbt");
	doerror(result);
	result = system("mv titles.dbase ../dbase.compiled");
	doerror(result);
	result = system("mv titles.xba ../dbase.compiled");
	doerror(result);
	result = system("mv titles.xbs ../dbase.compiled");
	doerror(result);
	result = system("mv titles.xbt ../dbase.compiled");
	doerror(result);

	/*
	 * STEP 5 - Compile and Sort the Author/Publication Databases
	 */
	printf("Creating: Author Database\n");
	result = system("./ccauthors ../dbase.compiled/authors.dbase");
	doerror(result);
	printf("Creating: Publication Database\n");
	result = system("./ccpubs -p OUT");
	doerror(result);
	result = system("./booksort OUT ../dbase.compiled/pubs.dbase");
	doerror(result);
	result = system("mv ../dbase.ascii/NOTES ../dbase.compiled/notes");
	doerror(result);

	/*
	 * STEP 6 - Compile and Sort the Awards Database
	 */
	printf("Creating: Awards Database\n");
	result = system("./ccawards -t ../dbase.compiled/awards.dbase");
	doerror(result);
	printf("Creating: Awards Author Index\n");
	result = system("./ccawards -a ../dbase.compiled/awards.xba");
	doerror(result);
	printf("Creating: Awards Year Index\n");
	result = system("./ccawards -y ../dbase.compiled/awards.xby");
	doerror(result);

	result = chdir("../dbase.compiled");
	if (result == -1) {
		fprintf(stderr, "Cannot cd to %s/dbase.compiled\n", base);
		exit(1);
	}

	if (!nostats) {
		/*
		 * STEP 7 - Create Statistics
		 */
		printf("Creating: Statistics\n");
		result = system("../bin/info 1 > ../dbase.compiled/stats_HTML");
		doerror(result);
		printf("Creating: Graphed Statistics\n");
		result = system("../bin/info 3 > ../dbase.compiled/stats2_HTML");
		doerror(result);
		printf("Creating: Oldest Living List\n");
		result = system("../bin/info 4 > ../dbase.compiled/oldest_HTML");
		doerror(result);
		printf("Creating: Oldest Non-Living List\n");
		result = system("../bin/info 28 > ../dbase.compiled/oldest_nl_HTML");
		doerror(result);
		printf("Creating: Youngest Non-Living List\n");
		result = system("../bin/info 29 > ../dbase.compiled/youngest_nl_HTML");
		doerror(result);
		printf("Creating: Youngest Living List\n");
		result = system("../bin/info 30 > ../dbase.compiled/youngest_HTML");
		doerror(result);
		printf("Creating: Author Directory\n");
		result = system("../bin/info 5 > ../dbase.compiled/stats4_HTML");
		doerror(result);

		/*
		 * STEP 8 - Cleanup
		 */
#ifdef AIX
		result = system("/usr/bin/date > ../dbase.compiled/.DATE");
#else
		result = system("/bin/date > ../dbase.compiled/.DATE");
#endif
		doerror(result);
		result = system("mv *_HTML ../htmls");
		doerror(result);
	}
	exit(0);
}
