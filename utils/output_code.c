#include "output_code.h"

void output_code(tree_node *n)
{
    ast_node *data = (ast_node *)get_data(n);
    assert(data->label.is_terminal == false && data->label.gms.nt == program, "output code is given program node");
    code *c = data->c;
    FILE *fptr = fopen("aisanaamnahihoga.asm", "w");
    for (int i = 0; i < c->num_str; i++)
    {
        fprintf(fptr, "%s", c->str[i]);
    }
    fclose(fptr);
}