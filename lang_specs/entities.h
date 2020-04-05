#include <stdbool.h>

#ifndef ENTITIES_H
#define ENTITIES_H

#define NUM_NT 58
#define NUM_T 56
#define MAX_SYMBOL_LEN 25

enum ____TERMINAL____
{
	DECLARE,
	MODULE,
	ID,
	SEMICOL,
	DRIVERDEF,
	DRIVER,
	PROGRAM,
	DRIVERENDDEF,
	DEF,
	ENDDEF,
	TAKES,
	INPUT,
	SQBO,
	SQBC,
	RETURNS,
	COLON,
	COMMA,
	INTEGER,
	REAL,
	BOOLEAN,
	ARRAY,
	OF,
	RANGEOP,
	START,
	END,
	BO,
	BC,
	GET_VALUE,
	PRINT,
	TRUE,
	FALSE,
	NUM,
	RNUM,
	ASSIGNOP,
	USE,
	WITH,
	PARAMETERS,
	PLUS,
	MINUS,
	MUL,
	DIV,
	AND,
	OR,
	LT,
	LE,
	GT,
	GE,
	EQ,
	NE,
	SWITCH,
	CASE,
	BREAK,
	DEFAULT,
	FOR,
	IN,
	WHILE,
	EPS,
	DOLLAR
};

typedef enum ____TERMINAL____ terminal;

enum ____NONTERMINAL____
{
	program,
	moduleDeclarations,
	otherModules,
	driverModule,
	otherModule,
	moduleDeclaration,
	module,
	moduleDef,
	input_plist,
	ret,
	output_plist,
	dataType,
	input_plist2,
	type,
	output_plist2,
	range_arrays,
	index_nt,
	statements,
	statement,
	ioStmt,
	simpleStmt,
	declareStmt,
	condionalStmt,
	iterativeStmt,
	var,
	boolConstt,
	var_id_num,
	whichId,
	assignmentStmt,
	moduleReuseStmt,
	whichStmt,
	lvalueIDStmt,
	lvalueARRStmt,
	expression,
	optional,
	idList,
	idList2,
	arithmeticOrBooleanExpr,
	unary_nt,
	unary_op,
	new_NT,
	arithmeticExpr,
	AnyTerm,
	arithmeticOrBooleanExpr2,
	logicalOp,
	AnyTerm2,
	relationalOp,
	term,
	arithmeticExpr2,
	op1,
	factor,
	term2,
	op2,
	caseStmts,
	default_nt,
	value,
	caseStmts2,
	range
};

typedef enum ____NONTERMINAL____ nonterminal;

union ____NUM_VAL____ {
	int int_val;
	float float_val;
};

typedef union ____NUM_VAL____ num_val;

struct ____LEXICAL_TOKEN____
{
	terminal t;
	char *lexeme;
	int line_num;
	int char_num;
	num_val nv;
};

typedef struct ____LEXICAL_TOKEN____ lexical_token;

union ____GRAMMAR_SYMBOL____ {
	terminal t;
	nonterminal nt;
};
typedef union ____GRAMMAR_SYMBOL____ gm_symbol;

struct ____GRAMMAR_UNIT____
{
	bool is_terminal;
	gm_symbol gms;
};
typedef struct ____GRAMMAR_UNIT____ gm_unit;

struct ____NONTERMINAL_FIRST____
{
	nonterminal nt;
	terminal *first;
	int num_terminals;
	int max_terminals;
};

typedef struct ____NONTERMINAL_FIRST____ nt_first;

struct ____GRAMMAR_FIRST____
{
	nt_first *first_set;
};

typedef struct ____GRAMMAR_FIRST____ gm_first;

struct ____NONTERMINAL_FOLLOW____
{
	nonterminal nt;
	terminal *follow;
	int num_terminals;
	int max_terminals;
};

typedef struct ____NONTERMINAL_FOLLOW____ nt_follow;

struct ____GRAMMAR_FOLLOW____
{
	nt_follow *follow_set;
};

typedef struct ____GRAMMAR_FOLLOW____ gm_follow;

struct ____PARSE_TREE_NODE____
{
	nonterminal nt;
};

typedef struct ____PARSE_TREE_NODE____ pt_node;

struct ____PARSE_TREE_LEAF____
{
	lexical_token *lt;
};

typedef struct ____PARSE_TREE_LEAF____ pt_leaf;

#endif
