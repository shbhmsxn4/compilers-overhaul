#include "grammar.h"

grammar *create_grammar()
{
    grammar *gm = (grammar *)calloc(1, sizeof(grammar));
    gm->rules = (gm_rule *)calloc(1, sizeof(gm_rule));
    gm->max_rules = 1;
    gm->num_rules = 0;
    return gm;
}

void expand_rule_space(grammar *gm)
{
    assert(gm->max_rules == gm->num_rules, "rule expansion called on correct condition");

    gm->rules = realloc((void *)(gm->rules), 2 * (gm->max_rules) * sizeof(gm_rule));

    assert((gm->rules) != NULL, "rule space expansion successful");

    gm->max_rules = gm->max_rules * 2;
}

void set_start_symbol(grammar *gm, nonterminal nt)
{
    gm->start_symbol = nt;
}

void add_rule(grammar *gm, nonterminal lhs, gm_unit *rhs, int rhs_len)
{
    if (gm->num_rules == gm->max_rules)
    {
        expand_rule_space(gm);
    }

    assert(gm->num_rules < gm->max_rules, "space sufficient in grammar for new rule");

    int idx = gm->num_rules;

    gm->rules[idx].lhs = lhs;

    gm->rules[idx].rhs = calloc(rhs_len, sizeof(gm_unit));
    gm->rules[idx].rhs_len = rhs_len;

    for (int i = 0; i < rhs_len; i++)
    {
        gm->rules[idx].rhs[i].is_terminal = rhs[i].is_terminal;
        if (rhs[i].is_terminal)
        {
            gm->rules[idx].rhs[i].gms.t = rhs[i].gms.t;
        }
        else if (!rhs[i].is_terminal)
        {
            gm->rules[idx].rhs[i].gms.nt = rhs[i].gms.nt;
        }
    }

    (gm->num_rules)++;
}

void expand_first_set(nt_first *fs)
{
    assert(fs->num_terminals == fs->max_terminals, "expanding first set on correct condition");

    fs->first = realloc((void *)(fs->first), 2 * (fs->max_terminals) * sizeof(terminal));

    assert((fs->first) != NULL, "first set expanded successfully");

    fs->max_terminals = fs->max_terminals * 2;
}

bool union_first_sets(nt_first *a, nt_first *b)
{
    bool set_updated = false;
    bool term_in_a;

    for (int i = 0; i < b->num_terminals; i++)
    {
        if (b->first[i] == EPS)
            continue;
        term_in_a = false;
        for (int j = 0; j < a->num_terminals; j++)
        {
            if (a->first[j] == b->first[i])
            {
                term_in_a = true;
                break;
            }
        }
        if (!term_in_a)
        {
            if (a->num_terminals == a->max_terminals)
            {
                expand_first_set(a);
            }

            assert(a->num_terminals < a->max_terminals, "space sufficient for new terminal in first set");

            a->first[a->num_terminals] = b->first[i];
            (a->num_terminals)++;
            set_updated = true;
        }
    }

    return set_updated;
}

gm_first *get_first(grammar *gm)
{
    gm_first *fs = calloc(1, sizeof(gm_first));

    fs->first_set = calloc(NUM_NT, sizeof(nt_first));

    for (int i = 0; i < NUM_NT; i++)
    {
        fs->first_set[i].nt = i;
        fs->first_set[i].num_terminals = 0;
        fs->first_set[i].max_terminals = 1;
        fs->first_set[i].first = calloc(1, sizeof(terminal));
    }

    for (int i = 0; i < gm->num_rules; i++)
    {
        if (gm->rules[i].rhs[0].is_terminal)
        {
            nonterminal nt = gm->rules[i].lhs;
            terminal t = gm->rules[i].rhs[0].gms.t;

            if (fs->first_set[nt].num_terminals == fs->first_set[nt].max_terminals)
            {
                expand_first_set(&(fs->first_set[nt]));
            }

            assert(fs->first_set[nt].num_terminals < fs->first_set[nt].max_terminals, "first set has sufficient space for new terminal");

            fs->first_set[nt].first[fs->first_set[nt].num_terminals] = t;

            (fs->first_set[nt].num_terminals)++;
        }
    }

    bool set_updated;

    do
    {
        set_updated = false;

        for (int i = 0; i < gm->num_rules; i++)
        {
            bool eps_streak = true;
            for (int j = 0; j < gm->rules[i].rhs_len; j++)
            {
                if (!eps_streak)
                {
                    break;
                }
                if (gm->rules[i].rhs[j].is_terminal)
                {
                    bool terminal_in_fs = false;
                    terminal t = gm->rules[i].rhs[j].gms.t;
                    nonterminal nt = gm->rules[i].lhs;
                    for (int k = 0; k < fs->first_set[nt].num_terminals; k++)
                    {
                        if (fs->first_set[nt].first[k] == t)
                            terminal_in_fs = true;
                    }
                    if (!terminal_in_fs)
                    {
                        if (fs->first_set[nt].num_terminals == fs->first_set[nt].max_terminals)
                        {
                            expand_first_set(&(fs->first_set[nt]));
                        }
                        assert(fs->first_set[nt].num_terminals < fs->first_set[nt].max_terminals, "first set has enough space for new terminal");
                        fs->first_set[nt].first[fs->first_set[nt].num_terminals] = t;
                        (fs->first_set[nt].num_terminals)++;
                    }
                    eps_streak = false;
                    break;
                }

                set_updated = union_first_sets(&(fs->first_set[gm->rules[i].lhs]), &(fs->first_set[gm->rules[i].rhs[j].gms.nt])) || set_updated;

                bool has_eps = false;
                for (int k = 0; k < fs->first_set[gm->rules[i].rhs[j].gms.nt].num_terminals; k++)
                {
                    if (fs->first_set[gm->rules[i].rhs[j].gms.nt].first[k] == EPS)
                    {
                        has_eps = true;
                    }
                }
                if (!has_eps)
                {
                    eps_streak = false;
                }
            }
            if (eps_streak)
            {
                nonterminal nt = gm->rules[i].lhs;
                bool eps_in_fs = false;
                for (int k = 0; k < fs->first_set[nt].num_terminals; k++)
                {
                    if (fs->first_set[nt].first[k] == EPS)
                        eps_in_fs = true;
                }
                if (!eps_in_fs)
                {
                    if (fs->first_set[nt].num_terminals == fs->first_set[nt].max_terminals)
                    {
                        expand_first_set(&(fs->first_set[nt]));
                    }
                    assert(fs->first_set[nt].num_terminals < fs->first_set[nt].max_terminals, "enough space for new terminal in first set");
                    fs->first_set[nt].first[fs->first_set[nt].num_terminals] = EPS;
                    (fs->first_set[nt].num_terminals)++;
                }
            }
        }

    } while (set_updated);

    return fs;
}

void expand_follow_set(nt_follow *fs)
{
    assert(fs->num_terminals == fs->max_terminals, "expanding first set on correct condition");

    fs->follow = realloc((void *)(fs->follow), 2 * (fs->max_terminals) * sizeof(terminal));

    assert((fs->follow) != NULL, "first set expanded successfully");

    fs->max_terminals = fs->max_terminals * 2;
}

bool union_first_follow_sets(nt_follow *a, nt_first *b)
{
    bool set_updated = false;
    bool terminal_in_set;

    for (int i = 0; i < b->num_terminals; i++)
    {
        terminal_in_set = false;
        for (int j = 0; j < a->num_terminals; j++)
        {
            if (a->follow[j] == b->first[i])
            {
                terminal_in_set = true;
            }
        }
        if (!terminal_in_set && b->first[i] != EPS)
        {
            if (a->num_terminals == a->max_terminals)
            {
                expand_follow_set(a);
            }
            assert(a->num_terminals < a->max_terminals, "sufficient space in follow set for new terminal in union");

            a->follow[a->num_terminals] = b->first[i];

            (a->num_terminals)++;

            set_updated = true;
        }
    }

    return set_updated;
}

bool union_follow_sets(nt_follow *a, nt_follow *b)
{
    bool set_updated = false;
    bool terminal_in_set;

    for (int i = 0; i < b->num_terminals; i++)
    {
        terminal_in_set = false;
        for (int j = 0; j < a->num_terminals; j++)
        {
            if (a->follow[j] == b->follow[i])
            {
                terminal_in_set = true;
                break;
            }
        }
        if (!terminal_in_set)
        {
            if (a->num_terminals == a->max_terminals)
            {
                expand_follow_set(a);
            }
            assert(a->num_terminals < a->max_terminals, "sufficient space in follow set for new terminal");

            a->follow[a->num_terminals] = b->follow[i];

            (a->num_terminals)++;

            set_updated = true;
        }
    }

    return set_updated;
}

gm_follow *get_follow(grammar *gm, gm_first *gmfs)
{
    gm_follow *fs = calloc(1, sizeof(gm_follow));

    fs->follow_set = calloc(NUM_NT, sizeof(nt_follow));

    for (int i = 0; i < NUM_NT; i++)
    {
        fs->follow_set[i].nt = i;
        fs->follow_set[i].max_terminals = 1;
        fs->follow_set[i].num_terminals = 0;
        fs->follow_set[i].follow = calloc(1, sizeof(terminal));
    }

    for (int i = 0; i < NUM_NT; i++)
    {
        if (i == gm->start_symbol)
        {
            if (fs->follow_set[i].num_terminals == fs->follow_set[i].max_terminals)
            {
                expand_follow_set(&(fs->follow_set[i]));
            }
            assert(fs->follow_set[i].num_terminals < fs->follow_set[i].max_terminals, "enough space for new terminal in follow set");

            fs->follow_set[i].follow[fs->follow_set[i].num_terminals] = DOLLAR;

            (fs->follow_set[i].num_terminals)++;

            break;
        }
    }

    bool set_updated;

    do
    {
        set_updated = false;

        for (int i = 0; i < gm->num_rules; i++)
        {
            for (int j = 0; j < gm->rules[i].rhs_len - 1; j++)
            {
                if (gm->rules[i].rhs[j].is_terminal == false)
                {
                    bool eps_streak = true;
                    int l = j + 1;
                    do
                    {
                        if (gm->rules[i].rhs[l].is_terminal == true)
                        {
                            nt_follow *a = &(fs->follow_set[gm->rules[i].rhs[j].gms.nt]);
                            terminal t = gm->rules[i].rhs[l].gms.t;
                            bool terminal_in_set = false;
                            for (int k = 0; k < a->num_terminals; k++)
                            {
                                if (a->follow[k] == t)
                                {
                                    terminal_in_set = true;
                                    break;
                                }
                            }
                            if (!terminal_in_set)
                            {
                                if (a->num_terminals == a->max_terminals)
                                {
                                    expand_follow_set(a);
                                }

                                assert(a->num_terminals < a->max_terminals, "enough space in follow set for new terminal");

                                a->follow[a->num_terminals] = t;

                                (a->num_terminals)++;

                                set_updated = true;
                            }
                            eps_streak = false;
                        }
                        else
                        {
                            set_updated = union_first_follow_sets(&(fs->follow_set[gm->rules[i].rhs[j].gms.nt]), &(gmfs->first_set[gm->rules[i].rhs[l].gms.nt])) || set_updated;
                            bool has_eps = false;
                            for (int k = 0; k < gmfs->first_set[gm->rules[i].rhs[l].gms.nt].num_terminals; k++)
                            {
                                if (gmfs->first_set[gm->rules[i].rhs[l].gms.nt].first[k] == EPS)
                                {
                                    has_eps = true;
                                    break;
                                }
                            }
                            if (!has_eps)
                            {
                                eps_streak = false;
                            }
                            else
                            {
                                l++;
                            }
                        }
                    } while (eps_streak && l < gm->rules[i].rhs_len);
                }
            }
            bool eps_streak = true;
            int j = gm->rules[i].rhs_len - 1;
            do
            {
                if (gm->rules[i].rhs[j].is_terminal)
                    break;
                set_updated = union_follow_sets(&(fs->follow_set[gm->rules[i].rhs[j].gms.nt]), &(fs->follow_set[gm->rules[i].lhs])) || set_updated;
                bool has_eps = false;
                for (int k = 0; k < gmfs->first_set[gm->rules[i].rhs[j].gms.nt].num_terminals; k++)
                {
                    if (gmfs->first_set[gm->rules[i].rhs[j].gms.nt].first[k] == EPS)
                    {
                        has_eps = true;
                        break;
                    }
                }
                if (!has_eps)
                    eps_streak = false;
                j--;
            } while (eps_streak && j >= 0);
        }

    } while (set_updated);

    return fs;
}