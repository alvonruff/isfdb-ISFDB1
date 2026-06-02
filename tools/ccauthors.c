/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)ccauthors.c	1.6	06/05/97 SFdbase";

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef SUNOS
#include <sys/unistd.h>
#else
#include <unistd.h>
#endif
#include "sfdbase.h"

extern int do_attribute(char *targetattr, attr_t *list, int options);

search_t        *title_list  = NULL;
FILE		*notes_fp    = NULL;
FILE		*titles_fp   = NULL;
int		notes_offset = 0;
extern object_t	*Objlist;


static void
search_file(char *filename)
{
	object_t	*tmp;
	extern int	line_number;

	/*
	 * Reset the line number, and parse the entire file
	 * into memory.
	 */
	line_number = 1;
	parse_pubs(filename);

	/*
	 * Now go through each entry, and reoutput the data
	 * in delimited format. Notes will go into the NOTES
	 * file, and offsets into the NOTES file will be placed
	 * into the notes field.
	 *
	 * WARNING: Order of the do_attribute() calls is critical.
	 */
	tmp = Objlist;
	while(tmp) {
		(void)printf("%s|", tmp->ob_name);
		do_attribute("LN", tmp->ob_attrs, NOOPTIONS);
		do_attribute("BP", tmp->ob_attrs, NOOPTIONS);
		do_attribute("BD", tmp->ob_attrs, NOOPTIONS);
		do_attribute("DD", tmp->ob_attrs, NOOPTIONS);
		do_attribute("PS", tmp->ob_attrs, NOOPTIONS);
		do_attribute("EM", tmp->ob_attrs, NOOPTIONS);
		do_attribute("WP", tmp->ob_attrs, NOOPTIONS);
		do_attribute("NT", tmp->ob_attrs, DO_NOTES);
		(void)printf("\n");
		tmp = tmp->ob_next;
	}

	Objlist = NULL;
}


int
main(int argc, char *argv[])
{
	char		path[256];
	struct stat	statbuf;
	FILE		*fp;
        int             result;

	if (argc != 2) {
		printf("usage: ccauthors [output file]\n");
		exit(1);
	}

	close(1);
	fp = freopen(argv[1], "wb", stdout);
	if (fp == NULL) {
		perror("ccauthors: Unable to open output file\n");
		exit(1);
	}

	/*
	 * This allows us to invoke the command anywhere, and
	 * still be able to find the database files.
	 */
	(void)strcpy(path, BASE);
	(void)strcat(path, "/dbase.ascii");
        result = chdir(path);
        if (result != 0) {
                printf("CHDIR to %s failed\n", path);
                exit(1);
        }

	/*
	 * If NOTES isn't present, create it. Otherwise,
	 * open the file and lseek to the end of it and
	 * note the current offset.
	 */
	if ( stat("NOTES", &statbuf) == -1) {
		notes_fp = fopen("NOTES", "wb+");
	} else {
		notes_fp = fopen("NOTES", "rb+");
		notes_offset = statbuf.st_size;
		(void)fseek(notes_fp, notes_offset, SEEK_SET);
	}

	/*
	 * Parse the AUTHORS file
	 */
	search_file("AUTHORS");

	(void)fclose(notes_fp);
	exit(0);

	/* NOTREACHED */
	return(0);
}
