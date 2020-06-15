# Compilador Simples
Projeto final da matéria de compiladores - BCC Sétimo semestre

## Objetivo

Nós gostariamos que fazer um compilador que compilasse uma versão simplificada da Linguagem de Programação Swift, sendo assim, nomearemos de Tiny Swift.

### Exemplo de Código 1 (Expectativa)

```sh
Class Teste {
  var count = 0
  var num1 = 1
  var num2 = 10
  
  while num1 <= num 2 {
    if num1 != 5
      count=count+num1
    else
      count=count+5
    num1=num1+1
  }
}
```
## Resultado

Para a realização deste projeto, foi necessário seguir os conceitos apresentados na tradução do [Tutorial](http://tutorialcompiladores.pbworks.com/w/page/22541623/FrontPage) sobre construção de compiladores de Jack W. Crenshaw com linguagem assembly de saída.

### Formalismo de Backus-Naur

- Corpo do Compilador
```sh
<program> ::= p <top-level decl> <main> 
<main> ::= { <block> }
<block> ::= ( <statement> )*
<statement> ::= <if> | <while> | <assignment>
```

- Expressões
```sh
<assignment> ::= <ident> '=' <expression>
<expression> ::= <first term> ( <addop> <term> )*
<first term> ::= <first factor> <rest>
<term> ::= <factor> <rest>
<rest> ::= ( <mulop> <factor> )*
<first factor> ::= [ <addop> ] <factor>
<factor> ::= <var> | <number> | ( <expression> )
 ```

- Expressões Booleanas
```sh
<bool-expr> ::= <bool-term> ( <orop> <bool-term> )*
<bool-term> ::= <not-factor> ( <andop> <not-factor> )*
<not-factor> ::= [ '!' ] <relation>                
<relation> ::= <expression> [ <relop> <expression> ]
```

- Construções de Controle
```sh
<if> ::= IF <bool-expression> <block> [ ELSE <block> ] ENDIF
<while> ::= WHILE <bool-expression> <block> ENDWHILE
```
### Exemplo de Código 2 (Resultado)
```sh
CLASS
VAR count, num1=1, num2=10
BEGIN
  WHILE num1 <= num2
    IF num1 <> 5 
      count=count+num1
    ELSE
      count=count+5
    ENDIF
    num1=num1+1
  ENDWHILE
END.
```

Como pode ser visto, nosso objetivo não foi alcançado por completo :sweat:... Isso ocorreu pois não conseguimos adaptar a tempo os resultados obtidos a partir do tutorial ao que queriamos, então alguns comandos precisarão ser alterados para que o compilador consiga compilar códigos como o exemplo 1.
