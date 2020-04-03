#include "parser.h"

tree *parse(lexer *l, grammar *gm, parse_table *pt)
{
    stack *parser_stack = create_stack();

    gm_unit *temp = NULL;

    temp = calloc(1, sizeof(gm_unit));
    temp->is_terminal = true;
    temp->gms.t = DOLLAR;
    push(parser_stack, (void *)temp);
    temp = NULL;

    temp = calloc(1, sizeof(gm_unit));
    temp->is_terminal = false;
    temp->gms.nt = gm->start_symbol;
    push(parser_stack, (void *)temp);
    temp = NULL;

    bool stack_empty = false;

    gm_unit *next_stack_unit = NULL;

    lexical_token *next_lexer_token = NULL;

    gm_rule *rule = NULL;

    next_lexer_token = get_next_token(l);

    if (next_lexer_token == NULL)
    {
        assert(false, "tokens available in lexer");
    }

    do
    {
        if (next_lexer_token->line_num == 0)
        {
            // lexical error has occurred
        }

        next_stack_unit = (gm_unit *)peek(parser_stack);

        if (next_stack_unit->is_terminal)
        {
            if (next_lexer_token->t == next_stack_unit->gms.t)
            {
                temp = (gm_unit *)pop(parser_stack);
                free(temp);
                temp = NULL;

                next_lexer_token = get_next_token(l);
            }
            else
            {
                // syntactical error has occurred
            }
        }
        else
        {
            rule = get_from_parse_table(pt, next_stack_unit->gms.nt, next_lexer_token->t);
            if (rule != NULL)
            {
                // pop the top of stack symbol and push rule rhs to stack
                temp = pop(parser_stack);
                free(temp);
                temp = NULL;

                for (int i = rule->rhs_len - 1; i >= 0; i--)
                {
                    temp = calloc(1, sizeof(gm_unit));
                    temp->is_terminal = rule->rhs[i].is_terminal;
                    if (temp->is_terminal)
                    {
                        temp->gms.t = rule->rhs[i].gms.t;
                    }
                    else
                    {
                        temp->gms.nt = rule->rhs[i].gms.nt;
                    }
                    push(parser_stack, (void *)temp);
                    temp = NULL;
                }
                rule = NULL;
            }
            else
            {
                // syntactical error has occurred
            }
        }

        next_stack_unit = (gm_unit *)peek(parser_stack);
        if (next_stack_unit->is_terminal && next_stack_unit->gms.t == DOLLAR)
        {
            stack_empty = true;
        }
    } while (!stack_empty);
}