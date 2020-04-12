#include <stdio.h>
#include "./lexer/lexer.h"
#include "./lang_specs/entities.h"
#include "./data_structs/hash_map.h"
#include "./utils/terminal_hash_map.h"
#include "./utils/nonterminal_hash_map.h"
#include "./data_structs/grammar.h"
#include "./utils/parse_grammar.h"
#include "./utils/print.h"
#include "./utils/generate_parse_table.h"
#include "./data_structs/parse_table.h"
#include "./data_structs/tree.h"
#include "./parser/parser.h"
#include "./utils/keyword_hash_map.h"
#include "./ast/generate_ast.h"
#include "./symbol_table/symbol_table.h"
#include "./symbol_table/symbol_table_def.h"
#include "./semantic_analyzer/semantics.h"

int main()
{
    hash_map *thm = create_terminal_hash_map(15);
    hash_map *nthm = create_nonterminal_hash_map(15);
    hash_map *khm = create_keyword_hash_map(15);

	/*lexer *l = create_lexer("/home/kunal/Desktop/github/compilers-overhaul/test2.txt", "/home/kunal/Desktop/github/compilers-overhaul/lang_specs/dfa_specs", 512, 32, 30, khm);*/
	lexer *l = create_lexer("/home/kunal/Desktop/github/compilers-overhaul/testcases_stage2/t10.txt", "/home/kunal/Desktop/github/compilers-overhaul/lang_specs/dfa_specs", 512, 32, 30, khm);

    grammar *gm = parse_grammar("/home/kunal/Desktop/github/compilers-overhaul/lang_specs/grammar", thm, nthm);

    gm_first *fi = get_first(gm);
    gm_follow *fo = get_follow(gm, fi);
    parse_table *pt = generate_parse_table(gm, fi, fo);
    tree *ptree = parse(l, gm, pt);

	/*print_parse_tree(ptree);*/

    tree *ast_tree = generate_ast(ptree);

    // print_grammar(gm);
    // print_first(fi);
    // print_follow(fo);
    // print_parse_table(pt);
	/*print_ast_tree(ast_tree);*/

	call_semantic_analyzer(ast_tree);

    return 0;
}
