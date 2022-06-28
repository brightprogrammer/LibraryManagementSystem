#ifndef ADDBOOK_H_
#define ADDBOOK_H_

#include <gtk/gtk.h>

/**
 * Helper struct to hold buffers of all text entries present in
 * the add book window. This will make our task of retrieving values
 * from those text entries after "Add Book" button click easier!.
 * */
typedef struct {
    GtkEntryBuffer* book_name;
    GtkEntryBuffer* author_name;
    GtkEntryBuffer* publisher_name;
    GtkEntryBuffer* isbn;
    GtkTextBuffer* description;
    GtkEntryBuffer* total_copies;
    GtkEntryBuffer* available_copies;
    GtkEntryBuffer* row;
    GtkEntryBuffer* shelf;
} AddBookWindowData;

/**
 * Add book for final into database.
 * This will retreive text values stored in buffers in AddBookWindowData struct
 * and create a Book struct and add it to library after proper checks.
 * */
void AddBookClbk(GtkButton* btn, gpointer parent_window);

/**
 * Add Book callback function. This will show a window to add new books.
 * */
void OpenAddNewBookWindowClbk(GtkButton* btn, gpointer user_data);

#endif // ADDBOOK_H_
