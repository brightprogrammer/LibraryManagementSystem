#ifndef LIBRARY_H_
#define LIBRARY_H_

#include <stddef.h>

#include "book.h"

/**
 * Represents the whole library system.
 * */
typedef struct {
    size_t num_books;
    Book** books;
} Library;

/**
 * Create a new library struct.
 * All books created are empty and are created using
 * CreateBook function. To destroy library
 *
 * @param numbooks Number of books to allocate space for.
 * @return Newly allocated library struct.
 * */
Library* CreateLibrary(size_t numbooks);

/**
 * Destroy library struct and all book structs inside it.
 *
 * @param lib Pointer to mallocd library struct.
 * */
void DestroyLibrary(Library* lib);

/**
 * Saves the data of all library books into a database file.
 *
 * @param dbfilename Name of database file to load data from.
 * @param lib Library data to save to file.
 * */
void SaveLibraryData(const char* dbfilename, Library* lib);

/**
 * Loads the data of all library books from a database file.
 *
 * @param dbfilename Name of database file to load data from.
 * @param lib Library data to save to file.
 * @return Pointer to library struct where all data is loaded.
 * */
Library* LoadLibraryData(const char* dbfilename);

#endif // LIBRARY_H_
