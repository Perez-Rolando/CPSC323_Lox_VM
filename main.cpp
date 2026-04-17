//The test driver 
//A simple test that helps me verify if the chunk and disassembler work
#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char* argv[]) {
// Create a chunk
    Chunk chunk;
    initChunk(&chunk);
    
// Add a constant to the constant pool
    int constant = addConstant(&chunk, 1.2);
    
// Write the instruction to load that constant
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);
    
// Write a return instruction
    writeChunk(&chunk, OP_RETURN, 123);
    
// Disassemble and print the chunk
    disassembleChunk(&chunk, "test chunk");
    
// Clean up
    freeChunk(&chunk);
    
    return 0;
}