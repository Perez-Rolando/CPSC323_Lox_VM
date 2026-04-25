//Hash Table for Strings
//Hash Table that is able to map ObjString to keys to values
#ifndef clox_table_h
#define clox_table_h

#include "common.h"
#include "value.h"

//Single Key Value pair for the structure of the Table
typedef struct {
    ObjString* key;
    Value value;
} Entry;

//The hash table structure
typedef struct {
    int count;
    int capacity;
    Entry* entries;
} Table;

//Starting up an Empty Hash Table
void initTable(Table* table);   // <-- ADD THIS LINE

//Free all memory used by a hash table
void freeTable(Table* table);

//Get a value from the table
bool tableGet(Table* table, ObjString* key, Value* value);  // <-- ADD THIS LINE

//Actually setting a Value in the Table
bool tableSet(Table* table, ObjString* key, Value value);

//Deleting an entry from a Table
bool tableDelete(Table* table, ObjString* key);

//Copying all the entries from one table to another
void tableAddAll(Table* from, Table* to);

//Finding a string from a Table 
//This is used to check if the string already exist or not before actually creating one
ObjString* tableFindString(Table* table, const char* chars, int length,
                            uint32_t hash);

#endif