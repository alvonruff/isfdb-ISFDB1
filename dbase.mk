#
#     (C) COPYRIGHT 1995-1998   Al von Ruff
#         ALL RIGHTS RESERVED
#
#     The copyright notice above does not evidence any actual or
#     intended publication of such source code.
#
#     sccsid[] = "@(#)dbase.mk	1.4	06 Dec 1996 SFdbase"
#

include .BASEDIR

all:
	cd tools && $(MAKE) all "CC=$(CC)" "CFLAGS=$(CFLAGS)" "BASE=$(BASE)" \
		"HTFAKE=$(HTFAKE)" "LDFLAGS=$(LDFLAGS)" "HTMLLOC=$(HTMLLOC)"
	cd tools && $(MAKE) install 
	cd src && $(MAKE) all "CC=$(CC)" "CFLAGS=$(CFLAGS)" "BASE=$(BASE)" \
		"HTFAKE=$(HTFAKE)" "LDFLAGS=$(LDFLAGS)" "HTMLLOC=$(HTMLLOC)" "CGIBIN=$(CGIBIN)"
	cd src && $(MAKE) install "CC=$(CC)" "CFLAGS=$(CFLAGS)" "BASE=$(BASE)" \
		"HTFAKE=$(HTFAKE)" "LDFLAGS=$(LDFLAGS)" "HTMLLOC=$(HTMLLOC)"

dbase:
	cd bin && ./ccdbase "$(BASE)"

dbase2:
	cd bin && ./ccdbase -n "$(BASE)"

stats:
	chmod 755 bin/ccstats
	cd bin && ./ccstats "$(BASE)"

install_htmls:
	chmod 755 htmls/make_htmls
	cd htmls && ./make_htmls "$(HTMLLOC)" "$(DATABASE)" "$(CFLAGS)"
	rm -f $(HTMLDIR)/*.html
	mv htmls/*.html $(HTMLDIR)
	cp htmls/*.gif $(HTMLDIR)

install_data:
	cp dbase.compiled/* $(CGIBIN)
	cp dbase.compiled/.DATE $(CGIBIN)

install_bin:
	chmod 755 src/makelinks
	cp bin/*.cgi $(CGIBIN)

clean:
	cd src && $(MAKE) clean

clobber:
	rm -rf SFdbase
	rm -f *.Z
	cd src && $(MAKE) clobber
	cd tools && $(MAKE) clobber
	cd bin && rm -f *
