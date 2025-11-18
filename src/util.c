/****************************************************/
/* File: util.c                                     */
/* Utility function implementation for C- scanner   */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "parse.h"

static int indentno = 0;

#define INDENT indentno+=4
#define UNINDENT indentno-=4

static void printSpaces(void) {
    for (int i = 0; i < indentno; i++)
        fprintf(listing, " ");
}

void printTree(TreeNode* tree) {
    if (tree == NULL) return;

    INDENT;
    while (tree != NULL) {
        printSpaces();

        /* 노드 정보 출력 (과제 명세서 기반) */
        if (tree->nodekind == StmtK) {
            switch (tree->kind.stmt) {
            case IfK: fprintf(listing, "If\n"); break;
            case ReturnK: fprintf(listing, "Return\n"); break;
            case WhileK: fprintf(listing, "While\n"); break;
            case CmpdK: fprintf(listing, "Compound Stmt\n"); break;
            default: fprintf(listing, "Unknown StmtK\n"); break;
            }
        }
        else if (tree->nodekind == ExpK) {
            switch (tree->kind.exp) {
            case OpK:
                fprintf(listing, "Op: ");
                switch (tree->attr.op) {
                case PLUS: fprintf(listing, "+\n"); break;
                case MINUS: fprintf(listing, "-\n"); break;
                case TIMES: fprintf(listing, "*\n"); break;
                case OVER: fprintf(listing, "/\n"); break;
                case LT: fprintf(listing, "<\n"); break;
                case LTE: fprintf(listing, "<=\n"); break;
                case GT: fprintf(listing, ">\n"); break;
                case GTE: fprintf(listing, ">=\n"); break;
                case EQ: fprintf(listing, "==\n"); break;
                case NEQ: fprintf(listing, "!=\n"); break;
                case ASSIGN: fprintf(listing, "=\n"); break;
                default: fprintf(listing, "Unknown Op\n"); break;
                }
                break;
            case ConstK: fprintf(listing, "Const: %d\n", tree->attr.val); break;
            case IdK: fprintf(listing, "Id: %s\n", tree->attr.name); break;
            case CallK: fprintf(listing, "Call: %s\n", tree->attr.name); break;
            default: fprintf(listing, "Unknown ExpK\n"); break;
            }
        }
        else if (tree->nodekind == DeclK) {
            switch (tree->kind.decl) {
            case VarK: fprintf(listing, "Var Decl: %s\n", tree->attr.name); break;
            case FunK: fprintf(listing, "Func Decl: %s\n", tree->attr.name); break;
            case ParamK: fprintf(listing, "Param: %s\n", tree->attr.name); break;
            default: fprintf(listing, "Unknown DeclK\n"); break;
            }
        }
        else {
            fprintf(listing, "Unknown node kind\n");
        }

        /* 자식 노드들 처리 */
        for (int i = 0; i < MAXCHILDREN; i++) {
            printTree(tree->child[i]);
        }

        /* 형제 노드 처리 */
        tree = tree->sibling;
    }
    UNINDENT;
}

void printToken(TokenType token, const char *tokenString)
{
    switch (token) {
    case ELSE: 
    case IF: 
    case INT: 
    case RETURN: 
    case VOID: 
    case WHILE:
        fprintf(listing, "reserved word: %s\n", tokenString);
        break;

    case PLUS: fprintf(listing, "+\n"); break;
    case MINUS: fprintf(listing, "-\n"); break;
    case TIMES: fprintf(listing, "*\n"); break;
    case OVER: fprintf(listing, "/\n"); break;
    case LT: fprintf(listing, "<\n"); break;
    case LTE: fprintf(listing, "<=\n"); break;
    case GT: fprintf(listing, ">\n"); break;
    case GTE: fprintf(listing, ">=\n"); break;
    case EQ: fprintf(listing, "==\n"); break;
    case NEQ: fprintf(listing, "!=\n"); break;
    case ASSIGN: fprintf(listing, "=\n"); break;
    case SEMI: fprintf(listing, ";\n"); break;
    case COMMA: fprintf(listing, ",\n"); break;
    case LPAREN: fprintf(listing, "(\n"); break;
    case RPAREN: fprintf(listing, ")\n"); break;
    case LBRACE: fprintf(listing, "{\n"); break;
    case RBRACE: fprintf(listing, "}\n"); break;
    case LBRACKET: fprintf(listing, "[\n"); break;
    case RBRACKET: fprintf(listing, "]\n"); break;

    case NUM:
        fprintf(listing, "NUM, val = %s\n", tokenString);
        break;
    case ID:
        fprintf(listing, "ID, name = %s\n", tokenString);
        break;
    case ENDFILE:
        fprintf(listing, "EOF\n");
        break;
    case ERROR:
        fprintf(listing, "ERROR: %s\n", tokenString);
        break;
    default:
        fprintf(listing, "Unknown token: %d\n", token);
    }
}
char* copyString(char* s) {
    if (s == NULL) return NULL;
    int n = strlen(s) + 1;
    char* t = (char*)malloc(n);
    if (t == NULL)
        fprintf(listing, "Error: Out of memory at line %d\n", lineno);
    else
        strcpy(t, s);
    return t;
}
/* newStmtNode: 문장 노드 생성 */
TreeNode* newStmtNode(StmtKind kind) {
    TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
    if (t == NULL) {
        fprintf(listing, "Error: Out of memory at line %d\n", lineno);
    }
    else {
        for (int i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        t->lineno = lineno;
    }
    return t;
}

/* newExpNode: 표현식 노드 생성 */
TreeNode* newExpNode(ExpKind kind) {
    TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
    if (t == NULL) {
        fprintf(listing, "Error: Out of memory at line %d\n", lineno);
    }
    else {
        for (int i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = ExpK;
        t->kind.exp = kind;
        t->lineno = lineno;
        t->type = Void; /* 기본값 */
    }
    return t;
}

/* newDeclNode: 선언 노드 생성 */
TreeNode* newDeclNode(DeclKind kind) {
    TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
    if (t == NULL) {
        fprintf(listing, "Error: Out of memory at line %d\n", lineno);
    }
    else {
        for (int i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = DeclK;
        t->kind.decl = kind;
        t->lineno = lineno;
    }
    return t;
}/****************************************************/
/* File: util.c                                     */
/* Utility function implementation for C- scanner   */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "parse.h"

static int indentno = 0;

#define INDENT indentno+=4
#define UNINDENT indentno-=4

static void printSpaces(void) {
    for (int i = 0; i < indentno; i++)
        fprintf(listing, " ");
}

void printTree(TreeNode* tree) {
    if (tree == NULL) return;

    INDENT;
    while (tree != NULL) {
        printSpaces();

        /* 노드 정보 출력 (과제 명세서 기반) */
        if (tree->nodekind == StmtK) {
            switch (tree->kind.stmt) {
            case IfK: fprintf(listing, "If\n"); break;
            case ReturnK: fprintf(listing, "Return\n"); break;
            case WhileK: fprintf(listing, "While\n"); break;
            case CmpdK: fprintf(listing, "Compound Stmt\n"); break;
            default: fprintf(listing, "Unknown StmtK\n"); break;
            }
        }
        else if (tree->nodekind == ExpK) {
            switch (tree->kind.exp) {
            case OpK:
                fprintf(listing, "Op: ");
                switch (tree->attr.op) {
                case PLUS: fprintf(listing, "+\n"); break;
                case MINUS: fprintf(listing, "-\n"); break;
                case TIMES: fprintf(listing, "*\n"); break;
                case OVER: fprintf(listing, "/\n"); break;
                case LT: fprintf(listing, "<\n"); break;
                case LTE: fprintf(listing, "<=\n"); break;
                case GT: fprintf(listing, ">\n"); break;
                case GTE: fprintf(listing, ">=\n"); break;
                case EQ: fprintf(listing, "==\n"); break;
                case NEQ: fprintf(listing, "!=\n"); break;
                case ASSIGN: fprintf(listing, "=\n"); break;
                default: fprintf(listing, "Unknown Op\n"); break;
                }
                break;
            case ConstK: fprintf(listing, "Const: %d\n", tree->attr.val); break;
            case IdK: fprintf(listing, "Id: %s\n", tree->attr.name); break;
            case CallK: fprintf(listing, "Call: %s\n", tree->attr.name); break;
            default: fprintf(listing, "Unknown ExpK\n"); break;
            }
        }
        else if (tree->nodekind == DeclK) {
            switch (tree->kind.decl) {
            case VarK: fprintf(listing, "Var Decl: %s\n", tree->attr.name); break;
            case FunK: fprintf(listing, "Func Decl: %s\n", tree->attr.name); break;
            case ParamK: fprintf(listing, "Param: %s\n", tree->attr.name); break;
            default: fprintf(listing, "Unknown DeclK\n"); break;
            }
        }
        else {
            fprintf(listing, "Unknown node kind\n");
        }

        /* 자식 노드들 처리 */
        for (int i = 0; i < MAXCHILDREN; i++) {
            printTree(tree->child[i]);
        }

        /* 형제 노드 처리 */
        tree = tree->sibling;
    }
    UNINDENT;
}

void printToken(TokenType token, const char *tokenString)
{
    switch (token) {
    case ELSE: 
    case IF: 
    case INT: 
    case RETURN: 
    case VOID: 
    case WHILE:
        fprintf(listing, "reserved word: %s\n", tokenString);
        break;

    case PLUS: fprintf(listing, "+\n"); break;
    case MINUS: fprintf(listing, "-\n"); break;
    case TIMES: fprintf(listing, "*\n"); break;
    case OVER: fprintf(listing, "/\n"); break;
    case LT: fprintf(listing, "<\n"); break;
    case LTE: fprintf(listing, "<=\n"); break;
    case GT: fprintf(listing, ">\n"); break;
    case GTE: fprintf(listing, ">=\n"); break;
    case EQ: fprintf(listing, "==\n"); break;
    case NEQ: fprintf(listing, "!=\n"); break;
    case ASSIGN: fprintf(listing, "=\n"); break;
    case SEMI: fprintf(listing, ";\n"); break;
    case COMMA: fprintf(listing, ",\n"); break;
    case LPAREN: fprintf(listing, "(\n"); break;
    case RPAREN: fprintf(listing, ")\n"); break;
    case LBRACE: fprintf(listing, "{\n"); break;
    case RBRACE: fprintf(listing, "}\n"); break;
    case LBRACKET: fprintf(listing, "[\n"); break;
    case RBRACKET: fprintf(listing, "]\n"); break;

    case NUM:
        fprintf(listing, "NUM, val = %s\n", tokenString);
        break;
    case ID:
        fprintf(listing, "ID, name = %s\n", tokenString);
        break;
    case ENDFILE:
        fprintf(listing, "EOF\n");
        break;
    case ERROR:
        fprintf(listing, "ERROR: %s\n", tokenString);
        break;
    default:
        fprintf(listing, "Unknown token: %d\n", token);
    }
}
char* copyString(char* s) {
    if (s == NULL) return NULL;
    int n = strlen(s) + 1;
    char* t = (char*)malloc(n);
    if (t == NULL)
        fprintf(listing, "Error: Out of memory at line %d\n", lineno);
    else
        strcpy(t, s);
    return t;
}
/* newStmtNode: 문장 노드 생성 */
TreeNode* newStmtNode(StmtKind kind) {
    TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
    if (t == NULL) {
        fprintf(listing, "Error: Out of memory at line %d\n", lineno);
    }
    else {
        for (int i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        t->lineno = lineno;
    }
    return t;
}

/* newExpNode: 표현식 노드 생성 */
TreeNode* newExpNode(ExpKind kind) {
    TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
    if (t == NULL) {
        fprintf(listing, "Error: Out of memory at line %d\n", lineno);
    }
    else {
        for (int i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = ExpK;
        t->kind.exp = kind;
        t->lineno = lineno;
        t->type = Void; /* 기본값 */
    }
    return t;
}

/* newDeclNode: 선언 노드 생성 */
TreeNode* newDeclNode(DeclKind kind) {
    TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
    if (t == NULL) {
        fprintf(listing, "Error: Out of memory at line %d\n", lineno);
    }
    else {
        for (int i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = DeclK;
        t->kind.decl = kind;
        t->lineno = lineno;
    }
    return t;
}
