#include "appstate.h"
#include "library.h"
#include "book.h"

AppState* CreateAppState(){
    AppState* appstate = (AppState*)malloc(sizeof(AppState));

    appstate->library = NULL;
    appstate->book_list = NULL;
    appstate->add_book_window_data = NULL;
}

void DestroyAppState(AppState* appstate){
    if(appstate == NULL) LogError("Attempt to double free for global app state object\n");

    // save library data before destroying it
    SaveLibraryData("library.ldb", appstate->library);

    // destroy library database
    DestroyLibrary(appstate->library);

    /* list box is destroyed by gtk itself so we don't need to handle that! */

    // destroy window data struct
    free(appstate->add_book_window_data);

    free((void*)appstate);
}
