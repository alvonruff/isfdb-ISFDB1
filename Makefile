#
#     (C) COPYRIGHT 1995-1998   Al von Ruff
#         ALL RIGHTS RESERVED
#
#     The copyright notice above does not evidence any actual or
#     intended publication of such source code.
#
#     sccsid[] = "@(#)Makefile	1.3	06 Dec 1996 SFdbase"
#

# The ISFDB has two distinct Web areas:
#
#	[1] The top-level htmls, which are actual files, and
#	[2] The database generated htmls, which are just formatted
#           dynamic data.
#
# This means that the top-level htmls can be located on a different
# system than the database itself. Since both the htmls and database 
# are intended to be portable, the following defines are used to alter 
# the file-based htmls, and to generate the dynamic htmls:
#
# HTMLDIR -  should be the local pathname to the directory where the
#            top-level htmls should be installed. Used to install the
#            altered top-level htmls.
# HTMLLOC -  should be set to hostname/directory path by which the 
#            above htmls are referred to. Used to munge the raw
#            top-level htmls into a usable format tailored for the
#            hosting system.
# CGIBIN   - Location to install the database binaries on the compiling system.
#            Some internet providers put the cgi-bin binaries on another host,
#            so this is where the local copies should go
# DATABASE - should be set to the "hostname:<optional port>" where the
#            database itself is installed. The default port is the
#            http daemon port
# HTFAKE   - should be set to the "/hostname/path" part of the URL which
#            leads the outside world to the CGIBIN binaries. May be the
#            same as CGIBIN, except it needs to have a leading "/".
#

#------ Modern Linux installation
CC              = cc
CFLAGS          = -O -DLINUX -DPOST_METHOD -DCHDIR
LDFLAGS         =
HTMLDIR         =/var/www/html
HTMLLOC         =/localhost
CGIBIN          =/var/www/cgi-bin
DATABASE        =localhost\/cgi-bin
HTFAKE          =/localhost\/cgi-bin
#------ SunOS SFSite installation
#CC              = cc
#CFLAGS          = -O -DLINUX -DPOST_METHOD -DCHDIR
#LDFLAGS         =
#HTMLDIR         =/proj/www/home/avonruff/public_html
#HTMLLOC         =www.urbana.css.mot.com\/home\/avonruff
#CGIBIN          =/home/avonruff/SFdbase/dbase.compiled
#DATABASE        =www.sfsite.com\/isfdb-bin
#HTFAKE          =/www.sfsite.com\/isfdb-bin
#------ AIX home system installation
#CC		= /usr/bin/cc
#CFLAGS		= -O -DLINUX -DPOST_METHOD
#LDFLAGS	= 
#HTMLDIR	=/home/avonruff/.html
#HTMLLOC	=www.cu-online.com\/~avonruff
#CGIBIN		=/home/avonruff/.html/cgi-bin
#DATABASE	=www.cu-online.com\/avon-bin
#HTFAKE		=/www.cu-online.com/avon-bin
#------ LINUX cu-online installation
#CC		= /usr/bin/cc
#CFLAGS		= -O -DLINUX -DPOST_METHOD
#LDFLAGS	= -static
#HTMLDIR	=/home/avonruff/.html
#HTMLLOC	=www.cu-online.com\/~avonruff
#CGIBIN		=/home/avonruff/.html/cgi-bin
#DATABASE	=www.cu-online.com\/avon-bin
#HTFAKE		=/www.cu-online.com/avon-bin

all:		doinclude
		$(MAKE) -f dbase.mk all

dbase:		doinclude
		$(MAKE) -f dbase.mk dbase

dbase2:		doinclude
		$(MAKE) -f dbase.mk dbase2

stats:		doinclude
		$(MAKE) -f dbase.mk stats

install:	doinclude
		$(MAKE) -f dbase.mk install_bin
		$(MAKE) -f dbase.mk install_htmls
		$(MAKE) -f dbase.mk install_data

clean:		doinclude
		$(MAKE) -f dbase.mk clean

clobber:	doinclude
		$(MAKE) -f dbase.mk clobber

doinclude:
		@echo "BASE=`pwd`"	 	>  .BASEDIR
		@echo "CC=$(CC)"		>> .BASEDIR
		@echo "CFLAGS=$(CFLAGS)"	>> .BASEDIR
		@echo "LDFLAGS=$(LDFLAGS)"	>> .BASEDIR
		@echo "HTFAKE=$(HTFAKE)"	>> .BASEDIR
		@echo "HTMLDIR=$(HTMLDIR)"	>> .BASEDIR
		@echo "HTMLLOC=$(HTMLLOC)"	>> .BASEDIR
		@echo "DATABASE=$(DATABASE)"	>> .BASEDIR
		@echo "CGIBIN=$(CGIBIN)"	>> .BASEDIR
		@echo "NETLIBS=$(NETLIBS)"	>> .BASEDIR
