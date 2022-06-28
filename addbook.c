#include "addbook.h"
#include "appstate.h"
#include "dialog.h"

// get global appstate
extern AppState* appstate;

void AddBookClbk(GtkButton* btn, gpointer parent_window){
    // get data struct from appstate
    AddBookWindowData* data = appstate->add_book_window_data;
    GtkWindow* pwin = GTK_WINDOW(parent_window);
    const char* tempstr;

    // first check whether another book with same ISBN number exists or not
    const char* isbn = gtk_entry_buffer_get_text(data->isbn);
    // continue only if no such match exists
    if(FindBookByISBN(appstate->library, isbn) == NULL){
        // create new book to hold data
        Book* newbook = CreateBook();

        /*
         * All entry buffers return data owned by gtk
         * Text buffers however return data owned by me
         * so it is my resposnibility to free that.
         * This is the reason why I don't use strdup for
         * gtk_text_buffer_get_text(...)
         * */

        // start retrieving all data from window
        // get book name and check for errors
        newbook->name = strdup(gtk_entry_buffer_get_text(data->book_name));
        if(strlen(newbook->name) == 0){
            ShowErrorMessageDialog(pwin, "Book name cannot be empty!");
            DestroyBook(newbook);
            return;
        }

        // get author name and check for errors
        newbook->author = strdup(gtk_entry_buffer_get_text(data->author_name));
        if(strlen(newbook->author) == 0){
            ShowErrorMessageDialog(pwin, "Author name cannot be empty!");
            DestroyBook(newbook);
            return;
        }

        // get publisher name and check for errors
        newbook->publisher = strdup(gtk_entry_buffer_get_text(data->publisher_name));
        if(strlen(newbook->publisher) == 0){
            ShowErrorMessageDialog(pwin, "Publisher name cannot be empty!");
            DestroyBook(newbook);
            return;
        }

        // get isbn and check for errors
        newbook->isbn = strdup(gtk_entry_buffer_get_text(data->isbn));
        if(strlen(newbook->isbn) == 0){
            ShowErrorMessageDialog(pwin, "ISBN cannot be empty!");
            DestroyBook(newbook);
            return;
        }

        // get total number of copies and check for errors
        tempstr = gtk_entry_buffer_get_text(data->total_copies);
        if(strlen(tempstr) == 0){
            ShowErrorMessageDialog(pwin, "Total number of copies cannot be empty!");
            DestroyBook(newbook);
            return;
        }
        newbook->total_copies = strtoull(tempstr, NULL, 10);

        // get available copies and check for errors
        tempstr = gtk_entry_buffer_get_text(data->available_copies);
        if(strlen(tempstr) == 0){
            ShowErrorMessageDialog(pwin, "Total number of available copies cannot be empty!");
            DestroyBook(newbook);
            return;
        }
        newbook->available_copies = strtoull(tempstr, NULL, 10);

        // check that total available copies not exceed total number of copies
        if(newbook->available_copies > newbook->total_copies){
            ShowErrorMessageDialog(pwin, "Number of available copies cannot exceed "
                                   "total number of copies!");
            DestroyBook(newbook);
            return;
        }

        // get row number and check for errors
        tempstr = gtk_entry_buffer_get_text(data->row);
        if(strlen(tempstr) == 0){
            ShowErrorMessageDialog(pwin, "Row number cannot be empty!");
            DestroyBook(newbook);
            return;
        }
        newbook->row = strtoull(tempstr, NULL, 10);

        // get shelf number and check for errors
        tempstr = gtk_entry_buffer_get_text(data->shelf);
        if(strlen(tempstr) == 0){
            ShowErrorMessageDialog(pwin, "Shelf number cannot be empty!");
            DestroyBook(newbook);
            return;
        }
        newbook->shelf = strtoull(tempstr, NULL, 10);


        // get description
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(data->description, &start);
        gtk_text_buffer_get_end_iter(data->description, &end);
        newbook->description = gtk_text_buffer_get_text(data->description, &start, &end, FALSE);
        if(strlen(newbook->description) == 0){
            ShowErrorMessageDialog(pwin, "Book description cannot be empty!");
            DestroyBook(newbook);
            return;
        }

        // finally add new book
        AddNewBookToLibrary(appstate->library, newbook);

        // add the book to books list
        GtkWidget* li = CreateBookListItem(newbook);
        gtk_list_box_append(GTK_LIST_BOX(appstate->book_list), li);
    }else{
        /* Show error message */
        ShowErrorMessageDialog(GTK_WINDOW(parent_window),  "A book with same ISBN number already exists!");
    }
}


/**
 * To be called whenever a close window event needs to be handled.
 * */
void CloseWindowClbk(GtkButton* btn, gpointer user_data){
    GtkWindow* win = GTK_WINDOW(user_data);
    gtk_window_destroy(win);
}


void OpenAddNewBookWindowClbk(GtkButton* btn, gpointer user_data){
    // create builder
    GtkBuilder* builder = gtk_builder_new_from_file("add_book.ui");

    // get window
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "add_book_window"));

    // attach event to close window
    GtkButton* cancel_btn = GTK_BUTTON(gtk_builder_get_object(builder, "cancel_btn"));
    g_signal_connect(cancel_btn, "clicked", G_CALLBACK(CloseWindowClbk), window);

    // attach event to take all data and add book to database
    GtkButton* add_book_btn = GTK_BUTTON(gtk_builder_get_object(builder, "add_book_btn"));
    g_signal_connect(add_book_btn, "clicked", G_CALLBACK(AddBookClbk), window);


    /**
     * AddBookWindowData struct helps here.
     * We store only buffers of what we want to retrieve text data from.
     * This way we don't have to go through child by child to detect which
     * field is what and then get text values from there.
     *
     * This might not be the best way but for now this works so I got with this...
     * This is named AddBookWindowData and not just AddBookData because members of this
     * struct are provided by gtk and directly related to a gtk widget.
     * */

    // allocate space for add book window data struct
    AddBookWindowData* data = (AddBookWindowData*)malloc(sizeof(AddBookWindowData));
    appstate->add_book_window_data = data;

    // get book name buffer
    GtkEntry* entry = GTK_ENTRY(gtk_builder_get_object(builder, "book_name_entry"));
    data->book_name = gtk_entry_get_buffer(entry);

    // get author name buffer
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "author_name_entry"));
    data->author_name = gtk_entry_get_buffer(entry);

    // get publisher name buffer
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "publisher_name_entry"));
    data->publisher_name = gtk_entry_get_buffer(entry);

    // get description buffer
    GtkTextView* tv = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "description_entry"));
    data->description = gtk_text_view_get_buffer(tv);

    // get isbn buffer
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "isbn_entry"));
    data->isbn = gtk_entry_get_buffer(entry);

    // get row buffer
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "row_entry"));
    data->row = gtk_entry_get_buffer(entry);

    // get shelf buffer
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "shelf_entry"));
    data->shelf = gtk_entry_get_buffer(entry);

    // get total copies buffer
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "total_copies_entry"));
    data->total_copies = gtk_entry_get_buffer(entry);

    // get available copies buffer
    entry = GTK_ENTRY(gtk_builder_get_object(builder, "available_copies_entry"));
    data->available_copies = gtk_entry_get_buffer(entry);

    // show widgets
    gtk_widget_show(GTK_WIDGET(window));

    // we do not need builder anymore
    g_object_unref(builder);
}
