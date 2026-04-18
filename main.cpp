//The test driver 
//Testing the scanner by tokenizing a simple Lox Program
#include <cstdio>
#include "common.h"
#include "scanner.h"

int main(int argc, const char* argv[]) {
    // Test source code
    const char* source = "var x = 10 + 5;\nprint x;";
    
    initScanner(source);
    
    int line = -1;
    for (;;) {
        Token token = scanToken();
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }
        printf("%2d '%.*s'\n", token.type, token.length, token.start);
        
        if (token.type == TOKEN_EOF) break;
    }
    
    return 0;
}