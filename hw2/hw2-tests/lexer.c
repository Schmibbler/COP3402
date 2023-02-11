#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "utilities.h"
#include "lexer.h"
#include "lexer_output.h"
#include "token.h"

#define VOCAB_LENGTH 34

static bool lexer_running = false;
static bool at_eof = false;
static FILE *fp = NULL;
static const char *file_name = NULL;
static unsigned int column = 1;
static unsigned int line = 1;
static char token_str[MAX_IDENT_LENGTH];


const token default_token();

// A static list of all possible words existing
// in the vocabulary of the PL/0 language
// Notice that the indices correspond with
// the enum definition in "token.h"
// 3 indices are missing (blank):
// "identsym", "numbersym", and "eofsym"
// This is because these are the special cases 
// to consider when comparing the token text
// to an existing vocabulary
static const char * vocabulary[34] = {
    ".", "const", ";", ",", "var", "procedure", ":=", 
    "call", "begin", "end", "if", "then", "else", 
    "while", "do", "read", "write", "skip", "odd", 
    "(", ")", "", "", "=", "<>", "<", "<=", ">", ">=",
    "+", "-", "*", "/", "",
};

// Assigns the type of token, of which there are three main options:
//     1. The token exists in the reserved word vocabulary
//     2. The token is a unique identifier
//     3. The token is an integer literal
void assign_type(token * t) {
    bool found = false;
    for (int i = 0; i < VOCAB_LENGTH; i++) {
        if (strcmp(t->text, vocabulary[i]) == 0) {
            found = true;
            t->typ = (token_type) i;
            return;
        }   
    }
    if (isalpha((t->text)[0]) && !found) {
        t->typ = identsym;
    }
    else if (isalnum((t->text)[0])) {
        char *ptr;
        t->typ = numbersym;
        t->value = strtol(t->text, &ptr, 10);
    }            
}

// Requires: fname != NULL
// Requires: fname is the name of a readable file
// Initialize the lexer and start it reading
// from the given file name
void lexer_open(const char *fname) {
    lexer_running = true;
    if (fname == NULL) {
        printf("File pointer is NULL\n");
        exit(1);
    }

    file_name = fname;
    fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("Unable to open file\n");
        exit(1);
    }
}

// Close the file the lexer is working on
// and make this lexer be done
void lexer_close() {
    fclose(fp);
    fp = NULL;
    lexer_running = false;
}

const token default_token() {
    token default_token;
    default_token.typ = eofsym;
    default_token.filename = "\0";
    default_token.line = 0;
    default_token.column = 1;
    default_token.text = NULL;
    default_token.value = 0;

    return default_token;
}

// Is the lexer's token stream finished
// (either at EOF or not open)?
bool lexer_done() {
    return !lexer_running | at_eof; // implement EOF token
}

// Requires: !lexer_done()
// Return the next token in the input file,
// advancing in the input
token lexer_next() {
    if (lexer_done())
        exit(1);

    char c;
    token new_token;
    new_token.filename = file_name;
    new_token.text = token_str;
    new_token.text[0] = '\0';

    int state = 1;
    int i = 0;

    while(!lexer_done()) {

        if (feof(fp)) {
            state = 5;
        }

        if (state != 5)
            c = fgetc(fp);

        if (c == '#') {
            while (c != '\n') {
                c = fgetc(fp);
                if (feof(fp)) {
                    lexer_running = false; 
                    at_eof = true;
                    return default_token();
                }
            }
            state = 1;
        }

        if (c == '\n') {
            line++;
            column = 1;
            c = fgetc(fp);
        }
       
        //printf("char: \'%c\' state: %d\n", c, state);

        switch(state) {
            case 1:
                new_token.column = column;
                if(isalpha(c)) {
                    ungetc(c, fp);
                    state = 2;
                } else if (isdigit(c)) {
                    ungetc(c, fp);
                    state = 4;
                } else if (isspace(c)) {
                    state = 1;
                    column++;
                } else if (ispunct(c)) {
                    ungetc(c, fp);
                    state = 3;                    
                } else if (iscntrl(c)) {
                    state = 1;
                    column++;
                }
                break;
            case 2:
                if(isalpha(c)) {
                    token_str[i] = c;
                    token_str[i + 1] = '\0';
                    i++;
                    column++;
                    state = 2;
                } else if (isdigit(c)) {
                    token_str[i] = c;
                    token_str[i + 1] = '\0';
                    i++;
                    column++;
                    state = 2;
                } else if (isspace(c)) {
                    ungetc(c, fp);
                    state = 5;
                } else if (ispunct(c)) {
                    ungetc(c, fp);
                    state = 5;

                } else if (iscntrl(c)) {
                    ungetc(c, fp);
                    state = 5;
                }
                break;
            case 3:
                if (isalpha(c)) {
                    ungetc(c, fp);
                    state = 5;
                }
                else if(isspace(c)) {
                    ungetc(c, fp);
                    state = 5;
            
                } else if (ispunct(c)) {
                    token_str[i] = c;
                    token_str[i + 1] = '\0';
                    i++;
                    column++;
                    state = 3;
                } else if (iscntrl(c)) {
                    ungetc(c, fp);
                    state = 5;
                }
                break;
            case 4:
                if (isalpha(c)) {
                    ungetc(c, fp);
                    state = 5;
                } else if (isdigit(c)) {
                    token_str[i] = c;
                    token_str[i + 1] = '\0';
                    i++;
                    column++;
                    state = 4;
                    
                } else if (isspace(c)) {
                    ungetc(c, fp);
                    state = 5;
                } else if (iscntrl(c)) {
                    ungetc(c, fp);
                    state = 5;
                } else if (ispunct(c)) {
                    ungetc(c, fp);
                    state = 5;
                }
                break;
                
            case 5:
                if (strlen(token_str) == 0) {
                    lexer_running = false; 
                    at_eof = true;
                    token eof = default_token();
                    eof.line = line++;
                    return eof;
                }
                    
                for (int x = 0; x < i; x++)
                    new_token.text[x] = token_str[x];
                    
                new_token.text[i] = '\0';
                new_token.line = line;

                assign_type(&new_token);
                return new_token;
                break;

            default:
                printf("State machine interrupt\n");
                exit(1);
                break;
        }    
        
    }
    
    return default_token();
}



// Requires: !lexer_done()
// Return the name of the current file
const char *lexer_filename() {
    if(lexer_done()) {
        printf("Lexer finished\n");
        return 0;
    }
    return file_name;
}

// Requires: !lexer_done()
// Return the line number of the next token
unsigned int lexer_line() {
    if (lexer_done())
        return 0;
    return line;
}

// Requires: !lexer_done()
// Return the column number of the next token
unsigned int lexer_column() {
    if (lexer_done())
        return 0;
    return column;
}

