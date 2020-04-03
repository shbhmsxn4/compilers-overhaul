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