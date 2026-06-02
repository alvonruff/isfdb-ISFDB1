# isfdb-ISFDB1

Snapshot of the first version of the ISFDB from 2001
This repo contains a snapshot of the original ISFDB, from 2001. As with the isfdb-preISFDB repo, these files were recovered from old, non-functional laptop.
Based on data in the FORTHCOMING file, and the latest magazines found in ZINES, this snapshot is from May/June 2001.  The first version of the ISFDB was a 
set of C-coded CGI executables that read data
from plaintext files.  It operated for 10 years, from mid-1995 until mid-2005, when it was replaced by ISFDB2, which consisted of a set of Python-coded 
scripts that read data from MySQL.

The files are presented as-was, but with some minor changes so that they can be compiled and run on modern Linux systems:

* Addition of function declarations
* Addition of a few includes, mostly <string.h>, <unistd.h>, and <stdlib.h>
* Numerous files and directories related to database management and distribution to production servers have been removed.
* On very recent versions of GCC, the code generated a massive number of warnings, so some light porting dealing with returned status values have been updated.
* The dataset and htmls files have had all email addresses redacted.

These files were not previously released publicly, so everything has been given modern attribution licenses, compatable with the
original informal license statement from 2001.  The C code has been given an MIT license, and the datafiles in dbase.ascii have a Creative Commons license.

# Installation

1. Edit the Makefile
2. run: make
3. run: make dbase
4. run: make install

## Makefile.  There are 5 defines which must be changed:

	HTMLDIR - This is the UNIX pathname to where html files should
		  be installed.

	CGIBIN  - This is the UNIX pathname to where cgi binaries should
		  be installed.

	HTMLLOC - This is the URL location as to where the html files will
		  be stored. Since this is passed into the compiler when
		  building the cgi binaries, the path delimiter must be
		  preceeded with a backslash (see examples in Makefile).

	DATABASE - This is the URL locations as to where the cgi binaries
		   will be stored. Same rules as HTMLLOC

	HTFAKE   - Basically DATABASE, preceeded by a slash. I forget why it's
		   like this, but it is needed.

The Makefile defaults will run unchanged on a Linux server with a recent Apache installation using /var/www
(so long as cgi-bin support is enabled).

# Theory of Operation:

While commercial databases were available for Unix in 1995, open-source options were limited. MySQL
was not publicly released until August 1996, and other previous 'open source' databases like INGRESS
or PostGres were not generally installed on early hosting systems. As such, the ISFDB used a common
poor-man's approach to the database problem (a similar approach was also utilized by the IMDB).

There are a set of human editable files in dbase.ascii. These contain bibliographic data in a pseudo-XML
format. XML would not be created until 1996 after the launch of the ISFDB, but the concept of a markup
language significantly predates the existence of XML (such as HTML itself). The format of the ISFDB input files were:

    Record_Title {
        TAGLIST
    }

Where a single tagged entry in the tag list consisted of:

        TAG=|content|

An entry from AUTHORS looked like:

    Philip K. Dick {
        LN=|Dick, Philip Kindred|
        BP=|USA|
        BD=|1928|
        DD=|1982|
        WP=|http://www.users.interport.net/~regulus/pkd/pkd-int.html|
    }

These files were 'compiled' into a dense, machine-readable format, such that there were 6 dbase files:

* authors.dbase
* awards.dbase
* interviews.dbase
* pubs.dbase
* reviews.dbase
* titles.dbase

In general the human-readable content would be collapsed into a single-line machine-readable entry of the form:

        Record_Title|Tag1_Content|Tag2_Content|Tag3_Content|..

So the P.K. Dick from AUTHORS above becomes the following entry in authors.dbase:

        Philip K. Dick|Dick, Philip Kindred|USA|1928|1982|||http://www.users.interport.net/~regulus/pkd/pkd-int.html||

In order to speed the search and rendering time, index files were created. These index files were:

* awards.xba - Indexed by author
* awards.xby - Indexed by year
* titles.xba - Indexed by author
* titles.xbs - Indexed by series
* titles.xby - Indexed by year

The index files create a mapping between two kinds of objects. For instance, to find all of the titles
for Philip K. Dick, you would first search the first field of titles.xba, and find the following entry:

        Philip K. Dick|907f08|906385|8f05e9|8ee00d|8ed978|8b5730|8b51f5|8a24e6|8a242 (...and so on...)

The hexidecimal values following Dick's name are offsets into the titles.dbase file. So The full algprithm is:

1. Find the Philip K. Dick entry in titles.xba
2. Read all the the title offsets
3. Open titles.dbase
4. For each offset, perform an lseek() and read the line stored at that offset.
5. Place the titles in the desired order, then format and print

Note that even today, 30 years later, the current ISFDB has the following SQL routines:

# SQLloadAwardsXBA
* SQLloadTitlesXBS

# Directory Structure

* dbase.ascii    - Contains the human-readable bibliographic information
* dbase.compiled - Contains the compiled version of that data
* tools          - Contains the tools source code used to compile the data
* bin            - Contains the tools executables
* src            - Contains the website code
* htmls          - Contains the static htmls of the website
