/****************************************************/
/* File: main.c                                     */
/* Main program for C- compiler                     */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
#include "symtab.h"   // 추가: 초기화 함수 사용
#include "semantic.h" // 추가: analyze 함수 사용

int lineno = 0;
FILE* source;
FILE* listing;
int EchoSource = TRUE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int Error = FALSE;

int main(int argc, char* argv[])
{
    TreeNode* syntaxTree;
    char pgm[120];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    strcpy(pgm, argv[1]);
    if (strchr(pgm, '.') == NULL)
        strcat(pgm, ".c-");

    source = fopen(pgm, "r");
    if (source == NULL) {
        fprintf(stderr, "File %s not found\n", pgm);
        exit(1);
    }

    listing = stdout;
    fprintf(listing, "\nC-Minus COMPILER START: %s\n", pgm);

    // 심볼 테이블 초기화
    initSymTab();

    // 1. 구문 분석 (Parsing)
    syntaxTree = parse();

    // 2. 의미 분석 (Semantic Analysis) - AST 생성 및 구문 오류가 없을 경우 실행
    if (syntaxTree != NULL && (Error == FALSE)) {
        analyze(syntaxTree);
    }

    // 3. AST 출력 (구문/의미 오류가 없을 경우)
    if (TraceParse && (Error == FALSE)) {
        fprintf(listing, "\nSyntax tree:\n");
        printTree(syntaxTree);
    }

    // 결과 메시지 출력
    if (Error) {
        fprintf(listing, "\nCompilation finished with errors.\n");
    }
    else {
        fprintf(listing, "\nCompilation finished successfully.\n");
    }

    fclose(source);
    return 0;
}
