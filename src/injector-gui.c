#include <gtk/gtk.h>
#include <stdio.h>

void on_file_set(GtkFileChooserButton *filechooserbutton, gpointer user_data) 
{
    const gchar *selected_file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooserbutton));
    // g_print("Selected File: %s\n", selected_file);
}

int inject(GtkApplication *widget, gpointer data)
{
	printf("haha lol\n");
	
	return 0;
}

int main(int argc, char *argv[]) 
{
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Soinjector GUI");
	gtk_window_set_default_size (GTK_WINDOW (window), 400, 200);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a file chooser button
    GtkWidget *filechooserbutton = gtk_file_chooser_button_new("Select .so file to inject", GTK_FILE_CHOOSER_ACTION_OPEN);
    g_signal_connect(filechooserbutton, "file-set", G_CALLBACK(on_file_set), NULL);
    GtkWidget *solabel = gtk_label_new("Choose a .so file to inject");

	// Create a PID section
	GtkWidget *pidlabel = gtk_label_new("Choose a process to inject into");

	// Create an inject button
	GtkWidget *injectbutton = gtk_button_new_with_label("Inject!");
	g_signal_connect(injectbutton, "clicked", G_CALLBACK(inject), NULL);

    // Create a vertical box and add the file chooser button and label to it
    GtkWidget *vbox = gtk_vbox_new(FALSE, 6);
	gtk_box_pack_start(GTK_BOX(vbox), solabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), filechooserbutton, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), pidlabel, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), injectbutton, FALSE, FALSE, 0);

    // Add the vertical box to the window
    gtk_container_add(GTK_CONTAINER(window), vbox);

	// Set the window to unresizable mode
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    gtk_widget_show_all(window);

    // GTK main loop
    gtk_main();

    return 0;
}
