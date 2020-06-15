# CompiladorSimples
Projeto final da matéria de compiladores - BCC Sétimo semestre

Formalismo de Backus-Naur
<program> ::= p <top-level decl> <main> 
<main> ::= { <block> }
<block> ::= ( <statement> )*
<statement> ::= <if> | <while> | <assignment>

//<assignment> ::= <ident> '=' <expression>
<expression> ::= <first term> ( <addop> <term> )*
<first term> ::= <first factor> <rest>
<term> ::= <factor> <rest>
<rest> ::= ( <mulop> <factor> )*
<first factor> ::= [ <addop> ] <factor>
 <factor> ::= <var> | <number> | ( <expression> )

/* Boolean Expressions */
<bool-expr> ::= <bool-term> ( <orop> <bool-term> )*
<bool-term> ::= <not-factor> ( <andop> <not-factor> )*
<not-factor> ::= [ '!' ] <relation>                
<relation> ::= <expression> [ <relop> <expression> ]

/* Construções de Controle */
<if> ::= IF <bool-expression> <block> [ ELSE <block> ] ENDIF
<while> ::= WHILE <bool-expression> <block> ENDWHILE
