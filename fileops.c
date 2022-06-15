#include "fileops.h"
#include "logger.h"
#include "string.h"
#include "malloc.h"

/**
 * Write string data to given file. String data is the size of
 * given string (8 bytes) and next the string itself.
 *
 * @param dbfile File to write data to.
 * @param str String to write data of.
 * */
void WriteStringDataToFile(FILE* dbfile, const char* str){
    if(dbfile == NULL)
        LogError("Invalid database file provided\n");

    if(str != NULL || strlen(str) != 0){
        // store string size
        size_t sz = strlen(str);
        sz = fwrite(&sz, sizeof(size_t), 1, dbfile);
        if(sz != 1)
            LogError("Failed to write complete data to dbfile\n");

        // store string
        sz = fwrite(str, 1, strlen(str), dbfile);
        if(sz != strlen(str))
            LogError("Failed to write complete data to dbfile\n");
    }else{
        // if string is empty then only write 0 in place of size
        // and no need to write any placeholder for string
        size_t sz = 0;
        sz = fwrite(&sz, sizeof(size_t), 1, dbfile);
        if(sz != 1){
            LogError("Failed to write complete data to dbfile\n");
        }
    }
}

/**
 * Load written string data from given dbfile.
 * To be used when there is a string data starting from current
 * cursor position.
 *
 * @warn User must free the string returned by this function
 * explicitly.
 *
 * @param dbfile Database file to load string data from.
 * @return malloc'd pointer to string.
 * */
const char* ReadStringDataFromFile(FILE* dbfile){
    size_t sz, strsz;

    // get size of string to allocate memory correspondingly
    sz = fread(&strsz, sizeof(size_t), 1, dbfile);
    if(sz != 1)
        LogError("Failed to read string data from dbfile\n");

    // if string is not an empty string then read
    // size of string and the string itself
    if(strsz != 0){
        // create memory for string to be loaded
        char* str = (char*)malloc(strsz+1);
        if(str == NULL)
            LogError("Failed to allocate memory for string\n");

        // read the string itself
        sz = fread(str, 1, strsz, dbfile);
        if(sz != strsz)
            LogError("Failed to read string data from dbfile\n");

        // null terminate string
        str[strsz] = 0;
        return str;
    }

    // if string is empty string then by default NULL will be returned
    return NULL;
}
