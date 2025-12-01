/****************************************************/
/* File: main.c                                     */
/* Main program for C- compiler (Java Code Gen)     */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
#include "symtab.h"
#include "semantic.h"
#include "cgen.h"

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

    /* 프로그램 실행 인자 체크 */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    /* 소스 파일 이름 처리 */
    strcpy(pgm, argv[1]);
    if (strchr(pgm, '.') == NULL)
        strcat(pgm, ".c");

    /* 소스 파일 열기 */
    source = fopen(pgm, "r");
    if (source == NULL) {
        fprintf(stderr, "File %s not found\n", pgm);
        exit(1);
    }

    listing = stdout;
    fprintf(listing, "\nC-Minus COMPILER START: %s\n", pgm);

    /* 심볼 테이블 초기화 */
    initSymTab();

    /* 1. 구문 분석 (Parsing) */
    syntaxTree = parse();

    /* 2. 의미 분석 (Semantic Analysis) */
    if (syntaxTree != NULL && (Error == FALSE)) {
        analyze(syntaxTree);
    }

    /* 3. AST 출력 */
    if (TraceParse && (Error == FALSE)) {
        fprintf(listing, "\nSyntax tree:\n");
        printTree(syntaxTree);
    }

    /* 4. 코드 생성 (오류 없는 경우 실행) */
    if (Error == FALSE) {
        char codefile[130];

        /* 출력 파일명 생성: example.c → example_out.java */
        char* dot = strrchr(argv[1], '.');
        if (dot != NULL) {
            size_t len = dot - argv[1];
            strncpy(codefile, argv[1], len);
            codefile[len] = '\0';
        }
        else {
            strcpy(codefile, argv[1]);
        }

        strcat(codefile, "_out.java");  // ← JAVA 파일 생성

        /* 코드 생성기 실행 */
        codeGen(syntaxTree, codefile);
    }

    /* 결과 메시지 */
    if (Error)
        fprintf(listing, "\nCompilation finished with errors.\n");
    else
        fprintf(listing, "\nCompilation finished successfully.\n");

    fclose(source);
    return 0;
}
