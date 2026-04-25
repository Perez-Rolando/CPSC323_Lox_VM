//Heap Allocated Values
//The Objects are dynamically allocated values like strings, functions, classes

#ifndef clox_object_h
#define clox_object_h

#include <cstdint>  
#include "common.h"
#include "value.h"

//The type of the object
typedef enum {
    OBJ_STRING,
} ObjType;

//This is the base structure for all Heap based Allocated objects
struct Obj{
    ObjType type;
    struct Obj* next;
};

//The string Object
struct ObjString {
    Obj obj;
    int length;
    char* chars;
    uint32_t hash;
};

//Taking ownership of a string
ObjString* takeString(char* chars, int length);

ObjString* copyString(const char* chars, int length);
void printObject(Value value);

//Getting the types of the object
#define OBJ_TYPE(value)        (AS_OBJ(value)->type)

//Checking if object is a string
#define IS_STRING(value)       isObjType(value, OBJ_STRING)

//Extracting a string from a vlaue
#define AS_STRING(value)       ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)      (((ObjString*)AS_OBJ(value))->chars)

//Checking if a Vlaue is an Object of any specstatic inline bool isObjType(Value value, ObjType type) {
static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

//Allocating a string Object from a C++ string
ObjString* copyString(const char* chars, int length);

//Printing an Object
void printObject(Value value);

#endif