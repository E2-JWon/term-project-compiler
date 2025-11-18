/****************************************************/
/* File: util.h                                     */
/* Utility functions for the C- scanner             */
/****************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

#include "parse.h"
#include "globals.h"


void printToken(TokenType token, const char *tokenString);
void printTree(TreeNode * tree);

TreeNode* newStmtNode(StmtKind kind);
TreeNode* newExpNode(ExpKind kind);
TreeNode* newDeclNode(DeclKind kind);

/* 토큰 문자열 복사를 위한 헬퍼 함수 */
char* copyString(char* s);
#endif

