#ifndef BOOK_H_
#define BOOK_H_

#include <stdio.h>
#include <stddef.h>

#include "logger.h"

/**
 * Stores data about a single book.
 * */
typedef struct {
    char* name;
    char* author;
    char* publisher;
    size_t num_tags;
    char** tags;
    char* description;
    char* isbn;
    size_t row;
    size_t shelf;
    size_t total_copies;
    size_t available_copies;
} Book;

/**
 * Create an empty book struct. Use this to create a book struct.
 * To destroy book data, use DestroyBook function.
 *
 * @return Newly allocated book struct.
 * */
Book* CreateBook();

/**
 * Destroy book struct. This will destroy all
 * mallocd fields in Book struct.
 *
 * @param b Book struct pointer.
 * */
void DestroyBook(Book* b);

/**
 * Saves data stored in book struct to given file stream.
 *
 * @param dbfile Database file to store data into.
 * @param book Book struct containing book data.
 * */
void SaveBookData(FILE* dbfile, Book* book);

/**
 * Load book data starting from current cursor position
 * from provided database file.
 *
 * @param dbfile Database file containing book data.
 * @param book Struct to load book data into.
 * */
void LoadBookData(FILE* dbfile, Book* book);

#endif // BOOK_H_
