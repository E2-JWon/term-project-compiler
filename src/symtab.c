#include "globals.h"
#include "symtab.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>

// 해시 테이블 초기화
BucketListPtr hashTable[SIZE];
// 전역 범위(0)에서 시작
int currentScope = 0; 

// 해시 테이블 초기화 함수 정의
void initSymTab() {
    for (int i = 0; i < SIZE; i++) {
        hashTable[i] = NULL;
    }
}

// 간단한 해시 함수: 문자열을 정수로 변환 후 SIZE로 나눈 나머지 사용
static int hash(char *key)
{
    int temp = 0;
    int i = 0;
    while (key[i] != '\0')
    {
        // 해시 함수 로직
        temp = ((temp << 2) + key[i]) % SIZE;
        i++;
    }
    return temp;
}

// 심볼 테이블에 항목 삽입
void st_insert(char *name, ExpType type, int loc, int scope, TreeNode *node)
{
    int h = hash(name);
    // 중복 검사는 semantic.c의 insertNode에서 담당
    
    BucketListPtr newBucket = (BucketListPtr)malloc(sizeof(struct BucketList));
    if (newBucket == NULL) {
        // 메모리 할당 실패 처리 (util.c의 lineno 대신 0 사용 또는 전역 Error 설정)
        // fprintf(listing, "Error: Out of memory in st_insert\n"); 
        return; 
    }
    
    newBucket->name = copyString(name); // util.c의 copyString 사용
    newBucket->type = type;
    newBucket->memloc = loc;
    newBucket->scope = scope;
    newBucket->node = node;
    
    // 리스트의 맨 앞에 삽입 (해시 충돌 처리)
    newBucket->next = hashTable[h]; 
    hashTable[h] = newBucket;
}

// 심볼 테이블에서 이름으로 항목 탐색 (가장 가까운 범위부터 찾음)
BucketListPtr st_lookup(char *name)
{
    int h = hash(name);
    BucketListPtr l = hashTable[h];
    // Global scope (scope 0)까지 탐색하며 가장 먼저 찾은 항목을 반환
    while (l != NULL)
    {
        if (strcmp(name, l->name) == 0)
            return l;
        l = l->next;
    }
    return NULL;
}

// 특정 범위 내에서만 항목 탐색 (주로 중복 선언 검사에 사용)
BucketListPtr st_lookup_scope(char *name, int scope)
{
    int h = hash(name);
    BucketListPtr l = hashTable[h];
    
    while (l != NULL)
    {
        // 이름과 범위가 모두 일치하는 항목을 찾음
        if (l->scope == scope && strcmp(name, l->name) == 0)
            return l;
        l = l->next;
    }
    return NULL;
}

// 특정 범위가 끝날 때 해당 범위의 모든 항목 삭제
void st_delete_scope(int scope)
{
    for (int h = 0; h < SIZE; h++)
    {
        BucketListPtr l = hashTable[h];
        BucketListPtr prev = NULL;

        while (l != NULL)
        {
            if (l->scope == scope)
            {
                // 현재 항목이 삭제할 범위인 경우
                BucketListPtr temp = l;
                if (prev == NULL) {
                    // 해시 테이블의 헤드를 삭제하는 경우
                    hashTable[h] = l->next;
                } 
                else {
                    // 리스트 중간 항목을 삭제하는 경우
                    prev->next = l->next;
                }
                
                l = l->next; // 다음 항목으로 이동
                free(temp->name);
                free(temp); 
            }
            else
            {
                // 현재 항목을 보존하고 다음으로 이동
                prev = l;
                l = l->next;
            }
        }
    }
}
