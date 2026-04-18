//The Scanner for Lexical Analysis This is used to turn the Human written code into something that the computer can actually read
//Converts Lox source code into a stream of tokens
//Scans on-demand rather than scanning the entire file upfront
#ifndef clox_scanner_h
#define clox_scanner_h

//ALL of the TokenTypes in Lox
typedef enum {
// Single-character tokens
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
    
    // One or two character tokens
    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,
    
    // Literals
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
    
    // Keywords
    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
    TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
    TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,
    
    TOKEN_ERROR, TOKEN_EOF
} TokenType;

//Token which is just a single lexixal token
//no string are used for allocation instead we use pointers that point into the source code 
//which is way more efficient

typedef struct {
    TokenType type;
    const char* start;  // Points to the first character of the token
    int length;         // Length of the token
    int line;           // Line number
} Token;

//source which is used to Initialize the scanner with source code 
void initScanner(const char* source);

//scanning and returning the next token
Token scanToken();

#endif