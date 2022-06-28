#ifndef APPSTATE_H_
#define APPSTATE_H_

#include <gtk/gtk.h>

#include "library.h"
#include "addbook.h"

/**
 * App state is a helper struct to keep track
 * of objects that need to be accessed globally anytime.
 *
 * For eg : Login status of users, library database etc...
 * */

/* THERE MUST BE ONLY ONE APP STATE THROUGHOUT THE APPLICATION!! */
typedef struct {
    Library* library;
    GtkListBox* book_list;
    AddBookWindowData* add_book_window_data;
} AppState;

/**
 * Create the AppState object.
 *
 * @return AppState*
 * */
AppState* CreateAppState();

/**
 * Destroy given app state.
 * Destruction of app state means application is being closed.
 * This will destroy all members.
 *
 * @param appstate Pointer to the globally maintained AppState.
 * */
void DestroyAppState(AppState* appstate);

#endif // APPSTATE_H_
