#
#     (C) COPYRIGHT 1995-2000   Al von Ruff
#         ALL RIGHTS RESERVED
#
#     The copyright notice above does not evidence any actual or
#     intended publication of such source code.
#
#     sccsid[] = "@(#)Makefile	1.15	01/28/98 SFdbase"
#

PLIST_OBJS	= plist.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

SFLIST_OBJS	= sflist.o \
		  print.o \
		  sort.o \
		  db_lib.o \
		  parse.o

PWORK_OBJS	= pwork.o \
		  awards.o \
		  parse.o \
		  print.o \
		  db_lib.o \
		  sort.o

RWORK_OBJS	= rwork.o \
		  awards.o \
		  parse.o \
		  print.o \
		  db_lib.o \
		  sort.o

PINFO_OBJS	= pinfo.o \
		  awards.o \
		  parse.o \
		  print.o \
		  db_lib.o \
		  sort.o

DENT_OBJS	= dataentry.o \
		  awards.o \
		  parse.o \
		  print.o \
		  db_lib.o \
		  sort.o

CINFO_OBJS	= cinfo.o \
		  awards.o \
		  parse.o \
		  print.o \
		  db_lib.o \
		  sort.o

XX_OBJS		= exact_author.o \
		  awards.o \
		  parse.o \
		  print.o \
		  db_lib.o \
		  sort.o

AEXACTAUTH_OBJS	= a_exact_author.o \
		  awards.o \
		  parse.o \
		  print.o \
		  db_lib.o \
		  sort.o

PBIBLIO_OBJS	= pbiblio.o \
		  awards.o \
		  parse.o \
		  print.o \
		  db_lib.o \
		  sort.o

AWYEAR_OBJS	= AwYear.o \
		  Aw.o \
		  parse.o \
		  print.o \
		  db_lib.o \
		  sort.o

AWGEN_OBJS	= AwGen.o \
		  Aw.o \
		  db_lib.o \
		  parse.o \
		  print.o \
		  sort.o

SEARCH_OBJS	= search.o \
		  db_lib.o \
		  parse.o \
		  sort.o \
		  print.o

SELEAW_OBJS	= selectaw.o \
		  db_lib.o \
		  parse.o \
		  sort.o \
		  print.o

UPDATEDB_OBJS	= updatedb.o \
		  db_lib.o \
		  parse.o \
		  sort.o \
		  print.o

UPLOAD_OBJ	= upload.o \
		  db_lib.o \
		  parse.o \
		  sort.o \
		  print.o

CHCKIN_OBJ	= checkin.o \
		  db_lib.o \
		  parse.o \
		  sort.o \
		  print.o

CHCKOUT_OBJ	= checkout.o \
		  db_lib.o \
		  parse.o \
		  sort.o \
		  print.o

SUBMIT_OBJS	= submit.o \
		  print.o \
		  sort.o \
		  parse.o \
		  db_lib.o

FORTH_OBJS	= forthcoming.o \
		  parse.o \
		  sort.o \
		  db_lib.o \
		  print.o

PSERIES_OBJS	= pseries.o \
		  parse.o \
		  print.o \
		  db_lib.o \
		  sort.o

SYN_OBJS	= psynop.o \
		  parse.o \
		  db_lib.o \
		  sort.o \
		  print.o

TARGETS		= cinfo.cgi \
		  pwork.cgi \
		  rwork.cgi \
		  pinfo.cgi \
		  forthcoming.cgi \
		  dataentry.cgi \
		  plist.cgi \
		  sflist \
		  pseries.cgi \
		  exact_author.cgi \
		  a_exact_author.cgi \
		  pbiblio.cgi \
		  AwYear.cgi \
		  AwGen.cgi \
		  search.cgi \
		  selectaw.cgi \
		  updatedb.cgi \
		  checkin.cgi \
		  upload.cgi \
		  checkout.cgi \
		  psynop.cgi \
		  submit.cgi

TARGETS2	= cinfo.cgi.exe \
		  pwork.cgi.exe \
		  rwork.cgi.exe \
		  pinfo.cgi.exe \
		  forthcoming.cgi.exe \
		  dataentry.cgi.exe \
		  plist.cgi.exe \
		  sflist \
		  pseries.cgi.exe \
		  exact_author.cgi.exe \
		  a_exact_author.cgi.exe \
		  pbiblio.cgi.exe \
		  AwYear.cgi.exe \
		  AwGen.cgi.exe \
		  search.cgi.exe \
		  selectaw.cgi.exe \
		  updatedb.cgi.exe \
		  checkin.cgi.exe \
		  upload.cgi.exe \
		  checkout.cgi.exe \
		  psynop.cgi.exe \
		  submit.cgi.exe

all:		$(TARGETS)

plist.cgi:	$(PLIST_OBJS)
		$(CC) $(LDFLAGS) -o plist.cgi $(PLIST_OBJS)

plist.cgi.exe:	$(PLIST_OBJS)
		$(CC) $(LDFLAGS) -o plist.cgi.exe $(PLIST_OBJS)

sflist:		$(SFLIST_OBJS)
		$(CC) $(LDFLAGS) -o sflist $(SFLIST_OBJS)

cinfo.cgi:	$(CINFO_OBJS)
		$(CC) $(LDFLAGS) -o cinfo.cgi $(CINFO_OBJS)

cinfo.cgi.exe:	$(CINFO_OBJS)
		$(CC) $(LDFLAGS) -o cinfo.cgi.exe $(CINFO_OBJS)

dataentry.cgi:	$(DENT_OBJS)
		$(CC) $(LDFLAGS) -o dataentry.cgi $(DENT_OBJS)

dataentry.cgi.exe:	$(DENT_OBJS)
		$(CC) $(LDFLAGS) -o dataentry.cgi.exe $(DENT_OBJS)

pwork.cgi:	$(PWORK_OBJS)
		$(CC) $(LDFLAGS) -o pwork.cgi $(PWORK_OBJS)

pwork.cgi.exe:	$(PWORK_OBJS)
		$(CC) $(LDFLAGS) -o pwork.cgi.exe $(PWORK_OBJS)

rwork.cgi:	$(RWORK_OBJS)
		$(CC) $(LDFLAGS) -o rwork.cgi $(RWORK_OBJS)

rwork.cgi.exe:	$(RWORK_OBJS)
		$(CC) $(LDFLAGS) -o rwork.cgi.exe $(RWORK_OBJS)

pinfo.cgi:	$(PINFO_OBJS)
		$(CC) $(LDFLAGS) -o pinfo.cgi $(PINFO_OBJS)

pinfo.cgi.exe:	$(PINFO_OBJS)
		$(CC) $(LDFLAGS) -o pinfo.cgi.exe $(PINFO_OBJS)

exact_author.cgi:	$(XX_OBJS)
		$(CC) $(LDFLAGS) -o exact_author.cgi $(XX_OBJS)

exact_author.cgi.exe:	$(XX_OBJS)
		$(CC) $(LDFLAGS) -o exact_author.cgi.exe $(XX_OBJS)

a_exact_author.cgi:	$(AEXACTAUTH_OBJS)
		$(CC) $(LDFLAGS) -o a_exact_author.cgi $(AEXACTAUTH_OBJS)

a_exact_author.cgi.exe:	$(AEXACTAUTH_OBJS)
		$(CC) $(LDFLAGS) -o a_exact_author.cgi.exe $(AEXACTAUTH_OBJS)

pbiblio.cgi:	$(PBIBLIO_OBJS)
		$(CC) $(LDFLAGS) -o pbiblio.cgi $(PBIBLIO_OBJS)

pbiblio.cgi.exe:	$(PBIBLIO_OBJS)
		$(CC) $(LDFLAGS) -o pbiblio.cgi.exe $(PBIBLIO_OBJS)

AwYear.cgi:	$(AWYEAR_OBJS)
		$(CC) $(LDFLAGS) -o AwYear.cgi $(AWYEAR_OBJS)

AwYear.cgi.exe:	$(AWYEAR_OBJS)
		$(CC) $(LDFLAGS) -o AwYear.cgi.exe $(AWYEAR_OBJS)

AwGen.cgi:	$(AWGEN_OBJS)
		$(CC) $(LDFLAGS) -o AwGen.cgi $(AWGEN_OBJS)

AwGen.cgi.exe:	$(AWGEN_OBJS)
		$(CC) $(LDFLAGS) -o AwGen.cgi.exe $(AWGEN_OBJS)

forthcoming.cgi:	$(FORTH_OBJS)
		$(CC) $(LDFLAGS) -o forthcoming.cgi $(FORTH_OBJS)

forthcoming.cgi.exe:	$(FORTH_OBJS)
		$(CC) $(LDFLAGS) -o forthcoming.cgi.exe $(FORTH_OBJS)

search.cgi:	$(SEARCH_OBJS)
		$(CC) $(LDFLAGS) -o search.cgi $(SEARCH_OBJS)

search.cgi.exe:	$(SEARCH_OBJS)
		$(CC) $(LDFLAGS) -o search.cgi.exe $(SEARCH_OBJS)

updatedb.cgi:	$(UPDATEDB_OBJS)
		$(CC) $(LDFLAGS) -o updatedb.cgi $(UPDATEDB_OBJS)

updatedb.cgi.exe:	$(UPDATEDB_OBJS)
		$(CC) $(LDFLAGS) -o updatedb.cgi.exe $(UPDATEDB_OBJS)

selectaw.cgi:	$(SELEAW_OBJS)
		$(CC) $(LDFLAGS) -o selectaw.cgi $(SELEAW_OBJS)

selectaw.cgi.exe:	$(SELEAW_OBJS)
		$(CC) $(LDFLAGS) -o selectaw.cgi.exe $(SELEAW_OBJS)

upload.cgi:	$(UPLOAD_OBJ)
		$(CC) $(LDFLAGS) -o upload.cgi $(UPLOAD_OBJ)

upload.cgi.exe:	$(UPLOAD_OBJ)
		$(CC) $(LDFLAGS) -o upload.cgi.exe $(UPLOAD_OBJ)

checkin.cgi:	$(CHCKIN_OBJ)
		$(CC) $(LDFLAGS) -o checkin.cgi $(CHCKIN_OBJ)

checkin.cgi.exe:	$(CHCKIN_OBJ)
		$(CC) $(LDFLAGS) -o checkin.cgi.exe $(CHCKIN_OBJ)

checkout.cgi:	$(CHCKOUT_OBJ)
		$(CC) $(LDFLAGS) -o checkout.cgi $(CHCKOUT_OBJ)

checkout.cgi.exe:	$(CHCKOUT_OBJ)
		$(CC) $(LDFLAGS) -o checkout.cgi.exe $(CHCKOUT_OBJ)

submit.cgi:	$(SUBMIT_OBJS)
		$(CC) $(LDFLAGS) -o submit.cgi $(SUBMIT_OBJS)

submit.cgi.exe:	$(SUBMIT_OBJS)
		$(CC) $(LDFLAGS) -o submit.cgi.exe $(SUBMIT_OBJS)

pseries.cgi:	$(PSERIES_OBJS)
		$(CC) $(LDFLAGS) -o pseries.cgi $(PSERIES_OBJS)

pseries.cgi.exe:	$(PSERIES_OBJS)
		$(CC) $(LDFLAGS) -o pseries.cgi.exe $(PSERIES_OBJS)

psynop.cgi:	$(SYN_OBJS)
		$(CC) $(LDFLAGS) -o psynop.cgi $(SYN_OBJS)

psynop.cgi.exe:	$(SYN_OBJS)
		$(CC) $(LDFLAGS) -o psynop.cgi.exe $(SYN_OBJS)

.c.o:
		$(CC) -c $(CFLAGS) $(INC) -DBASE=\"$(BASE)\" -DHTFAKE=\"$(HTFAKE)\" \
			-DHTMLLOC=\"$(HTMLLOC)\" -DCGIBIN=\"$(CGIBIN)\" $*.c

install:	all
		if [ -f sflist.exe ]; \
		then \
			cp *.exe ../bin ; \
		else \
			cp $(TARGETS) ../bin ; \
		fi;


clean:
		rm -f *.o

clobber:	clean
		rm -f $(TARGETS) *.exe
