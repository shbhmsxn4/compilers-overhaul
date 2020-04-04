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
            set_data(astn, make_ast_node(program, false));
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
    }

    if (lhs == moduleDeclaration)
    {
    }

    if (lhs == otherModules)
    {
    }

    if (lhs == otherModule)
    {
    }

    if (lhs == driverModule)
    {
    }

    if (lhs == module)
    {
    }

    if (lhs == ret)
    {
    }

    if (lhs == input_plist)
    {
    }

    if (lhs == input_plist2)
    {
    }

    if (lhs == output_plist)
    {
    }

    if (lhs == output_plist2)
    {
    }

    if (lhs == dataType)
    {
    }

    if (lhs == range_arrays)
    {
    }

    if (lhs == type)
    {
    }

    if (lhs == moduleDef)
    {
    }

    if (lhs == statements)
    {
    }

    if (lhs == statement)
    {
    }

    if (lhs == ioStmt)
    {
    }

    if (lhs == boolConstt)
    {
    }

    if (lhs == var_id_num)
    {
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