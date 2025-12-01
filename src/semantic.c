#include "globals.h"
#include "symtab.h"
#include "parse.h"
#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 메모리 위치 카운터 (스코프별) */
static int location[100];

/* 현재 처리 중인 함수의 반환 타입 (Return 문 검사용) */
static ExpType currentFunctionType = Void;

/* 에러 출력 헬퍼 */
static void typeError(TreeNode* t, char* message)
{
    fprintf(listing, "Semantic Error at line %d: %s\n", t->lineno, message);
    Error = TRUE;
}

/* AST 순회 함수: preProc → 자식들 → postProc → sibling */
static void traverse(TreeNode* t,
    void (*preProc)(TreeNode*),
    void (*postProc)(TreeNode*))
{
    if (t != NULL)
    {
        if (preProc != NULL)
            preProc(t);

        for (int i = 0; i < MAXCHILDREN; i++)
            traverse(t->child[i], preProc, postProc);

        if (postProc != NULL)
            postProc(t);

        traverse(t->sibling, preProc, postProc);
    }
}

/* =================================================== */
/* Pre-Process: 심볼 테이블 삽입 및 스코프 진입        */
/* =================================================== */
static void insertNode(TreeNode* t)
{
    switch (t->nodekind)
    {
    case DeclK:
    {
        char* name = t->attr.name;
        ExpType type = t->type;

        /* 함수 선언 시: 반환 타입 추적 시작 */
        if (t->kind.decl == FunK) {
            currentFunctionType = t->type;
        }

        /* 현재 스코프에서 중복 선언인지 검사 */
        if (st_lookup_scope(name, currentScope) != NULL) {
            typeError(t, "Redeclaration of identifier in the same scope.");
            break;
        }

        /* 심볼 테이블 삽입 */
        int loc = location[currentScope]++;
        st_insert(name, type, loc, currentScope, t);

        /* FunK: 함수 몸체로 들어가기 전에 스코프 증가 */
        if (t->kind.decl == FunK) {
            currentScope++;
            location[currentScope] = 0;
        }
    }
    break;

    case StmtK:
        /* CmpdK: 복합 문장 '{ ... }' 진입 시 스코프 증가 */
        if (t->kind.stmt == CmpdK) {
            currentScope++;
            location[currentScope] = 0;
        }
        break;

    default:
        break;
    }
}

/* =================================================== */
/* Post-Process: 타입 검사 및 스코프 탈출              */
/* =================================================== */
static void checkNode(TreeNode* t)
{
    BucketListPtr l;

    /* 1. 타입 체크 */
    switch (t->nodekind)
    {
    case ExpK:
        switch (t->kind.exp)
        {
        case OpK:
            /* 자식 노드가 먼저 처리된 상태이므로 type 정보 있음 */
            if (t->attr.op == ASSIGN) {
                if (t->child[0]->kind.exp != IdK) {
                    typeError(t, "Assignment target must be a simple variable.");
                }
                else if (t->child[0]->type != t->child[1]->type) {
                    typeError(t, "Type mismatch in assignment.");
                }
                t->type = t->child[0]->type;
            }
            else if (t->attr.op >= LT && t->attr.op <= NEQ) { /* 비교 연산 */
                if (t->child[0]->type != Integer || t->child[1]->type != Integer)
                    typeError(t, "Operands must be 'int' for comparison.");
                t->type = Integer;
            }
            else { /* 산술 연산 (+, -, *, /) */
                if (t->child[0]->type != Integer || t->child[1]->type != Integer)
                    typeError(t, "Operands must be 'int' for arithmetic operation.");
                t->type = Integer;
            }
            break;

        case IdK:
            l = st_lookup(t->attr.name);
            if (l == NULL) {
                typeError(t, "Undeclared variable used.");
                t->type = Void;
            }
            else {
                t->type = l->type;
            }
            break;

        case CallK:
            /* 내장 함수 처리: input(), output()
             * - input()  : int 반환
             * - output() : void 반환
             */
            if (strcmp(t->attr.name, "input") == 0) {
                t->type = Integer;
                break;
            }
            if (strcmp(t->attr.name, "output") == 0) {
                t->type = Void;
                break;
            }

            /* 나머지 함수는 심볼테이블에서 찾는다 */
            l = st_lookup(t->attr.name);
            if (l == NULL) {
                typeError(t, "Undeclared function called.");
                t->type = Void;  /* 안전한 기본값 */
            }
            else {
                /* 함수인지 확인 */
                if (l->node->nodekind != DeclK || l->node->kind.decl != FunK) {
                    typeError(t, "Identifier is not a function.");
                    t->type = Void;
                }
                else {
                    /* 함수의 반환 타입을 호출 노드에 반영 */
                    t->type = l->type;
                }
            }
            break;

        case ConstK:
            t->type = Integer;
            break;

        default:
            break;
        }
        break;

    case StmtK:
        switch (t->kind.stmt)
        {
        case IfK:
        case WhileK:
            if (t->child[0] != NULL && t->child[0]->type != Integer) {
                typeError(t->child[0], "Condition must be 'int'.");
            }
            break;

        case ReturnK:
        {
            ExpType returnType =
                (t->child[0] == NULL) ? Void : t->child[0]->type;
            if ((currentFunctionType == Void && returnType != Void) ||
                (currentFunctionType == Integer && returnType != Integer)) {
                typeError(t, "Return type mismatch.");
            }
        }
        break;

        default:
            break;
        }
        break;

    default:
        break;
    }

    /* 2. 스코프 탈출 처리 */
    if (t->nodekind == StmtK && t->kind.stmt == CmpdK) {
        /* 복합 문장을 빠져나갈 때 로컬 변수 삭제 */
        st_delete_scope(currentScope);
        currentScope--;
    }
    else if (t->nodekind == DeclK && t->kind.decl == FunK) {
        /* 함수 정의가 끝날 때 파라미터/로컬 변수 삭제 */
        st_delete_scope(currentScope);
        currentScope--;
    }
}

/* 최종 의미 분석 메인 함수 */
void analyze(TreeNode* syntaxTree)
{
    currentScope = 0;
    location[0] = 0;

    if (TraceParse)
        fprintf(listing, "\nBuilding Symbol Table & Checking Types...\n");

    /* insertNode(Pre): 선언 등록, 스코프 진입
       checkNode(Post): 타입 검사, 스코프 탈출 */
    traverse(syntaxTree, insertNode, checkNode);

    if (!Error) {
        fprintf(listing, "\nSemantic analysis completed successfully.\n");
    }
}
