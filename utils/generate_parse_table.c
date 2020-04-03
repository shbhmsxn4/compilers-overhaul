#include "generate_parse_table.h"

parse_table *generate_parse_table(grammar *gm, gm_first *fi, gm_follow *fo)
{
    parse_table *pt = create_parse_table(NUM_NT, NUM_T);

    bool eps_streak;

    for (int i = 0; i < gm->num_rules; i++)
    {
        eps_streak = true;

        int j = 0;
        gm_rule *rule = &(gm->rules[i]);

        while (eps_streak && j < rule->rhs_len)
        {
            if (rule->rhs[0].is_terminal && rule->rhs[0].gms.t == EPS)
            {
                break;
            }
            if (rule->rhs[j].is_terminal)
            {
                add_to_parse_table(pt, rule, rule->lhs, rule->rhs[j].gms.t);
                eps_streak = false;
            }
            else
            {
                bool has_eps = false;
                for (int k = 0; k < fi->first_set[rule->rhs[j].gms.nt].num_terminals; k++)
                {
                    terminal t = fi->first_set[rule->rhs[j].gms.nt].first[k];

                    if (t == EPS)
                    {
                        has_eps = true;
                    }
                    else
                    {
                        add_to_parse_table(pt, rule, rule->lhs, t);
                    }
                }
                if (!has_eps)
                {
                    eps_streak = false;
                }
                j++;
            }
        }

        if (eps_streak)
        {
            for (int k = 0; k < fo->follow_set[rule->lhs].num_terminals; k++)
            {
                add_to_parse_table(pt, rule, rule->lhs, fo->follow_set[rule->lhs].follow[k]);
            }
        }
    }

    return pt;
}