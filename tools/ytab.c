/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 */

static char sccsid[] = "@(#)ytab.c	1.3	06/05/97 SFdbase";


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sfdbase.h"

static char	current_title[8192];
static char	current_attr_name[8192];
static char	current_attr_val[8192];
static char	input_string[8192];
int		line_number	= 1;
object_t	*Objlist = NULL;
static object_t	*Objend = NULL;
static attr_t	*Attrlist = NULL;

extern char	configfile[];

static void
push_string(char *target)
{
	char	*ptr;
	char	*t2;

	/*
	 * First, translate any newline or tab to 
	 * a space.
	 */
	ptr = target;
	while(*ptr) {
		if ((*ptr == '\n') || (*ptr == '\t'))
			*ptr = ' ';
		ptr++;
	}

	/*
	 * Now concat the string into current_attr_val,
	 * purging multiple spaces on the fly.
	 */
	t2 = target;
	while( strstr(t2, "  ") ) {
		ptr = (char *)(strstr(t2, "  "));
		if (ptr) {
			ptr++;
			*ptr = 0;
		}

		/*
		 * Remove leading whitespace.
		 */
		while(*t2 == ' ') {
			t2++;
		}
		if (*t2) {
			(void)strcat(current_attr_val, t2);
		}
		t2 = ++ptr;
	}
	(void)strcat(current_attr_val, t2);
}


void
yyparse()
{
	int 		index;
	int 		input;
	attr_t		*Attr;
	object_t	*Obj;
	char		*ptr;

	/*CONSTCOND*/
	while(1) {

		/*
		 * Check for comments
		 */
		input = getchar();
		if (input == '#') {
			int input2;

			input2 = getchar();
			if (input2 == '#') {
				while( input2 != '\n') {
					input2 = getchar();
				}
				line_number++;
				continue;
			} else {
				ungetc(input2, stdin);
				ungetc(input, stdin);
			}
		} else {
			ungetc(input, stdin);
		}
			
		/*
		 * Read in the title, and put it in current_title
		 */
		index = 0;
		/*CONSTCOND*/
		while(1) {
			input = getchar();
			if (input == -1) {
				return;
			} else if (input == '\n') {
				if (index == 0) {
					(void)fprintf(stderr, "%s - Error line %d: blank line\n", 
						configfile, line_number);
					exit(1);
				} else if (index < 2) {
					(void)fprintf(stderr, "%s - Error line %d: malformed title\n", 
						configfile, line_number);
					exit(1);
				} else {
					line_number++;
				}

				/*
				 * Back up to the brace
				 */
				index--;
				while(current_title[index] != '{' ) {
					index--;
					if (index == 0) {
						(void)fprintf(stderr, "%s - Error line %d: '{' expected\n", 
							configfile, line_number);
						exit(1);
					}
				}
	
				/*
				 * Back up to the first space before the brace
				 */
				index--;
				while(current_title[index] == ' ' ) {
					index--;
				}
	
				/*
				 * Now Null terminate
				 */
				current_title[index+1] = 0;
#define DEBUG
#ifdef DEBUG
{
	unsigned char *tmp;

	tmp = current_title;
	while(*tmp) {
		if ( !isascii(*tmp) && (*tmp < 0xa1) ) {
			(void)fprintf(stderr, "%s - Error line %d: non-ascii character found\n", 
				configfile, line_number);
			exit(1);
		}
		tmp++;
	}
}
#endif
				break;
			} else {
				current_title[index] = (char)input;
				index++;
			}
		}

		/*
		 * Read in attribute name/value pairs
		 */
		/*CONSTCOND*/
		while(1) {
			input = getchar();
			if (input == -1) {
				return;
			} else if (input == '\n') {
				line_number++;
			} else if (input == '}') {

				/*
				 * Finish this record
				 */
				Obj = (object_t *)malloc( sizeof( object_t ));
				if ( Obj == NULL ) {
					(void)printf("Out of memory\n");
					exit(1);
				}
				Obj->ob_next = NULL;
				ptr = (char *)&(current_title[ strlen(current_title) - 1 ]);
				while( *ptr == ' ') {
					*ptr = 0;
					ptr--;
				}
				Obj->ob_name = (char *)malloc( strlen(current_title) + 1);
				(void)strcpy(Obj->ob_name, current_title); 
				Obj->ob_attrs = Attrlist;
				Attrlist = NULL;
				if (Objlist == NULL) {
					Objlist = Objend = Obj;
				} else {
					Objend->ob_next = Obj;
					Objend = Obj;
				}

				/*
				 * Flush out the newline
				 */
				/*CONSTCOND*/
				while(1) {
					input = getchar();
					if (input == -1) {
						return;
					} else if (input == '\n') {
						line_number++;
						break;
					}
				}
				break;
			}

			/*
			 * Flush leading whitespace
			 */
			/*CONSTCOND*/
			while(1) {
				input = getchar();
				if (input == -1) {
					return;
				} else if (input == '\n') {
					line_number++;
				} else if ((input != ' ') && (input != '\t')) {
					break;
				}
			}

			/*
			 * Read in attribute name
			 */
			index = 0;
			while (input != '=') {
				current_attr_name[index] = (char)input;
				index++;
				input = getchar();
				if (input == -1) {
					return;
				} else if (input == '\n') {
					line_number++;
				} else if (input == '|') {
					(void)fprintf(stderr, "%s - Error line %d: '=' expected. Found '|'\n", 
						configfile, line_number);
					exit(1);
				} else if (input == '{') {
					(void)fprintf(stderr, "%s - Error line %d: '=' expected. Found '{'\n", 
						configfile, line_number);
					exit(1);
				} else if (input == '}') {
					(void)fprintf(stderr, "%s - Error line %d: '=' expected. Found '}'\n", 
						configfile, line_number);
					exit(1);
				}
			}
			current_attr_name[index] = 0;

			/*
			 * Flush past the bar
			 */
			input = getchar();
			if (input == -1) {
				return;
			} else if (input != '|') {
				(void)fprintf(stderr, "%s - Error line %d: leading '|' expected\n", 
					configfile, line_number);
				exit(1);
			}


			/*
			 * Read in attribute value
			 */
			index = 0;
			current_attr_val[0] = 0;
			input = getchar();
			if (input == -1)
				return;
			if (input == '\n')
				line_number++;
			while (input != '|') {
				if (input == '=') {
					(void)fprintf(stderr, "%s - Error line %d: trailing '|' expected\n", 
						configfile, line_number);
					exit(1);
				}
				input_string[index] = (char)input;
				index++;
				input = getchar();
				if (input == -1)
					return;
				if (input == '\n')
					line_number++;
			}
			input_string[index] = 0;
			push_string(input_string);


			/*
			 * Now push this attribute pair on the Attrlist
			 */
			Attr = (attr_t *)malloc(sizeof(attr_t));
			if ( Attr == NULL ) {
				(void)printf("Out of memory\n");
				exit(1);
			}
			Attr->at_name[0] = current_attr_name[0];
			Attr->at_name[1] = current_attr_name[1];
			Attr->at_name[2] = 0;
			Attr->at_value = (char *)malloc( strlen(current_attr_val) + 1 );
			if (current_attr_val[0] == ' ') {
				(void)fprintf(stderr, "%s - Warning: leading space in attribute, line %d\n", 
					configfile, line_number);
			}
			if (current_attr_val[strlen(current_attr_val)-1] == ' ') {
				(void)fprintf(stderr, "%s - Warning: trailing space in attribute, line %d\n", 
					configfile, line_number);
			}
			(void)strcpy(Attr->at_value, current_attr_val);
			current_attr_val[0] = 0;
			Attr->at_next = Attrlist;
			Attrlist = Attr;

			/*
			 * Flush trailing characters.
			 */
			/*CONSTCOND*/
			while(1) {
				input = getchar();
				if (input == -1)
					return;
				if (input == '\n') {
					line_number++;
					break;
				} else { 
					(void)fprintf(stderr, "%s - Warning: trailing characters, line %d\n", 
						configfile, line_number);
				}
			}
		}
	}
}


void
parse_pubs(char *file)
{
	FILE *fp;

	fp = freopen(file, "r", stdin);
	if ( fp == NULL ) {
		(void)fprintf(stderr, "Can't open %s\n", file);
		exit(1);
	}

	(void)strcpy(configfile, file);
	yyparse();
	fclose(fp);
}
