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
#include <errno.h>
#ifdef SUNOS
#include <sys/unistd.h>
#endif
#include "sfdbase.h"

char	finalfilename[256];
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
char	tmpnotes[8192];
char	tmpsynopsis[8192];
entry	entries[MAX_ENTRIES];
int	style = 0;

char	vtitle[100][256];
char	vauthor[100][256];
char	vlength[100][64];
char	vyear[100][64];

#ifdef CYGWIN
int errno;
#endif


int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int		loop;
	int		max;
	int		content_length;
	FILE		*fp;
	char *		filename;
	char *		target;
	char *		filestart;
	char *		file_end;

	printf("Content-type: text/html\n\n");
	if(strcmp(getenv("REQUEST_METHOD"), "POST") ) {
		printf("This script should be referenced with a METHOD of POST.\n");
		printf("<A HREF=\"http://www.ncsa.uiuc.edu/SDG/Software/Mosaic/Docs/fill-out-forms/overview.html\">forms overview</A>.%c",10);
		exit(1);
	}
	
	content_length = atoi(getenv("CONTENT_LENGTH"));
	for(loop=0; content_length && (!feof(stdin)); loop++) {
		max = loop;
		entries[loop].val = (char *)fmakeword(stdin, 0, &content_length);
		entries[loop].name = (char *)makeword(entries[loop].val, '=');
printf("name[%d]=%s\n", loop, entries[loop].name); fflush(stdout);
printf("val[%d] =%s\n", loop, entries[loop].val); fflush(stdout);
	}

        printf("<html><head>\n");
        printf("<title>Stuff</title></head>\n");
        printf("<body bgcolor=#ffffff>\n");
	printf("<pre>\n");
printf("%d\n", __LINE__); fflush(stdout);

	if (content_length != 0) {
		printf("Bad content length (%d)\n", content_length);
		printf("</pre>\n");
		fflush(stdout);
		exit(0);
	}
printf("%d\n", __LINE__); fflush(stdout);

	/*
	 * [1] Find the 1st double quote. This will be the 
	 * beginning of the string that describes the value
	 * type. When found, look for the trailing double
	 * quote, and NULL out the string there. The variable
	 * 'target' will point to this string.
	 */
	target = (char *)strchr(entries[0].val, '\"');
	if (target) {
		target++;
		filename = (char *)strchr(target, '\"');
		*filename = 0;
		filename++;
	} else {
		printf("Can't find file start\n");
		exit(0);
	}

	/*
	 * Now find the selected filename. Look for the literal
	 * string 'filename', the move the string pointer 10 places
	 * to skip 'filename' and the trailing equal sign and double
	 * quote. The look for the trailing double quote, and NULL
	 * out the string there.
	 */
	filename = (char *)strstr(filename, "filename");
	filename += 10;
	filestart = (char *)strchr(filename, '\"');
	if (filestart) {
		*filestart = 0;
		filestart++;
	} else {
		printf("Can't find file start\n");
		exit(0);
	}

	/*
	 * Now find the beginning of the file. All award files
	 * begin with a comment block, so look for the ##
	 */
	filestart = (char *)strstr(filestart, "##");
	if (!filestart) {
		printf("Can't find file start\n");
	}

	if (strcmp(target, filename)) {
		printf("<b>*** Checkin Failed ***</b>\n\n");
		printf("Checkin target (%s) doesn't match submitted filename (%s)\n",
			target, filename);
		exit(0);
	}

	/*
	 * Find the end of the file, then backup until you hit the
	 * last brace, then go forward and whack the rest of the file.
	 */
	file_end = filestart;
	while(*file_end) {
		file_end++;
	}
	while(*file_end != '}' ) {
		file_end--;
	}
	file_end += 2;
	*file_end = 0;
		
	sprintf(finalfilename, "checkins/%s", filename);
	fp = fopen(finalfilename, "wb+");		
	if (fp == NULL) {
		printf("Can't open %s: %d\n", filename, errno);
		exit(0);
	}

	fprintf(fp, "%s", filestart);

	printf("</pre>\n");
	fflush(stdout);
	fclose(fp);
	exit(0);
}
