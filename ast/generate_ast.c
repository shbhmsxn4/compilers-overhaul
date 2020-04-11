#include "../symbol_table/symbol_table.h"
#include "../symbol_table/symbol_table_def.h"
#include "generate_ast.h"

tree_node *apply_ast_rules(tree_node *ptn, tree_node *astn);

tree *generate_ast(tree *pt)
{
    tree *ast_tree = create_tree();
    set_root(ast_tree, NULL);
    tree_node *ast_root = get_root(ast_tree);
    tree_node *pt_root = get_root(pt);
    apply_ast_rules(pt_root, ast_root);
    return ast_tree;
}

bool ast_rule_matches(gm_rule *rule, pt_node *n)
{
    if (rule->lhs != n->rule->lhs)
    {
        assert(false, "how did this happen? (AST)");
    }
    int rhs_len = rule->rhs_len;
    if (rhs_len != n->rule->rhs_len)
    {
        return false;
    }
    bool is_same = true;
    for (int i = 0; i < rhs_len; i++)
    {
        if (rule->rhs[i].is_terminal)
        {
            if (rule->rhs[i].gms.t != n->rule->rhs[i].gms.t)
            {
                is_same = false;
            }
        }
        else
        {
            if (rule->rhs[i].gms.nt != n->rule->rhs[i].gms.nt)
            {
                is_same = false;
            }
        }
    }
    return is_same;
}

void init_rule_rhs(gm_rule *rule, int a)
{
    rule->rhs = (gm_unit *)calloc(a, sizeof(gm_unit));
    assert(rule->rhs != NULL, "ast rule rhs space allocated");
    rule->rhs_len = a;
}

void add_rule_rhs(gm_rule *rule, int idx, int symbol_num, bool is_terminal)
{
    assert(idx < rule->rhs_len, "ast rule population index valid");
    rule->rhs[idx].is_terminal = is_terminal;
    if (is_terminal)
    {
        rule->rhs[idx].gms.t = symbol_num;
    }
    else
    {
        rule->rhs[idx].gms.nt = symbol_num;
    }
}

ast_node *make_ast_node(int label, bool is_terminal)
{
    ast_node *n = (ast_node *)calloc(1, sizeof(ast_node));
    n->ll = NULL;
    n->label.is_terminal = is_terminal;
    if (is_terminal)
    {
        n->label.gms.t = label;
    }
    else
    {
        n->label.gms.nt = label;
    }
    n->is_leaf = false;
	n->type = -1;
    return n;
}

ast_leaf *make_ast_leaf(int label, bool is_terminal, lexical_token *ltk)
{
    ast_leaf *l = (ast_leaf *)calloc(1, sizeof(ast_leaf));
    l->label.is_terminal = is_terminal;
    if (is_terminal)
    {
        l->label.gms.t = label;
    }
    else
    {
        l->label.gms.nt = label;
    }
    l->ltk = ltk;
    l->is_leaf = true;
	l->type = terminal_to_type(label);
    return l;
}

tree_node *ast_prepend(tree_node *astn, void *data)
{
    tree_node *new_node = create_tree_node(data);
    ast_node *n = (ast_node *)get_data(astn);
    assert(n->ll != NULL, "ast prepend applied to valid linked list");
    ll_prepend(n->ll, (void *)new_node);
    return new_node;
}

tree_node *apply_ast_rules(tree_node *ptn, tree_node *astn)
{
    /*
    pt_node *tempptnode = (pt_node *)get_data(ptn);
    char *tempstring = calloc(50, sizeof(char));
    nonterminal_name(tempptnode->nt, tempstring);
    printf("ASTing %s\n", tempstring);
    free(tempstring);
    tree_node *temptreenode = astn;
    while (temptreenode->parent != NULL)
    {
        temptreenode = temptreenode->parent;
    }
    if (temptreenode != NULL)
    {
        print_ast_subtree(temptreenode, 0);
        printf("\n\n");
    }
    */

    int num_children = get_num_children(ptn);
    assert(num_children > 0, "ast rule not being applied on a parse tree leaf");
    nonterminal lhs = ((pt_node *)get_data(ptn))->nt;
	char nt[100];
	nonterminal_name(lhs, nt);
    gm_unit *rhs;
    int rhs_len = num_children;
    rhs = (gm_unit *)calloc(num_children, sizeof(gm_unit));
    for (int i = 0; i < num_children; i++)
    {
        tree_node *child = get_child(ptn, i);
        if (get_num_children(child) > 0)
        {
            rhs[i].is_terminal = false;
            rhs[i].gms.nt = ((pt_node *)get_data(child))->nt;
        }
        else
        {
            rhs[i].is_terminal = true;
            rhs[i].gms.t = ((pt_leaf *)get_data(child))->lt->t;
        }
    }

    gm_rule *rule = calloc(1, sizeof(rule));

    if (lhs == program)
    {
        init_rule_rhs(rule, 4);
        rule->lhs = program;
        add_rule_rhs(rule, 0, moduleDeclarations, false);
        add_rule_rhs(rule, 1, otherModules, false);
        add_rule_rhs(rule, 2, driverModule, false);
        add_rule_rhs(rule, 3, otherModules, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            set_data(astn, (void *)make_ast_node(program, false));
            add_child(astn, NULL);
            add_child(astn, NULL);
            add_child(astn, NULL);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 0), get_child(astn, 0));
            apply_ast_rules(get_child(ptn, 1), get_child(astn, 1));
            apply_ast_rules(get_child(ptn, 2), get_child(astn, 2));
            apply_ast_rules(get_child(ptn, 3), get_child(astn, 3));
        }
        free(rule->rhs);
    }

    if (lhs == moduleDeclarations)
    {
        init_rule_rhs(rule, 2);
        rule->lhs = moduleDeclarations;
        add_rule_rhs(rule, 0, moduleDeclaration, false);
        add_rule_rhs(rule, 1, moduleDeclarations, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 1), astn);
            tree_node *t = ast_prepend(astn, NULL);
            apply_ast_rules(get_child(ptn, 0), t);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = moduleDeclarations;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(moduleDeclarations, false);
            n->ll = create_linked_list();
            set_data(astn, (void *)n);
        }
        free(rule->rhs);
    }

    if (lhs == moduleDeclaration)
    {
        init_rule_rhs(rule, 4);
        rule->lhs = moduleDeclaration;
        add_rule_rhs(rule, 0, DECLARE, true);
        add_rule_rhs(rule, 1, MODULE, true);
        add_rule_rhs(rule, 2, ID, true);
        add_rule_rhs(rule, 3, SEMICOL, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 2)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            ast_node *n = calloc(1, sizeof(ast_node));
            n->ll = NULL;
            n->label.is_terminal = false;
            n->label.gms.nt = moduleDeclaration;
            set_data(astn, (void *)n);
            add_child(astn, (void *)l);
        }
        free(rule->rhs);
    }

    if (lhs == otherModules)
    {
        init_rule_rhs(rule, 2);
        rule->lhs = otherModules;
        add_rule_rhs(rule, 0, module, false);
        add_rule_rhs(rule, 1, otherModules, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 1), astn);
            tree_node *n = ast_prepend(astn, NULL);
            apply_ast_rules(get_child(ptn, 0), n);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = otherModules;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(otherModules, false);
            n->ll = create_linked_list();
            set_data(astn, (void *)n);
        }
        free(rule->rhs);
    }

    if (lhs == driverModule)
    {
        init_rule_rhs(rule, 5);
        rule->lhs = driverModule;
        add_rule_rhs(rule, 0, DRIVERDEF, true);
        add_rule_rhs(rule, 1, DRIVER, true);
        add_rule_rhs(rule, 2, PROGRAM, true);
        add_rule_rhs(rule, 3, DRIVERENDDEF, true);
        add_rule_rhs(rule, 4, moduleDef, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(moduleDef, false);
            set_data(astn, (void *)n);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 4), get_child(astn, 0));
        }
        free(rule->rhs);
    }

    if (lhs == module)
    {
        init_rule_rhs(rule, 12);
        rule->lhs = module;
        add_rule_rhs(rule, 0, DEF, true);
        add_rule_rhs(rule, 1, MODULE, true);
        add_rule_rhs(rule, 2, ID, true);
        add_rule_rhs(rule, 3, ENDDEF, true);
        add_rule_rhs(rule, 4, TAKES, true);
        add_rule_rhs(rule, 5, INPUT, true);
        add_rule_rhs(rule, 6, SQBO, true);
        add_rule_rhs(rule, 7, input_plist, false);
        add_rule_rhs(rule, 8, SQBC, true);
        add_rule_rhs(rule, 9, SEMICOL, true);
        add_rule_rhs(rule, 10, ret, false);
        add_rule_rhs(rule, 11, moduleDef, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(module, false);
            set_data(astn, (void *)n);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 2)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            add_child(astn, (void *)l);
            add_child(astn, NULL);
            add_child(astn, NULL);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 7), get_child(astn, 1));
            apply_ast_rules(get_child(ptn, 10), get_child(astn, 2));
            apply_ast_rules(get_child(ptn, 11), get_child(astn, 3));
        }
        free(rule->rhs);
    }

    if (lhs == ret)
    {
        init_rule_rhs(rule, 5);
        rule->lhs = ret;
        add_rule_rhs(rule, 0, RETURNS, true);
        add_rule_rhs(rule, 1, SQBO, true);
        add_rule_rhs(rule, 2, output_plist, false);
        add_rule_rhs(rule, 3, SQBC, true);
        add_rule_rhs(rule, 4, SEMICOL, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 2), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = ret;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(output_plist, false);
            n->ll = create_linked_list();
            set_data(astn, n);
        }
        free(rule->rhs);
    }

    if (lhs == input_plist)
    {
        init_rule_rhs(rule, 4);
        rule->lhs = input_plist;
        add_rule_rhs(rule, 0, ID, true);
        add_rule_rhs(rule, 1, COLON, true);
        add_rule_rhs(rule, 2, dataType, false);
        add_rule_rhs(rule, 3, input_plist2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 3), astn);
            tree_node *t = ast_prepend(astn, NULL);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            add_child(t, (void *)l);
            add_child(t, NULL);
            apply_ast_rules(get_child(ptn, 2), get_child(t, 1));
        }
        free(rule->rhs);
    }

    if (lhs == input_plist2)
    {
        init_rule_rhs(rule, 5);
        rule->lhs = input_plist2;
        add_rule_rhs(rule, 0, COMMA, true);
        add_rule_rhs(rule, 1, ID, true);
        add_rule_rhs(rule, 2, COLON, true);
        add_rule_rhs(rule, 3, dataType, false);
        add_rule_rhs(rule, 4, input_plist2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 4), astn);
            tree_node *t = ast_prepend(astn, NULL);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 1)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            add_child(t, (void *)l);
            add_child(t, NULL);
            apply_ast_rules(get_child(ptn, 3), get_child(t, 1));
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = input_plist2;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(input_plist, false);
            n->ll = create_linked_list();
            set_data(astn, (void *)n);
        }
        free(rule->rhs);
    }

    if (lhs == output_plist)
    {
        init_rule_rhs(rule, 4);
        rule->lhs = output_plist;
        add_rule_rhs(rule, 0, ID, true);
        add_rule_rhs(rule, 1, COLON, true);
        add_rule_rhs(rule, 2, type, false);
        add_rule_rhs(rule, 3, output_plist2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 3), astn);
            tree_node *t = ast_prepend(astn, NULL);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            add_child(t, (void *)l);
            add_child(t, NULL);
            apply_ast_rules(get_child(ptn, 2), get_child(t, 1));
        }
        free(rule->rhs);
    }

    if (lhs == output_plist2)
    {
        init_rule_rhs(rule, 5);
        rule->lhs = output_plist2;
        add_rule_rhs(rule, 0, COMMA, true);
        add_rule_rhs(rule, 1, ID, true);
        add_rule_rhs(rule, 2, COLON, true);
        add_rule_rhs(rule, 3, type, false);
        add_rule_rhs(rule, 4, output_plist2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 4), astn);
            tree_node *t = ast_prepend(astn, NULL);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 1)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            add_child(t, (void *)l);
            add_child(t, NULL);
            apply_ast_rules(get_child(ptn, 3), get_child(t, 1));
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = output_plist2;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(output_plist, false);
            n->ll = create_linked_list();
            set_data(astn, (void *)n);
        }
        free(rule->rhs);
    }

    if (lhs == dataType)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = dataType;
        add_rule_rhs(rule, 0, INTEGER, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(INTEGER, true, ltk);
            ast_node *n = make_ast_node(INTEGER, true);
            set_data(astn, (void *)n);
            add_child(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = dataType;
        add_rule_rhs(rule, 0, REAL, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(REAL, true, ltk);
            ast_node *n = make_ast_node(REAL, true);
            set_data(astn, (void *)n);
            add_child(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = dataType;
        add_rule_rhs(rule, 0, BOOLEAN, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(BOOLEAN, true, ltk);
            ast_node *n = make_ast_node(BOOLEAN, true);
            set_data(astn, (void *)n);
            add_child(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 6);
        rule->lhs = dataType;
        add_rule_rhs(rule, 0, ARRAY, true);
        add_rule_rhs(rule, 1, SQBO, true);
        add_rule_rhs(rule, 2, range_arrays, false);
        add_rule_rhs(rule, 3, SQBC, true);
        add_rule_rhs(rule, 4, OF, true);
        add_rule_rhs(rule, 5, type, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(ARRAY, true);
            set_data(astn, (void *)n);
            add_child(astn, NULL);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 2), get_child(astn, 0));
            apply_ast_rules(get_child(ptn, 5), get_child(astn, 1));
        }
        free(rule->rhs);
    }

    if (lhs == range_arrays)
    {
        init_rule_rhs(rule, 3);
        rule->lhs = range_arrays;
        add_rule_rhs(rule, 0, index_nt, false);
        add_rule_rhs(rule, 1, RANGEOP, true);
        add_rule_rhs(rule, 2, index_nt, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(range_arrays, false);
            set_data(astn, (void *)n);
            add_child(astn, NULL);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 0), get_child(astn, 0));
            apply_ast_rules(get_child(ptn, 2), get_child(astn, 1));
        }
        free(rule->rhs);
    }

    if (lhs == type)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = type;
        add_rule_rhs(rule, 0, INTEGER, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(INTEGER, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = type;
        add_rule_rhs(rule, 0, REAL, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(REAL, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = type;
        add_rule_rhs(rule, 0, BOOLEAN, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(BOOLEAN, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);
    }

    if (lhs == moduleDef)
    {
        init_rule_rhs(rule, 3);
        rule->lhs = moduleDef;
        add_rule_rhs(rule, 0, START, true);
        add_rule_rhs(rule, 1, statements, false);
        add_rule_rhs(rule, 2, END, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 1), astn);
        }
        free(rule->rhs);
    }

    if (lhs == statements)
    {
        init_rule_rhs(rule, 2);
        rule->lhs = statements;
        add_rule_rhs(rule, 0, statement, false);
        add_rule_rhs(rule, 1, statements, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 1), astn);
            tree_node *t = ast_prepend(astn, NULL);
            apply_ast_rules(get_child(ptn, 0), t);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = statements;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(statements, false);
            n->ll = create_linked_list();
            set_data(astn, (void *)n);
        }
        free(rule->rhs);
    }

    if (lhs == statement)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = statement;
        add_rule_rhs(rule, 0, ioStmt, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = statement;
        add_rule_rhs(rule, 0, simpleStmt, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = statement;
        add_rule_rhs(rule, 0, declareStmt, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = statement;
        add_rule_rhs(rule, 0, condionalStmt, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = statement;
        add_rule_rhs(rule, 0, iterativeStmt, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);
    }

    if (lhs == ioStmt)
    {
        init_rule_rhs(rule, 5);
        rule->lhs = ioStmt;
        add_rule_rhs(rule, 0, GET_VALUE, true);
        add_rule_rhs(rule, 1, BO, true);
        add_rule_rhs(rule, 2, ID, true);
        add_rule_rhs(rule, 3, BC, true);
        add_rule_rhs(rule, 4, SEMICOL, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(input_stmt, false);
            set_data(astn, (void *)n);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 2)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            add_child(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 5);
        rule->lhs = ioStmt;
        add_rule_rhs(rule, 0, PRINT, true);
        add_rule_rhs(rule, 1, BO, true);
        add_rule_rhs(rule, 2, var, false);
        add_rule_rhs(rule, 3, BC, true);
        add_rule_rhs(rule, 4, SEMICOL, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(output_stmt, false);
            set_data(astn, (void *)n);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 2), get_child(astn, 0));
        }
        free(rule->rhs);
    }

    if (lhs == boolConstt)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = boolConstt;
        add_rule_rhs(rule, 0, TRUE, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(TRUE, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = boolConstt;
        add_rule_rhs(rule, 0, FALSE, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(FALSE, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);
    }

    if (lhs == var_id_num)
    {
        init_rule_rhs(rule, 2);
        rule->lhs = var_id_num;
        add_rule_rhs(rule, 0, ID, true);
        add_rule_rhs(rule, 1, whichId, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(var_id_num, false);
            set_data(astn, (void *)n);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            add_child(astn, (void *)l);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 1), get_child(astn, 1));
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = var_id_num;
        add_rule_rhs(rule, 0, NUM, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(NUM, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = var_id_num;
        add_rule_rhs(rule, 0, RNUM, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(RNUM, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);
    }

    if (lhs == var)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = var;
        add_rule_rhs(rule, 0, var_id_num, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = var;
        add_rule_rhs(rule, 0, boolConstt, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);
    }

    if (lhs == whichId)
    {
        init_rule_rhs(rule, 3);
        rule->lhs = whichId;
        add_rule_rhs(rule, 0, SQBO, true);
        add_rule_rhs(rule, 1, index_nt, false);
        add_rule_rhs(rule, 2, SQBC, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 1), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = whichId;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            assert(get_data(astn) == NULL, "whichID->EPS gives NULL");
            return astn;
        }
        free(rule->rhs);
    }

    if (lhs == simpleStmt)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = simpleStmt;
        add_rule_rhs(rule, 0, assignmentStmt, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = simpleStmt;
        add_rule_rhs(rule, 0, moduleReuseStmt, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);
    }

    if (lhs == assignmentStmt)
    {
        init_rule_rhs(rule, 2);
        rule->lhs = assignmentStmt;
        add_rule_rhs(rule, 0, ID, true);
        add_rule_rhs(rule, 1, whichStmt, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(assignmentStmt, false);
            set_data(astn, (void *)n);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            add_child(astn, (void *)l);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 1), get_child(astn, 1));
        }
        free(rule->rhs);
    }

    if (lhs == whichStmt)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = whichStmt;
        add_rule_rhs(rule, 0, lvalueIDStmt, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = whichStmt;
        add_rule_rhs(rule, 0, lvalueARRStmt, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);
    }

    if (lhs == lvalueIDStmt)
    {
        init_rule_rhs(rule, 3);
        rule->lhs = lvalueIDStmt;
        add_rule_rhs(rule, 0, ASSIGNOP, true);
        add_rule_rhs(rule, 1, expression, false);
        add_rule_rhs(rule, 2, SEMICOL, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(lvalueIDStmt, false);
            set_data(astn, (void *)n);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 1), get_child(astn, 0));
        }
        free(rule->rhs);
    }

    if (lhs == lvalueARRStmt)
    {
        init_rule_rhs(rule, 6);
        rule->lhs = lvalueARRStmt;
        add_rule_rhs(rule, 0, SQBO, true);
        add_rule_rhs(rule, 1, index_nt, false);
        add_rule_rhs(rule, 2, SQBC, true);
        add_rule_rhs(rule, 3, ASSIGNOP, true);
        add_rule_rhs(rule, 4, expression, false);
        add_rule_rhs(rule, 5, SEMICOL, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(lvalueARRStmt, false);
            set_data(astn, (void *)n);
            add_child(astn, NULL);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 1), get_child(astn, 0));
            apply_ast_rules(get_child(ptn, 4), get_child(astn, 1));
        }
        free(rule->rhs);
    }

    if (lhs == index_nt)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = index_nt;
        add_rule_rhs(rule, 0, NUM, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(NUM, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = index_nt;
        add_rule_rhs(rule, 0, ID, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);
    }

    if (lhs == moduleReuseStmt)
    {
        init_rule_rhs(rule, 8);
        rule->lhs = moduleReuseStmt;
        add_rule_rhs(rule, 0, optional, false);
        add_rule_rhs(rule, 1, USE, true);
        add_rule_rhs(rule, 2, MODULE, true);
        add_rule_rhs(rule, 3, ID, true);
        add_rule_rhs(rule, 4, WITH, true);
        add_rule_rhs(rule, 5, PARAMETERS, true);
        add_rule_rhs(rule, 6, idList, false);
        add_rule_rhs(rule, 7, SEMICOL, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(moduleReuseStmt, false);
            set_data(astn, (void *)n);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 3)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            add_child(astn, NULL);
            add_child(astn, (void *)l);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 0), get_child(astn, 0));
            apply_ast_rules(get_child(ptn, 6), get_child(astn, 2));
        }
        free(rule->rhs);
    }

    if (lhs == optional)
    {
        init_rule_rhs(rule, 4);
        rule->lhs = optional;
        add_rule_rhs(rule, 0, SQBO, true);
        add_rule_rhs(rule, 1, idList, false);
        add_rule_rhs(rule, 2, SQBC, true);
        add_rule_rhs(rule, 3, ASSIGNOP, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 1), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = optional;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            assert(get_data(astn) == NULL, "optional->EPS gives NULL");
            return astn;
        }
        free(rule->rhs);
    }

    if (lhs == idList)
    {
        init_rule_rhs(rule, 2);
        rule->lhs = idList;
        add_rule_rhs(rule, 0, ID, true);
        add_rule_rhs(rule, 1, idList2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 1), astn);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            ast_prepend(astn, (void *)l);
        }
        free(rule->rhs);
    }

    if (lhs == idList2)
    {
        init_rule_rhs(rule, 3);
        rule->lhs = idList2;
        add_rule_rhs(rule, 0, COMMA, true);
        add_rule_rhs(rule, 1, ID, true);
        add_rule_rhs(rule, 2, idList2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 2), astn);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 1)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            ast_prepend(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = idList2;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(idList, false);
            n->ll = create_linked_list();
            set_data(astn, (void *)n);
        }
        free(rule->rhs);
    }

    if (lhs == expression)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = expression;
        add_rule_rhs(rule, 0, arithmeticOrBooleanExpr, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = expression;
        add_rule_rhs(rule, 0, unary_nt, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);
    }

    if (lhs == unary_nt)
    {
        init_rule_rhs(rule, 2);
        rule->lhs = unary_nt;
        add_rule_rhs(rule, 0, unary_op, false);
        add_rule_rhs(rule, 1, new_NT, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(unary_nt, false);
            set_data(astn, (void *)n);
            add_child(astn, NULL);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 0), get_child(astn, 0));
            apply_ast_rules(get_child(ptn, 1), get_child(astn, 1));
        }
        free(rule->rhs);
    }

    if (lhs == new_NT)
    {
        init_rule_rhs(rule, 3);
        rule->lhs = new_NT;
        add_rule_rhs(rule, 0, BO, true);
        add_rule_rhs(rule, 1, arithmeticExpr, false);
        add_rule_rhs(rule, 2, BC, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 1), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = new_NT;
        add_rule_rhs(rule, 0, var_id_num, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);
    }

    if (lhs == unary_op)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = unary_op;
        add_rule_rhs(rule, 0, PLUS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(PLUS, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = unary_op;
        add_rule_rhs(rule, 0, MINUS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(MINUS, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);
    }

    if (lhs == arithmeticOrBooleanExpr)
    {
        init_rule_rhs(rule, 2);
        rule->lhs = arithmeticOrBooleanExpr;
        add_rule_rhs(rule, 0, AnyTerm, false);
        add_rule_rhs(rule, 1, arithmeticOrBooleanExpr2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            tree_node *t = apply_ast_rules(get_child(ptn, 1), astn);
            apply_ast_rules(get_child(ptn, 0), t);
        }
        free(rule->rhs);
    }

    if (lhs == arithmeticOrBooleanExpr2)
    {
        init_rule_rhs(rule, 3);
        rule->lhs = arithmeticOrBooleanExpr2;
        add_rule_rhs(rule, 0, logicalOp, false);
        add_rule_rhs(rule, 1, AnyTerm, false);
        add_rule_rhs(rule, 2, arithmeticOrBooleanExpr2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            tree_node *t = apply_ast_rules(get_child(ptn, 2), astn);
            ast_node *n = make_ast_node(logicalOp, false);
            set_data(t, (void *)n);
            add_child(t, NULL);
            add_child(t, NULL);
            add_child(t, NULL);
            apply_ast_rules(get_child(ptn, 0), get_child(t, 1));
            apply_ast_rules(get_child(ptn, 1), get_child(t, 2));
            return get_child(t, 0);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = arithmeticOrBooleanExpr2;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            assert(get_data(astn) == NULL, "arithmeticOrBooleanExpr2->EPS gives NULL");
            return astn;
        }
        free(rule->rhs);
    }

    if (lhs == AnyTerm)
    {
        init_rule_rhs(rule, 2);
        rule->lhs = AnyTerm;
        add_rule_rhs(rule, 0, arithmeticExpr, false);
        add_rule_rhs(rule, 1, AnyTerm2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            tree_node *t = apply_ast_rules(get_child(ptn, 1), astn);
            apply_ast_rules(get_child(ptn, 0), t);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = AnyTerm;
        add_rule_rhs(rule, 0, boolConstt, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);
    }

    if (lhs == AnyTerm2)
    {
        init_rule_rhs(rule, 2);
        rule->lhs = AnyTerm2;
        add_rule_rhs(rule, 0, relationalOp, false);
        add_rule_rhs(rule, 1, arithmeticExpr, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(relationalOp, false);
            set_data(astn, (void *)n);
            add_child(astn, NULL);
            add_child(astn, NULL);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 0), get_child(astn, 1));
            apply_ast_rules(get_child(ptn, 1), get_child(astn, 2));
            return get_child(astn, 0);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = AnyTerm2;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            assert(get_data(astn) == NULL, "AnyTerm2->EPS gives NULL");
            return astn;
        }
        free(rule->rhs);
    }

    if (lhs == arithmeticExpr)
    {
        init_rule_rhs(rule, 2);
        rule->lhs = arithmeticExpr;
        add_rule_rhs(rule, 0, term, false);
        add_rule_rhs(rule, 1, arithmeticExpr2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            tree_node *t = apply_ast_rules(get_child(ptn, 1), astn);
            apply_ast_rules(get_child(ptn, 0), t);
        }
        free(rule->rhs);
    }

    if (lhs == arithmeticExpr2)
    {
        init_rule_rhs(rule, 3);
        rule->lhs = arithmeticExpr2;
        add_rule_rhs(rule, 0, op1, false);
        add_rule_rhs(rule, 1, term, false);
        add_rule_rhs(rule, 2, arithmeticExpr2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            tree_node *t = apply_ast_rules(get_child(ptn, 2), astn);
            ast_node *n = make_ast_node(op1, false);
            set_data(t, (void *)n);
            add_child(t, NULL);
            add_child(t, NULL);
            add_child(t, NULL);
            apply_ast_rules(get_child(ptn, 0), get_child(t, 1));
            apply_ast_rules(get_child(ptn, 1), get_child(t, 2));
            return get_child(t, 0);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = arithmeticExpr2;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            assert(get_data(astn) == NULL, "arithmeticExpr2->EPS gives NULL");
            return astn;
        }
        free(rule->rhs);
    }

    if (lhs == term)
    {
        init_rule_rhs(rule, 2);
        rule->lhs = term;
        add_rule_rhs(rule, 0, factor, false);
        add_rule_rhs(rule, 1, term2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            tree_node *t = apply_ast_rules(get_child(ptn, 1), astn);
            apply_ast_rules(get_child(ptn, 0), t);
        }
        free(rule->rhs);
    }

    if (lhs == term2)
    {
        init_rule_rhs(rule, 3);
        rule->lhs = term2;
        add_rule_rhs(rule, 0, op2, false);
        add_rule_rhs(rule, 1, factor, false);
        add_rule_rhs(rule, 2, term2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            tree_node *t = apply_ast_rules(get_child(ptn, 2), astn);
            ast_node *n = make_ast_node(op2, false);
            set_data(t, (void *)n);
            add_child(t, NULL);
            add_child(t, NULL);
            add_child(t, NULL);
            apply_ast_rules(get_child(ptn, 0), get_child(t, 1));
            apply_ast_rules(get_child(ptn, 1), get_child(t, 2));
            return get_child(t, 0);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = term2;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            assert(get_data(astn) == NULL, "term2->EPS gives NULL");
            return astn;
        }
        free(rule->rhs);
    }

    if (lhs == factor)
    {
        init_rule_rhs(rule, 3);
        rule->lhs = factor;
        add_rule_rhs(rule, 0, BO, true);
        add_rule_rhs(rule, 1, arithmeticOrBooleanExpr, false);
        add_rule_rhs(rule, 2, BC, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 1), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = factor;
        add_rule_rhs(rule, 0, var_id_num, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);
    }

    if (lhs == op1)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = op1;
        add_rule_rhs(rule, 0, PLUS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(PLUS, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = op1;
        add_rule_rhs(rule, 0, MINUS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(MINUS, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);
    }

    if (lhs == op2)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = op2;
        add_rule_rhs(rule, 0, MUL, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(MUL, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = op2;
        add_rule_rhs(rule, 0, DIV, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(DIV, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);
    }

    if (lhs == logicalOp)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = logicalOp;
        add_rule_rhs(rule, 0, AND, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(AND, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = logicalOp;
        add_rule_rhs(rule, 0, OR, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(OR, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);
    }

    if (lhs == relationalOp)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = relationalOp;
        add_rule_rhs(rule, 0, LT, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(LT, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = relationalOp;
        add_rule_rhs(rule, 0, LE, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(LE, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = relationalOp;
        add_rule_rhs(rule, 0, GT, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(GT, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = relationalOp;
        add_rule_rhs(rule, 0, GE, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(GE, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = relationalOp;
        add_rule_rhs(rule, 0, EQ, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(EQ, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = relationalOp;
        add_rule_rhs(rule, 0, NE, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(NE, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);
    }

    if (lhs == declareStmt)
    {
        init_rule_rhs(rule, 5);
        rule->lhs = declareStmt;
        add_rule_rhs(rule, 0, DECLARE, true);
        add_rule_rhs(rule, 1, idList, false);
        add_rule_rhs(rule, 2, COLON, true);
        add_rule_rhs(rule, 3, dataType, false);
        add_rule_rhs(rule, 4, SEMICOL, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(declareStmt, false);
            set_data(astn, (void *)n);
            add_child(astn, NULL);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 1), get_child(astn, 0));
            apply_ast_rules(get_child(ptn, 3), get_child(astn, 1));
        }
        free(rule->rhs);
    }

    if (lhs == condionalStmt)
    {
        init_rule_rhs(rule, 8);
        rule->lhs = condionalStmt;
        add_rule_rhs(rule, 0, SWITCH, true);
        add_rule_rhs(rule, 1, BO, true);
        add_rule_rhs(rule, 2, ID, true);
        add_rule_rhs(rule, 3, BC, true);
        add_rule_rhs(rule, 4, START, true);
        add_rule_rhs(rule, 5, caseStmts, false);
        add_rule_rhs(rule, 6, default_nt, false);
        add_rule_rhs(rule, 7, END, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(condionalStmt, false);
            set_data(astn, (void *)n);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 2)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            add_child(astn, (void *)l);
            add_child(astn, NULL);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 5), get_child(astn, 1));
            apply_ast_rules(get_child(ptn, 6), get_child(astn, 2));
        }
        free(rule->rhs);
    }

    if (lhs == caseStmts)
    {
        init_rule_rhs(rule, 7);
        rule->lhs = caseStmts;
        add_rule_rhs(rule, 0, CASE, true);
        add_rule_rhs(rule, 1, value, false);
        add_rule_rhs(rule, 2, COLON, true);
        add_rule_rhs(rule, 3, statements, false);
        add_rule_rhs(rule, 4, BREAK, true);
        add_rule_rhs(rule, 5, SEMICOL, true);
        add_rule_rhs(rule, 6, caseStmts2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 6), astn);
            ast_node *n = make_ast_node(caseStmtNode, false);
            tree_node *tn = ast_prepend(astn, (void *)n);
            add_child(tn, NULL);
            add_child(tn, NULL);
            apply_ast_rules(get_child(ptn, 1), get_child(tn, 0));
            apply_ast_rules(get_child(ptn, 3), get_child(tn, 1));
        }
        free(rule->rhs);
    }

    if (lhs == caseStmts2)
    {
        init_rule_rhs(rule, 7);
        rule->lhs = caseStmts2;
        add_rule_rhs(rule, 0, CASE, true);
        add_rule_rhs(rule, 1, value, false);
        add_rule_rhs(rule, 2, COLON, true);
        add_rule_rhs(rule, 3, statements, false);
        add_rule_rhs(rule, 4, BREAK, true);
        add_rule_rhs(rule, 5, SEMICOL, true);
        add_rule_rhs(rule, 6, caseStmts2, false);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            apply_ast_rules(get_child(ptn, 6), astn);
            ast_node *n = make_ast_node(caseStmtNode, false);
            tree_node *tn = ast_prepend(astn, (void *)n);
            add_child(tn, NULL);
            add_child(tn, NULL);
            apply_ast_rules(get_child(ptn, 1), get_child(tn, 0));
            apply_ast_rules(get_child(ptn, 3), get_child(tn, 1));
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = caseStmts2;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(caseStmts, false);
            n->ll = create_linked_list();
            set_data(astn, (void *)n);
        }
        free(rule->rhs);
    }

    if (lhs == value)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = value;
        add_rule_rhs(rule, 0, NUM, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(NUM, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = value;
        add_rule_rhs(rule, 0, TRUE, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(TRUE, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = value;
        add_rule_rhs(rule, 0, FALSE, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(FALSE, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);
    }

    if (lhs == default_nt)
    {
        init_rule_rhs(rule, 5);
        rule->lhs = default_nt;
        add_rule_rhs(rule, 0, DEFAULT, true);
        add_rule_rhs(rule, 1, COLON, true);
        add_rule_rhs(rule, 2, statements, false);
        add_rule_rhs(rule, 3, BREAK, true);
        add_rule_rhs(rule, 4, SEMICOL, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(default_nt, false);
            set_data(astn, (void *)n);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 2), get_child(astn, 0));
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = default_nt;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            assert(get_data(astn) == NULL, "default_nt->EPS gives NULL");
            return astn;
        }
        free(rule->rhs);
    }

    if (lhs == iterativeStmt)
    {
        init_rule_rhs(rule, 9);
        rule->lhs = iterativeStmt;
        add_rule_rhs(rule, 0, FOR, true);
        add_rule_rhs(rule, 1, BO, true);
        add_rule_rhs(rule, 2, ID, true);
        add_rule_rhs(rule, 3, IN, true);
        add_rule_rhs(rule, 4, range, false);
        add_rule_rhs(rule, 5, BC, true);
        add_rule_rhs(rule, 6, START, true);
        add_rule_rhs(rule, 7, statements, false);
        add_rule_rhs(rule, 8, END, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(for_loop, false);
            set_data(astn, (void *)n);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 2)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            add_child(astn, (void *)l);
            add_child(astn, NULL);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 4), get_child(astn, 1));
            apply_ast_rules(get_child(ptn, 7), get_child(astn, 2));
        }
        free(rule->rhs);

        init_rule_rhs(rule, 7);
        rule->lhs = iterativeStmt;
        add_rule_rhs(rule, 0, WHILE, true);
        add_rule_rhs(rule, 1, BO, true);
        add_rule_rhs(rule, 2, arithmeticOrBooleanExpr, false);
        add_rule_rhs(rule, 3, BC, true);
        add_rule_rhs(rule, 4, START, true);
        add_rule_rhs(rule, 5, statements, false);
        add_rule_rhs(rule, 6, END, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(while_loop, false);
            set_data(astn, (void *)n);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(WHILE, true, ltk);
            add_child(astn, (void *)l);
            add_child(astn, NULL);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 2), get_child(astn, 1));
            apply_ast_rules(get_child(ptn, 5), get_child(astn, 2));
        }
        free(rule->rhs);
    }

    if (lhs == range)
    {
        init_rule_rhs(rule, 3);
        rule->lhs = range;
        add_rule_rhs(rule, 0, NUM, true);
        add_rule_rhs(rule, 1, RANGEOP, true);
        add_rule_rhs(rule, 2, NUM, true);
        if (ast_rule_matches(rule, (pt_node *)get_data(ptn)))
        {
            ast_node *n = make_ast_node(range, false);
            set_data(astn, (void *)n);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(NUM, true, ltk);
            add_child(astn, (void *)l);
            ltk = ((pt_leaf *)get_data(get_child(ptn, 2)))->lt;
            l = make_ast_leaf(NUM, true, ltk);
            add_child(astn, (void *)l);
        }
        free(rule->rhs);
    }
}

/*

init_rule_rhs(rule, );
        rule->lhs = ;
        add_rule_rhs(rule, 0, , );
        if (ast_rule_matches())
        {
        }
        free(rule->rhs);

*/
