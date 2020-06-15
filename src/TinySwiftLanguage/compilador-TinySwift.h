#ifndef __COMPILADOR_H__
#define __COMPILADOR_H__
#define STORAGE_SIZE 1000
#define MAXTOKEN 16

char token;               /* código do token atual */
char value[MAXTOKEN + 1]; /* texto do token atual */
char symbolStorage[STORAGE_SIZE];
char nextToken; /* O caracter lido "antecipadamente" (lookahead) */
int labelCount;

/* protótipos */

/* Craddle */
void init();
void nextChar();
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void match(char c);
void getName();
char getNum();
void emit(char *fmt, ...);

/* Process Steps */
void prog();
void header();
void declareVariables();
void prolog();
void epilog();
void mainblock();
void assignment();
void block();

/* Variables */
void declareVariable();
void allocVar(char *name);
int intable(char *name);
void undefined(char *name);
void skipWhite();
void newline();
void addsymbol(char *name);

/* Rotinas de geracao de código */
void asm_clear();
void asm_negative();
void asm_loadconst(int i);
void asm_loadvar(char *name);
void asm_push();
void asm_popadd();
void asm_popsub();
void asm_popmul();
void asm_popdiv();
void asm_store(char *name);
void asm_not();
void asm_popand();
void asm_popor();
void asm_popxor();
void asm_popcompare();
void asm_relop(char op);
void asm_jmp(int label);
void asm_jmpfalse(int label);

/* Operacoes */
void factor();
void negfactor();
void firstfactor();
void multiply();
void divide();
void term1();
void term();
void firstterm();
void add();
void subtract();
void expression();
void assignment();
int isaddop(char c);
int ismulop(char c);
int isorop(char c);
int isrelop(char c);
int newlabel();
void relation();
void notfactor();

/* Booleans */
void boolterm();
void boolor();
void boolxor();
void boolexpression();

/* Controles */
void doif();
void dowhile();
void doread();
void dowrite();

/* Analise Lexica */
int lookup(char *s, char *list[], int size);
void scan();
void matchstring(char *s);

#endif