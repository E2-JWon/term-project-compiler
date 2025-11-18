#ifndef _PARSE_H_
#define _PARSE_H_

#include "globals.h"

/* NodeKind: 문장(Stmt), 표현식(Exp), 선언(Decl) */
typedef enum { StmtK, ExpK, DeclK } NodeKind;

/* StmtKind: If, Return, While, Cmpd */
typedef enum { IfK, ReturnK, WhileK, CmpdK } StmtKind;

/* ExpKind: 연산(Op), 상수(Const), 변수(Id), 함수호출(Call) */
typedef enum { OpK, ConstK, IdK, CallK } ExpKind;

/* DeclKind: 변수(Var), 함수(Fun), 파라미터(Param) */
typedef enum { VarK, FunK, ParamK } DeclKind;

/* C-Minus는 int, void만 사용 (Type checking용) */
typedef enum { Void, Integer } ExpType;

#define MAXCHILDREN 3

typedef struct treeNode {
    struct treeNode* child[MAXCHILDREN];
    struct treeNode* sibling;
    int lineno;
    NodeKind nodekind;
    union {
        StmtKind stmt;
        ExpKind exp; 
        DeclKind decl;
    } kind;
    union {
        TokenType op;
        int val;
        char* name;
    } attr;
    ExpType type;
} TreeNode;

/* 파서의 시작 함수 */
TreeNode* parse(void);

#endif
