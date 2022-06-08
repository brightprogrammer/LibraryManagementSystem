/**
 * @file menu.h
 * @date 8th June 2022
 * @copyright Copyright (c) GroupOne Software Solutions. All Rights Reserved.
 * */

#ifndef MENU_H_
#define MENU_H_

#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <string.h>

#include "logger.h"

/**
 * menu.h exposes a method to dynamically handle menus.
 * Normally one would keep adding features and keep on modifying
 * the hardcoded menu list and event handling cases.
 *
 * In a dynamic menu handling system, one would register a menu item,
 * a short description like 'q' for 'Quit' or 'c' for 'Cancel' and
 * a callback function with a callback data. When such an event
 * happens, the callback function (if provided) with callback
 * data (if provided) will be called.
 * */

/**
 * Represents a single menu item.
 * */
typedef struct{
    /// Describes when to trigger this event.
    char key;
    /// Short description to help when printing the menu.
    const char* short_description;
    /// This will be called when an event occurs.
    void (*callback)(void*);
} MenuItem;

/**
 * Represents a complete menu.
 * */
typedef struct{
    /// Name of this menu. This will be printed on top
    /// when `Menu_PrintMenu` is called.
    const char* name;
    /// Number of items in menu.
    size_t size;
    /// Menu item array.
    MenuItem* menu;
} Menu;

/**
 * Create a new Menu of given size. This function
 * must be called in order to support functions provided in
 * this module.
 *
 * @param name Name of this menu.
 * @param size Number of menu items in this menu.
 * @return Pointer to newly allocated Menu struct.
 * */
Menu* Menu_CreateMenu(const char* name, size_t size){
    // allocate space for menu
    Menu* m = (Menu*)malloc(sizeof(Menu));
    if(m == NULL){
        Logger_PrintError("Failed to allocate memory for Menu");
        return NULL;
    }

    // copy name of menu
    if(name != NULL){
        // temporary memory to store copied string
        char* tmpname = (char*)malloc(sizeof(size)+1);
        // null terminate string
        // since malloc does not guarantee array to be initialized with 0
        tmpname[size] = 0;
        // copy name
        m->name = (const char*)strcpy(tmpname, name);
    }

    // create menu item array
    m->size = size;
    m->menu = (MenuItem*)malloc(sizeof(MenuItem)*size);
    if(m->menu == NULL){
        Logger_PrintError("Failed to allocate memory for MenuItem array");
        return NULL;
    }

    return m;
}

/**
 * Destroys the given menu. Frees all allocated memory.
 * This function must be called when the created Menu
 * is no longer required.
 * */
void Menu_DestroyMenu(Menu* m){
    // Check whether there is a double free or not
    // Providing a way for double free can introduce
    // serious security bugs in software.
    if(m == NULL){
        Logger_PrintWarning("Double free for a Menu");
        return;
    }

    // free menu item array
    if(m->menu != NULL){
        free((void*)m->menu);
        m->menu = NULL;
        m->size = 0;
    }

    // finally free
    free((void*)m);
}

/**
 * Prints the given menu on screen
 * like an enumerated list.
 * */
void Menu_PrintMenu(Menu* m){
    for(size_t i = 0; i < m->size; i++){
        printf("  [%2zu] | %c -> %s\n", i, m->menu[i].key, m->menu[i].short_description);
    }
}

void Menu_AddItem(Menu* m){

}

#endif // MENU_H_
