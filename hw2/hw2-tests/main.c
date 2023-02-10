#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

int main (int argc, char *argv) {

    lexer_open(argv[0]);
    lexer_output();

}

