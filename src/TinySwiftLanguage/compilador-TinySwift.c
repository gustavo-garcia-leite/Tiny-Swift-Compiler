#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "compilador-TinySwift.h"

char *commandList[LIST_SZ] = {"IF", "ELSE", "ENDIF", "WHILE", "ENDWHILE", "READ", "WRITE", "VAR", "BEGIN", "END", "PROGRAM"};
char *commandCode = "ileweRWvbep"; /* lista de palavras-chave */
int nsym;                          /* número de entradas na tabela de símbolos */

/* inicializacao do compilador */
void init()
{
    nsym = 0;
    nextChar();
    scan();
}

/* Le o próximo caracter da entrada */
void nextChar()
{
    nextToken = getchar();
}

/* exibe uma mensagem de erro formatada */
void error(char *fmt, ...)
{
    fprintf(stderr, "error: %s \n", fmt);
}

/* exibe uma mensagem de erro formatada e sai */
void fatal(char *fmt, ...)
{
    va_list args;

    fputs("Error: ", stderr);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);

    exit(1);
}

/* alerta sobre alguma entrada esperada */
void expected(char *fmt, ...)
{
    fprintf(stderr, "Error: %s expected\n", fmt);
    exit(1);
}

/* verifica se entrada combina com o esperado */
void match(char c)
{
    char s[2];
    newline();
    if (nextToken == c)
        nextChar();
    else
    {
        s[0] = c;    /* uma conversao rápida (e feia) */
        s[1] = '\0'; /* de um caracter em string */
        expected(s);
    }
    skipWhite();
}

/* recebe o nome de um identificador */
void getName()
{
    int i;
    newline();
    if (!isalpha(nextToken))
        expected("Name");
    for (i = 0; isalnum(nextToken) && i < MAXTOKEN; i++)
    {
        value[i] = toupper(nextToken);
        nextChar();
    }
    value[i] = '\0';
    token = 'x';
    skipWhite();
}

/* recebe um número inteiro */
int getNum()
{
    newline();
    int i = 0;
    if (!isdigit(nextToken))
        expected("Integer");
    while (isdigit(nextToken))
    {
        i *= 10;
        i += nextToken - '0';
        nextChar();
    }
    skipWhite();
    return i;
}

/* emite uma instrucao seguida por uma nova linha */
void emit(char *fmt, ...)
{
    va_list args;
    putchar('\t');
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    putchar('\n');
}

void prog()
{
    matchstring("PROGRAM");
    header();
    declareVariables();
    mainblock();
    match('.');
}

/* emite o código inicial necessário para o montador */
void header()
{
    printf(".model small\n");
    printf(".stack\n");
    printf(".code\n");
    printf("extrn READ:near, WRITE:near\n");
    printf("PROG segment byte public\n");
    printf("\tassume cs:PROG,ds:PROG,es:PROG,ss:PROG\n");
}

void prolog()
{
    printf("MAIN:\n");
    printf("\tmov ax, PROG\n");
    printf("\tmov ds, ax\n");
    printf("\tmov es, ax\n");
}

void epilog()
{
    printf("\tmov ax,4C00h\n");
    printf("\tint 21h\n");
    printf("PROG ends\n");
    printf("\tend MAIN\n");
}

/* Aqui rodará a parte de "código" realmente */
void mainblock()
{
    matchstring("BEGIN");
    prolog();
    block();
    matchstring("END");
    epilog();
}

void declareVariable()
{
    for (;;)
    {
        getName();
        allocVar(value);
        newline();
        if (nextToken != ',')
            break;
        match(',');
    }
}

void declareVariables()
{
    scan();
    while (token != 'b')
    {
        switch (token)
        {
        case 'v':
            declareVariable();
            break;
        default:
            error("Unrecognized keyword.");
            expected("BEGIN");
            break;
        }
        scan();
    }
}

void allocVar(char *name)
{
    int value = 0, signal = 1;
    addsymbol(name);
    newline();
    if (nextToken == '=')
    {
        match('=');
        newline();
        if (nextToken == '-')
        {
            match('-');
            signal = -1;
        }
        value = signal * getNum();
    }
    printf("%s:\tdw %d\n", name, value);
}

int intable(char *name)
{
    if (lookup(name, symbolStorage, nsym) < 0)
        return 0;
    return 1;
}

/* analisa e traduz um comando de atribuição */
void assignment()
{
    char name[MAXTOKEN + 1];
    strcpy(name, value);
    match('=');
    boolexpression();
    asm_store(name);
}

void block()
{
    int follow = 0;
    do
    {
        scan();
        switch (token)
        {
        case 'i':
            doif();
            break;
        case 'w':
            dowhile();
            break;
        case 'R':
            doread();
            break;
        case 'W':
            dowrite();
            break;
        case 'e':
        case 'l':
            follow = 1;
            break;
        default:
            assignment();
            break;
        }
    } while (!follow);
}

/* pula caracteres de espaço */
void skipWhite()
{
    while (nextToken == ' ' || nextToken == '\t')
        nextChar();
}

void newline()
{
    while (nextToken == '\n')
    {
        nextChar();
        skipWhite();
    }
}

/* zera o registrador primário */
void asm_clear()
{
    printf("\txor ax, ax\n");
}

/* negativa o reg. primário */
void asm_negative()
{
    printf("\tneg ax\n");
}

/* carrega uma constante numérica no reg. prim. */
void asm_loadconst(int i)
{
    printf("\tmov ax, %d\n", i);
}

/* carrega uma variável no reg. prim. */
void asm_loadvar(char *name)
{
    if (!intable(name))
        undefined(name);
    printf("\tmov ax, word ptr %s\n", name);
}

/* coloca reg. prim. na pilha */
void asm_push()
{
    printf("\tpush ax\n");
}

/* adiciona o topo da pilha ao reg. prim. */
void asm_popadd()
{
    printf("\tpop bx\n");
    printf("\tadd ax, bx\n");
}

/* subtrai o reg. prim. do topo da pilha */
void asm_popsub()
{
    printf("\tpop bx\n");
    printf("\tsub ax, bx\n");
    printf("\tneg ax\n");
}

/* multiplica o topo da pilha pelo reg. prim. */
void asm_popmul()
{
    printf("\tpop bx\n");
    printf("\timul bx\n");
}

/* divide o topo da pilha pelo reg. prim. */
void asm_popdiv()
{
    printf("\tpop bx\n");
    printf("\txchg ax, bx\n");
    printf("\tcwd\n");
    printf("\tidiv bx\n");
}

/* armazena reg. prim. em variável */
void asm_store(char *name)
{
    if (!intable(name))
        undefined(name);
    printf("\tmov word ptr %s, ax\n", name);
}

/* avisa a respeito de um identificador desconhecido */

void undefined(char *name)
{
    int i;
    fprintf(stderr, "Error: Undefined identifier %s\n", name);
    fprintf(stderr, "Symbol table:\n");
    for (i = 0; i < nsym; i++)
        fprintf(stderr, "%d: %s\n", i, symbolStorage[i]);
    exit(1);
}

/* analisa e traduz um fator matemático */
void factor()
{
    newline();
    if (nextToken == '(')
    {
        match('(');
        boolexpression();
        match(')');
    }
    else if (isalpha(nextToken))
    {
        getName();
        asm_loadvar(value);
    }
    else
        asm_loadconst(getNum());
}

/* analisa e traduz um fator negativo */
void negfactor()
{
    match('-');
    if (isdigit(nextToken))
        asm_loadconst(-getNum());
    else
    {
        factor();
        asm_negative();
    }
}

/* analisa e traduz um fator inicial */
void firstfactor()
{
    newline();
    switch (nextToken)
    {
    case '+':
        match('+');
        factor();
        break;
    case '-':
        negfactor();
        break;
    default:
        factor();
        break;
    }
}

/* reconhece e traduz uma multiplicação */
void multiply()
{
    match('*');
    factor();
    asm_popmul();
}

/* reconhece e traduz uma divisão */
void divide()
{
    match('/');
    factor();
    asm_popdiv();
}

int isaddop(char c)
{
    return (c == '+') || (c == '-');
}

int ismulop(char c)
{
    return (c == '*') || (c == '/');
}

/* código comum usado por "term" e "firstterm" */
void term1()
{
    newline();
    while (ismulop(nextToken))
    {
        asm_push();
        switch (nextToken)
        {
        case '*':
            multiply();
            break;
        case '/':
            divide();
            break;
        }
        newline();
    }
}

/* analisa e traduz um termo matemático */
void term()
{
    factor();
    term1();
}

/* analisa e traduz um termo inicial */
void firstterm()
{
    firstfactor();
    term1();
}

/* reconhece e traduz uma adição */
void add()
{
    match('+');
    term();
    asm_popadd();
}

/* reconhece e traduz uma subtração*/
void subtract()
{
    match('-');
    term();
    asm_popsub();
}

/* analisa e traduz uma expressão matemática */
void expression()
{
    firstterm();
    newline();
    while (isaddop(nextToken))
    {
        asm_push();
        switch (nextToken)
        {
        case '+':
            add();
            break;
        case '-':
            subtract();
            break;
        }
        newline();
    }
}

int isorop(char c)
{
    return (c == '|' || c == '~');
}

int isrelop(char c)
{
    return (strchr("=#<>", c) != NULL);
}

/* inverte reg. prim. */
void asm_not()
{
    printf("\tnot ax\n");
}

/* "E" do topo da pilha com reg. prim. */
void asm_popand()
{
    printf("\tpop bx\n");
    printf("\tand ax, bx\n");
}

/* "OU" do topo da pilha com reg. prim. */
void asm_popor()
{
    printf("\tpop bx\n");
    printf("\tor ax, bx\n");
}

/* "OU-exclusivo" do topo da pilha com reg. prim. */
void asm_popxor()
{
    printf("\tpop bx\n");
    printf("\txor ax, bx\n");
}

/* compara topo da pilha com reg. prim. */
void asm_popcompare()
{
    printf("\tpop bx\n");
    printf("\tcmp bx, ax\n");
}

/* gera um novo rótulo único */
int newlabel()
{
    return labelCount++;
}

/* altera reg.primário(e flags, indiretamente) conforme a comparação */

void asm_relop(char op)
{
    char *jump;
    int l1, l2;
    l1 = newlabel();
    l2 = newlabel();
    switch (op)
    {
    case '=':
        jump = "je";
        break;
    case '#':
        jump = "jne";
        break;
    case '<':
        jump = "jl";
        break;
    case '>':
        jump = "jg";
        break;
    case 'L':
        jump = "jle";
        break;
    case 'G':
        jump = "jge";
        break;
    }
    printf("\t%s L%d\n", jump, l1);
    printf("\txor ax, ax\n");
    printf("\tjmp L%d\n", l2);
    printf("L%d:\n", l1);
    printf("\tmov ax, -1\n");
    printf("L%d:\n", l2);
}

/* desvio incondicional */
void asm_jmp(int label)
{
    printf("\tjmp L%d\n", label);
}

/* desvio se falso (0) */
void asm_jmpfalse(int label)
{
    printf("\tjz L%d\n", label);
}

/* lê um valor para o registrador primário e armazena em variável */
void asm_read()
{
    printf("\tcall READ\n");
    asm_store(value);
}

/* mostra valor do reg. primário */
void asm_write()
{
    printf("\tcall WRITE\n");
}

/* analisa e traduz uma relação */
void relation()
{
    char op;
    expression();
    if (isrelop(nextToken))
    {
        op = nextToken;
        match(op); /* só para remover o operador do caminho */
        if (op == '<')
        {
            if (nextToken == '>')
            { /* <> */
                match('>');
                op = '#';
            }
            else if (nextToken == '=')
            {
                match('=');
                op = 'L';
            }
        }
        else if (op == '>' && nextToken == '=')
        {
            match('=');
            op = 'G';
        }
        asm_push();
        expression();
        asm_popcompare();
        asm_relop(op);
    }
}

/* analisa e traduz um fator booleano com NOT inicial */
void notfactor()
{
    if (nextToken == '!')
    {
        match('!');
        relation();
        asm_not();
    }
    else
        relation();
}

/* analisa e traduz um termo booleano */
void boolterm()
{
    notfactor();
    newline();
    while (nextToken == '&')
    {
        asm_push();
        match('&');
        notfactor();
        asm_popand();
        newline();
    }
}

/* reconhece e traduz um "OR" */
void boolor()
{
    match('|');
    boolterm();
    asm_popor();
}

/* reconhece e traduz um "xor" */
void boolxor()
{
    match('~');
    boolterm();
    asm_popxor();
}

/* analisa e traduz uma expressão booleana */
void boolexpression()
{
    boolterm();
    newline();
    while (isorop(nextToken))
    {
        asm_push();
        switch (nextToken)
        {
        case '|':
            boolor();
            break;
        case '~':
            boolxor();
            break;
        }
        newline();
    }
}

/* analiza e traduz um comando IF-ELSE-ENDIF */
void doif()
{
    int l1, l2;
    boolexpression();
    l1 = newlabel();
    l2 = l1;
    asm_jmpfalse(l1);
    block();
    if (token == 'l')
    {
        l2 = newlabel();
        asm_jmp(l2);
        printf("L%d:\n", l1);
        block();
    }
    printf("L%d:\n", l2);
    matchstring("ENDIF");
}

/* analiza e traduz um comando WHILE-ENDWHILE */
void dowhile()
{
    int l1, l2;
    l1 = newlabel();
    l2 = newlabel();
    printf("L%d:\n", l1);
    boolexpression();
    asm_jmpfalse(l2);
    block();
    matchstring("ENDWHILE");
    asm_jmp(l1);
    printf("L%d:\n", l2);
}

int lookup(char *s, char *list[], int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        if (strcmp(list[i], s) == 0)
            return i;
    }
    return -1;
}

void scan()
{
    int kw;
    getName();
    kw = lookup(value, commandList, LIST_SZ);
    if (kw == -1)
        token = 'x';
    else
        token = commandCode[kw];
}

void matchstring(char *s)
{
    if (strcmp(value, s) != 0)
        expected(s);
}

void addsymbol(char *name)
{
    char *newsym;
    if (intable(name))
    {
        fprintf(stderr, "Duplicated variable name: %s", name);
        exit(1);
    }
    if (nsym >= STORAGE_SIZE)
    {
        fatal("Symbol table full!");
    }
    newsym = (char *)malloc(sizeof(char) * (strlen(name) + 1));
    if (newsym == NULL)
        fatal("Out of memory!");
    strcpy(newsym, name);
    symbolStorage[nsym++] = newsym;
}

void doread()
{
    match('(');
    for (;;)
    {
        getName();
        asm_read();
        newline();
        if (nextToken != ',')
            break;
        match(',');
    }
    match(')');
}

void dowrite()
{
    match('(');
    for (;;)
    {
        expression();
        asm_write();
        newline();
        if (nextToken != ',')
            break;
        match(',');
    }
    match(')');
}