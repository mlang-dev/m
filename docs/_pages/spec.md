---
layout: page
title: spec
permalink: /spec/
description: 
nav: true
---
# m language specification

## Extended BNF Notation
The m language grammar is given in this specification using a simple Extended Backus-Naur Form (EBNF) from [W3C EBNF](https://www.w3.org/TR/REC-xml/#sec-notation).

### Lexical Rules
letter          ::= [a-zA-Z]<br/>
character       ::= any printable ASCII character<br/>
digit           ::= [0-9]<br/>
id              ::= _\*(letter)+(digit)\*<br/>
sign            ::= "+" | "-"
point           ::= "."
exponent-marker ::= "e" | "E" | "s" | "S" | "d" | "D" | "f" | "F" | "l" | "L"
exponent        ::= exponent-marker sign? (digit)+
floatconst      ::= sign? (digit)* point (digit)+ exponent
intconst        ::= (digit)\*<br/>
charconst       ::= 'character' | '\n' | '\0'<br/>
stringconst     ::= "character*"<br/>
comments        ::= #(character)*<br/>
nl              ::= '\n' | '\r\n'
indent          ::= indentation marker: increase to a new indentation level
dedent          ::= dedentation marker: decrease to a old indentation level

### Syntax Rules
Program         ::= (Declare | Function)\*<br/>
Declare         ::= Type_Decl | Var_Decl<br/>
Var_Decl        ::= id(:Type)?<br/>
Type            ::= () | bool | char | int | double | id <br/>
Type_Decl       ::= type id = Var_Decl\* <br/>
Function        ::= id (Var_Decl )\*= Block_Statement | nl? ident Block_Statement dedent<br/>
Block_Statement ::= (Statement nl)+<br/>
Statement       ::= if Expression Expression else Expression<br/>
                    |for id "in" Expression..Expression nl ident Block_Statement degent<br/>
                    |Assignment<br/>
                    |Call_Expression
Assignment      ::= id(.id)* = Expression<br/>
Expression      ::= '-'Expression<br/>
                    |'!'Expression<br/>
                    |Expression BinOp Expression<br/>
                    |Expression RelOp Expression<br/>
                    |Expression LogOp Expression<br/>
                    |Call_Expression(.id)*<br/>
                    |'('Expression')'<br/>
                    |intconst<br/>
                    |charconst<br/>
                    |stringconst<br/>
                    |floatconst<br/>
Call_Expression ::= id (Expression )*<br/>
BinOp           ::= + | - | * | / <br/>
RelOp           ::= == | != | <= | < | >= | > |<br/>
LogOp           ::= && | || |<br/>

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

