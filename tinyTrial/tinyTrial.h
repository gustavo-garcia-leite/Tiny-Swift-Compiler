#ifndef __COMPILADOR_H__
#define __COMPILADOR_H__

/* variáveis */
char look; /* O caracter lido "antecipadamente" (lookahead) */

void add();
void assignment();
void divide();
void error(char *fmt, ...);
void emit(char *fmt, ...);
void expected(char *fmt, ...);
void expression();
void factor();
void fatal(char *fmt, ...);
void getName(char *name);
void getNum(char *num);
void ident();
void init();
int isAddOp(char c);
void skipWhite();
void multiply();
void nextChar();
void subtract();
void term();

#endif
