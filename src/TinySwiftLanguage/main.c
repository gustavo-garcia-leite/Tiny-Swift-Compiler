#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include "compilador-TinySwift.h"

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    prog();
    if (nextToken != '\n')
        fatal("Unexpected data after \'.\'");
    return 0;
}