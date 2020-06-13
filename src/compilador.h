#ifndef __COMPILADOR_H__
#define __COMPILADOR_H__

void init();
void nextChar();
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void match(char c);
char getName();
char getNum();
void emit(char *fmt, ...);
void term();
void expression();
void add();
void subtract();

#endif
