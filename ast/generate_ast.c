#include "generate_ast.h"

tree *generate_ast(tree *pt)
{
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

void apply_ast_rules(tree_node *ptn, tree_node *astn)
{
    int num_children = get_num_children(ptn);
    assert(num_children > 0, "ast rule not being applied on a parse tree leaf");
    nonterminal lhs = ((pt_node *)get_data(ptn))->nt;
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
        if (ast_rule_matches())
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
        if (ast_rule_matches())
        {
            apply_ast_rules(get_child(ptn, 1), astn);
            tree_node *t = ast_prepend(astn, NULL);
            apply_ast_rules(get_child(ptn, 0), t);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = moduleDeclarations;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches())
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
        if (ast_rule_matches())
        {
            lexical_token *ltk = (pt_leaf *)get_data(get_child(ptn, 2));
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
        if (ast_rule_matches())
        {
            apply_ast_rules(get_child(ptn, 1), astn);
            tree_node *n = ast_prepend(astn, NULL);
            apply_ast_rules(get_child(ptn, 0), n);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = otherModules;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches())
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
        if (ast_rule_matches())
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
        if (ast_rule_matches())
        {
            ast_node *n = make_ast_node(module, false);
            set_data(astn, (void *)n);
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 2)))->lt;
            ast_leaf *l = make_ast_leaf(ID, true, ltk);
            add_child(astn, (void *)l);
            add_child(astn, NULL);
            add_child(astn, NULL);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 7), getchild(astn, 1));
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
        if (ast_rule_matches())
        {
            apply_ast_rules(get_child(ptn, 2), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = ret;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches())
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
        if (ast_rule_matches())
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
        if (ast_rule_matches())
        {
            apply_ast_rules(get_child(ptn, 3), astn);
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
        if (ast_rule_matches())
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
        if (ast_rule_matches())
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
        if (ast_rule_matches())
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
        if (ast_rule_matches())
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
        if (ast_rule_matches())
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
        if (ast_rule_matches())
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
        if (ast_rule_matches())
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
        if (ast_rule_matches())
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
        if (ast_rule_matches())
        {
            ast_node *n = make_ast_node(range_arrays, false);
            set_data(astn, (void *)n);
            add_child(astn, NULL);
            add_child(astn, NULL);
            apply_ast_rules(get_child(ptn, 0), get_child(astn, 0));
            apply_ast_rules(get_child(ptn, 2), get_child(astn, 2));
        }
        free(rule->rhs);
    }

    if (lhs == type)
    {
        init_rule_rhs(rule, 1);
        rule->lhs = type;
        add_rule_rhs(rule, 0, INTEGER, true);
        if (ast_rule_matches())
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(INTEGER, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = type;
        add_rule_rhs(rule, 0, REAL, true);
        if (ast_rule_matches())
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(REAL, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = type;
        add_rule_rhs(rule, 0, BOOLEAN, true);
        if (ast_rule_matches())
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
        if (ast_rule_matches())
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
        add_rule_rhs(rule, 0, statements, false);
        if (ast_rule_matches())
        {
            apply_ast_rules(get_child(ptn, 1), astn);
            tree_node *t = ast_prepend(astn, NULL);
            apply_ast_rules(get_child(ptn, 0), t);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = statements;
        add_rule_rhs(rule, 0, EPS, true);
        if (ast_rule_matches())
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
        if (ast_rule_matches())
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = statement;
        add_rule_rhs(rule, 0, simpleStmt, false);
        if (ast_rule_matches())
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = statement;
        add_rule_rhs(rule, 0, declareStmt, false);
        if (ast_rule_matches())
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = statement;
        add_rule_rhs(rule, 0, condionalStmt, false);
        if (ast_rule_matches())
        {
            apply_ast_rules(get_child(ptn, 0), astn);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = statement;
        add_rule_rhs(rule, 0, iterativeStmt, false);
        if (ast_rule_matches())
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
        if (ast_rule_matches())
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
        if (ast_rule_matches())
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
        if (ast_rule_matches())
        {
            lexical_token *ltk = ((pt_leaf *)get_data(get_child(ptn, 0)))->lt;
            ast_leaf *l = make_ast_leaf(TRUE, true, ltk);
            set_data(astn, (void *)l);
        }
        free(rule->rhs);

        init_rule_rhs(rule, 1);
        rule->lhs = boolConstt;
        add_rule_rhs(rule, 0, FALSE, true);
        if (ast_rule_matches())
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
        if (ast_rule_matches())
        {
        }
        free(rule->rhs);
    }

    if (lhs == var)
    {
    }

    if (lhs == whichId)
    {
    }

    if (lhs == simpleStmt)
    {
    }

    if (lhs == assignmentStmt)
    {
    }

    if (lhs == whichStmt)
    {
    }

    if (lhs == lvalueIDStmt)
    {
    }

    if (lhs == lvalueARRStmt)
    {
    }

    if (lhs == index_nt)
    {
    }

    if (lhs == moduleReuseStmt)
    {
    }

    if (lhs == optional)
    {
    }

    if (lhs == idList)
    {
    }

    if (lhs == idList2)
    {
    }

    if (lhs == expression)
    {
    }

    if (lhs == unary_nt)
    {
    }

    if (lhs == new_NT)
    {
    }

    if (lhs == unary_op)
    {
    }

    if (lhs == arithmeticOrBooleanExpr)
    {
    }

    if (lhs == arithmeticOrBooleanExpr2)
    {
    }

    if (lhs == AnyTerm)
    {
    }

    if (lhs == AnyTerm2)
    {
    }

    if (lhs == arithmeticExpr)
    {
    }

    if (lhs == arithmeticExpr2)
    {
    }

    if (lhs == term)
    {
    }

    if (lhs == term2)
    {
    }

    if (lhs == factor)
    {
    }

    if (lhs == op1)
    {
    }

    if (lhs == op2)
    {
    }

    if (lhs == logicalOp)
    {
    }

    if (lhs == relationalOp)
    {
    }

    if (lhs == declareStmt)
    {
    }

    if (lhs == condionalStmt)
    {
    }

    if (lhs == caseStmts)
    {
    }

    if (lhs == caseStmts2)
    {
    }

    if (lhs == value)
    {
    }

    if (lhs == default_nt)
    {
    }

    if (lhs == iterativeStmt)
    {
    }

    if (lhs == range)
    {
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