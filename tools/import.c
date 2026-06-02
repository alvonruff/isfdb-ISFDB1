
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "sfdbase.h"

extern pub_t *pub_list;

int
main(int argc, char *argv[])
{
	pub_t   *tmp;
	FILE	*fp;

	load_forthcoming(argv[1]);

	/*
	 * First create forthcoming file
	 */
	fp = fopen("forthcoming", "wb+");
	tmp = pub_list;
	while(tmp) {
		fprintf(fp, "%s {\n", tmp->pu_title);
		fprintf(fp, "\tAB=|%s|\n", tmp->pu_abbreviation);
		fprintf(fp, "\tAE=|%s|\n", tmp->pu_author);
		fprintf(fp, "\tYR=|%s|\n", tmp->pu_year);
		fprintf(fp, "\tPB=|%s|\n", tmp->pu_publisher);
		fprintf(fp, "\tPP=|%s|\n", tmp->pu_pages);
		fprintf(fp, "\tTP=|%s|\n", tmp->pu_type);
		fprintf(fp, "\tIS=|%s|\n", tmp->pu_isbn);
		fprintf(fp, "\tCV=|%s|\n", tmp->pu_cover);
		fprintf(fp, "\tCT=|%s|\n", tmp->pu_bcover);
		fprintf(fp, "\tPR=|%s|\n", tmp->pu_price);
		fprintf(fp, "\tNT=|%s|\n", tmp->pu_notes);

		fprintf(fp, "}\n");
		tmp = tmp->pu_next;
	}
	fclose(fp);

	/*
	 * Create NOVELS file
	 */
	fp = fopen("NOVELS", "wb+");
	tmp = pub_list;
	while(tmp) {
		if ( !strcmp(tmp->pu_bcover, "n")) {
			fprintf(fp, "%s {\n", tmp->pu_title);
			fprintf(fp, "\tAE=|%s|\n", tmp->pu_author);
			fprintf(fp, "\tYR=|%s|\n", tmp->pu_year);
			fprintf(fp, "\tPB=|%s|\n", tmp->pu_abbreviation);
			fprintf(fp, "}\n");
		}
		tmp = tmp->pu_next;
	}
	fclose(fp);

	/*
	 * Create COLLECTIONS file
	 */
	fp = fopen("COLLECTIONS", "wb+");
	tmp = pub_list;
	while(tmp) {
		if ( !strcmp(tmp->pu_bcover, "c")) {
			fprintf(fp, "%s {\n", tmp->pu_title);
			fprintf(fp, "\tAE=|%s|\n", tmp->pu_author);
			fprintf(fp, "\tYR=|%s|\n", tmp->pu_year);
			fprintf(fp, "\tPB=|%s|\n", tmp->pu_abbreviation);
			fprintf(fp, "}\n");
		}
		tmp = tmp->pu_next;
	}
	fclose(fp);

	/*
	 * Create ANTHOLOGIES file
	 */
	fp = fopen("ANTHOLOGIES", "wb+");
	tmp = pub_list;
	while(tmp) {
		if ( !strcmp(tmp->pu_bcover, "a")) {
			fprintf(fp, "%s {\n", tmp->pu_title);
			fprintf(fp, "\tAE=|%s|\n", tmp->pu_author);
			fprintf(fp, "\tYR=|%s|\n", tmp->pu_year);
			fprintf(fp, "\tPB=|%s|\n", tmp->pu_abbreviation);
			fprintf(fp, "}\n");
		}
		tmp = tmp->pu_next;
	}
	fclose(fp);

	/*
	 * Create NONFICTION file
	 */
	fp = fopen("NONFICTION", "wb+");
	tmp = pub_list;
	while(tmp) {
		if ( !strcmp(tmp->pu_bcover, "nf")) {
			fprintf(fp, "%s {\n", tmp->pu_title);
			fprintf(fp, "\tAE=|%s|\n", tmp->pu_author);
			fprintf(fp, "\tYR=|%s|\n", tmp->pu_year);
			fprintf(fp, "\tPB=|%s|\n", tmp->pu_abbreviation);
			fprintf(fp, "}\n");
		}
		tmp = tmp->pu_next;
	}
	fclose(fp);

	/*
	 * Create OMNIBUS file
	 */
	fp = fopen("OMNIBUS", "wb+");
	tmp = pub_list;
	while(tmp) {
		if ( !strcmp(tmp->pu_bcover, "o")) {
			fprintf(fp, "%s {\n", tmp->pu_title);
			fprintf(fp, "\tAE=|%s|\n", tmp->pu_author);
			fprintf(fp, "\tYR=|%s|\n", tmp->pu_year);
			fprintf(fp, "\tPB=|%s|\n", tmp->pu_abbreviation);
			fprintf(fp, "}\n");
		}
		tmp = tmp->pu_next;
	}
	fclose(fp);
        return(0);
}
