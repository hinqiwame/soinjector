#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void error_popup(const gchar *message) 
{
    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_file_set(GtkFileChooserButton *filechooserbutton, gpointer user_data) 
{
	const gchar *selected_file_path = NULL;
	selected_file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooserbutton));
    const gchar *selected_file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooserbutton));
    // g_print("Selected File: %s\n", selected_file);
}

const char *get_process_name_from_pid(int pid) 
{
    char procpath[256];
    snprintf(procpath, sizeof(procpath), "/proc/%d/cmdline", pid);

    FILE *cmdfile = fopen(procpath, "r");
    if (cmdfile != NULL) {
        char processname[256];
        size_t bytesRead = fread(processname, 1, sizeof(processname) - 1, cmdfile);
        fclose(cmdfile);

        // null-terminate the string
        processname[bytesRead] = '\0';

        return strdup(processname);
    }

    return NULL;  // Return null if the process is not AAAAAHHH
}

void list_processes(GtkComboBoxText *process_combobox) 
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir("/proc");
    if (dir == NULL) {
        perror("Error opening /proc directory");
        exit(EXIT_FAILURE);
    }

    // Clear existing entries
    gtk_combo_box_text_remove_all(process_combobox);

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
            const char *process_name = get_process_name_from_pid(atoi(entry->d_name));
            gtk_combo_box_text_append_text(process_combobox, g_markup_printf_escaped("%s - %s", entry->d_name, process_name));  
        }
    }

    closedir(dir);
}

void inject(GtkWidget *widget, gpointer data, const gchar *file_path) 
{
    // Retrieve the selected PID from the combo box
    GtkWidget *process_combobox = GTK_WIDGET(data);
    const gchar *selected_pid = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(process_combobox));
    if (selected_pid != NULL) {
		const char *determiner = " - ";
		const char *actual_pid = strtok(selected_pid, determiner);
		const char *quoted_file_path = g_shell_quote(file_path);
        printf("Injecting %s into process with PID %s...\n", quoted_file_path, actual_pid);
		const char *command = g_strdup_printf("./injector %s %s", file_path, quoted_file_path);
		int result = system(command);

		if (result == -1) {
			error_popup("Either an injector issue or injector binary was not found.\nYou can get the latest binary of injector here: https://github.com/hinqiwame/soinjector/releases/latest");
		} else {
			printf("Done.\n");
		}

		g_free((gpointer)command);
        g_free((gpointer)selected_pid);
    } else {
        error_popup("Please select a process to inject into!");
    }
}

int main(int argc, char *argv[]) 
{
    gtk_init(&argc, &argv);

    if (geteuid() != 0) {
        error_popup("Please run this program as sudo!");
        exit(EXIT_FAILURE);
    }

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Soinjector GUI");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a file chooser button
    GtkWidget *file_chooser_button = gtk_file_chooser_button_new("Select .so file to inject", GTK_FILE_CHOOSER_ACTION_OPEN);
    g_signal_connect(file_chooser_button, "file-set", G_CALLBACK(on_file_set), NULL);
    GtkWidget *solabel = gtk_label_new("Choose a .so file to inject");

    // Create a PID section
    GtkWidget *pid_label = gtk_label_new("Choose a process to inject into");

    // Create a process combo box
    GtkWidget *process_combobox = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(process_combobox), "Select a process");
    list_processes(GTK_COMBO_BOX_TEXT(process_combobox));

    // Create an inject button
    GtkWidget *inject_button = gtk_button_new_with_label("Inject!");
	g_signal_connect(inject_button, "clicked", G_CALLBACK(inject), process_combobox); 

    // Create a vertical box and add the file chooser button, label, process combo box, and inject button to it
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_pack_start(GTK_BOX(vbox), solabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), file_chooser_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), pid_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), process_combobox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), inject_button, FALSE, FALSE, 0);

    // Add the vertical box to the window
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Set the window to unresizable mode
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    gtk_widget_show_all(window);

    // GTK main loop
    gtk_main();

    return 0;
}
