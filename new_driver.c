/*
 Group- 35
 2017A7PS0082P		Laksh Singla
 2017A7PS0148P 		Kunal Mohta
 2017A7PS0191P 		Suyash Raj
 2017A7PS0302P 		Shubham Saxena
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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


int main (int argc, char *argv[]) {

	/*
	 *if (argc != 3 || (argc == 3 && strcmp(argv[0], "./stage1exe") != 0)) {
	 *    printf("Incorrect execution format. Expected:-\n");
	 *    printf("./stage1exe testcase.txt parsetreeOutFile.txt\n");
	 *    return 0;
	 *}
	 */

	printf("\n**************** ERPLAG COMPILER - PHASE 2 ****************\n");
	/*
	 *printf("\nIMPLEMENTATION STATUS:-\n");
	 *printf("-> Both lexer and syntax analysis modules implemented and compile successfully.\n");
	 *printf("-> FIRST & FOLLOW sets automated.\n");
	 *printf("-> Modules work for all testcases (t1-t6).\n");
	 *printf("-> Parse tree constructed.\n");
	 *printf("-> Error recovery done for both lexer & parser.\n");
	 */
	

	while (1) {
		printf("\n");
		printf("Options:-\n");
		printf("0. Exit\n");
		printf("1. Lexer\n");
		printf("2. Parser\n");
		printf("3. AST\n");
		printf("4. Memory\n");
		printf("5. Symbol Table\n");
		printf("6. Activation Record\n");
		printf("7. Static and Dynamic arrays\n");
		printf("8. Error reporting and total compiling time\n");
		printf("9. Code generation\n");

		int user_inp;
		printf("\nSelect option> ");
		scanf("%d", &user_inp);
		printf("\n");

		switch (user_inp) {
			case 0:
				printf("Option selected: 0 (Exit)\n");
				printf("Exiting...\n");
				return 0;
				break;

			case 1:
				printf("Option selected: 1 (Lexer)\n");

				break;

			case 2:
				printf("Option selected: 2 (Parser)\n");

				break;

			case 3:
				printf("Option selected: 3 (AST)\n");

				break;

			case 4:
				printf("Option selected: 4 (Memory)\n");

				break;

			case 5:
				printf("Option selected: 5 (Symbol Table)\n");

				hash_map *thm = create_terminal_hash_map(15);
				hash_map *nthm = create_nonterminal_hash_map(15);
				hash_map *khm = create_keyword_hash_map(15);

				lexer *l = create_lexer("/home/kunal/Desktop/github/compilers-overhaul/test2.txt", "/home/kunal/Desktop/github/compilers-overhaul/lang_specs/dfa_specs", 512, 32, 30, khm);
				/*lexer *l = create_lexer("/home/kunal/Desktop/github/compilers-overhaul/testcases_stage2/t10.txt", "/home/kunal/Desktop/github/compilers-overhaul/lang_specs/dfa_specs", 512, 32, 30, khm);*/

				grammar *gm = parse_grammar("/home/kunal/Desktop/github/compilers-overhaul/lang_specs/grammar", thm, nthm);

				gm_first *fi = get_first(gm);
				gm_follow *fo = get_follow(gm, fi);
				parse_table *pt = generate_parse_table(gm, fi, fo);
				tree *ptree = parse(l, gm, pt);

				tree *ast_tree = generate_ast(ptree);
				/*print_ast_tree(ast_tree);*/
				
				hash_map *main_st = call_semantic_analyzer(ast_tree);
				print_symbol_table(main_st);

				break;

			case 6:
				printf("Option selected: 6 (Activation Record)\n");

				break;

			case 7:
				printf("Option selected: 7 (Static and Dynamic arrays)\n");

				break;

			case 8:
				printf("Option selected: 8 (Error reporting and total compiling time)\n");

				break;

			case 9:
				printf("Option selected: 9 (Code generation)\n");

				break;

		}
	}
}
