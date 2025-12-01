#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "globals.h"
#include "parse.h"

// AST를 두 단계로 순회하여 의미 분석을 수행하는 메인 함수
void analyze(TreeNode *syntaxTree);

#endif
