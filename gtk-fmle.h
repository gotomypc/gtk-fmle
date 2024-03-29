/**
 * @file gtk-fmle.h
 * @author Ruei-Yuan Lu
 *
 *
 */

#ifndef GTK_FMLE_H_
#define GTK_FMLE_H_

#include <gtk/gtk.h>


struct MainWindow
{
	GtkWidget*	window;

	GtkWidget*	fmsurl;
	GtkWidget*	fmskey;

	GtkWidget*	inw;
	GtkWidget*	inh;

	GtkWidget*	offx;
	GtkWidget*	offy;

	GtkWidget*	infps;

	GtkWidget*	select_desktop;
	GtkWidget*	select_window;

	GtkWidget*	alsa;

	GtkWidget*	ab;
	GtkWidget*	ar;

	GtkWidget*	preset;
	GtkWidget*	options;
	GtkWidget*	outw;
	GtkWidget*	outh;
	GtkWidget*	outfps;
	GtkWidget*	latency;
	GtkWidget*	threads;
	GtkWidget*	volume;

	GtkWidget*	mp4;
	GtkWidget*	out_path;
	GtkWidget*	out_filename;

	pid_t		ffmpeg;

	char		input[256];
	char		video[256];
	char		audio[256];
	char		output[256];
	char		file_output[256];
	char		url[256];

};


extern struct MainWindow main_window;


#endif /* GTK_FMLE_H_ */
