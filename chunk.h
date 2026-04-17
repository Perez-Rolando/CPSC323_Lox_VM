// Chunk = a group of bytecode instructions
// Contains:
// - array of byte code instructions (opcodes and operands)
// - A constant pool which is used for literal values
// - Line number information for error reporting
#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

// OPCODE - The instruction set for the Lox VM
typedef enum {
    OP_CONSTANT,    // Load a constant from the constant pool
    OP_RETURN,      // Return from the current function
} OpCode;

// Chunk - The Dynamic array of bytecode
typedef struct {
    int count;            // Number of bytes currently in use
    int capacity;         // Total allocated capacity
    uint8_t* code;        // Dynamic array of bytecode
    int* lines;           // Parallel array of line numbers (for error reporting)
    ValueArray constants; // Constant pool - stores literal values
} Chunk;

void initChunk(Chunk* chunk); //initialize a new empty chunk
                              //this needs to be used before using a chunk
                              
void freeChunk(Chunk* chunk); //free all the memory used by a chunk
                              //after you call this you can reuse the chunk by calling initChunk again

//Adding a byte to a chunk
//chunk: The Chunk to write to 
//byte : The byte to append to which is usually an opcode or operand
//line : The source line nummber the byte corresponds to
void writeChunk(Chunk* chunk, uint8_t byte, int line);

//Adding a constant value to the chunk's constant pool
//chunk: The chunk to add to
//value: The value to add to 
//index: where the constant was stored
int addConstant(Chunk* chunk, Value value);

#endif