#include "nonterminal_hash_map.h"

hash_map *create_nonterminal_hash_map(int num_buckets)
{
    hash_map *hm = create_hash_map(num_buckets);
    int *temp;

    temp = calloc(1, sizeof(int));
    *temp = 0;
    add_to_hash_map(hm, "program", temp);

    temp = calloc(1, sizeof(int));
    *temp = 1;
    add_to_hash_map(hm, "moduleDeclarations", temp);

    temp = calloc(1, sizeof(int));
    *temp = 2;
    add_to_hash_map(hm, "otherModules", temp);

    temp = calloc(1, sizeof(int));
    *temp = 3;
    add_to_hash_map(hm, "driverModule", temp);

    temp = calloc(1, sizeof(int));
    *temp = 4;
    add_to_hash_map(hm, "otherModule", temp);

    temp = calloc(1, sizeof(int));
    *temp = 5;
    add_to_hash_map(hm, "moduleDeclaration", temp);

    temp = calloc(1, sizeof(int));
    *temp = 6;
    add_to_hash_map(hm, "module", temp);

    temp = calloc(1, sizeof(int));
    *temp = 7;
    add_to_hash_map(hm, "moduleDef", temp);

    temp = calloc(1, sizeof(int));
    *temp = 8;
    add_to_hash_map(hm, "input_plist", temp);

    temp = calloc(1, sizeof(int));
    *temp = 9;
    add_to_hash_map(hm, "ret", temp);

    temp = calloc(1, sizeof(int));
    *temp = 10;
    add_to_hash_map(hm, "output_plist", temp);

    temp = calloc(1, sizeof(int));
    *temp = 11;
    add_to_hash_map(hm, "dataType", temp);

    temp = calloc(1, sizeof(int));
    *temp = 12;
    add_to_hash_map(hm, "input_plist2", temp);

    temp = calloc(1, sizeof(int));
    *temp = 13;
    add_to_hash_map(hm, "type", temp);

    temp = calloc(1, sizeof(int));
    *temp = 14;
    add_to_hash_map(hm, "output_plist2", temp);

    temp = calloc(1, sizeof(int));
    *temp = 15;
    add_to_hash_map(hm, "range_arrays", temp);

    temp = calloc(1, sizeof(int));
    *temp = 16;
    add_to_hash_map(hm, "index_nt", temp);

    temp = calloc(1, sizeof(int));
    *temp = 17;
    add_to_hash_map(hm, "statements", temp);

    temp = calloc(1, sizeof(int));
    *temp = 18;
    add_to_hash_map(hm, "statement", temp);

    temp = calloc(1, sizeof(int));
    *temp = 19;
    add_to_hash_map(hm, "ioStmt", temp);

    temp = calloc(1, sizeof(int));
    *temp = 20;
    add_to_hash_map(hm, "simpleStmt", temp);

    temp = calloc(1, sizeof(int));
    *temp = 21;
    add_to_hash_map(hm, "declareStmt", temp);

    temp = calloc(1, sizeof(int));
    *temp = 22;
    add_to_hash_map(hm, "condionalStmt", temp);

    temp = calloc(1, sizeof(int));
    *temp = 23;
    add_to_hash_map(hm, "iterativeStmt", temp);

    temp = calloc(1, sizeof(int));
    *temp = 24;
    add_to_hash_map(hm, "var", temp);

    temp = calloc(1, sizeof(int));
    *temp = 25;
    add_to_hash_map(hm, "boolConstt", temp);

    temp = calloc(1, sizeof(int));
    *temp = 26;
    add_to_hash_map(hm, "var_id_num", temp);

    temp = calloc(1, sizeof(int));
    *temp = 27;
    add_to_hash_map(hm, "whichId", temp);

    temp = calloc(1, sizeof(int));
    *temp = 28;
    add_to_hash_map(hm, "assignmentStmt", temp);

    temp = calloc(1, sizeof(int));
    *temp = 29;
    add_to_hash_map(hm, "moduleReuseStmt", temp);

    temp = calloc(1, sizeof(int));
    *temp = 30;
    add_to_hash_map(hm, "whichStmt", temp);

    temp = calloc(1, sizeof(int));
    *temp = 31;
    add_to_hash_map(hm, "lvalueIDStmt", temp);

    temp = calloc(1, sizeof(int));
    *temp = 32;
    add_to_hash_map(hm, "lvalueARRStmt", temp);

    temp = calloc(1, sizeof(int));
    *temp = 33;
    add_to_hash_map(hm, "expression", temp);

    temp = calloc(1, sizeof(int));
    *temp = 34;
    add_to_hash_map(hm, "optional", temp);

    temp = calloc(1, sizeof(int));
    *temp = 35;
    add_to_hash_map(hm, "idList", temp);

    temp = calloc(1, sizeof(int));
    *temp = 36;
    add_to_hash_map(hm, "idList2", temp);

    temp = calloc(1, sizeof(int));
    *temp = 37;
    add_to_hash_map(hm, "arithmeticOrBooleanExpr", temp);

    temp = calloc(1, sizeof(int));
    *temp = 38;
    add_to_hash_map(hm, "unary_nt", temp);

    temp = calloc(1, sizeof(int));
    *temp = 39;
    add_to_hash_map(hm, "unary_op", temp);

    temp = calloc(1, sizeof(int));
    *temp = 40;
    add_to_hash_map(hm, "new_NT", temp);

    temp = calloc(1, sizeof(int));
    *temp = 41;
    add_to_hash_map(hm, "arithmeticExpr", temp);

    temp = calloc(1, sizeof(int));
    *temp = 42;
    add_to_hash_map(hm, "AnyTerm", temp);

    temp = calloc(1, sizeof(int));
    *temp = 43;
    add_to_hash_map(hm, "arithmeticOrBooleanExpr2", temp);

    temp = calloc(1, sizeof(int));
    *temp = 44;
    add_to_hash_map(hm, "logicalOp", temp);

    temp = calloc(1, sizeof(int));
    *temp = 45;
    add_to_hash_map(hm, "AnyTerm2", temp);

    temp = calloc(1, sizeof(int));
    *temp = 46;
    add_to_hash_map(hm, "relationalOp", temp);

    temp = calloc(1, sizeof(int));
    *temp = 47;
    add_to_hash_map(hm, "term", temp);

    temp = calloc(1, sizeof(int));
    *temp = 48;
    add_to_hash_map(hm, "arithmeticExpr2", temp);

    temp = calloc(1, sizeof(int));
    *temp = 49;
    add_to_hash_map(hm, "op1", temp);

    temp = calloc(1, sizeof(int));
    *temp = 50;
    add_to_hash_map(hm, "factor", temp);

    temp = calloc(1, sizeof(int));
    *temp = 51;
    add_to_hash_map(hm, "term2", temp);

    temp = calloc(1, sizeof(int));
    *temp = 52;
    add_to_hash_map(hm, "op2", temp);

    temp = calloc(1, sizeof(int));
    *temp = 53;
    add_to_hash_map(hm, "caseStmts", temp);

    temp = calloc(1, sizeof(int));
    *temp = 54;
    add_to_hash_map(hm, "default_nt", temp);

    temp = calloc(1, sizeof(int));
    *temp = 55;
    add_to_hash_map(hm, "value", temp);

    temp = calloc(1, sizeof(int));
    *temp = 56;
    add_to_hash_map(hm, "caseStmts2", temp);

    temp = calloc(1, sizeof(int));
    *temp = 57;
    add_to_hash_map(hm, "range", temp);

    temp = NULL;
    return hm;
}