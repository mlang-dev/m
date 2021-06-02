---
layout: page
title: spec
permalink: /spec/
description: m language specification
nav: true
---



## Extended BNF Notation
The m language grammar is given in this specification using a simple Extended Backus-Naur Form (EBNF) from [W3C EBNF](https://www.w3.org/TR/REC-xml/#sec-notation).


### Lexical Rules
```
letter          ::= [a-zA-Z]
character       ::= any printable ASCII character
digit           ::= [0-9]
id              ::= [_a-zA-Z][_a-zA-Z0-9]*
sign            ::= "+" | "-"
point           ::= "."
exponent-marker ::= "e" | "E" | "s" | "S" | "d" | "D" | "f" | "F" | "l" | "L"
exponent        ::= exponent-marker sign? (digit)+
floatconst      ::= sign? (digit)* point (digit)+ exponent
intconst        ::= (digit)\*
charconst       ::= 'character' | '\n' | '\0'
stringconst     ::= "character*"
comments        ::= #(character)*
nl              ::= '\n' | '\r\n'
indent          ::= indentation marker: increase to a new indentation level
dedent          ::= dedentation marker: decrease to a old indentation level
```

### Syntax Rules
```
Program         ::= (Declare | Function)\*
Declare         ::= Type_Decl | Var_Decl
Var_Decl        ::= id(:Type)?
Type            ::= () | bool | char | int | double | id 
Type_Decl       ::= type id = Var_Decl\* 
Function        ::= id (Var_Decl )\*= Block_Statement | nl? ident Block_Statement dedent
Block_Statement ::= (Statement nl)+
Statement       ::= if Expression Expression else Expression
                    |for id "in" Expression..Expression nl ident Block_Statement degent
                    |Assignment
                    |Call_Expression
Assignment      ::= id(.id)* = Expression
Expression      ::= '-'Expression
                    |'!'Expression
                    |Expression BinOp Expression
                    |Expression RelOp Expression
                    |Expression LogOp Expression
                    |Call_Expression(.id)*
                    |'('Expression')'
                    |intconst
                    |charconst
                    |stringconst
                    |floatconst
Call_Expression ::= id (Expression )*
BinOp           ::= + | - | * | / 
RelOp           ::= == | != | <= | < | >= | > |
LogOp           ::= && | || |

### Operator Associativities and Precedences
Precedence by descended order
    Operator        Associativity
    !, -(unary)     right to left
    *, /            left to right
    +, -            left to right
    <, <=, >, >=    left to right
    ==, !=          left to right
    &&              left to right
    ||              left to right
```
