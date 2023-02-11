#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "lexer_output.h"



int main (int argc, char **argv) {
    lexer_open(argv[1]);
    lexer_output();
}

