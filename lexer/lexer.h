#include <stdlib.h>
#include <string.h>
#include "../data_structs/dfa.h"
#include "../utils/parse_dfa.h"
#include "../file_handler/file_handler.h"
#include "../lang_specs/entities.h"

#ifndef LEXER_H
#define LEXER_H

struct ____LEXER____ {
    dfa * d;
    char * dfa_spec_filepath;
    char * source_code_filepath;
    int buffer_size;
    int max_retract_margin;
    file_handler * fh;
    int max_lexeme_len;
};

typedef struct ____LEXER____ lexer;

lexer * create_lexer(char * source_code_filepath, char * dfa_spec_filepath, int buffer_size, int max_retract_margin, int max_lexeme_len);

void destroy_lexer(lexer * l);

lexical_token *get_next_token(lexer *l);
/*

Functions to be implemented:

get_next_token

*/

#endif