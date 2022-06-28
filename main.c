#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <gtk/gtk.h>
#include <glib/gstdio.h>

#include "book.h"
#include "library.h"
#include "appstate.h"
#include "addbook.h"
#include "booklist.h"

// Maintains global app state
extern AppState* appstate = NULL;

/**
 * Search changed callback will be triggered whenever search string
 * is changed for book search entry.
 * */
void SearchChangedClbk(GtkSearchEntry* search_entry, gpointer list_box){
    const gchar* filter_string = gtk_editable_get_text(GTK_EDITABLE(search_entry));
    gtk_list_box_set_filter_func(GTK_LIST_BOX(list_box), BookSearchFilterFunc, (void*)filter_string, NULL);
}

/**
 * App activate callback function.
 * */
void AppActivateClbk(GtkApplication *app, gpointer user_data){
    GtkBuilder *builder = gtk_builder_new_from_file("lims.ui");

    // get window and set window for this app
    GtkWindow *window = GTK_WINDOW(gtk_builder_get_object(builder, "mainwindow"));
    gtk_window_set_application(window, GTK_APPLICATION(app));
    gtk_window_set_default_size(window, 960, 540); /* 16:9 ratio window */

    // get book list and set it to appstate
    GtkListBox* book_list = GTK_LIST_BOX(gtk_builder_get_object(builder, "books_list_box"));
    appstate->book_list = book_list;
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
    g_signal_connect(book_search, "changed", G_CALLBACK(SearchChangedClbk), book_list);

    // get add new book button
    GtkButton* add_new_book = GTK_BUTTON(gtk_builder_get_object(builder, "add_new_book_btn"));
    g_signal_connect(add_new_book, "clicked", G_CALLBACK(OpenAddNewBookWindowClbk), NULL);

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

    // destroy global AppState
    DestroyAppState(appstate);

    return status;
}
