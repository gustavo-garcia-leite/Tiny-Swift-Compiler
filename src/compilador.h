#ifndef __COMPILADOR_H__
#define __COMPILADOR_H__

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
void match(char c);
void multiply();
void nextChar();
void subtract();
void term();

#endif
