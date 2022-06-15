#ifndef FILEOPS_H_
#define FILEOPS_H_

#include <stdio.h>

/**
 * Write string data to given file. String data is the size of
 * given string (8 bytes) and next the string itself.
 *
 * @param dbfile File to write data to.
 * @param str String to write data of.
 * */
void WriteStringDataToFile(FILE* dbfile, const char* str);

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
const char* ReadStringDataFromFile(FILE* dbfile);

#endif // FILEOPS_H_
