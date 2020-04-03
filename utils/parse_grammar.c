#include "parse_grammar.h"

grammar *parse_grammar(char *filename, hash_map *thm, hash_map *nthm)
{
    grammar *gm = create_grammar();
    set_start_symbol(gm, program);
    char *temp = calloc(200, sizeof(char));
    FILE *fptr = fopen(filename, "r");
    assert(fptr != NULL, "grammar file opened successfully");

    while (true)
    {

        if (fscanf(fptr, "%[^\n]\n", temp) == EOF)
        {
            break;
        }
        int i1 = 0;
        int i2 = 0;

        while (temp[i2] != ' ')
        {
            i2++;
        }

        temp[i2] = '\0';

        int *dat = (int *)fetch_from_hash_map(nthm, temp);

        if (dat == NULL)
        {
            assert(false, "lhs in grammar file recognized");
        }
        else
        {
            nonterminal lhs = *dat;

            int rhs_len = 0;
            if (temp[i2 + 1] != ' ')
                rhs_len++;
            for (int j = i2 + 1;; j++)
            {
                if (temp[j + 1] == '\0')
                    break;
                if (temp[j] == ' ' && temp[j + 1] != ' ')
                    rhs_len++;
            }

            gm_unit *rhs = calloc(rhs_len, sizeof(gm_unit));

            for (int j = 0; j < rhs_len; j++)
            {
                temp[i2] = ' ';
                while (temp[i2] == ' ')
                    i2++;
                i1 = i2;
                while (temp[i2] != ' ' && temp[i2] != '\0')
                    i2++;
                temp[i2] = '\0';
                dat = (int *)fetch_from_hash_map(nthm, temp + i1);
                if (dat == NULL)
                {
                    dat = (int *)fetch_from_hash_map(thm, temp + i1);
                    if (dat == NULL)
                    {
                        assert(false, "rhs symbol in grammar file recognized");
                    }
                    else
                    {
                        rhs[j].is_terminal = true;
                        rhs[j].gms.t = *dat;
                    }
                }
                else
                {
                    rhs[j].is_terminal = false;
                    rhs[j].gms.nt = *dat;
                }
            }

            add_rule(gm, lhs, rhs, rhs_len);

            free(rhs);
        }
    }

    free(temp);
    return gm;
}