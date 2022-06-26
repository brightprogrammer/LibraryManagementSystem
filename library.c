#include "library.h"

#include <string.h>
#include <stdlib.h>

Library* CreateLibrary(size_t numbooks){
    Library *lib = (Library*)malloc(sizeof(Library));

    lib->num_books = 0;

    if(numbooks){
        lib->num_books = numbooks;
        lib->books = (Book**)malloc(numbooks * sizeof(Book*));

        // create books
        for(size_t i = 0; i < numbooks; i++)
            lib->books[i] = CreateBook();
    }else lib->books = NULL;

    return lib;
}

void DestroyLibrary(Library* lib){
    if(lib == NULL)
        LogError("Attempt to double free library data\n");

    // destroy book one by one
    for(size_t i = 0; i < lib->num_books; i++){
        if(lib->books[i])
            DestroyBook(lib->books[i]);
        lib->books[i] = NULL;
    }

    // free books array
    if(lib->books) free((void*)lib->books);

    lib->num_books = 0;
    lib->books = NULL;

    free(lib);
}

/**
 * FORMAT SPECIFICATION :
 * ;-------------------------------------;
 * ;            LIBSLIBDBSTART           ;
 * ;-------------------------------------;
 * ; number of books                     ;
 * ;-------------------------------------;
 * ;             LIMSBOOKSTART           ;
 * ;-------------------------------------;
 * ;
 * ;-------------------------------------;
 * ;             LIMSSBOOKEND            ;
 * ;-------------------------------------;
 * ;-------------------------------------;
 * ;             LIMSBOOKSTART           ;
 * ;-------------------------------------;
 * ;
 * ;-------------------------------------;
 * ;             LIMSSBOOKEND            ;
 * ;-------------------------------------;
 * ;           .                         ;
 * ;           . (multiple books)        ;
 * ;           .                         ;
 * ;-------------------------------------;
 * ;             LIBSLIBDBEND            ;
 * ;-------------------------------------;
 * */

void SaveLibraryData(const char* dbfilename, Library* lib){
    FILE* dbfile = fopen(dbfilename, "w");
    if(dbfile == NULL){
        LogError("Failed to open database file to write library data");
    }

    // write signature bytes to the starting of file
    // to mark that this is a valid database file
    const char* lib_start_sig = "LIMSLIBDBSTART";
    const char* lib_end_sig = "LIMSLIBDBEND";

    // write library db start signature
    size_t writesz = fwrite(lib_start_sig, 1, strlen(lib_start_sig), dbfile);
    if(writesz != strlen(lib_start_sig)){
        LogError("Failed to write complete data in database file");
    }

    // save the number of books in library
    writesz = fwrite(&lib->num_books, sizeof(lib->num_books), 1, dbfile);
    if(writesz != 1){
        LogError("Failed to write complete data in database file");
    }

    // write all book data
    for(size_t i = 0; i < lib->num_books; i++)
        SaveBookData(dbfile, lib->books[i]);

    // write library db end signature
    writesz = fwrite(lib_end_sig, 1, strlen(lib_end_sig), dbfile);
    if(writesz != strlen(lib_end_sig)){
        LogError("Failed to write complete data in database file");
    }

    // finally close file after writing
    fclose(dbfile);
}

Library* LoadLibraryData(const char* dbfilename){
    // open file for reading
    FILE* dbfile = fopen(dbfilename, "r");
    if(dbfile == NULL){
        LogError("Failed to open database file to read library data\n");
    }

    // library data start and end signature
    const char* lib_start_sig = "LIMSLIBDBSTART";
    const char* lib_end_sig = "LIMSLIBDBEND";

    // create temporary buffer
    char buf[20];
    memset(buf, 0, 20);

    // read and verify the start signature
    size_t readsz = fread(buf, 1, strlen(lib_start_sig), dbfile);
    if(readsz != strlen(lib_start_sig))
        LogError("Failed to read complete data from dbfile\n");

    // get number of books in library database
    size_t numbooks;
    readsz = fread(&numbooks, sizeof(size_t), 1, dbfile);
    if(readsz != 1)
        LogError("Failed to read complete data from dbfile\n");

    // create library
    Library *lib = CreateLibrary(numbooks);

    // load books one by one
    for(size_t i = 0; i < numbooks; i++)
        LoadBookData(dbfile, lib->books[i]);

    // read end signature
    memset(buf, 0, 20);
    readsz = fread(&buf, 1, strlen(lib_end_sig), dbfile);
    if(readsz != strlen(lib_end_sig))
        LogError("Failed to read complete data from dbfile\n");

    // finally close after reading
    fclose(dbfile);

    return lib;
}

/**
 * TODO: Create/Use a hash function to convert ISBN to an integer
 * and sort elements in database according to those integer values.
 * Use this to implement a faster search.
 *
 * At present this function performs a linear search which is good
 * for small database but quite slow for large database. Not fit for
 * production.
 * */
Book* GetBookByISBN(Library* library, const char* isbn){
    // go through each book and search
    for(size_t i = 0; i < library->num_books; i++){
        if(strcmp(isbn, library->books[i]->isbn) == 0) return library->books[i];
    }

    return NULL;
}
