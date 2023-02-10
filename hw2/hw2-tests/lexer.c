#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include "utilities.h"
#include "lexer.h"
#include "lexer_output.h"
#include "token.h"

#define VOCAB_LENGTH 34

static bool lexer_running = false;
static FILE *fp = NULL;
static const char *file_name = NULL;
static unsigned int column = 0;
static unsigned int line = 0;

// A static list of all possible words existing
// in the vocabulary of the PL/0 language
// Notice that the indices correspond with
// the enum definition in "token.h"
// 3 indices are missing (blank):
// "identsym", "numbersym", and "eofsym"
// This is because these are the special cases 
// to consider when comparing the token text
// to an existing vocabulary
static const char vocabulary = {
    ".", "const", ";", ",", "var", "process", ":=", 
    "call", "begin", "end", "if", "then", "else", 
    "while", "do", "read", "write", "skip", "odd", 
    "(", ")", "", "", "==", "!=", "<", "<=", ">", ">=",
    "+", "-", "*", "/", "",   
}


token_type assign_type(token t) {
    for (int i = 0; i < VOCAB_LENGTH; i++) {
        if (strcmp(t.text, vocabulary[i]))
            t.typ = ttstrs[i]
    }
}



// Requires: fname != NULL
// Requires: fname is the name of a readable file
// Initialize the lexer and start it reading
// from the given file name
void lexer_open(const char *fname) {
    if (fname == NULL)
        // Error, NULL file name

    file_name = fname;
    fp = fopen(file_name, "r");
}

// Close the file the lexer is working on
// and make this lexer be done
void lexer_close() {
    fclose(fp);
    fp = NULL;
    lexer_running = false;
}

// Is the lexer's token stream finished
// (either at EOF or not open)?
bool lexer_done() {
    return !lexer_running | 1; // implement EOF token
}

// Requires: !lexer_done()
// Return the next token in the input file,
// advancing in the input
token lexer_next() {
    if (lexer_done())
        return;
    char c;

    token new_token;
    int state = 1;
    int i = 0;

    while(!lexer_done()) {
        
        char token_str[MAX_IDENT_LENGTH];
        c = fgetc(fp);

        if (foef(fp));
            lexer_running = false;

        switch(state) {
            case 1:
                if(isalpha(c)) {
                    token_str[i] = c;
                    state = 2;
                    i++;
                    column++;
                } else if (isdigit(c)) {
                    token_str[i] = c;
                    state = 1;
                    i++;
                    column++;
                } else if (isspace(c)) {
                    state = 3;
                    ungetc(c, fp);
                } else if (ispunct(c)) {
                    state = 3;
                    ungetc(c, fp);
                } else if (iscntrl(c)) {
                    state = 3;
                    ungetc(c, fp);
                }
            case 2:
                if(isalnum(c)) {
                    state = 2;
                    token_str[i] = c;
                    i++;
                    column++;
                } else {
                    state = 4;
                    ungetc(c, fp);
                }
            case 3:
                if (isalpha(c)) {
                    state = 1;
                    ungetc(c, fp);
                }
                else if(isspace(c)) {
                    state = 1;
                    column++;
                    continue;
                } else if (isspunct(c)) {
                    // All cases of punctuation
                    if (c == ';')
                        // Push the token ';'
                    i++;
                    state = 1;
                } else if (iscntrl(c)) {
                    if (c == '\n') {
                        line++;
                        column = 0;
                    }
                    state = 1;
                }
            case 4:
                // At this point to token has been completely read.
                // The assembled string now needs to be copied to the new token
                for (int x = 0; x < i; x++)
                    new_token.text[x] = token_str[x];
                new_token.text[i + 1] = '\0';

                new_token.column = column - 1;
                new_token.line = line;
                new_token.filename = file_name;
                new_token.typ = assign_type(new_token);                    
                state = 1;
                i = 0;
        }

        
        
    }

    
}

// Assigns the type of token, of which there are three main options:
//     1. The token exists in the reserved word vocabulary
//     2. The token is a unique identifier
//     3. The token is an integer literal
token_type assign_type(token t) {
    if(lexer_done())
        return;

    
    
}

// Requires: !lexer_done()
// Return the name of the current file
const char *lexer_filename() {
    if(lexer_done())
        return;
    return file_name;
}

// Requires: !lexer_done()
// Return the line number of the next token
unsigned int lexer_line() {
    if (lexer_done())
        return;
    return line;
}

// Requires: !lexer_done()
// Return the column number of the next token
unsigned int lexer_column() {
    if (lexer_done())
        return;
    return column;
}

