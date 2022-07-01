#include "book.h"
#include "fileops.h"

#include <string.h>
#include <stdlib.h>

Book* CreateBook(){
    Book* b = (Book*)malloc(sizeof(Book));

    b->name = NULL;
    b->author = NULL;
    b->publisher = NULL;
    b->num_tags = 0;
    b->tags = NULL;
    b->description = NULL;
    b->isbn = (char*)malloc(14);
    memset((void*)b->isbn, 0, 13);
    b->row = 0;
    b->shelf = 0;
    b->total_copies = 0;
    b->available_copies = 0;

    return b;
}

void DestroyBook(Book* b){
    if(b == NULL)
        LogError("Attempt to double free book data\n");

    if(b->name) free((void*)b->name);
    if(b->author) free((void*)b->author);
    if(b->publisher) free((void*)b->publisher);

    // free all tags
    if(b->tags){
        for(size_t i = 0; i < b->num_tags; i++)
            if(b->tags[i]) free((void*)b->tags[i]);
        free((void*)b->tags);
    }

    if(b->description) free((void*)b->description);
    free((void*)b->isbn);

    b->row = 0;
    b->shelf = 0;
    b->total_copies = 0;
    b->available_copies = 0;

    free((void*)b);
}

/**
 * FORMAT SPECIFICATION :
 * Before any book block a signature "LIMSBOOKSTART" is written
 * in order to identify starting of a book block and in the end
 * "LIMSBOOKEND" is written.
 * Before any array/string is written, size of that array is
 * stored.
 *
 * ;----------------------------;
 * ;        LIMSBOOKSTART       ;
 * ;----------------------------;
 * ; size of book name string   ;
 * ; book name                  ;
 * ; size of author name string ;
 * ; author name                ;
 * ; size of publisher string   ;
 * ; publisher                  ;
 * ; number of tags             ;
 * ; tag                        ;
 * ; tag                        ;
 * ; .                          ;
 * ; .                          ;
 * ; .                          ;
 * ; description length         ;
 * ; description                ;
 * ; isbn                       ;
 * ; book id                    ;
 * ; total copies               ;
 * ; available copies           ;
 * ;----------------------------;
 * ;          LIMSBOOKEND       ;
 * ;----------------------------;
 * */

void SaveBookData(FILE* dbfile, Book* book){
    size_t sz;
    const char* block_start_sig = "LIMSBOOKSTART";
    const char* block_end_sig = "LIMSBOOKEND";

    // write signature before any book data block
    sz = fwrite(block_start_sig, 1, strlen(block_start_sig), dbfile);
    if(sz != strlen(block_start_sig))
        LogError("Failed to write complete data to dbfile\n");

    // write book name to file
    WriteStringDataToFile(dbfile, book->name);

    // write author name to file
    WriteStringDataToFile(dbfile, book->author);

    // write publisher name to file
    WriteStringDataToFile(dbfile, book->publisher);

    // write number of tags
    sz = fwrite(&book->num_tags, sizeof(size_t), 1, dbfile);
    if(sz != 1)

        LogError("Failed to write complete data to dbfile\n");
    // write tags data
    for(size_t i = 0; i < book->num_tags; i++)
        WriteStringDataToFile(dbfile, book->tags[i]);


    // write description
    WriteStringDataToFile(dbfile, book->description);

    // store book isbn
    sz = fwrite(book->isbn, 1, 13, dbfile);
    if(sz != 13)
        LogError("Failed to write complete data to dbfile\n");

    // store book row
    sz = fwrite(&book->row, sizeof(size_t), 1, dbfile);
    if(sz != 1)
        LogError("Failed to write complete data to dbfile\n");

    // store book shelf
    sz = fwrite(&book->shelf, sizeof(size_t), 1, dbfile);
    if(sz != 1)
        LogError("Failed to write complete data to dbfile\n");

    // store total copies
    sz = fwrite(&book->total_copies, sizeof(size_t), 1, dbfile);
    if(sz != 1)
        LogError("Failed to write complete data to dbfile\n");

    // store available copies
    sz = fwrite(&book->available_copies, sizeof(size_t), 1, dbfile);
    if(sz != 1)
        LogError("Failed to write complete data to dbfile\n");

    // write end block
    sz = fwrite(block_end_sig, 1, strlen(block_end_sig), dbfile);
    if(sz != strlen(block_end_sig))
        LogError("Failed to write complete data to dbfile\n");
}

void LoadBookData(FILE* dbfile, Book* book){
    // create temporary buffer
    char* buf[20];
    memset(buf, 0, 20);

    size_t sz;
    const char* block_start_sig = "LIMSBOOKSTART";
    const char* block_end_sig = "LIMSBOOKEND";

    // read start signature and verify
    sz = fread(buf, 1, strlen(block_start_sig), dbfile);
    if(sz != strlen(block_start_sig))
        LogError("Failed to read complete data from dbfile\n");

    // compare read string with signature
    if(strcmp((const char*)buf, block_start_sig) != 0)
        LogError("INVALID or CURROPT database file.\n");

    // read book name
    book->name = ReadStringDataFromFile(dbfile);

    // read auhtor name
    book->author = ReadStringDataFromFile(dbfile);

    // read publisher name
    book->publisher = ReadStringDataFromFile(dbfile);

    // get number of tags from file
    sz = fread(&book->num_tags, sizeof(size_t), 1, dbfile);
    if(sz != 1)
        LogError("Failed to read data from database file\n");

    // allocate space for tags
    char** tags = (char**)malloc(book->num_tags);
    if(tags == NULL)
        LogError("Failed to allocate space for tags array\n");

    // read tags
    for(size_t i = 0; i < book->num_tags; i++)
        book->tags[i] = ReadStringDataFromFile(dbfile);

    // read description
    book->description = ReadStringDataFromFile(dbfile);

    // read isbn
    memset((char*)book->isbn, 0, 14);
    sz = fread((void*)book->isbn, 1, 13, dbfile);
    if(sz != 13)
        LogError("Failed to read data from dbfile\n");

    // read book row
    sz = fread(&book->row, sizeof(size_t), 1, dbfile);
    if(sz != 1)
        LogError("Failed to read data from dbfile\n");

    // read book shelf
    sz = fread(&book->shelf, sizeof(size_t), 1, dbfile);
    if(sz != 1)
        LogError("Failed to read data from dbfile\n");

    // read book total copies
    sz = fread(&book->total_copies, sizeof(size_t), 1, dbfile);
    if(sz != 1)
        LogError("Failed to read data from dbfile\n");

    // read book available copies
    sz = fread(&book->available_copies, sizeof(size_t), 1, dbfile);
    if(sz != 1)
        LogError("Failed to read data from dbfile\n");

    // read and verify block end signature
    // reset buf first
    memset(buf, 0, 20);
    sz = fread(buf, 1, strlen(block_end_sig), dbfile);
    if(sz != strlen(block_end_sig))
        LogError("INVALID or CURROPT database file\n");
}
