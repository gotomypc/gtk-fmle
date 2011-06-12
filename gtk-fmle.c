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
		//strcpy(command, "ffmpeg ");

		{
			int	w, h, x, y, fps;
			const char* device;

			w = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.inw));
			h = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.inh));
			x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.offx));
			y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.offy));

			fps = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.infps));

			device = gtk_entry_get_text(GTK_ENTRY(main_window.alsa));

			sprintf(main_window.input, "-f x11grab -s %dx%d -r %d -i :0.0+%d,%d -f alsa -ac 2 -i %s", w, h, fps, x, y, device);
		}

		{
			int w, h, f, b, bt;
			char resol[32] = {};
			char fps[16] = {};
			const char* preset = NULL;

			preset = gtk_entry_get_text(GTK_ENTRY(main_window.vpre));

			w = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.outw));
			h = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.outh));
			f = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.outfps));
			b = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.bitrate));
			bt = gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window.ratetol));


			if(w&&h)
			{
				sprintf(resol, "-s %dx%d", w, h);
			}

			if(f)
			{
				sprintf(fps, "-r %d", f);
			}

			sprintf(main_window.video, "-vcodec libx264 -vpre %s %s %s -b %dk -bt %dk", preset, resol, fps, b, bt);
		}


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

		{
			const char* url = NULL;
			const char* key = NULL;


			url = gtk_entry_get_text(GTK_ENTRY(main_window.fmsurl));
			key = gtk_entry_get_text(GTK_ENTRY(main_window.fmskey));



			sprintf(main_window.url, "-f flv \"%s/%s flashver=FMLE/3.0%%20(compatible;%%20FMSc/1.0)\"", url, key);
		}


		sprintf(command, "ffmpeg %s %s %s %s -threads 0 %s </dev/null", main_window.input, main_window.video, main_window.audio, main_window.output, main_window.url);
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


	main_window.select_window 	= GTK_WIDGET(gtk_builder_get_object(builder, "select_window"));
	main_window.select_desktop 	= GTK_WIDGET(gtk_builder_get_object(builder, "select_desktop"));


	gtk_builder_connect_signals(builder, NULL);

	gtk_widget_show_all(main_window.window);
	read_conf();
	selectdesktop_clicked(NULL, NULL);

	gtk_main();

	return EXIT_SUCCESS;
}

