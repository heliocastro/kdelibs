%{

/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "global.h"
#include "kjs.h"
#include "nodes.h"
#include "lexer.h"

extern int yylex();
int yyerror (const char *);
bool automatic();

using namespace KJS;

%}

%union {
  int                 ival;
  double              dval;

  UString             *ustr;
  void                *rxp; /* TODO */
  Node                *node;
  StatementNode       *stat;
  ParameterNode       *param;
  FuncDeclNode        *func;
  ProgramNode         *prog;
  AssignExprNode      *init;
  SourceElementNode   *src;
  SourceElementsNode  *srcs;
  StatListNode        *slist;
  ArgumentsNode       *args;
  ArgumentListNode    *alist;
  VarDeclNode         *decl;
  VarDeclListNode     *vlist;
  CaseBlockNode       *cblk;
  ClauseListNode      *clist;
  CaseClauseNode      *ccl;
  ElementNode         *elm;
  ElisionNode         *eli;
  Operator            op;
}

%start Program

/* expect a shift/reduce conflict from the "dangling else" problem
   when using bison the warning can be supressed */
// %expect 1

/* literals */
%token NULLTOKEN TRUETOKEN FALSETOKEN
%token STRING DECIMAL INTEGER
%token REGEXP

/* keywords */
%token BREAK CASE DEFAULT FOR NEW VAR CONTINUE
%token FUNCTION RETURN VOID DELETE
%token IF THIS DO WHILE ELSE IN INSTANCEOF TYPEOF
%token SWITCH WITH RESERVED
%token THROW TRY CATCH FINALLY

/* for debugging purposes */
%token DEBUG

/* punctuators */
%token EQEQ NE                     /* == and != */
%token STREQ STRNEQ                /* === and !== */
%token LE GE                       /* < and > */
%token OR AND                      /* || and && */
%token PLUSPLUS MINUSMINUS         /* ++ and --  */
%token LSHIFT                      /* << */
%token RSHIFT URSHIFT              /* >> and >>> */
%token PLUSEQUAL MINUSEQUAL        /* += and -= */
%token MULTEQUAL DIVEQUAL          /* *= and /= */
%token LSHIFTEQUAL                 /* <<= */
%token RSHIFTEQUAL URSHIFTEQUAL    /* >>= and >>>= */
%token ANDEQUAL MODEQUAL           /* &= and %= */
%token XOREQUAL OREQUAL            /* ^= and |= */

/* terminal types */
%token <ival> INTEGER
%token <dval> DOUBLE
%token <ustr> STRING
%token <ustr> IDENT
%token <rxp>  REGEXP

/* automatically inserted semicolon */
%token AUTO

/* non-terminal types */
%type <node>  Literal PrimaryExpr Expr MemberExpr NewExpr CallExpr
%type <node>  ArrayLiteral
%type <node>  LeftHandSideExpr PostfixExpr UnaryExpr
%type <node>  MultiplicativeExpr AdditiveExpr
%type <node>  ShiftExpr RelationalExpr EqualityExpr
%type <node>  BitwiseANDExpr BitwiseXORExpr BitwiseORExpr
%type <node>  LogicalANDExpr LogicalORExpr
%type <node>  ConditionalExpr AssignmentExpr
%type <node>  ExprOpt
%type <node>  CallExpr
%type <node>  Catch Finally

%type <stat>  Statement Block
%type <stat>  VariableStatement EmptyStatement ExprStatement
%type <stat>  IfStatement IterationStatement ContinueStatement
%type <stat>  BreakStatement ReturnStatement WithStatement
%type <stat>  SwitchStatement LabelledStatement
%type <stat>  ThrowStatement TryStatement

%type <slist> StatementList
%type <init>  Initializer
%type <func>  FunctionDeclaration
%type <src>   SourceElement
%type <srcs>  SourceElements
%type <param> FormalParameterList
%type <op>    AssignmentOperator
%type <prog>  Program
%type <args>  Arguments
%type <alist> ArgumentList 
%type <vlist> VariableDeclarationList
%type <decl>  VariableDeclaration
%type <cblk>  CaseBlock
%type <ccl>   CaseClause DefaultClause
%type <clist> CaseClauses  CaseClausesOpt 
%type <eli>   Elision ElisionOpt
%type <elm>   ElementList

%%

Literal:
    NULLTOKEN                      { $$ = new NullNode(); }
  | TRUETOKEN                      { $$ = new BooleanNode(true); }
  | FALSETOKEN                     { $$ = new BooleanNode(false); }
  | INTEGER                        { $$ = new NumberNode($1); }
  | DOUBLE                         { $$ = new NumberNode($1); }
  | STRING                         { $$ = new StringNode($1); delete $1; }
  | REGEXP                         { $$ = new NullNode(); /* TODO */ }
;

PrimaryExpr:
    THIS                           { $$ = new ThisNode(); }
  | IDENT                          { $$ = new ResolveNode($1);
                                     delete $1; }
  | Literal
  | ArrayLiteral
  | '(' Expr ')'                   { $$ = new GroupNode($2); }
;

ArrayLiteral:
    '[' ElisionOpt ']'                 { $$ = new ArrayNode($2); }
  | '[' ElementList ']'                { $$ = new ArrayNode($2, 0L); }
  | '[' ElementList ',' ElisionOpt ']' { $$ = new ArrayNode($2, $4); }
;

ElementList:
    ElisionOpt AssignmentExpr      { $$ = new ElementNode($1, $2); }
  | ElementList ',' ElisionOpt AssignmentExpr
                                   { $$ = $1->append($3, $4); }
;

ElisionOpt:
    /* nothing */                  { $$ = 0L; }
  | Elision
;

Elision:
    ','                            { $$ = new ElisionNode(0L); }
  | Elision ','                    { $$ = new ElisionNode($1); }
;

MemberExpr:
    PrimaryExpr
  | MemberExpr '[' Expr ']'        { $$ = new AccessorNode1($1, $3); }
  | MemberExpr '.' IDENT           { $$ = new AccessorNode2($1, $3);
                                     delete $3; }
  | NEW MemberExpr Arguments       { $$ = new NewExprNode($2, $3); }
;

NewExpr:
    MemberExpr
  | NEW NewExpr                    { $$ = new NewExprNode($2); }
;

CallExpr:
    MemberExpr Arguments           { $$ = new FunctionCallNode($1, $2); }
  | CallExpr Arguments             { $$ = new FunctionCallNode($1, $2); }
  | CallExpr '[' Expr ']'          { $$ = new AccessorNode1($1, $3); }
  | CallExpr '.' IDENT             { $$ = new AccessorNode2($1, $3); }
;

Arguments:
    '(' ')'                        { $$ = new ArgumentsNode(0L); }
  | '(' ArgumentList ')'           { $$ = new ArgumentsNode($2); }
;

ArgumentList:
    AssignmentExpr                  { $$ = new ArgumentListNode($1); }
  | ArgumentList ',' AssignmentExpr { $$ = new ArgumentListNode($1, $3); }
;

LeftHandSideExpr:
    NewExpr
  | CallExpr
;

PostfixExpr:    /* TODO: no line terminator here */
    LeftHandSideExpr
  | LeftHandSideExpr PLUSPLUS      { $$ = new PostfixNode($1, OpPlusPlus); }
  | LeftHandSideExpr MINUSMINUS    { $$ = new PostfixNode($1, OpMinusMinus); }
;

UnaryExpr:
    PostfixExpr
  | DELETE UnaryExpr               { $$ = new DeleteNode($2); }
  | VOID UnaryExpr                 { $$ = new VoidNode($2); }
  | TYPEOF UnaryExpr               { $$ = new TypeOfNode($2); }
  | PLUSPLUS UnaryExpr             { $$ = new PrefixNode(OpPlusPlus, $2); }
  | AUTO PLUSPLUS UnaryExpr        { $$ = new PrefixNode(OpPlusPlus, $3); }
  | MINUSMINUS UnaryExpr           { $$ = new PrefixNode(OpMinusMinus, $2); }
  | AUTO MINUSMINUS UnaryExpr      { $$ = new PrefixNode(OpMinusMinus, $3); }
  | '+' UnaryExpr                  { $$ = new UnaryPlusNode($2); }
  | '-' UnaryExpr                  { $$ = new NegateNode($2); }
  | '~' UnaryExpr                  { $$ = new BitwiseNotNode($2); }
  | '!' UnaryExpr                  { $$ = new LogicalNotNode($2); }
;       

MultiplicativeExpr:
    UnaryExpr
  | MultiplicativeExpr '*' UnaryExpr { $$ = new MultNode($1, $3, '*'); }
  | MultiplicativeExpr '/' UnaryExpr { $$ = new MultNode($1, $3, '/'); }
  | MultiplicativeExpr '%' UnaryExpr { $$ = new MultNode($1,$3,'%'); }
;

AdditiveExpr:
    MultiplicativeExpr
  | AdditiveExpr '+' MultiplicativeExpr { $$ = new AddNode($1, $3, '+'); }
  | AdditiveExpr '-' MultiplicativeExpr { $$ = new AddNode($1, $3, '-'); }
;

ShiftExpr:
    AdditiveExpr
  | ShiftExpr LSHIFT AdditiveExpr  { $$ = new ShiftNode($1, OpLShift, $3); }
  | ShiftExpr RSHIFT AdditiveExpr  { $$ = new ShiftNode($1, OpRShift, $3); }
  | ShiftExpr URSHIFT AdditiveExpr { $$ = new ShiftNode($1, OpURShift, $3); }
;

RelationalExpr:
    ShiftExpr
  | RelationalExpr '<' ShiftExpr
                           { $$ = new RelationalNode($1, OpLess, $3); }
  | RelationalExpr '>' ShiftExpr
                           { $$ = new RelationalNode($1, OpGreater, $3); }
  | RelationalExpr LE ShiftExpr
                           { $$ = new RelationalNode($1, OpLessEq, $3); }
  | RelationalExpr GE ShiftExpr
                           { $$ = new RelationalNode($1, OpGreaterEq, $3); }
  | RelationalExpr INSTANCEOF ShiftExpr
                           { $$ = new RelationalNode($1, OpInstanceOf, $3); }
;

EqualityExpr:
    RelationalExpr
  | EqualityExpr EQEQ RelationalExpr   { $$ = new EqualNode($1, OpEqEq, $3); }
  | EqualityExpr NE RelationalExpr     { $$ = new EqualNode($1, OpNotEq, $3); }
  | EqualityExpr STREQ RelationalExpr  { $$ = new EqualNode($1, OpStrEq, $3); }
  | EqualityExpr STRNEQ RelationalExpr { $$ = new EqualNode($1, OpStrNEq, $3);}
;

BitwiseANDExpr:
    EqualityExpr
  | BitwiseANDExpr '&' EqualityExpr { $$ = new BitOperNode($1, OpBitAnd, $3); }
;

BitwiseXORExpr:
    BitwiseANDExpr
  | BitwiseXORExpr '^' EqualityExpr { $$ = new BitOperNode($1, OpBitXOr, $3); }
;

BitwiseORExpr:
    BitwiseXORExpr
  | BitwiseORExpr '|' EqualityExpr  { $$ = new BitOperNode($1, OpBitOr, $3); }
;

LogicalANDExpr:
    BitwiseORExpr
  | LogicalANDExpr AND BitwiseORExpr
                           { $$ = new BinaryLogicalNode($1, OpAnd, $3); }
;

LogicalORExpr:
    LogicalANDExpr
  | LogicalORExpr OR BitwiseANDExpr
                           { $$ = new BinaryLogicalNode($1, OpOr, $3); }
;

ConditionalExpr:
    LogicalORExpr
  | LogicalORExpr '?' AssignmentExpr ':' AssignmentExpr
                           { $$ = new ConditionalNode($1, $3, $5); }
;

AssignmentExpr:
    ConditionalExpr
  | LeftHandSideExpr AssignmentOperator AssignmentExpr
                           { $$ = new AssignNode($1, $2, $3);}
;

AssignmentOperator:
    '='                            { $$ = OpEqual; }
  | PLUSEQUAL                      { $$ = OpPlusEq; }
  | MINUSEQUAL                     { $$ = OpMinusEq; }
  | MULTEQUAL                      { $$ = OpMultEq; }
  | DIVEQUAL                       { $$ = OpDivEq; }
  | LSHIFTEQUAL                    { $$ = OpLShift; }
  | RSHIFTEQUAL                    { $$ = OpRShift; }
  | URSHIFTEQUAL                   { $$ = OpURShift; }
  | ANDEQUAL                       { $$ = OpAndEq; }
  | XOREQUAL                       { $$ = OpXOrEq; }
  | OREQUAL                        { $$ = OpOrEq; }
  | MODEQUAL                       { $$ = OpModEq; }
;

Expr:
    AssignmentExpr
  | Expr ',' AssignmentExpr        { $$ = new CommaNode($1, $3); }
;

Statement:
    Block
  | VariableStatement
  | EmptyStatement
  | ExprStatement
  | IfStatement
  | IterationStatement
  | ContinueStatement
  | BreakStatement
  | ReturnStatement
  | WithStatement
  | SwitchStatement
  | LabelledStatement
  | ThrowStatement
  | TryStatement
  | DEBUG '(' Expr ')'             { $$ = new DebugNode($3); }
;

Block:
    '{' '}'                        { $$ = new BlockNode(0L); }
  | '{' StatementList '}'          { $$ = new BlockNode($2); }
;

StatementList:
    Statement                      { $$ = new StatListNode($1); }
  | StatementList Statement        { $$ = new StatListNode($1, $2); }
;

VariableStatement:
    VAR VariableDeclarationList ';' { $$ = new VarStatementNode($2); }
  | VAR VariableDeclarationList error { if (automatic())
                                          $$ = new VarStatementNode($2);
                                        else
					  YYABORT; }
;

VariableDeclarationList:
    VariableDeclaration            { $$ = new VarDeclListNode($1); }
  | VariableDeclarationList ',' VariableDeclaration
                                   { $$ = new VarDeclListNode($1, $3); }
;

VariableDeclaration:
    IDENT                          { $$ = new VarDeclNode($1, 0); delete $1; }
  | IDENT Initializer              { $$ = new VarDeclNode($1, $2); delete $1; }
;

Initializer:
    '=' AssignmentExpr             { $$ = new AssignExprNode($2); }
;

EmptyStatement:
    ';'                            { $$ = new EmptyStatementNode(); }
;

ExprStatement:
    Expr ';'                       { $$ = new ExprStatementNode($1); }
  | Expr error                     { if (automatic())
                                       $$ = new ExprStatementNode($1);
                                     else
				       YYABORT; }
;

IfStatement: /* shift/reduce conflict due to dangling else */
    IF '(' Expr ')' Statement      { $$ = new IfNode($3, $5, 0L); }
  | IF '(' Expr ')' Statement ELSE Statement
                                   { $$ = new IfNode($3, $5, $7); }
;

IterationStatement:
    DO Statement WHILE '(' Expr ')' { $$ = new DoWhileNode($2, $5); }
  | WHILE '(' Expr ')' Statement   { $$ = new WhileNode($3, $5); }
  | FOR '(' ExprOpt ';' ExprOpt ';' ExprOpt ')'
            Statement              { $$ = new ForNode($3, $5, $7, $9); }
  | FOR '(' VAR VariableDeclarationList ';' ExprOpt ';' ExprOpt ')'
            Statement              { $$ = new ForNode($4, $6, $8, $10); }
  | FOR '(' LeftHandSideExpr IN Expr ')'
            Statement              { $$ = new ForInNode($3, $5, $7); }
  | FOR '(' VAR IDENT IN Expr ')'
            Statement              { $$ = new ForInNode($4, 0L, $6, $8);
                                     delete $4; }
  | FOR '(' VAR IDENT Initializer IN Expr ')'
            Statement              { $$ = new ForInNode($4, $5, $7, $9);
                                     delete $4; }
;

ExprOpt:
    /* nothing */                  { $$ = 0L; }
  | Expr
;

ContinueStatement:
    CONTINUE ';'                   { $$ = new ContinueNode(); }
  | CONTINUE IDENT ';'             { $$ = new ContinueNode($2); delete $2; }
;

BreakStatement:
    BREAK ';'                      { $$ = new BreakNode(); }
  | BREAK IDENT ';'                { $$ = new BreakNode($2); delete $2; }
;

ReturnStatement:
    RETURN ';'                     { $$ = new ReturnNode(0L); }
  | RETURN Expr ';'                { $$ = new ReturnNode($2); }
;

WithStatement:
    WITH '(' Expr ')' Statement    { $$ = new WithNode($3, $5); }
;

SwitchStatement:
    SWITCH '(' Expr ')' CaseBlock  { $$ = new SwitchNode($3, $5); }
;

CaseBlock:
    '{' CaseClausesOpt '}'         { $$ = new CaseBlockNode($2, 0L, 0L); }
  | '{' CaseClausesOpt DefaultClause CaseClausesOpt '}'
                                   { $$ = new CaseBlockNode($2, $3, $4); }
;

CaseClausesOpt:
    /* nothing */                  { $$ = 0L; }
  | CaseClauses
;

CaseClauses:
    CaseClause                     { $$ = new ClauseListNode($1); }
  | CaseClauses CaseClause         { $$ = $1->append($2); }
;

CaseClause:
    CASE Expr ':'                  { $$ = new CaseClauseNode($2, 0L); }
  | CASE Expr ':' StatementList    { $$ = new CaseClauseNode($2, $4); }
;

DefaultClause:
    DEFAULT ':'                    { $$ = new CaseClauseNode(0L, 0L);; }
  | DEFAULT ':' StatementList      { $$ = new CaseClauseNode(0L, $3); }
;

LabelledStatement:
    IDENT ':' Statement            { $$ = new LabelNode($1, $3); delete $1; }
;

ThrowStatement:
    THROW ';'                      { $$ = new ThrowNode(0L); }
  | THROW Expr ';'                 { $$ = new ThrowNode($2); }
;

TryStatement:
    TRY Block Catch                { $$ = new TryNode(); /* TODO */ } 
  | TRY Block Finally              { $$ = new TryNode(); /* TODO */ } 
  | TRY Block Catch Finally        { $$ = new TryNode(); /* TODO */ } 
;

Catch:
    CATCH '(' IDENT ')' Block      { $$ = new CatchNode(); /* TODO */ }
;

Finally:
    FINALLY Block                  { $$ = new FinallyNode($2); } 
;

FunctionDeclaration:
    FUNCTION IDENT '(' ')' Block   { $$ = new FuncDeclNode($2, 0L, $5);
                                     delete $2; }
  | FUNCTION IDENT '(' FormalParameterList ')' Block
                                   { $$ = new FuncDeclNode($2, $4, $6);
                                     delete $2; }
;

FormalParameterList:
    IDENT                          { $$ = new ParameterNode($1); delete $1; }
  | FormalParameterList ',' IDENT  { $$ = new ParameterNode($1, $3);
	                             delete $3; }
;

Program:
    SourceElements                 { $$ = new ProgramNode($1); }
;

SourceElements:
    SourceElement                  { $$ = new SourceElementsNode($1); }
  | SourceElements SourceElement   { $$ = new SourceElementsNode($1, $2); }
;

SourceElement:
    Statement                      { $$ = new SourceElementNode($1); }
  | FunctionDeclaration            { $$ = new SourceElementNode($1); }
;

%%

int yyerror (const char *)  /* Called by yyparse on error */
{
//  fprintf(stderr, "ERROR: %s at line %d\n",
//	  s, KJScript::lexer()->lineNo());
  return 1;
}

/* may we automatically insert a semicolon ? */
bool automatic()
{
  if (yychar == '}')
    return true;
  else if (KJScript::lexer()->prevTerminator())
    return true;

  return false;
}
