#include "dialog.h"

void ShowErrorMessageDialog(GtkWindow* parent_window, const char* msg){
    /* Display error message */
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;
    GtkMessageDialog* dialog = GTK_MESSAGE_DIALOG(gtk_message_dialog_new(parent_window, flags,
                                                                         GTK_MESSAGE_ERROR,
                                                                         GTK_BUTTONS_CLOSE,
                                                                         msg));

    // Destroy the dialog when the user responds to it
    // (e.g. clicks a button)
    g_signal_connect (dialog, "response",
                      G_CALLBACK (gtk_window_destroy),
                      NULL);

    gtk_widget_show(GTK_WIDGET(dialog));
}


void ShowConfirmationDialog(GtkWindow* pwin, const char* msg,
                            void (*response)(GtkDialog* self, gint response_id, gpointer user_data), gpointer user_data){
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkDialog* dialog = gtk_dialog_new_with_buttons (msg, pwin, flags,
                                                     "_OK", GTK_RESPONSE_ACCEPT,
                                                     "_Cancel", GTK_RESPONSE_REJECT,
                                                     NULL);

    g_signal_connect(dialog, "response", G_CALLBACK(response), user_data);
    gtk_widget_show(dialog);
}
