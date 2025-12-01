#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"
#include "parse.h" // TreeNode 정의를 포함

// 최대 심볼 테이블 크기 (해시 테이블 크기)
#define SIZE 211

// 심볼 테이블 항목 구조체 정의
typedef struct BucketList
{
    char *name;               // 식별자 이름
    ExpType type;             // 식별자의 타입 (Integer/Void)
    int memloc;               // 메모리 위치 (주소 할당 시 사용)
    int scope;                // 식별자가 선언된 범위 (0: Global, 1이상: Local)
    TreeNode *node;           // 이 심볼을 선언한 AST 노드 포인터 (추가 정보 저장용)
    struct BucketList *next;  // 해시 충돌 및 연결 리스트용
} * BucketListPtr;

// 심볼 테이블 (해시 테이블 배열)
extern BucketListPtr hashTable[SIZE];

// 현재 유효 범위 (Scope) 카운터
extern int currentScope;

// --- 함수 선언 ---
void st_insert(char *name, ExpType type, int loc, int scope, TreeNode *node);
BucketListPtr st_lookup(char *name);
BucketListPtr st_lookup_scope(char *name, int scope);
void st_delete_scope(int scope);
void initSymTab(); // 해시 테이블 초기화 함수 선언

#endif
