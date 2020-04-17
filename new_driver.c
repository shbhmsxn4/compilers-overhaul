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
#include "./driver.h"

bool display_err_flag = true;
bool array_only_flag = false;

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
	hash_map *thm, *nthm, *khm;
	lexer *l;
	grammar *gm;
	gm_first *fi;
	gm_follow *fo;
	parse_table *pt;
	tree *ptree, *ast_tree;
	hash_map *main_st;

	/*char test_file[150] = "/home/kunal/Desktop/github/compilers-overhaul/revisedtests/t5.txt";*/
	/*char test_file[150] = "/home/kunal/Desktop/github/compilers-overhaul/revisedtests/Sample_Symbol_table.txt";*/
	char test_file[150] = "/home/kunal/Desktop/github/compilers-overhaul/test2.txt";
	char dfa_specs_file[150] = "/home/kunal/Desktop/github/compilers-overhaul/lang_specs/dfa_specs";
	char grammar_file[150] = "/home/kunal/Desktop/github/compilers-overhaul/lang_specs/grammar";

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
				printf("Option selected: 0 (Exit)\n\n");
				printf("Exiting...\n");
				return 0;
				break;

			case 1:
				printf("Option selected: 1 (Lexer)\n\n");

				break;

			case 2:
				printf("Option selected: 2 (Parser)\n\n");

				break;

			case 3:
				printf("Option selected: 3 (AST)\n\n");

				break;

			case 4:
				printf("Option selected: 4 (Memory)\n\n");

				display_err_flag = false;
				array_only_flag = false;

				thm = create_terminal_hash_map(15);
				nthm = create_nonterminal_hash_map(15);
				khm = create_keyword_hash_map(15);

				l = create_lexer(test_file, dfa_specs_file, 512, 32, 30, khm);
				gm = parse_grammar(grammar_file, thm, nthm);

				fi = get_first(gm);
				fo = get_follow(gm, fi);
				pt = generate_parse_table(gm, fi, fo);
				ptree = parse(l, gm, pt);

				ast_tree = generate_ast(ptree);

				int pt_mem = 0, pt_nodes = 0;
				calc_mem_parse_tree(ptree, &pt_mem, &pt_nodes);
				printf("Total number of Parse tree nodes: %d\n", pt_nodes);
				printf("Total memory used by Parse tree: %d bytes\n", pt_mem);
				printf("\n");

				int ast_mem = 0, ast_nodes = 0;
				calc_mem_ast(ast_tree, &ast_mem, &ast_nodes);
				printf("Total number of AST tree nodes: %d\n", ast_nodes);
				printf("Total memory used by AST: %d bytes\n", ast_mem);
				printf("\n");

				double comp_ratio = ((double)(pt_mem - ast_mem))/((double)pt_mem);
				printf("Compression ratio: %lf\n", comp_ratio*100);
				break;

			case 5:
				printf("Option selected: 5 (Symbol Table)\n\n");

				display_err_flag = false;
				array_only_flag = false;

				thm = create_terminal_hash_map(15);
				nthm = create_nonterminal_hash_map(15);
				khm = create_keyword_hash_map(15);

				l = create_lexer(test_file, dfa_specs_file, 512, 32, 30, khm);
				gm = parse_grammar(grammar_file, thm, nthm);

				fi = get_first(gm);
				fo = get_follow(gm, fi);
				pt = generate_parse_table(gm, fi, fo);
				ptree = parse(l, gm, pt);

				ast_tree = generate_ast(ptree);
				/*print_ast_tree(ast_tree);*/
				
				main_st = call_semantic_analyzer(ast_tree);
				print_symbol_table(main_st);

				break;

			case 6:
				printf("Option selected: 6 (Activation Record size)\n\n");

				display_err_flag = false;
				array_only_flag = false;

				thm = create_terminal_hash_map(15);
				nthm = create_nonterminal_hash_map(15);
				khm = create_keyword_hash_map(15);

				l = create_lexer(test_file, dfa_specs_file, 512, 32, 30, khm);
				gm = parse_grammar(grammar_file, thm, nthm);
			
				fi = get_first(gm);
				fo = get_follow(gm, fi);
				pt = generate_parse_table(gm, fi, fo);
				ptree = parse(l, gm, pt);

				ast_tree = generate_ast(ptree);
				/*print_ast_tree(ast_tree);*/
				
				main_st = call_semantic_analyzer(ast_tree);
				print_ar_size(main_st);

				break;

			case 7:
				printf("Option selected: 7 (Static and Dynamic arrays)\n\n");

				display_err_flag = false;
				array_only_flag = true;

				thm = create_terminal_hash_map(15);
				nthm = create_nonterminal_hash_map(15);
				khm = create_keyword_hash_map(15);

				l = create_lexer(test_file, dfa_specs_file, 512, 32, 30, khm);
				gm = parse_grammar(grammar_file, thm, nthm);
				
				fi = get_first(gm);
				fo = get_follow(gm, fi);
				pt = generate_parse_table(gm, fi, fo);
				ptree = parse(l, gm, pt);

				ast_tree = generate_ast(ptree);
				/*print_ast_tree(ast_tree);*/
				
				main_st = call_semantic_analyzer(ast_tree);
				print_symbol_table(main_st);

				break;

			case 8:
				printf("Option selected: 8 (Error reporting and total compiling time)\n\n");

				clock_t start_time, end_time;
				double total_CPU_time, total_CPU_time_in_seconds;
				start_time = clock();


				display_err_flag = true;
				array_only_flag = false;

				thm = create_terminal_hash_map(15);
				nthm = create_nonterminal_hash_map(15);
				khm = create_keyword_hash_map(15);

				l = create_lexer(test_file, dfa_specs_file, 512, 32, 30, khm);
				gm = parse_grammar(grammar_file, thm, nthm);
				
				fi = get_first(gm);
				fo = get_follow(gm, fi);
				pt = generate_parse_table(gm, fi, fo);
				ptree = parse(l, gm, pt);

				// TODO: if lexical/syntactical errs, don't procede

				ast_tree = generate_ast(ptree);
				/*print_ast_tree(ast_tree);*/
				
				main_st = call_semantic_analyzer(ast_tree);

				printf("\n\n");
				end_time = clock();
				total_CPU_time = (double) (end_time - start_time);
				total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
				printf("TOTAL CPU TIME: %f cycles\n", total_CPU_time);
				printf("TOTAL CPU TIME IN SECONDS: %f seconds\n", total_CPU_time_in_seconds);

				break;

			case 9:
				printf("Option selected: 9 (Code generation)\n\n");

				break;
			
			default:
				printf("Incorrect option\n\n");
				break;
		}
	}
}
