//Object Implemenntation
#include <cstdio>
#include <cstring>
#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"
/**
 * Object Implementation
 * 
 * Reference: https://craftinginterpreters.com/strings.html
 */

#include <cstdio>
#include <cstring>
#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

//Allocate an object of the given size and type
#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

//Allocate memory for an object
static Obj* allocateObject(size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(nullptr, 0, size);
    object->type = type;
    
    // Add to the VM's linked list of objects
    object->next = vm.objects;
    vm.objects = object;
    
    return object;
}

//Allocateing a string object
static ObjString* allocateString(char* chars, int length) {
    ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    return string;
}

//Createing a string object by copying characters
ObjString* copyString(const char* chars, int length) {
    char* heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString(heapChars, length);
}

//Taking Ownership of a charcter array
ObjString* takeString(char* chars, int length) {
    return allocateString(chars, length);
}

//Printing an object
void printObject(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}