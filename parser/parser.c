#include "parser.h"

pt_node *make_pt_node(nonterminal nt, gm_rule *rule)
{
    pt_node *n = calloc(1, sizeof(pt_node));
    n->nt = nt;
    n->rule = rule;
    return n;
}

pt_leaf *make_pt_leaf(lexical_token *lt)
{
    pt_leaf *l = calloc(1, sizeof(pt_leaf));
    l->lt = lt;
    return l;
}

tree_node *get_next_pt_node(tree_node *c)
{
    tree_node *p = get_parent(c);
    if (p == NULL)
        return NULL;
    int idx = 0;
    int num_children = get_num_children(p);
    for (int i = 0; i < num_children; i++)
    {
        if (c == get_child(p, i))
        {
            idx = i;
            break;
        }
    }
    if (idx == num_children - 1)
    {
        return get_next_pt_node(p);
    }
    else
    {
        return get_child(p, idx + 1);
    }
}

tree *parse(lexer *l, grammar *gm, parse_table *pt)
{
    stack *parser_stack = create_stack();

    tree *ptree = create_tree();
    set_root(ptree, (void *)make_pt_node(program, NULL));
    tree_node *cur = get_root(ptree);

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

        // printf("%d %d %d\n", next_lexer_token->line_num, next_lexer_token->char_num, next_lexer_token->t);
        // char temp_char_urgh;
        // print_parse_stack(parser_stack);
        // scanf("%c", &temp_char_urgh);

        if (next_stack_unit->is_terminal)
        {

            if (next_stack_unit->gms.t == EPS)
            {
                temp = (gm_unit *)pop(parser_stack);
                free(temp);
                temp = NULL;
                lexical_token *ltk = calloc(1, sizeof(lexical_token));
                ltk->t = EPS;
                ltk->lexeme = NULL;
                pt_leaf *leaf = make_pt_leaf(ltk);
                set_data(cur, (void *)leaf);
                cur = get_next_pt_node(cur);
            }
            else if (next_lexer_token->t == next_stack_unit->gms.t)
            {
                temp = (gm_unit *)pop(parser_stack);
                free(temp);
                temp = NULL;
                pt_leaf *leaf = make_pt_leaf(next_lexer_token);
                set_data(cur, (void *)leaf);
                next_lexer_token = get_next_token(l);
                cur = get_next_pt_node(cur);
            }
            else
            {
                // syntactical error has occurred
				while (next_stack_unit->is_terminal) {
					printf("Syntax error at line %d - lexeme %s\n", next_lexer_token->line_num, next_lexer_token->lexeme);
					temp = (gm_unit *)pop(parser_stack);
					free(temp);
					temp = NULL;
					next_lexer_token = get_next_token(l);
				}
            }
        }
        else
        {

            rule = get_from_parse_table(pt, next_stack_unit->gms.nt, next_lexer_token->t);
            if (rule != NULL)
            {
                // pop the top of stack symbol and push rule rhs to stack
                pt_node *n = get_data(cur);
                n->rule = rule;
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
                        add_child_at(cur, NULL, 0);
                    }
                    else
                    {
                        temp->gms.nt = rule->rhs[i].gms.nt;
                        pt_node *n = make_pt_node(temp->gms.nt, NULL);
                        add_child_at(cur, (void *)n, 0);
                    }
                    push(parser_stack, (void *)temp);
                    temp = NULL;
                }
                rule = NULL;
                cur = get_child(cur, 0);
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
    return ptree;
}
