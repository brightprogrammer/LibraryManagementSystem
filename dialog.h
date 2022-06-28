#ifndef MESSAGEDIALOG_H_
#define MESSAGEDIALOG_H_

#include <gtk/gtk.h>

/**
 * Show error message dialog. Dialog box destroys with parent.
 *
 * @param pwin Parent window hosting this message. Can be NULL.
 * @param msg Message to be displayed.
 * */
void ShowErrorMessageDialog(GtkWindow* pwin, const char* msg);

/**
 * Show confirmation dialog. Dialog box destroys with parent.
 *
 * @param pwin Parent window.
 * @param msg Message to be displayed.
 * @param response Response handler function.
 * @param user_data User data to be passed to response handler.
 * */
void ShowConfirmationDialog(GtkWindow* pwin, const char* msg,
                            void (*response)(GtkDialog* self, gint response_id, gpointer user_data),
                            gpointer user_data);

#endif // MESSAGEDIALOG_H_
