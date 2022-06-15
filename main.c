#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "library.h"

/**
 * Print intro text about this software.
 * */
void PrintIntro(){
    puts(";---------------------------------------------------------------------------------;");
    puts(";                         LiMS Library Management System                          ;");
    puts(";        Copyright (c) GroupOne Software Solutions 2022. All Rights Reserved      ;");
    puts(";---------------------------------------------------------------------------------;");
}

/**
 * Resets the screen by clearing the terminal.
 * */
void ResetScreen(){
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

void BookSearchPage(){
    ResetScreen();
    PrintIntro();

    puts(";---------------------------------------------------------------------------------;");
    puts(";                                   BOOK SEARCH                                   ;");
    puts(";---------------------------------------------------------------------------------;");
    printf("Please enter book name : ");

    char bookname[40];
    scanf("%s", bookname);
}

void LibrarianLoginPage(){

}

void StudentLoginPage(){

}

void RegisterNewAccountPage(){

}

// prints main page menu onto screen
void PrintMainPage(){
    puts(";---------------------------------------------------------------------------------;");
    puts(";                                   MAIN PAGE                                     ;");
    puts(";---------------------------------------------------------------------------------;");
    puts("; [1] Book Search                                                                 ;");
    puts("; [2] Librarian Login                                                             ;");
    puts("; [3] Student Login                                                               ;");
    puts("; [4] Register New Account                                                        ;");
    puts("; [q] Quit LiMS                                                                   ;");
    puts(";---------------------------------------------------------------------------------;");
    puts("; Please enter any one of the options above.                                      ;");
    puts(";---------------------------------------------------------------------------------;");
}

void MainPage(){
    bool is_running = true;

    while(is_running){
        // reset screen and print intro
        ResetScreen();
        PrintIntro();

        // print main page menu
        PrintMainPage();

        // handle input
        char c = getchar();

        if(c == '1'){
            BookSearchPage();
        }else if(c == '2'){
            LibrarianLoginPage();
        }else if(c == '3'){
            StudentLoginPage();
        }else if(c == '4'){
            RegisterNewAccountPage();
        }else if(c == 'q'){
            is_running = false;
        }
    }
}

const char* InputString(const char* msg){
    size_t bufsz = 1024;
    static char buf[1024];

    memset((void*)buf, 0, bufsz);
    printf("%s : ", msg);
    scanf("%s", buf);

    char* str = (char*)malloc(strlen(buf)+1);
    if(str == NULL)
        LogError("Failed to allocate memory for input string\n");

    // copy string from buffer to allocated memory
    strcpy(str, buf);

    return str;
}

size_t InputSize(const char* msg){
    size_t input;

    printf("%s : ", msg);
    scanf("%zu", &input);

    return input;
}

void PrintBookData(Book* b){
    printf("Book Name\t: %s\n", b->name);
    printf("Author Name\t: %s\n", b->author);
    printf("Publisher\t: %s\n", b->publisher);
}

int main(){
    /* MainPage(); */

    puts("Book Entry");

    size_t numbooks = 4;
    Library* lib;
    /* CreateLibrary(numbooks); */

    /* for(size_t i = 0; i < numbooks; i++){ */
    /*     printf("\n\nBook[%zu]\n", i); */

    /*     Book* b = lib->books[i]; */

    /*     b->name = InputString("Book Name"); */
    /*     b->author = InputString("Author Name"); */
    /*     b->publisher = InputString("Publisher Name"); */

    /*     b->num_tags = InputSize("Number Of Tags"); */
    /*     b->tags = (const char**)malloc(b->num_tags*sizeof(char*)); */
    /*     for(size_t i = 0; i < b->num_tags; i++){ */
    /*         b->tags[i] = InputString("Enter Tag"); */
    /*     } */

    /*     b->description = InputString("Book Description"); */

    /*     const char* isbn = InputString("ISBN 13"); */
    /*     memcpy((void*)b->isbn, isbn, 13); */

    /*     b->row = InputSize("Book Row"); */
    /*     b->shelf = InputSize("Book Shelf"); */
    /*     b->total_copies = InputSize("Total Copies"); */
    /*     b->available_copies = InputSize("Available Copies"); */
    /* } */

    /* for(size_t i = 0; i < numbooks; i++) */
    /*     PrintBookData(lib->books[i]); */

    /* SaveLibraryData("library.ldb", lib); */
    /* DestroyLibrary(lib); */

    lib = LoadLibraryData("library.ldb");
    for(size_t i = 0; i < numbooks; i++)
        PrintBookData(lib->books[i]);

    return 0;
}
