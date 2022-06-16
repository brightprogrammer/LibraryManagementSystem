
# Library Management System (LiMS)

Library management project made for 1st year CS102 Lab course at BIT Mesra. Completely written in C.

In this project, we write the complete database management backend by ourselves. We implement basic **CRUD** functionality for all our database components.
We need to effectively maintain 2 databases :  
- Books database
- User (Student/Librarian) database

Explanation of file structure : 
- `main.c` implements basic application code.
- `logger.h` and `logger.c` provide basic logging functionalities.
- `fileops.h` and `fileops.c`provide basic read write function for complex data to file. Used by `book` and `library` modules.
- `book.h` and `book.c` handles read/write functionality for single book data.
- `library.h` and `library.c` handles create/read/update/delete (CRUD) functionality for whole library database.

#### Common Methods/Conventions Used Throught Project
- When saving a string data in file, first we store the size of string by dumping a `size_t` variable that contains size of string and then the string is written to file. This makes it easier to determine the actual string size beforehand and we can allocate exact memory required to string to be loaded before reading it in memory.
- Most of the memory allocations done are dynamic (use of `malloc`).
- **Create** and **Destroy** functions are implemented for structs like `Library` and `Book` in order to handle automatic destruction in one call.
- This means when a `Library` struct is destroyed, all `Book` structs and data inside these `Book` structs are destroyed.
- Whenever reading/writing data from/to a file, always check the size of data read/written and compare with the actual data size. If the size does not match then raise an error by calling `LogError`.
- Before dumping a block of data, for e.g. `Book` data, a signature `LIMSBOOKSTART` is written and in the end, end signature `LIMSBOOKEND` is written. This is to mark the start and end of a `Book` data block. Although this is not necessary, this can give hints whether the data is altered/tampered or not. Similary when dumping library data, `LIMSLIBDBSTART` and `LIMSLIBDBEND` signatures are used.

#### Copyright
<p xmlns:dct="http://purl.org/dc/terms/" xmlns:vcard="http://www.w3.org/2001/vcard-rdf/3.0#">
<a rel="license"
href="http://creativecommons.org/publicdomain/zero/1.0/">
<img src="http://i.creativecommons.org/p/zero/1.0/88x31.png" style="border-style: none;" alt="CC0" />
</a>
<br />
To the extent possible under law,
<a rel="dct:publisher"
href="https://brightprogrammer.netlify.app">
<span property="dct:title">Siddharth Mishra</span></a> and their lab team-mates
has waived all copyright and related or neighboring rights to
<span property="dct:title">Library Management System</span>.
    This work is published from:
    <span property="vcard:Country" datatype="dct:ISO3166"
content="IN" about="https://github.com/brightprogrammer/LibraryManagementSystem">
India</span>.
    </p>
