/*
Group- 35
2017A7PS0082P		Laksh Singla
2017A7PS0148P 		Kunal Mohta
2017A7PS0191P 		Suyash Raj
2017A7PS0302P 		Shubham Saxena
*/

#include "lexer.h"

lexical_token *lexer_action(lexer *l, int final_state, char *lexeme, int line_num, int char_num);

lexer *create_lexer(char *source_code_filepath, char *dfa_spec_filepath, int buffer_size, int max_retract_margin, int max_lexeme_len, hash_map *khm)
{
    lexer *new_lexer = (lexer *)calloc(1, sizeof(lexer));

    new_lexer->dfa_spec_filepath = calloc(strlen(dfa_spec_filepath) + 1, sizeof(char));
    strcpy(new_lexer->dfa_spec_filepath, dfa_spec_filepath);

    new_lexer->source_code_filepath = calloc(strlen(source_code_filepath) + 1, sizeof(char));
    strcpy(new_lexer->source_code_filepath, source_code_filepath);

    new_lexer->buffer_size = buffer_size;
    new_lexer->max_retract_margin = max_retract_margin;
    new_lexer->max_lexeme_len = max_lexeme_len;

    new_lexer->fh = create_file_handler(new_lexer->source_code_filepath, buffer_size, max_retract_margin);

    new_lexer->d = parse_dfa(new_lexer->dfa_spec_filepath);

    new_lexer->khm = khm;

    return new_lexer;
}

void destroy_lexer(lexer *l)
{
    destroy_dfa(l->d);
    destroy_file_handler(l->fh);
    free(l->dfa_spec_filepath);
    free(l->source_code_filepath);
    free(l);
}

lexical_token *get_next_token(lexer *l)
{
    char c;
    bool is_comment = false;
    do
    {
        c = get_char(l->fh);
    } while (is_whitespace(c) && !(c == EOF));

    if (c == EOF)
    {
        lexical_token *ltk = calloc(1, sizeof(lexical_token));
        ltk->t = DOLLAR;
        ltk->nv.int_val = 0;
        ltk->line_num = get_line_num(l->fh);
        ltk->char_num = get_char_num(l->fh);
        ltk->lexeme = NULL;
        return ltk;
    }

    char *lexeme = (char *)calloc(l->max_lexeme_len, sizeof(char));
    int lexeme_idx = 0;
    int line_num = get_line_num(l->fh);
    int char_num = get_char_num(l->fh) - 1;

    dfa_state *s = get_start_state(l->d);

    while (true)
    {
        if (!is_comment)
        {
            lexeme[lexeme_idx] = c;
            lexeme_idx++;
            if (lexeme_idx > 1)
            {
                if (lexeme[0] == '*' && lexeme[1] == '*')
                {
                    is_comment = true;
                }
            }
        }
        s = traverse_by_input(l->d, s, c);
        if (s == NULL)
        {
            // Error condition
            lexical_token *ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
            ltk->lexeme = (char *)calloc(1, sizeof(char));
            ltk->lexeme[0] = lexeme[0];
            ltk->line_num = 0;
            ltk->char_num = 0;

            // retract all but one characters read
            retract(l->fh, lexeme_idx - 1);
            free(lexeme);
            s = NULL;
            return ltk;
        }
        else if (get_final(s))
        {
            break;
        }
        else
        {
            c = get_char(l->fh);
        }
    }

    lexeme[lexeme_idx] = '\0';

    lexical_token *ltk = lexer_action(l, get_state_id(s), lexeme, line_num, char_num);

    free(lexeme);
    s = NULL;

    return ltk;
}

lexical_token *lexer_action(lexer *l, int final_state, char *lexeme, int line_num, int char_num)
{
    lexical_token *ltk;
    switch (final_state)
    {
    case 2:
        // ******** TO IMPLEMENT : ret1savekeywordid ******
        retract(l->fh, 1);
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        lexeme[strlen(lexeme) - 1] = '\0';
        void *hm_result = fetch_from_hash_map(l->khm, lexeme);
        if (hm_result == NULL)
        {
            ltk->t = ID;
            ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
            strcpy(ltk->lexeme, lexeme);
            ltk->line_num = line_num;
            ltk->char_num = char_num;
            ltk->nv.int_val = 0;
        }
        else
        {
            terminal t = *((int *)hm_result);
            ltk->t = t;
            /*ltk->lexeme = NULL;*/
            ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
            strcpy(ltk->lexeme, lexeme);
            ltk->line_num = line_num;
            ltk->char_num = char_num;
            ltk->nv.int_val = 0;
        }
        return ltk;
        break;
    case 4:
        retract(l->fh, 1);
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = NUM;
        lexeme[strlen(lexeme) - 1] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = atoi(ltk->lexeme);
        return ltk;
        break;
    case 6:
        retract(l->fh, 2);
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = NUM;
        lexeme[strlen(lexeme) - 2] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = atoi(ltk->lexeme);
        return ltk;
        break;
    case 8:
        retract(l->fh, 1);
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = RNUM;
        lexeme[strlen(lexeme) - 1] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.float_val = parse_float(lexeme);
        return ltk;
        break;
    case 12:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = SEMICOL;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 14:
        retract(l->fh, 1);
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = LT;
        lexeme[strlen(lexeme) - 1] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 15:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = LE;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    // case 16:
    //     ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
    //     ltk->t = DEF;
    //     ltk->lexeme = NULL;
    //     ltk->line_num = line_num;
    //     ltk->char_num = char_num;
    //     ltk->nv.int_val = 0;
    //     return ltk;
    //     break;
    case 17:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = MINUS;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 18:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = SQBO;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 19:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = SQBC;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 20:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = BO;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 21:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = BC;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 22:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = PLUS;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 24:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = RANGEOP;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 26:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = ASSIGNOP;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 27:
        retract(l->fh, 1);
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = COLON;
        lexeme[strlen(lexeme) - 1] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 29:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = EQ;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 31:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = NE;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 32:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = DIV;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 34:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = GE;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    // case 35:
    //     ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
    //     ltk->t = ENDDEF;
    //     ltk->lexeme = NULL;
    //     ltk->line_num = line_num;
    //     ltk->char_num = char_num;
    //     ltk->nv.int_val = 0;
    //     return ltk;
    //     break;
    case 36:
        retract(l->fh, 1);
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = GT;
        lexeme[strlen(lexeme) - 1] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 37:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = COMMA;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 39:
        retract(l->fh, 1);
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = MUL;
        lexeme[strlen(lexeme) - 1] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 42:
        return get_next_token(l);
        break;
    case 43:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = DRIVERDEF;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 44:
        retract(l->fh, 1);
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = DEF;
        lexeme[strlen(lexeme) - 1] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 45:
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = DRIVERENDDEF;
        lexeme[strlen(lexeme)] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    case 46:
        retract(l->fh, 1);
        ltk = (lexical_token *)calloc(1, sizeof(lexical_token));
        ltk->t = ENDDEF;
        lexeme[strlen(lexeme) - 1] = '\0';
        ltk->lexeme = (char *)calloc(strlen(lexeme) + 1, sizeof(char));
        strcpy(ltk->lexeme, lexeme);
        /*ltk->lexeme = NULL;*/
        ltk->line_num = line_num;
        ltk->char_num = char_num;
        ltk->nv.int_val = 0;
        return ltk;
        break;
    default:
        assert(false, "final state action exists");
        break;
    }
}
