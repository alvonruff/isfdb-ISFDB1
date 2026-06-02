#
#     (C) COPYRIGHT 1995-2000   Al von Ruff
#         ALL RIGHTS RESERVED
#
#     The copyright notice above does not evidence any actual or
#     intended publication of such source code.
#
#     sccsid[] = "@(#)Makefile	1.14	06/05/97 SFdbase"
#
#/

BOOKSORT_OBJS	= booksort.o

CCDBASE_OBJS	= ccdbase.o

TILE_OBJ	= tileawards.o

MOSTSF_OBJ      = mostsf2.o \
		  ytab.o \
		  db_lib.o \
		  print.o \
		  sort.o \
		  parse.o

INFO_OBJS	= info.o \
		  print.o \
		  db_lib.o \
		  sort.o \
		  parse.o

IMPORT_OBJS	= import.o \
		  print.o \
		  db_lib.o \
		  sort.o \
		  parse.o

CCAWARDS_OBJS	= ccawards.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  doattr.o \
		  parse.o

CCTITLES_OBJ	= cctitles.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  doattr.o \
		  parse.o

CCC_OBJ		= merge.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

CHECK_OBJ	= checkauthors.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

FIX_OBJ		= fixessays.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

CAP_OBJ		= cap.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

NEWDB_OBJ	= newdb.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

CONT_OBJ	= content.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

AWDIFF_OBJ	= awdiff.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

AWREPT_OBJ	= awrept.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

LEXINT_OBJ	= lexint.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

LEXEXT_OBJ	= lexext.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

PUBINT_OBJ	= pubint.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

PUBEXT_OBJ	= pubext.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

PUBDUPS_OBJ	= pubdups.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

CCPUBS_OBJS	= ccpubs.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  doattr.o \
		  parse.o

CCFORTH_OBJS	= ccforth.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  doattr.o \
		  parse.o

CFRTH_OBJS	= checkforth.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  doattr.o \
		  parse.o

MKFOR_OBJS	= makeforth.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  doattr.o \
		  parse.o

CHKPUBS_OBJ	= checkpubs.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

CCAUTH_OBJS	= ccauthors.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  doattr.o \
		  parse.o

CCREVW_OBJS	= ccreviews.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  doattr.o \
		  parse.o

CCINTV_OBJS	= ccinterv.o \
		  ytab.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  doattr.o \
		  parse.o

TARGETS		= booksort \
		  cctitles \
		  ccdbase \
		  makeforth \
		  merge \
		  checkauthors \
		  fixessays \
		  cap \
		  lexint \
		  content \
		  awdiff \
		  awrept \
		  newdb \
		  newdata \
		  lexext \
		  pubint \
		  mostsf2 \
		  pubext \
		  pubdups \
		  ccpubs \
		  ccforth \
		  checkforth \
		  checkpubs \
		  ccauthors \
		  ccreviews \
		  ccinterv \
		  ccawards \
		  info \
		  import \
		  tileawards

all:		links $(TARGETS)

booksort:	$(BOOKSORT_OBJS)
		$(CC) $(CFLAGS) -o booksort $(BOOKSORT_OBJS)

ccdbase:	$(CCDBASE_OBJS)
		$(CC) $(CFLAGS) -o ccdbase $(CCDBASE_OBJS)

ccawards:	$(CCAWARDS_OBJS)
		$(CC) $(CFLAGS) -o ccawards $(CCAWARDS_OBJS)

cctitles:	$(CCTITLES_OBJ)
		$(CC) $(CFLAGS) -o cctitles $(CCTITLES_OBJ)

merge:		$(CCC_OBJ)
		$(CC) $(CFLAGS) -o merge $(CCC_OBJ)

checkauthors:	$(CHECK_OBJ)
		$(CC) $(CFLAGS) -o checkauthors $(CHECK_OBJ)

fixessays:	$(FIX_OBJ)
		$(CC) $(CFLAGS) -o fixessays $(FIX_OBJ)

cap:		$(CAP_OBJ)
		$(CC) $(CFLAGS) -o cap $(CAP_OBJ)

pubdups:	$(PUBDUPS_OBJ)
		$(CC) $(CFLAGS) -o pubdups $(PUBDUPS_OBJ)

content:	$(CONT_OBJ)
		$(CC) $(CFLAGS) -o content $(CONT_OBJ)

awdiff:		$(AWDIFF_OBJ)
		$(CC) $(CFLAGS) -o awdiff $(AWDIFF_OBJ)

awrept:		$(AWREPT_OBJ)
		$(CC) $(CFLAGS) -o awrept $(AWREPT_OBJ)

newdb:		$(NEWDB_OBJ)
		$(CC) $(CFLAGS) -o newdb $(NEWDB_OBJ)

lexint:		$(LEXINT_OBJ)
		$(CC) $(CFLAGS) -o lexint $(LEXINT_OBJ)

lexext:		$(LEXEXT_OBJ)
		$(CC) $(CFLAGS) -o lexext $(LEXEXT_OBJ)

pubint:		$(PUBINT_OBJ)
		$(CC) $(CFLAGS) -o pubint $(PUBINT_OBJ)

pubext:		$(PUBEXT_OBJ)
		$(CC) $(CFLAGS) -o pubext $(PUBEXT_OBJ)

info:		$(INFO_OBJS)
		$(CC) $(CFLAGS) -o info $(INFO_OBJS)

import:		$(IMPORT_OBJS)
		$(CC) $(CFLAGS) -o import $(IMPORT_OBJS)

ccpubs:		$(CCPUBS_OBJS)
		$(CC) $(CFLAGS) -o ccpubs $(CCPUBS_OBJS)

ccforth:	$(CCFORTH_OBJS)
		$(CC) $(CFLAGS) -o ccforth $(CCFORTH_OBJS)

checkforth:	$(CFRTH_OBJS)
		$(CC) $(CFLAGS) -o checkforth $(CFRTH_OBJS)

makeforth:	$(MKFOR_OBJS)
		$(CC) $(CFLAGS) -o makeforth $(MKFOR_OBJS)

checkpubs:	$(CHKPUBS_OBJ)
		$(CC) $(CFLAGS) -o checkpubs $(CHKPUBS_OBJ)

ccauthors:	$(CCAUTH_OBJS)
		$(CC) $(CFLAGS) -o ccauthors $(CCAUTH_OBJS)

ccreviews:	$(CCREVW_OBJS)
		$(CC) $(CFLAGS) -o ccreviews $(CCREVW_OBJS)

ccinterv:	$(CCINTV_OBJS)
		$(CC) $(CFLAGS) -o ccinterv $(CCINTV_OBJS)

newdata:	newdata.c
		$(CC) $(CFLAGS) -o newdata newdata.c

tileawards:	$(TILE_OBJ)
		$(CC) $(CFLAGS) -O -o tileawards $(TILE_OBJ)

mostsf2:	$(MOSTSF_OBJ)
		$(CC) $(CFLAGS) -O -o mostsf2 $(MOSTSF_OBJ)

.c.o:
		$(CC) -c $(CFLAGS) $(INC) $*.c -DBASE=\"$(BASE)\" -DHTFAKE=\"$(HTFAKE)\" -DHTMLLOC=\"$(HTMLLOC)\"

links:
		rm -f ../src/sfdbase.h
		rm -f ../src/parse.c
		rm -f ../src/print.c
		rm -f ../src/sort.c
		rm -f ../src/db_lib.c
		ln  sfdbase.h ../src/sfdbase.h
		ln  parse.c ../src/parse.c
		ln  print.c ../src/print.c
		ln  sort.c ../src/sort.c
		ln  db_lib.c ../src/db_lib.c


install:	links all
		chmod 755 ccstats
		cp ccstats ../bin
		if [ -f booksort.exe ]; \
		then \
			cp *.exe ../bin ; \
		else \
			cp $(TARGETS) ../bin ; \
		fi;

clean:
		rm -f *.o

clobber:	clean
		rm -f $(TARGETS) *.exe
