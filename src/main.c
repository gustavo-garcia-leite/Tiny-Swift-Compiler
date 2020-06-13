#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include "compilador.h"

/* variáveis */
char look; /* O caracter lido "antecipadamente" (lookahead) */

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    assignment();
    if (look != '\n')
        expected("NewLine");

    return 0;
}