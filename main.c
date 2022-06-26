#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <gtk/gtk.h>
#include <glib/gstdio.h>

#include "library.h"

// Library database is loaded globally
static Library* ldb;

/// Represents a global application state
/// This will hold data like what database is laoded,
/// Which user is logged in
/// Some widgets that need to be accessed globally
/// etc...
/// THERE MUST BE ONLY ONE APP STATE THROUGHOUT THE APPLICATION!!
typedef struct {
    Library* library;
    GtkListBox* book_list_box;
} AppState;

// Maintains global app state
static AppState* appstate;

/**
 * Create the AppState object.
 *
 * @return AppState*
 * */
AppState* CreateAppState(){
    AppState* appstate = (AppState*)malloc(sizeof(AppState));

    appstate->library = NULL;
    appstate->book_list_box = NULL;
}

/**
 * Destroy given app state.
 * AppState only holds pointer to required data
 * and is not responsible for destruction or creation of
 * any member. All deallocations must be done before
 * calling this destructor function.
 *
 * @param appstate Pointer to the globally maintained AppState.
 * */
void DestroyAppState(AppState* appstate){
    if(appstate == NULL) LogError("Attempt to double free for global app state object\n");

    free((void*)appstate);
}

/**
 * Helper function to get data from book list row widget.
 * Returns a pointer to 3 string values. First one contains
 * name of the book, second contains name of author and third
 * is for isbn number.
 *
 * @param row Pointer to GtkListBoxRow widget to get data from.
 * @return Array of 3 string values. To be free'd after use.
 * */
const char** GetDataFromBookListRow(GtkListBoxRow* row){
    // allocate space for array of size 3
    char** row_data = (char**)malloc(3*sizeof(char*));

    // get box in list row that contains label widgets
    GtkWidget* box = gtk_widget_get_first_child(GTK_WIDGET(row));

    // get book name
    GtkWidget* child = GTK_WIDGET(gtk_widget_get_first_child(GTK_WIDGET(box)));
    row_data[0] = gtk_label_get_label(child);

    // get author name
    child = GTK_WIDGET(gtk_widget_get_next_sibling(GTK_WIDGET(child)));
    row_data[1] = gtk_label_get_label(child);

    // get isbn number
    child = GTK_WIDGET(gtk_widget_get_next_sibling(GTK_WIDGET(child)));
    row_data[2] = gtk_label_get_label(child);

    return row_data;
}

/**
 * Callback for handling when a row is actiaved.
 * This is supposed to open a modal window displaying book information.
 *
 * @param self List box that triggered this event.
 * @param row Row item selected.
 * @param user_data User data passed to callback.
 *
 * */
static void BookListRowActivatedClbk(GtkListBox* self, GtkListBoxRow* row, gpointer user_data){
    // get data from row widget
    char** row_data = GetDataFromBookListRow(row);

    // third element in row data contains isbn number
    // use isbn to find book
    Book* book = GetBookByISBN(appstate->library, row_data[2]);

    // load builder from ui file
    GtkBuilder *builder = gtk_builder_new_from_file("book_data_display.ui");

    // get window from builder
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "book_data_display_window"));
    gtk_window_set_default_size(window, 400, 180);

    // temp var to hold retrieved widgets from builder
    GtkWidget* widget;

    // change book name label
    widget = GTK_WIDGET(gtk_builder_get_object(builder, "book_name_label"));
    gtk_label_set_label(GTK_LABEL(widget), book->name);

    widget = GTK_WIDGET(gtk_builder_get_object(builder, "author_name_label"));
    gtk_label_set_label(GTK_LABEL(widget), book->author);

    widget = GTK_WIDGET(gtk_builder_get_object(builder, "publisher_name_label"));
    gtk_label_set_label(GTK_LABEL(widget), book->publisher);

    widget = GTK_WIDGET(gtk_builder_get_object(builder, "isbn_label"));
    gtk_label_set_label(GTK_LABEL(widget), book->isbn);

    widget = GTK_WIDGET(gtk_builder_get_object(builder, "description_label"));
    gtk_label_set_label(GTK_LABEL(widget), book->description);

    // buffer to hold size_t to string conversions
    char buf[128];

    widget = GTK_WIDGET(gtk_builder_get_object(builder, "total_copies_label"));
    snprintf(buf, sizeof(buf), "%zu", book->total_copies);
    gtk_label_set_label(GTK_LABEL(widget), buf);

    widget = GTK_WIDGET(gtk_builder_get_object(builder, "available_copies_label"));
    snprintf(buf, sizeof(buf), "%zu", book->available_copies);
    gtk_label_set_label(GTK_LABEL(widget), buf);

    widget = GTK_WIDGET(gtk_builder_get_object(builder, "location_label"));
    snprintf(buf, sizeof(buf), "Row %zu and Shelf %zu", book->row, book->shelf);
    gtk_label_set_label(GTK_LABEL(widget), buf);

    // show widgets inside window
    gtk_widget_show(GTK_WIDGET(window));

    // destroy builder
    g_object_unref(builder);
}

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
gboolean BookSearchFilterFunc(GtkListBoxRow* row, gpointer filter_string){
    // if search bar is empty then show all books by default
    if(strlen(filter_string) == 0) return true;

    // get data from row
    char** row_data = GetDataFromBookListRow(row);

    // check for a match in book name
    if(strstr(row_data[0], filter_string) != NULL) return true;

    // check for a match in author name
    if(strstr(row_data[1], filter_string) != NULL) return true;

    // check for a match in isbn number
    if(strstr(row_data[2], filter_string) != NULL) return true;

    // return false by default
    return false;
}

/**
 * Create a book list item displaying book name, author name and isbn number
 * for the given book.
 *
 * @return GtkListBoxRow*
 * */
GtkWidget* CreateBookListItem(Book* book){
    if(book == NULL) LogError("NULL passed instead of a Book pointer.\n");

    // this will act as a list item
    GtkListBoxRow* li_row = gtk_list_box_row_new();

    // create box to hold other children
    GtkBox* li_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_set_homogeneous(li_box, true);
    gtk_list_box_row_set_child(li_row, li_box);

    // create labels
    GtkWidget* bookname = GTK_WIDGET(gtk_label_new(book->name));
    GtkWidget* authorname = GTK_WIDGET(gtk_label_new(book->author));
    GtkWidget* isbn = GTK_WIDGET(gtk_label_new(book->isbn));

    // add to box
    gtk_box_append(li_box, bookname);
    gtk_box_append(li_box, authorname);
    gtk_box_append(li_box, isbn);

    return li_row;
}

/**
 * Search changed callback will be triggered whenever search string
 * is changed for book search entry.
 * */
static void SearchChangedClbk(GtkSearchEntry* search_entry, gpointer user_data){
    const gchar* filter_string = gtk_editable_get_text(GTK_EDITABLE(search_entry));
    gtk_list_box_set_filter_func(GTK_LIST_BOX(appstate->book_list_box), BookSearchFilterFunc, (void*)filter_string, NULL);
}

/**
 * App activate callback function.
 * */
static void AppActivateClbk(GtkApplication *app, gpointer user_data){
    GtkBuilder *builder = gtk_builder_new_from_file("lims.ui");

    // get window and set window for this app
    GtkWindow *window = GTK_WINDOW(gtk_builder_get_object(builder, "mainwindow"));
    gtk_window_set_application(window, GTK_APPLICATION(app));

    // get book list and set it to appstate
    GtkListBox* book_list = GTK_LIST_BOX(gtk_builder_get_object(builder, "books_list_box"));
    appstate->book_list_box = book_list;
    g_signal_connect(book_list, "row_activated", G_CALLBACK(BookListRowActivatedClbk), NULL);

    // load library data and set to global appstate
    Library* ldb = LoadLibraryData("library.ldb");
    appstate->library = ldb;

    // add items to list
    for(size_t i = 0; i < ldb->num_books; i++){
        GtkWidget* li = CreateBookListItem(ldb->books[i]);
        gtk_list_box_append(GTK_LIST_BOX(book_list), li);
    }

    // get search entry and attach signals
    GtkSearchEntry* book_search = GTK_SEARCH_ENTRY(gtk_builder_get_object(builder, "book_search_entry"));
    g_signal_connect(book_search, "changed", G_CALLBACK(SearchChangedClbk), NULL);

    // show widgets
    gtk_widget_show(GTK_WIDGET(window));

    // we do not need builder anymore
    g_object_unref(builder);
}

int main(int argc, char** argv){
    GtkApplication *app;
    int status;

    // create global appstate
    appstate = CreateAppState();

    // create application
    app = gtk_application_new("com.lims.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(AppActivateClbk), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    // destroy library database
    DestroyLibrary(appstate->library);

    // destroy global AppState
    DestroyAppState(appstate);

    return status;
}
