/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)booksort.c	1.6	06/05/97 SFdbase";

#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/*
 * This program sorts a list of data, ignoring leading articles.
 * That is: "The Fine Title"  comes before "Last Title", since
 * the "The" is ignored. Maximum line length is 8k.
 */


/*
 * Data structure which holds a line of data. The sorted list
 * takes the form of a binary tree for performance reasons, hence 
 * the left and right pointers.
 */
typedef struct entry {
	char 	     *entry_buf;
	struct entry *left;
	struct entry *right;
} entry_t;
static struct entry *head = NULL;

static char input_buffer[8192];

/*
 * insert() inserts a data record into the binary tree
 */
static void
insert(current)
	struct entry *current;
{
	struct entry *tmp;
	char *newstring;
	char *oldstring;
	int  value;

	/*
	 * Initialize the left and right pointers.
	 */
	current->left = NULL;
	current->right = NULL;

	/*
	 * If there are no records in the binary tree,
	 * it's a no-brainer.
	 */
	if (head == NULL) {
		head = current;
		return;
	}
	
	/*
	 * Set up newstring such that leading articles are
	 * ignored.
	 */
	newstring = current->entry_buf;
	if ((newstring[0] == 'A') || (newstring[0] == 'T') ||
	    (newstring[0] == 'a') || (newstring[0] == 't')) {
		if (strncmp(newstring,"The ", 4) == 0)
			newstring += 4;
		else if (strncmp(newstring,"A ", 2) == 0)
			newstring += 2;
		else if (strncmp(newstring,"An ", 3) == 0)
			newstring += 3;
		else if (strncmp(newstring,"the ", 4) == 0)
			newstring += 4;
		else if (strncmp(newstring,"a ", 2) == 0)
			newstring += 2;
		else if (strncmp(newstring,"an ", 3) == 0)
			newstring += 3;
	}


	/*
	 * Now traverse the binary tree looking for leaf on
	 * which to hang the new record.
	 */
	tmp = head;
	while (tmp) {

		/*
		 * Ignore the leading article on the record
		 * we are checking.
		 */
		oldstring = tmp->entry_buf;
		if ((oldstring[0] == 'A') || (oldstring[0] == 'T') ||
		    (oldstring[0] == 'a') || (oldstring[0] == 't')) {
			if (strncmp(oldstring,"The ", 4) == 0)
				oldstring += 4;
			else if (strncmp(oldstring,"A ", 2) == 0)
				oldstring += 2;
			else if (strncmp(oldstring,"An ", 3) == 0)
				oldstring += 3;
			else if (strncmp(oldstring,"the ", 4) == 0)
				oldstring += 4;
			else if (strncmp(oldstring,"a ", 2) == 0)
				oldstring += 2;
			else if (strncmp(oldstring,"an ", 3) == 0)
				oldstring += 3;
		}

		/*
		 * If the new record is "less" than the target, then
		 * put the new record on the left subtree. Otherwise,
		 * it goes on the right. If the subtree is NULL, simply
		 * place the new record there; otherwise follow the link.
		 */
		value = strcmp(newstring, oldstring);
		if ( value <= 0) {
			if (tmp->left) {
				tmp = tmp->left;
			} else {
				tmp->left = current;
				return;
			}
		} else {
			if (tmp->right) {
				tmp = tmp->right;
			} else {
				tmp->right = current;
				return;
			}
		}
	}
}


/*
 * dumplist() dumps the binary tree to stdout. It
 * is a recursive function, first outputing the
 * left side of the subtree, then the target
 * record, then the right side of the subtree.
 */
static void
dumplist(struct entry *tmp)
{
	if(tmp->left) {
		dumplist(tmp->left);
	}
	(void)printf("%s\n", tmp->entry_buf);
	if(tmp->right) {
		dumplist(tmp->right);
	}
}


int
main(argc, argv)
	int argc;
	char *argv[];
{
	struct entry	*current;	/* Data structure for the current title */
	char		*tmp;		/* Points to the current position	*/
	FILE		*fp;		/* Points to the input file		*/
	int		input = 0;	/* Holds the current character		*/
	FILE		*fpout;

	/*
	 * Check the argument count, then go ahead and open the file
	 */
	if (argc != 3) {
		(void)printf("Usage: booksort filename outfile\n");
		exit(1);
	}
	fp = fopen(argv[1], "rb");
	if (fp == NULL) {
		perror("Couldn't open database");
		exit(1);
	}
	close(1);
	fpout = freopen(argv[2], "wb", stdout);
	if (fpout == NULL) {
		perror("Couldn't open output file");
		exit(1);
	}

	/*CONSTCOND*/
	while(1) {

		/*
		 * Allocate a data structure for the next title
		 */
		current = (struct entry *)malloc(sizeof(struct entry));
		if (current == NULL) {
			(void)printf("booksort: out of memory\n");
			exit(1);
		}

		/*
		 * Set up tmp to point into the character array, read
		 * in the title string, then NULL-terminate it.
		 */
		tmp = input_buffer;

		/*CONSTCOND*/
		while(1) {
			input = getc(fp);
			if (input == -1)
				goto dumpit;
			if (input == '\n')
				break;
			*tmp  = (char)input;
			tmp++;
		}
		*tmp = input = 0;

		/*
		 * Allocate just enough memory to hold this title and
		 * move the title into the new memory.
		 */
		current->entry_buf = (char *)malloc( strlen(input_buffer) + 1);
		(void)strcpy(current->entry_buf, input_buffer);

		/*
		 * Push the title data structure onto the binary tree
		 */
		insert(current);
	}

dumpit:
	dumplist(head);
	exit(0);
	return(0);
}
