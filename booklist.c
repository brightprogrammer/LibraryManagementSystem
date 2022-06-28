#include "booklist.h"
#include "appstate.h"

// get global appstate
extern AppState* appstate;

const char** GetDataFromBookListRow(GtkListBoxRow* row){
    // allocate space for array of size 3
    const char** row_data = (const char**)malloc(3*sizeof(char*));

    // get box in list row that contains label widgets
    GtkWidget* box = gtk_widget_get_first_child(GTK_WIDGET(row));

    // get book name
    GtkWidget* child = GTK_WIDGET(gtk_widget_get_first_child(GTK_WIDGET(box)));
    row_data[0] = gtk_label_get_label(GTK_LABEL(child));

    // get author name
    child = GTK_WIDGET(gtk_widget_get_next_sibling(GTK_WIDGET(child)));
    row_data[1] = gtk_label_get_label(GTK_LABEL(child));

    // get isbn number
    child = GTK_WIDGET(gtk_widget_get_next_sibling(GTK_WIDGET(child)));
    row_data[2] = gtk_label_get_label(GTK_LABEL(child));

    return row_data;
}

/**
 * Handler for confirmation dialog box response.
 * This is where the final deletion of book happens.
 * */
void DeleteBookResponseHandler(GtkDialog* dialog, gint response_id, gpointer user_data){
    if(response_id == GTK_RESPONSE_ACCEPT){
        // get book from user data
        Book* book = (Book*)user_data;

        // delete book from book list
        for(size_t i = 0; i < appstate->library->num_books; i++){
            // get items one by one
            GtkListBoxRow* row = gtk_list_box_get_row_at_index(appstate->book_list, i);

            // if isbn matches then delete book
            const char** row_data = GetDataFromBookListRow(row);
            if(strcmp(row_data[2], book->isbn) == 0){
                gtk_list_box_remove(appstate->book_list, row);
                break;
            }
            free(row_data);
        }

        // delete book from database
        DeleteBookFromLibrary(appstate->library, book);

        // finally destroy dialog
        gtk_window_destroy(GTK_WINDOW(dialog));
    }
}

/**
 * Callback to handle deletion of book from library database.
 * */
void DeleteBookFromLibraryClbk(GtkWidget* widget, gpointer user_data){
    // first confirm whether to delete or not
    ShowConfirmationDialog(NULL, "Are you sure you want to permanently delete this book from library database?",
                           DeleteBookResponseHandler, user_data);
}

/**
 * NOTE : Might be a good idea to move this code to a new bookdatadisplay.h/c file.
 * */
void BookListRowActivatedClbk(GtkListBox* self, GtkListBoxRow* row, gpointer user_data){
    // get data from row widget
    const char** row_data = GetDataFromBookListRow(row);

    // third element in row data contains isbn number
    // use isbn to find book
    Book* book = FindBookByISBN(appstate->library, row_data[2]);

    // free row data since that was allocated using malloc
    free(row_data);

    // load builder from ui file
    GtkBuilder *builder = gtk_builder_new_from_file("book_data_display.ui");

    // get window from builder
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "book_data_display_window"));
    gtk_window_set_default_size(window, 400, 180);

    // temp var to hold retrieved widgets from builder
    GtkWidget* widget;

    // attach close buttons to corresponding event handler
    widget = GTK_WIDGET(gtk_builder_get_object(builder, "close_display_btn"));
    g_signal_connect_swapped(widget, "clicked", G_CALLBACK(gtk_window_destroy), window);

    // attach update book data button to corresponding event handler

    // attach delete book data button to corresponding event handler
    widget = GTK_WIDGET(gtk_builder_get_object(builder, "delete_book_btn"));
    g_signal_connect(widget, "clicked", G_CALLBACK(DeleteBookFromLibraryClbk), book);

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

gboolean BookSearchFilterFunc(GtkListBoxRow* row, gpointer filter_string){
    // if search bar is empty then show all books by default
    if(strlen(filter_string) == 0) return true;

    // get data from row
    const char** row_data = GetDataFromBookListRow(row);

    // check for a match in book name
    if(strstr(row_data[0], filter_string) != NULL) return true;

    // check for a match in author name
    if(strstr(row_data[1], filter_string) != NULL) return true;

    // check for a match in isbn number
    if(strstr(row_data[2], filter_string) != NULL) return true;

    // return false by default
    return false;
}

GtkWidget* CreateBookListItem(Book* book){
    if(book == NULL) LogError("NULL passed instead of a Book pointer.\n");

    // this will act as a list item
    GtkListBoxRow* li_row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());

    // create box to hold other children
    GtkBox* li_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2));
    gtk_box_set_homogeneous(li_box, true);
    gtk_list_box_row_set_child(li_row, GTK_WIDGET(li_box));

    // create labels
    GtkWidget* bookname = GTK_WIDGET(gtk_label_new(book->name));
    GtkWidget* authorname = GTK_WIDGET(gtk_label_new(book->author));
    GtkWidget* isbn = GTK_WIDGET(gtk_label_new(book->isbn));

    // add to box
    gtk_box_append(li_box, bookname);
    gtk_box_append(li_box, authorname);
    gtk_box_append(li_box, isbn);

    return GTK_WIDGET(li_row);
}
