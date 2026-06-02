/*
 *     (C) COPYRIGHT 1995-2000   Al von Ruff
 *         ALL RIGHTS RESERVED
 *
 *     The copyright notice above does not evidence any actual or
 *     intended publication of such source code.
 *
 *     static char sccsid[] = "@(#)sfdbase.h	1.15	06/10/97 SFdbase";
 */

#ifndef _INC_SFDBASE_H
#define _INC_SFDBASE_H


#define SMALLSIZE	64
#define MEDIUMSIZE	256
#define BIGSIZE		1024
#define LARGESIZE	16384


typedef struct search {
	int		se_numyear;
	int		se_marker;
	char		*se_title;
	char		*se_author;
	char		*se_pseudonym;
	char		*se_series;
	char		*se_superseries;
	char		*se_year;
	char		*se_type;
	char		*se_pubs;
	char		*se_seriesnum;
	char		*se_offset;
	char		*se_storylen;
	char		*se_notes;
	char		*se_synopsis;
	char		*se_awtags;
	char		*se_translator;
	struct award	*se_awards;
	struct search	*se_next;
} search_t;


/*
 * This data structure is used to store all publication data.
 */
typedef struct pub {
	int		pu_numyear;
	char		*pu_title;
	char		*pu_abbreviation;
	char		*pu_author;
	char		*pu_year;
	char		*pu_isbn;
	char		*pu_publisher;
	char		*pu_price;
	char		*pu_pages;
	char		*pu_type;
	char		*pu_cover;
	char		*pu_coverpicture;
	char		*pu_bcover;
	char		*pu_bcoverpicture;
	char		*pu_notes;
	struct pub	*pu_next;
} pub_t;


/*
 * This data structure is used to store all review data.
 */
typedef struct review {
	int		rv_marker;
	char		*rv_title;
	char		*rv_revtitle;
	char		*rv_author;
	char		*rv_reviewer;
	char		*rv_year;
	char		*rv_abbreviation;
	char		*rv_pageno;
	char		*rv_notes;
	struct review	*rv_next;
} review_t;


/*
 * This data structure is used to store an award record. aw_name points
 * at the name of the award (Hugo, Nebula, etc...), aw_year contains the
 * year, aw_type helps to determine whether the title was a novel, collection,
 * etc..., aw_level is the numerical value of the award (1=First Place, and
 * so on).
 */
typedef struct award {
	int		aw_marker;
	char		*aw_title;
	char		*aw_author;
	char		*aw_year;
	char		*aw_types;
	char		*aw_typel;
	char		*aw_level;
	char		*aw_movie;
	char		*aw_tag;
	char		*aw_award;
	struct award	*aw_next;
} award_t;


/*
 * This data structure is used to store all interview data.
 */
typedef struct inter {
	int		it_marker;
	char		*it_title;
	char		*it_inttitle;
	char		*it_author;
	char		*it_interviewer;
	char		*it_year;
	char		*it_abbreviation;
	char		*it_pageno;
	char		*it_notes;
	struct inter	*it_next;
} inter_t;


/*
 * This data astructure is used when a list of authors is
 * needed, for instance when searching for author names
 * which match some substring (like "Tom").
 */
typedef struct author {
	char		*au_author;
	char		*au_lastname;
	char		*au_offsets;
	int		au_small;
	int		au_length;
	struct author	*au_prev;
	struct author	*au_next;
	search_t	*au_list;
	search_t	*au_end;
} author_t;


/*
 * This structure is used to create the titles indexed by
 * series file. The titles.dbase file is parsed, and all
 * series info is sucked out of it. For each unique series
 * name, a series_t structure is allocated. For each publication
 * in that series, the ascii offset within the titles.dbase
 * file is inserted into the ss_offsets field.
 */
typedef struct series {
	char		*ss_series;
	char		*ss_offsets;
	int		ss_small;
	int		ss_length;
	struct series	*ss_prev;
	struct series	*ss_next;
	search_t	*ss_list;
	search_t	*ss_end;
} series_t;


/*
 * Titles may have associated publication tags, which describe where that
 * title has been published. All of the publication tags in the database
 * are collected into a binary tree at compile time in order to construct
 * the tag index. This data structure is the basic building block of that
 * tree. ta_tag points to a tag name. ta_offsets points to a list of offsets
 * into the titles.dbase file of all the titles published in the publication
 * associated with a particular tag. The ta_left and ta_right fields are
 * used to construct the tree. If ta_small is true, then the ta_offsets
 * field is of size SMALLSIZE; if it is false, then the ta_offsets field
 * is the length described by ta_length. This is a balanced optimization
 * between memory and CPU usage. If ta_offsets has a length specified by
 * ta_length, then ta_offsets will have to be realloc'd if a new offset is
 * added to the list. To save on this overhead, ta_offsets has an initial
 * length of SMALLSIZE, in which most tags will fit, reducing the need for
 * extensive memory management. On those rare occasions when a publication
 * needs a lot of space, then the ta_length algorithm comes into play.
 */
typedef struct tag {
	char		*ta_tag;
	char		*ta_offsets;
	int		 ta_small;
	int		 ta_length;
	struct tag	*ta_left;
	struct tag	*ta_right;
} tag_t;


/*
 * This data structure is used to store information about an author. The
 * fields are pretty self-explanitory, except that the au_biog field points
 * to an offset (in ascii, for portability) which is an offset into the
 * notes file of the author's biography.
 */
typedef struct audata {
	char    *au_name;
	char    *au_legalname;
	char    *au_birthplace;
	char    *au_birthdate;
	char    *au_deathdate;
	char    *au_pseudonyms;
	char    *au_email;
	char    *au_webpage;
	char	*au_biog;
	struct audata *au_next;
} audata_t;


/*
 * This data structure is used to store author information. au_actual
 * points to the author's working name, while au_pseduo points to the
 * author's pseudonym used on this particular work.
 */
typedef struct auset {
	char		*au_actual;
	char		*au_pseudo;
	struct auset	*au_next;
} auset_t;


/*
 * This data structure is used to store a single attribute name and
 * value. For instance in YR=|1997|, the at_name would by "YR" and the
 * at_value would be "1997".
 */
typedef struct attr {
	char		 at_name[4];
	char		*at_value;
	struct attr	*at_next;
} attr_t;


/*
 * This data structure is used to store a raw database record. ob_name 
 * usually points to the title of the record, while ob_attrs points to
 * a linked list of attr structures.
 */
typedef struct object {
	char		*ob_name;
	attr_t		*ob_attrs;
	struct object	*ob_next;
} object_t;


/*
 * This data structure is used when posting data via one of the
 * data submission forms. The input is in the form of attribute name and
 * attribute value. MAX_ENTRIES describes the maximum number of such pairs
 * which can be posted.
 */
typedef struct {
	char *name;
	char *val;
} entry;
#define MAX_ENTRIES             10000


/*
 * Some useful prototypes
 */
extern void html_print_authors(char *, int);
extern int html_print_award_cite_msg(char *, char *, char *);
extern int print_title(int, char *, int, int, char *);
extern int parse_field(FILE *, char *, int *, int);
extern int parse_field_or_eol(FILE *, char *, int *, int);
extern int parse_to_eol(FILE *, int *);
extern void print_awardname(char *);
extern void postamble();
extern int load_pubs(int, char *);
extern award_t *parse_award_entry(FILE *, int *, int);
extern search_t *parse_title_entry(FILE *, int *, int);
extern void parse_pubs(char *);
extern int load_forthcoming(char *file);
extern char *makeword(char *line, char stop);
extern char *fmakeword(FILE *f, char stop, int *cl);
extern int getquerieint();
extern audata_t *get_author(char *author);
extern void plustospace(char *str);
extern void unescape_url(char *url);
extern void read_and_printwrap(int filetype, char *coffset, int position, int rightmargin, int indent);
extern void print_fatal(char *msg);
extern void html_print_authors_compressed(char *authors, int leftmargin);
extern char *lastname(char *fullname);
extern void lowercase(unsigned char *target, unsigned char *input);
extern int load_reviews(int filteroption, char *filter);
extern auset_t * decompose(char *string);
extern void separate(auset_t *input);
extern void print_total_found(search_t *ptr);
extern void print_titles_bycat(search_t *list);
extern void print_search_series(search_t *list);
extern void print_total_found(search_t *ptr);
extern int total_found(search_t *ptr);
extern char * bquote(char *string);
extern void insert_author(author_t *au);
extern int html_print1_author( char *author, char *pseudo, int position, int rightmargin, int indent);
extern void print_totals_found(search_t *ptr);
extern void print_titles_author(search_t *list);
extern void sort_by_year(search_t **oldlist);
extern void print_pub_entries(search_t *list, char *exact_author, char *target, char *title, int corrections);
extern void sortpub_by_year(pub_t **oldlist);
extern void print_menus( char *author, search_t *list, int alpha);
extern void print_preamble( search_t *list);
extern void print_entries(search_t *list, char *exact_author, char *target, char *title, int corrections);
extern int load_interviews(int filteroption, char *filter);
extern void print_series(search_t *list, char *exact_author, int corrections, int dotype );
extern int printwrap(char *buffer, int position, int rightmargin, int indent);
extern int posit_check(int position, int rightmargin, int indent);
extern int html_print_all_authors(char *authors, int indent, int position, int rightmargin);
extern void print_zine_bycat(search_t *list);
extern void print_zine_bycat2(search_t *list);
extern void print_menus3(char *pub );
extern char * load_note(char *coffset);
extern void print_submission_notes();
extern void print_pub_citations(char *citations, int correction);
extern void print_menus2(char *pub );
extern void sort_by_yearold(search_t **oldlist);
extern char * escape_string(unsigned char *string);


/*
 * Title printing options
 */
#define T_NOOPT		0x00
#define T_WINNER	0x01
#define T_PREINDENT	0x02
#define T_NOPAD		0x04
#define T_VARIANT	0x08
#define T_QUOTE		0x10

/*
 * Search options
 */
#define F_NOOPT		0x0
#define F_PLIST		0x1
#define F_EXACT		0x2
#define F_YEAR		0x3
#define F_FORTH		0x4
#define F_TAUTHOR	0x5
#define F_RAUTHOR	0x6

/*
 * Misc defines
 */
#define RIGHTMARGIN	50	/* Use if authors follow */
#define RIGHTMARGIN2	70	/* Otherwise, use this */

#define USE_NOTES	1
#define USE_SYNOPSIS	2
#define USE_BIOG	3

#define PARSE_MALLOC	0
#define PARSE_NOMALLOC	1

#define NOOPTIONS	0x00
#define DO_NOTES	0x01
#define DO_TITLES	0x02
#define DO_REVIEWS	0x04
#define DO_SYNOP	0x10

#endif /* _INC_SFDBASE_H */
