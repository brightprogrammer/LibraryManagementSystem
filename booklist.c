#include "booklist.h"
#include "appstate.h"
#include "dialog.h"

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
                gtk_list_box_remove(appstate->book_list, GTK_WIDGET(row));
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

void UpdateBookClbk(GtkWidget* w, gpointer parent_window){
    // get data struct from appstate
    AddBookWindowData* data = appstate->add_book_window_data;
    GtkWindow* pwin = GTK_WINDOW(parent_window);
    const char* tempstr;

    const char* isbn = gtk_entry_buffer_get_text(data->isbn);
    Book* book = FindBookByISBN(appstate->library, isbn);

    // start retrieving all data from window
    // get book name and check for errors
    book->name = strdup(gtk_entry_buffer_get_text(data->book_name));
    if(strlen(book->name) == 0){
        ShowErrorMessageDialog(pwin, "Book name cannot be empty!");
        DestroyBook(book);
        return;
    }

    // get author name and check for errors
    book->author = strdup(gtk_entry_buffer_get_text(data->author_name));
    if(strlen(book->author) == 0){
        ShowErrorMessageDialog(pwin, "Author name cannot be empty!");
        DestroyBook(book);
        return;
    }

    // get publisher name and check for errors
    book->publisher = strdup(gtk_entry_buffer_get_text(data->publisher_name));
    if(strlen(book->publisher) == 0){
        ShowErrorMessageDialog(pwin, "Publisher name cannot be empty!");
        DestroyBook(book);
        return;
    }

    // no need to update isbn

    // get total number of copies and check for errors
    tempstr = gtk_entry_buffer_get_text(data->total_copies);
    if(strlen(tempstr) == 0){
        ShowErrorMessageDialog(pwin, "Total number of copies cannot be empty!");
        DestroyBook(book);
        return;
    }
    book->total_copies = strtoull(tempstr, NULL, 10);

    // get available copies and check for errors
    tempstr = gtk_entry_buffer_get_text(data->available_copies);
    if(strlen(tempstr) == 0){
        ShowErrorMessageDialog(pwin, "Total number of available copies cannot be empty!");
        DestroyBook(book);
        return;
    }
    book->available_copies = strtoull(tempstr, NULL, 10);

    // check that total available copies not exceed total number of copies
    if(book->available_copies > book->total_copies){
        ShowErrorMessageDialog(pwin, "Number of available copies cannot exceed "
                               "total number of copies!");
        DestroyBook(book);
        return;
    }

    // get row number and check for errors
    tempstr = gtk_entry_buffer_get_text(data->row);
    if(strlen(tempstr) == 0){
        ShowErrorMessageDialog(pwin, "Row number cannot be empty!");
        DestroyBook(book);
        return;
    }
    book->row = strtoull(tempstr, NULL, 10);

    // get shelf number and check for errors
    tempstr = gtk_entry_buffer_get_text(data->shelf);
    if(strlen(tempstr) == 0){
        ShowErrorMessageDialog(pwin, "Shelf number cannot be empty!");
        DestroyBook(book);
        return;
    }
    book->shelf = strtoull(tempstr, NULL, 10);


    // get description
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(data->description, &start);
    gtk_text_buffer_get_end_iter(data->description, &end);
    book->description = gtk_text_buffer_get_text(data->description, &start, &end, FALSE);
    if(strlen(book->description) == 0){
        ShowErrorMessageDialog(pwin, "Book description cannot be empty!");
        DestroyBook(book);
        return;
    }

    // find index of book in book list
    size_t idx = 0;
    for(size_t i = 0; i < appstate->library->num_books; i++){
        // get row data from each book
        GtkListBoxRow* row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(appstate->book_list), i);
        const char** row_data = GetDataFromBookListRow(row);

        // compare isbn
        if(strcmp(row_data[2], book->isbn) == 0){
            GtkWidget* li = CreateBookListItem(book);
            gtk_list_box_remove(GTK_LIST_BOX(appstate->book_list), GTK_WIDGET(row));
            gtk_list_box_append(GTK_LIST_BOX(appstate->book_list), GTK_WIDGET(li));
            break;
        }

    }

    // finally destroy window
    gtk_window_destroy(pwin);
}

// interface of update book is same as add book
void UpdateBookDataClbk(GtkWidget* w, gpointer user_data){
     // create builder
    GtkBuilder* builder = gtk_builder_new_from_file("add_book.ui");

    // get window
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "add_book_window"));

    // attach event to close window
    GtkButton* cancel_btn = GTK_BUTTON(gtk_builder_get_object(builder, "cancel_btn"));
    g_signal_connect_swapped(cancel_btn, "clicked", G_CALLBACK(gtk_window_destroy), window);

    // attach event to take all data and add book to database
    GtkButton* update_book_btn = GTK_BUTTON(gtk_builder_get_object(builder, "add_book_btn"));
    gtk_button_set_label(update_book_btn, "Update Book");
    g_signal_connect(update_book_btn, "clicked", G_CALLBACK(UpdateBookClbk), window);

    /**
     * Can't use AddBookWindowData  struct here.
     * because pointers in that struct are valid only for that window.
     * Here we have a new window so new pointers :-)
     * */
    AddBookWindowData* data = (AddBookWindowData*)malloc(sizeof(AddBookWindowData));
    appstate->add_book_window_data = data;

    // get book from user data
    Book* book = (Book*)user_data;

    // set book name for book name buffer
    GtkEntry* entry = GTK_ENTRY(gtk_builder_get_object(builder, "book_name_entry"));
    GtkEntryBuffer* ebuff = gtk_entry_get_buffer(entry);
    data->book_name = ebuff;
    gtk_entry_buffer_set_text(ebuff, book->name, strlen(book->name));

    // set author name in author name buffer
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "author_name_entry"));
    ebuff = gtk_entry_get_buffer(entry);
    data->author_name = ebuff;
    gtk_entry_buffer_set_text(ebuff, book->author, strlen(book->author));

    // set publisher name in buffer
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "publisher_name_entry"));
    ebuff = gtk_entry_get_buffer(entry);
    data->publisher_name = ebuff;
    gtk_entry_buffer_set_text(data->publisher_name, book->publisher, strlen(book->publisher));

    // set description in buffer
    GtkTextView* tv = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "description_entry"));
    GtkTextBuffer* tbuff = gtk_text_view_get_buffer(tv);
    data->description = tbuff;
    gtk_text_buffer_set_text(tbuff, book->description, strlen(book->description));

    // set isbn in buffer
    // NOTE : Might crash here!
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "isbn_entry"));
    ebuff = gtk_entry_get_buffer(entry);
    data->isbn = ebuff;
    gtk_entry_buffer_set_text(ebuff, book->isbn, strlen(book->isbn));
    // cant change isbn number
    gtk_widget_set_sensitive(GTK_WIDGET(entry), FALSE);

    // temporary buffer to hold size_t to string conversions
    char buff[10];

    // get row buffer
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "row_entry"));
    ebuff = gtk_entry_get_buffer(entry);
    data->row = ebuff;
    sprintf(buff, "%zu", book->row);
    gtk_entry_buffer_set_text(ebuff, buff, strlen(buff));

    // get shelf buffer
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "shelf_entry"));
    ebuff = gtk_entry_get_buffer(entry);
    data->shelf = ebuff;
    sprintf(buff, "%zu", book->shelf);
    gtk_entry_buffer_set_text(ebuff, buff, strlen(buff));

    // get total copies buffer
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "total_copies_entry"));
    ebuff = gtk_entry_get_buffer(entry);
    data->total_copies = ebuff;
    sprintf(buff, "%zu", book->total_copies);
    gtk_entry_buffer_set_text(ebuff, buff, strlen(buff));


    // get available copies buffer
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "available_copies_entry"));
    ebuff = gtk_entry_get_buffer(entry);
    data->available_copies = ebuff;
    sprintf(buff, "%zu", book->available_copies);
    gtk_entry_buffer_set_text(ebuff, buff, strlen(buff));

    // show widgets
    gtk_widget_show(GTK_WIDGET(window));

    // we do not need builder anymore
    g_object_unref(builder);
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
    widget = GTK_WIDGET(gtk_builder_get_object(builder, "update_book_data_btn"));
    g_signal_connect(widget, "clicked", G_CALLBACK(UpdateBookDataClbk), book);

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
