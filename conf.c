#include "conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void	read_conf(void)
{
	const char*	home_dir;
	FILE*		fp = NULL;
	char		fullpath[256] = {};
	char		string[256] = {};

	home_dir=getenv("HOME");
	sprintf(fullpath, "%s/.gtk-fmle.conf", home_dir);

	fp = fopen(fullpath, "r");

	if(!fp)	return;

	fscanf(fp, "StreamKey = %s\n", string);
	if(!strcmp(string, "StreamKey")) string[0] = 0;
	gtk_entry_set_text(GTK_ENTRY(main_window.fmskey), string);

	fclose(fp);
}

void	write_conf(void)
{
	const char*	home_dir;
	FILE*		fp = NULL;
	char		fullpath[256] = {};
	const char*	string;

	home_dir=getenv("HOME");
	sprintf(fullpath, "%s/.gtk-fmle.conf", home_dir);

	fp = fopen(fullpath, "w");

	if(!fp)	return;

	string = gtk_entry_get_text(GTK_ENTRY(main_window.fmskey));
	fprintf(fp, "StreamKey = %s\n", string);

	fclose(fp);

}
