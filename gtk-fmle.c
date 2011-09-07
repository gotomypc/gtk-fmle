/**
 *
 *
 * @file gtk-fmle.c
 * @author Ruei-Yuan Lu
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libintl.h>
#include <gtk/gtk.h>
#include <sys/wait.h>
#include <unistd.h>
#include "gtk-fmle.h"
#include "conf.h"
#include "gtk-fmle-glade.h"


struct MainWindow main_window;

pid_t run_cmd(char* cmd)
{
	pid_t	pid=0;
	int	result;

	pid=fork();

	if(!pid)
	{
		setpgrp();
		result = execl("/bin/sh", "sh", "-c", cmd, NULL);
		_exit(-127);
	}

	return pid;
}


void window_delete(GtkWidget* widget, GdkEvent* event, void* ptr)
{
	write_conf();
	gtk_main_quit();
}

void selectwindow_clicked(GtkWidget* widget, void* ptr)
{
	int	w, h, x, y;
	FILE*	fp;

	fp = popen("xwininfo |grep -E \\(Absolute\\)+\\|\\(Width\\)\\|\\(Height\\) | grep -E [0-9]+ -o","r");
	fscanf(fp, "%d\n%d\n%d\n%d\n", &x, &y, &w, &h);
	fclose(fp);

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(main_window.inw), w);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(main_window.inh), h);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(main_window.offx), x);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(main_window.offy), y);

}

void selectdesktop_clicked(GtkWidget* widget, void* ptr)
{
	int	w, h, x, y;
	FILE*	fp;

	fp = popen("xwininfo -root |grep -E \\(Absolute\\)+\\|\\(Width\\)\\|\\(Height\\) | grep -E [0-9]+ -o","r");
	fscanf(fp, "%d\n%d\n%d\n%d\n", &x, &y, &w, &h);
	fclose(fp);

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(main_window.inw), w);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(main_window.inh), h);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(main_window.offx), x);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(main_window.offy), y);

}

void start_toggled(GtkWidget* widget, void* ptr)
{
	int	stat;
	char	command[512] = {};

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
	{
		/* x11grab settings */
		{
			int	w, h, x, y, fps;
			float	latency;
			const char* device;

			w = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.inw));
			h = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.inh));
			x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.offx));
			y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.offy));

			fps = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.infps));
			latency = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.latency))/1000.0;

			device = gtk_entry_get_text(GTK_ENTRY(main_window.alsa));

			sprintf(main_window.input, "-f x11grab -s %dx%d -r %d -i :0.0+%d,%d -f alsa -ac 2 -itsoffset %.2f -i %s", w, h, fps, x, y, latency, device);
		}

		/* x264 settings */
		{
			int w, h, f, b, bt, crf;
			char resol[32] = {};
			char fps[16] = {};
			char mode[64] = {};
			const char* preset = NULL;

			preset = gtk_entry_get_text(GTK_ENTRY(main_window.vpre));

			w = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.outw));
			h = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.outh));
			f = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.outfps));
			b = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.bitrate));
			bt = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.ratetol));
			crf = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.crf));


			if(w&&h)
			{
				sprintf(resol, "-s %dx%d", w, h);
			}

			if(f)
			{
				sprintf(fps, "-r %d", f);
			}

			if(crf)
			{
				sprintf(mode, "-crf %d", crf);
			}else
			{
				sprintf(mode, "-b %dk -bt %dk", b, bt);
			}

			sprintf(main_window.video, "-vcodec libx264 -vpre %s %s %s %s", preset, resol, fps, mode);
		}

		/* AAC settings */
		{
			int ab, ar;
			char rate[16] = {};

			ab = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.ab));
			ar = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.ar));

			if(ar)
			{
				sprintf(rate, "-ar %d", ar);
			}

			sprintf(main_window.audio, "-acodec libfaac -ab %dk %s", ab, rate);

		}

		/* Flash Media Server settings */
		{
			const char* url = NULL;
			const char* key = NULL;

			url = gtk_entry_get_text(GTK_ENTRY(main_window.fmsurl));
			key = gtk_entry_get_text(GTK_ENTRY(main_window.fmskey));


			sprintf(main_window.url, "-f flv \"%s/%s flashver=FMLE/3.0%%20(compatible;%%20FMSc/1.0)\"", url, key);
		}

		/* File output settings */
		{
			int enable_mp4;

			enable_mp4 = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(main_window.mp4));
			main_window.file_output[0] = 0;
			if(enable_mp4)
				sprintf(main_window.url, "-y -f mp4 %s/%s", gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(main_window.out_path)), gtk_entry_get_text(GTK_ENTRY(main_window.out_filename)));

		}

		int number_of_threads = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.threads));

		sprintf(command, "ffmpeg %s %s %s -threads %d %s %s </dev/null", main_window.input, main_window.video, main_window.audio, number_of_threads, main_window.url, ""/*main_window.file_output*/);
		main_window.ffmpeg = run_cmd(command);
		puts(command);
	}
	else
	{
		killpg(main_window.ffmpeg, SIGTERM);
		waitpid(main_window.ffmpeg, &stat, 0);
		main_window.ffmpeg = 0;
	}
}


int main(int argc, char** argv)
{
	GtkBuilder* builder = NULL;

	memset(&main_window, 0, sizeof(main_window));
	bindtextdomain("gtk-fmle", "/usr/share/locale");
	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_set_translation_domain(builder, "gtk-fmle");
	//gtk_builder_add_from_file(builder, "gtk-fmle.glade", NULL);
	gtk_builder_add_from_string(builder, gtk_fmle_glade, strlen(gtk_fmle_glade), NULL);


	main_window.window 	= GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	main_window.fmskey 	= GTK_WIDGET(gtk_builder_get_object(builder, "fmskey"));
	main_window.fmsurl 	= GTK_WIDGET(gtk_builder_get_object(builder, "fmsurl"));
	main_window.infps 	= GTK_WIDGET(gtk_builder_get_object(builder, "infps"));
	main_window.inw 	= GTK_WIDGET(gtk_builder_get_object(builder, "inw"));
	main_window.inh 	= GTK_WIDGET(gtk_builder_get_object(builder, "inh"));
	main_window.offx 	= GTK_WIDGET(gtk_builder_get_object(builder, "offx"));
	main_window.offy 	= GTK_WIDGET(gtk_builder_get_object(builder, "offy"));
	main_window.ab 		= GTK_WIDGET(gtk_builder_get_object(builder, "ab"));
	main_window.ar 		= GTK_WIDGET(gtk_builder_get_object(builder, "ar"));
	main_window.alsa	= GTK_WIDGET(gtk_builder_get_object(builder, "alsa"));
	main_window.vpre	= GTK_WIDGET(gtk_builder_get_object(builder, "vpre"));
	main_window.outw 	= GTK_WIDGET(gtk_builder_get_object(builder, "outw"));
	main_window.outh 	= GTK_WIDGET(gtk_builder_get_object(builder, "outh"));
	main_window.outfps 	= GTK_WIDGET(gtk_builder_get_object(builder, "outfps"));
	main_window.bitrate 	= GTK_WIDGET(gtk_builder_get_object(builder, "bitrate"));
	main_window.ratetol 	= GTK_WIDGET(gtk_builder_get_object(builder, "ratetol"));
	main_window.crf 	= GTK_WIDGET(gtk_builder_get_object(builder, "crf"));
	main_window.latency 	= GTK_WIDGET(gtk_builder_get_object(builder, "latency"));
	main_window.mp4 	= GTK_WIDGET(gtk_builder_get_object(builder, "mp4"));
	main_window.out_path 	= GTK_WIDGET(gtk_builder_get_object(builder, "out_path"));
	main_window.out_filename= GTK_WIDGET(gtk_builder_get_object(builder, "out_filename"));
	main_window.threads	= GTK_WIDGET(gtk_builder_get_object(builder, "threads"));


	main_window.select_window 	= GTK_WIDGET(gtk_builder_get_object(builder, "select_window"));
	main_window.select_desktop 	= GTK_WIDGET(gtk_builder_get_object(builder, "select_desktop"));


	gtk_builder_connect_signals(builder, NULL);

	gtk_widget_show_all(main_window.window);
	read_conf();
	selectdesktop_clicked(NULL, NULL);

	gtk_main();

	return EXIT_SUCCESS;
}

