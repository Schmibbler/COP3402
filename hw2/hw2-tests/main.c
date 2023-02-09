#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

int main (int argc, char *argv) {

    

}

// Requires: fname != NULL
// Requires: fname is the name of a readable file
// Initialize the lexer and start it reading
// from the given file name
void lexer_open(const char *fname);

// Close the file the lexer is working on
// and make this lexer be done
void lexer_close();

// Is the lexer's token stream finished
// (either at EOF or not open)?
bool lexer_done();

// Requires: !lexer_done()
// Return the next token in the input file,
// advancing in the input
token lexer_next();

// Requires: !lexer_done()
// Return the name of the current file
const char *lexer_filename();

// Requires: !lexer_done()
// Return the line number of the next token
unsigned int lexer_line();

// Requires: !lexer_done()
// Return the column number of the next token
unsigned int lexer_column();


