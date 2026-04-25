//Virtual Machine the Execution Engine
//Made to execute the bytecode using a stack based architecture using "fetch-decode-execute" cycle
#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "table.h"
#include "value.h"

#define STACK_MAX 256

//The Virtual Machine State is made up of:
// -chunk:     The bytecode being executed
// - ip:       Instruction Pointer (points to next instruction to execute)
// - stack:    The value stack for operands
// - stackTop: Points to the next empty slot in the stack

typedef struct {
    Chunk* chunk;           // The chunk of bytecode executing
    uint8_t* ip;            // Instruction pointer (points to next byte to execute)
    Value stack[STACK_MAX]; // The value stack
    Value* stackTop;        // Points just past the last used stack slot
    Table strings;
    Obj* objects;
} VM;

//InnterpretResult returns the code for interperitation
typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

// Make VM global accessible for object allocation
extern VM vm;

//initVM is used to initialize the VM which is called before using the actual VM
void initVM();

//freeVM is used to free all the resources that are being used by the VM
void freeVM();

InterpretResult interpret(const char* source); //Interpreting a chunk of bytecode

void push(Value value);//Pushing a value onto the stack
Value pop();           //popping a value off the stack

#endif
