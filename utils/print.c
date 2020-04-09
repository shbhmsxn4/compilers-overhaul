#include "print.h"

void print_first(gm_first *fi)
{
    char *temp = calloc(50, sizeof(char));
    printf("FIRST SETS\n");
    for (int i = 0; i < NUM_NT; i++)
    {
        nt_first *a = fi->first_set + i;
        nonterminal_name(a->nt, temp);
        printf("%s : {", temp);
        for (int j = 0; j < a->num_terminals; j++)
        {
            terminal_name(a->first[j], temp);
            printf("%s, ", temp);
        }
        printf("}\n");
    }
    free(temp);
}

void print_follow(gm_follow *fo)
{
    char *temp = calloc(50, sizeof(char));
    printf("FOLLOW SETS\n");
    for (int i = 0; i < NUM_NT; i++)
    {
        nt_follow *a = fo->follow_set + i;
        nonterminal_name(a->nt, temp);
        printf("%s : {", temp);
        for (int j = 0; j < a->num_terminals; j++)
        {
            terminal_name(a->follow[j], temp);
            printf("%s, ", temp);
        }
        printf("}\n");
    }
    free(temp);
}

void print_rule(gm_rule *rule)
{
    char *temp = calloc(50, sizeof(char));
    nonterminal_name(rule->lhs, temp);
    printf("%s -> ", temp);
    for (int j = 0; j < rule->rhs_len; j++)
    {
        if (rule->rhs[j].is_terminal)
        {
            terminal_name(rule->rhs[j].gms.t, temp);
            printf("%s ", temp);
        }
        else
        {
            nonterminal_name(rule->rhs[j].gms.nt, temp);
            printf("%s ", temp);
        }
    }
    free(temp);
}

void print_grammar(grammar *gm)
{
    char *temp = calloc(50, sizeof(char));
    printf("GRAMMAR\nStart symbol: ");
    nonterminal_name(gm->start_symbol, temp);
    printf("%s\nRules:\n", temp);
    for (int i = 0; i < gm->num_rules; i++)
    {
        print_rule(&(gm->rules[i]));
    }
    printf("\n");

    free(temp);
}

void print_parse_table(parse_table *pt)
{
    char *temp = calloc(50, sizeof(char));
    for (int i = 0; i < pt->num_rows; i++)
    {
        nonterminal_name(i, temp);
        printf("%s => ", temp);
        for (int j = 0; j < pt->num_cols; j++)
        {
            gm_rule *rule = (gm_rule *)get_from_parse_table(pt, i, j);
            if (rule == NULL)
            {
                printf(" - ");
            }
            else
            {
                printf(" ");
                print_rule(rule);
                printf(" ");
            }
        }
        printf("\n");
    }
    free(temp);
}

void print_parse_subtree(tree_node *ptn, int level)
{

    char *temp = calloc(50, sizeof(char));
    int num_children = get_num_children(ptn);
    if (num_children == 0)
    {

        pt_leaf *l = (pt_leaf *)get_data(ptn);

        for (int j = 0; j < level; j++)
        {
            printf("| ");
        }

        terminal_name(l->lt->t, temp);
        printf("%s (%d, %d)\n", temp, l->lt->line_num, l->lt->char_num);
    }
    else
    {
        pt_node *n = get_data(ptn);
        nonterminal_name(n->nt, temp);
        for (int j = 0; j < level; j++)
        {
            printf("| ");
        }
        printf("%s\n", temp);
        for (int i = 0; i < num_children; i++)
        {
            print_parse_subtree(get_child(ptn, i), level + 1);
        }
    }
    free(temp);
}

void print_parse_tree(tree *pt)
{
    tree_node *root = get_root(pt);
    print_parse_subtree(root, 0);
}

void print_ast_subtree(tree_node *tn, int level)
{
    char *temp = calloc(50, sizeof(char));
    void *data = get_data(tn);
    if (data == NULL)
    {
        for (int i = 0; i < level; i++)
        {
            printf("| ");
        }
        printf("skipped\n");
        free(temp);
        return;
    }
    if (((ast_node *)data)->is_leaf == false)
    {
        ast_node *n = (ast_node *)data;
        for (int i = 0; i < level; i++)
        {
            printf("| ");
        }
        if (n->label.is_terminal)
        {
            terminal_name(n->label.gms.t, temp);
            printf("%s", temp);
        }
        else
        {
            nonterminal_name(n->label.gms.nt, temp);
            printf("%s", temp);
        }
        if (n->ll != NULL)
        {
            printf(" [%d LL ele's]", n->ll->num_nodes);
            for (int j = 0; j < n->ll->num_nodes; j++)
            {
                printf("\nLL");
                print_ast_subtree(ll_get(n->ll, j), level);
            }
        }
        printf("\n");
    }
    else
    {
        ast_leaf *l = (ast_leaf *)data;
        for (int i = 0; i < level; i++)
        {
            printf("| ");
        }
        printf("(L)");
        if (l->label.is_terminal)
        {
            terminal_name(l->label.gms.t, temp);
            printf("%s", temp);
        }
        else
        {
            nonterminal_name(l->label.gms.nt, temp);
            printf("%s", temp);
        }
        printf("\n");
    }

    int num_children = get_num_children(tn);
    for (int i = 0; i < num_children; i++)
    {
        tree_node *child = get_child(tn, i);
        print_ast_subtree(child, level + 1);
    }
    free(temp);
}

void print_ast_tree(tree *ast)
{
    tree_node *root = get_root(ast);
    print_ast_subtree(root, 0);
}

void print_parse_stack(stack *s)
{
    char *temp = calloc(50, sizeof(char));
    stack_node *n = s->top;
    int num_elements = s->num_elements;
    for (int i = 0; i < num_elements; i++)
    {
        gm_unit *gu = (gm_unit *)n->data;
        if (gu->is_terminal)
        {
            terminal_name(gu->gms.t, temp);
            printf("%s\n", temp);
        }
        else
        {
            nonterminal_name(gu->gms.nt, temp);
            printf("%s\n", temp);
        }
        n = n->next;
    }
    free(temp);
}