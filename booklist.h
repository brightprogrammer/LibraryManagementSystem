#ifndef BOOKLIST_H_
#define BOOKLIST_H_

#include <gtk/gtk.h>

#include "book.h"

/**
 * Helper function to get data from book list row widget.
 * Returns a pointer to 3 string values. First one contains
 * name of the book, second contains name of author and third
 * is for isbn number.
 *
 * @param row Pointer to GtkListBoxRow widget to get data from.
 * @return Array of 3 string values. To be free'd after use.
 * */
const char** GetDataFromBookListRow(GtkListBoxRow* row);

/**
 * Callback for handling when a row is actiaved.
 * This is supposed to open a modal window displaying book information.
 *
 * @param self List box that triggered this event.
 * @param row Row item selected.
 * @param user_data User data passed to callback.
 * */
void BookListRowActivatedClbk(GtkListBox* self, GtkListBoxRow* row, gpointer user_data);

/**
 * Filter function for book search. Takes the filter string as user data argument
 * and returns whether to show the given row or not in boolean.
 * Type of this function is same as type of `GtkListBoxFilterFunc`
 *
 * Go (here)[https://docs.gtk.org/gtk4/method.ListBox.set_filter_func.html] for
 * reference.
 *
 * @return gboolean TRUE if row is to be show, FALSE otherwise.
 *  */
gboolean BookSearchFilterFunc(GtkListBoxRow* row, gpointer filter_string);

/**
 * Create a book list item displaying book name, author name and isbn number
 * for the given book.
 *
 * @return GtkListBoxRow*
 * */
GtkWidget* CreateBookListItem(Book* book);

#endif // BOOKLIST_H_
