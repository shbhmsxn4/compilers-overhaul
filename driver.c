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

int main()
{
    hash_map *thm = create_terminal_hash_map(5);
    hash_map *nthm = create_nonterminal_hash_map(5);

    lexer *l = create_lexer("/Users/shubhamsaxena/compiler-project/compiler/t1.txt", "/Users/shubhamsaxena/compiler-project/compiler/lang_specs/dfa_specs", 512, 32, 30);

    grammar *gm = parse_grammar("/Users/shubhamsaxena/compiler-project/compiler/lang_specs/grammar", thm, nthm);

    gm_first *fi = get_first(gm);
    gm_follow *fo = get_follow(gm, fi);
    parse_table *pt = generate_parse_table(gm, fi, fo);

    print_grammar(gm);
    print_first(fi);
    print_follow(fo);
    print_parse_table(pt);

    return 0;
}