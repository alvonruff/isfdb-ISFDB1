/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 */

static char sccsid[] = "@(#)AwGen.c	1.12	05/01/98 SFdbase";

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "sfdbase.h"

#ifdef CYGWIN
int	errno;
#endif

entry		entries[MAX_ENTRIES];
search_t	*title_list = NULL;
char		path[256];
char		lockpath[256];

void
dump_error()
{
	int fd, result;

	sprintf(path, "/tmp/isfdbco.%d", getpid());
#ifdef CYGWIN
	fd = open(path, O_RDONLY|O_BINARY);
#else
	fd = open(path, O_RDONLY);
#endif
	if (fd == -1) {
		return;
	}

	while(1) {
		char buffer[257];

		result = read(fd, buffer, 256);
		if (result > 0) {
			buffer[result] = 0;
			printf("%s", buffer);
		} else {
			break;
		}
	}
	fflush(stdout);
	close(fd);
	unlink(path);
}

int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	int	fd, result;
	char	award[32];
	char	owner[32];
	char	type[32];
	char	password[32];
	int	content_length;
	int	loop;
	int	max;
	int	locked = 0;
	int	unexpand = 0;
	char	*startptr;
	char	*endptr;
	char	*ptr;

	printf("Content-type: text/html\n\n");

	if(strcmp(getenv("REQUEST_METHOD"), "POST") ) {
		print_fatal("This script should only be referenced with a METHOD of POST.");
	}
	
	if(strcmp(getenv("CONTENT_TYPE"), "application/x-www-form-urlencoded")) {
		print_fatal("This script can only be used to decode form results.");
	}

	content_length = atoi(getenv("CONTENT_LENGTH"));
	for(loop=0; content_length && (!feof(stdin)); loop++) {
		max = loop;
		entries[loop].val = (char *)fmakeword(stdin, '&', &content_length);
		plustospace(entries[loop].val);
		unescape_url(entries[loop].val);
		entries[loop].name = (char *)makeword(entries[loop].val, '=');
	}

	for(loop=0; loop <= max; loop++) {
		if (strncmp(entries[loop].name, "password", 8) == 0) {
			if (entries[loop].val[0]) {
				strcpy(password, entries[loop].val);
			} else {
				password[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "award", 6) == 0) {
			if (entries[loop].val[0]) {
				strcpy(award, entries[loop].val);
			} else {
				award[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "owner", 5) == 0) {
			if (entries[loop].val[0]) {
				strcpy(owner, entries[loop].val);
			} else {
				owner[0] = 0;
			}
		} else if (strncmp(entries[loop].name, "lock", 4) == 0) {
			if (entries[loop].val[0]) {
				if ( strcmp(entries[loop].val, "locked") == 0) {
					locked   = 1;
					unexpand = 0;
				} else if ( strcmp(entries[loop].val, "unlocked") == 0) {
					locked   = 0;
					unexpand = 0;
				} else if ( strcmp(entries[loop].val, "unexpand") == 0) {
					locked   = 0;
					unexpand = 1;
				}
			}
		} else {
		}
	}

	if ( strncmp(award, "Hugo", 4) == 0) {
		strcpy(type, "HUGOS");
	} else if ( strncmp(award, "Nebula ", 6) == 0) {
		strcpy(type, "NEBULAS");
	} else if ( strncmp(award, "WFA", 3) == 0) {
		strcpy(type, "WFA");
	} else if ( strncmp(award, "Campbell", 8) == 0) {
		strcpy(type, "CAMPBELL");
	} else if ( strncmp(award, "Chesley", 7) == 0) {
		strcpy(type, "CHESLEY");
	} else if ( strncmp(award, "Analog", 6) == 0) {
		strcpy(type, "ANALOGAW");
	} else if ( strncmp(award, "Clarke", 6) == 0) {
		strcpy(type, "CLARKE");
	} else if ( strncmp(award, "Locus Poll", 10) == 0) {
		strcpy(type, "LOCUS");
	} else if ( strncmp(award, "Mythopoeic", 10) == 0) {
		strcpy(type, "MYTHO");
	} else if ( strncmp(award, "PK Dick", 7) == 0) {
		strcpy(type, "PKDICK");
	} else if ( strncmp(award, "Apollo", 6) == 0) {
		strcpy(type, "APOLLO");
	} else if ( strncmp(award, "Stoker", 6) == 0) {
		strcpy(type, "STOKER");
	} else if ( strncmp(award, "Aurora", 6) == 0) {
		strcpy(type, "AURORA");
	} else if ( strncmp(award, "Sturgeon", 8) == 0) {
		strcpy(type, "STURGEON");
	} else if ( strncmp(award, "Tiptree", 7) == 0) {
		strcpy(type, "TIPTREE");
	} else if ( strncmp(award, "Lambda", 6) == 0) {
		strcpy(type, "LAMBDA");
	} else if ( strncmp(award, "HOMer", 5) == 0) {
		strcpy(type, "HOMER");
	} else if ( strncmp(award, "Hall of Fame", 12) == 0) {
		strcpy(type, "BADFILE");
	} else if ( strncmp(award, "JWCA", 4) == 0) {
		strcpy(type, "JWCA");
	} else if ( strncmp(award, "Prometheus", 10) == 0) {
		strcpy(type, "PROMETHEUS");
	} else if ( strncmp(award, "RetroHugo", 9) == 0) {
		strcpy(type, "RETROH");
	} else if ( strncmp(award, "SFBC", 4) == 0) {
		strcpy(type, "SFBC");
	} else if ( strncmp(award, "SF Chronicle", 12) == 0) {
		strcpy(type, "CHRON");
	} else if ( strncmp(award, "BFA", 3) == 0) {
		strcpy(type, "BFA");
	} else if ( strncmp(award, "Ditmar", 6) == 0) {
		strcpy(type, "DITMAR");
	} else if ( strncmp(award, "Aurealis", 8) == 0) {
		strcpy(type, "AUREALIS");
	} else if ( strncmp(award, "AsimovR", 7) == 0) {
		strcpy(type, "ASIMOVR");
	} else if ( strncmp(award, "AsimovU", 7) == 0) {
		strcpy(type, "ASIMOVU");
	} else if ( strncmp(award, "Gandalf", 7) == 0) {
		strcpy(type, "GANDALF");
	} else if ( strncmp(award, "BSFA", 4) == 0) {
		strcpy(type, "BSFA");
	} else if ( strncmp(award, "Balrog", 6) == 0) {
		strcpy(type, "BALROG");
	} else if ( strncmp(award, "IFA", 4) == 0) {
		strcpy(type, "IFA");
	} else if ( strncmp(award, "Sidewise", 8) == 0) {
		strcpy(type, "SIDEWISE");
	} else if ( strncmp(award, "ComptonCrook", 12) == 0) {
		strcpy(type, "COMPTON");
	} else if ( strncmp(award, "Skylark", 7) == 0) {
		strcpy(type, "SKYLARK");
	} else if ( strncmp(award, "Gaughan", 7) == 0) {
		strcpy(type, "GAUGHAN");
	} else if ( strncmp(award, "Imaginaire", 7) == 0) {
		strcpy(type, "IMAGINAIRE");
	} else if ( strncmp(award, "IHG", 3) == 0) {
		strcpy(type, "IHG");
	} else if ( strncmp(award, "Deathrealm", 10) == 0) {
		strcpy(type, "DEATHR");
	} else if ( strncmp(award, "Rhysling", 8) == 0) {
		strcpy(type, "RHYSLING");
	}

	if ( strcmp(password, "abc123") ) {
		printf("<html><head>\n");
		printf("<title>Award Checkout Error</title>");
		printf("<body bgcolor=#ffffff>\n");
		printf("<b>*** Checkout Failed: Bad Password</b>\n");
		printf("</body></html>\n");
		exit(0);
	}

	result = chdir("awdb");
	if (result) {
		printf("<html><head>\n");
		printf("<title>Award Checkout Error</title>");
		printf("<body bgcolor=#ffffff>\n");
		printf("<b>*** Checkout Failed: Can't cd to awdb directory. (errno=%d)</b>\n", errno);
		printf("</body></html>\n");
		exit(0);
	}

	sprintf(lockpath, "RCS/lock.%s", type);

	(void)unlink(type);
	if (locked) {
#ifdef CYGWIN
		fd = open(lockpath, O_RDONLY|O_BINARY);
#else
		fd = open(lockpath, O_RDONLY);
#endif
		if (fd == -1) {
			sprintf(path, "/usr/bin/co -l %s > /tmp/isfdbco.%d 2>&1", type, getpid());
		} else {
			bzero(lockpath, 32);
			result = read(fd, lockpath, 32);
			printf("<html><head>\n");
			printf("<title>Award Checkout Error</title>");
			printf("<body bgcolor=#ffffff>\n");
                        if (result > 0) {
			        printf("<b>*** Checkout Failed: The file (%s) is locked by %s\n", type, lockpath);
                        } else {
			        printf("<b>*** Checkout Failed:\n");
                        }
			printf("<br><p>\n");
			printf("</b></body></html>\n");
			exit(0);
		}
	} else {
		sprintf(path, "/usr/bin/co %s > /tmp/isfdbco.%d 2>&1", type, getpid());
	}

	result = system(path);
	if (result) {
		printf("<html><head>\n");
		printf("<title>Award Checkout Error</title>");
		printf("<body bgcolor=#ffffff>\n");
		printf("<b>*** Checkout Failed: Can't extract file (%s).\n", type);
		printf("<br><p>\n");
		dump_error();
		printf("</b></body></html>\n");
		exit(0);
	}

	if (locked) {
#ifdef CYGWIN
		fd = open(lockpath, O_RDWR|O_CREAT|O_BINARY, 0666);
#else
		fd = open(lockpath, O_RDWR|O_CREAT, 0666);
#endif
		if (fd == -1) {
			printf("<html><head>\n");
			printf("<title>Award Checkout Error</title>");
			printf("<body bgcolor=#ffffff>\n");
			printf("<b>*** Checkout Failed: Can't create lockfile %s. (errno=%d)</b>\n", 
				lockpath, errno);
			printf("</body></html>\n");
			exit(0);
		}
		sprintf(lockpath, "%s\n", owner);
		write(fd, lockpath, strlen(lockpath));
		close(fd);
	}

	sprintf(path, "%s", type);
#ifdef CYGWIN
	fd = open(path, O_RDONLY|O_BINARY);
#else
	fd = open(path, O_RDONLY);
#endif
	if (fd == -1) {
		printf("<html><head>\n");
		printf("<title>Award Checkout Error</title>");
		printf("<body bgcolor=#ffffff>\n");
		printf("<b>*** Checkout Failed: Can't open %s. (errno=%d)</b>\n", path, errno);
		printf("</body></html>\n");
		exit(0);
	}

	while(1) {
		char buffer[257];

		result = read(fd, buffer, 256);
		if (result > 0) {
			buffer[result] = 0;
			printf("%s", buffer);
		} else {
			break;
		}
	}
	fflush(stdout);
	close(fd);

#ifdef REMOVE
	printf("<html><head>\n");
	printf("<title>Award Checkin: %s </title></head>\n", type );
	printf("<body bgcolor=#ffffff>\n");
	printf("<h1>Submitting [%s] for Checkin</h1>\n", type);
	printf("<hr>\n");
	printf("Select a local file for submission:\n");
	printf("<form\n");
	printf("  ENCTYPE=\"multipart/form-data\"\n");
	printf("  METHOD=\"POST\"\n");
	printf("  ACTION=\"http:/%s/checkin.cgi\">\n", HTFAKE);
	printf("<br>\n");
	printf("<p>\n");
	printf("<INPUT\n");
	printf("  NAME=\"%s\"\n", type);
	printf("  TYPE=\"file\"\n");
	printf("<BR>\n");
	printf("<INPUT TYPE=\"submit\" VALUE=\"Submit file for checkin\">\n");
	printf("<INPUT TYPE=\"reset\" VALUE=\"Reset\">\n");
	printf("</FORM>\n");
#endif

	return(0);
}
