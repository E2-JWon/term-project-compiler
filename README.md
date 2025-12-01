✅ README.md

# 요구 사항 + 실행 방법 + 프로젝트 설명 README

## 📘 C-Minus Compiler (Scanner → Parser → Semantic Analyzer → Java Code Generator)

이 프로젝트는 C-Minus 언어로 작성된 프로그램을

1. 어휘 분석(Scanner)

2. 구문 분석(Parser, Recursive Descent)

3. 의미 분석(Semantic Analysis, Symbol Table 기반 타입 검사)

4. Java 코드 생성(Code Generation)

의 네 단계를 거쳐 Java 실행 프로그램으로 변환하는 컴파일러입니다.

출력된 Java 코드는 javac로 컴파일 후 실행할 수 있습니다.


## 📁 프로젝트 구조
src/

 ├── globals.h         # 공통 전역 정의
 
 ├── util.c / util.h   # 트리 출력, 문자열 복사 유틸리티
 
 ├── scan.c / scan.h   # 스캐너 (DFA 기반 토큰 인식)
 
 ├── parse.c / parse.h # 재귀 하강 파서 (AST 생성)
 
 ├── symtab.c / symtab.h # 심볼 테이블 (스코프 추적, 변수/함수 등록)
 
 ├── semantic.c / semantic.h # 의미 분석 (타입 검사)
 
 ├── cgen.c / cgen.h   # Java 코드 생성기
 
 ├── main.c            # 전체 컴파일 흐름 제어
 
 ├── test1.c           # 예제 1 (factorial)
 
└── test2.c           #  예제 2 (gcd)



### 이 저장소에는 자동 생성된 파일(.exe/.class/out.java/out.c) 은 포함하지 않음.
### 컴파일러가 다시 생성할 수 있기 때문입니다.

## 🛠️ 빌드 및 실행 방법

아래 명령은 Windows PowerShell 기준입니다.

## 📌 0. 소스 폴더 이동
```
cd src
```

## 📌 1. 컴파일러 빌드
```
gcc main.c scan.c parse.c util.c symtab.c semantic.c cgen.c -o compiler
```

빌드 성공 후:
```
compiler.exe
```

파일이 생성됩니다.

## 📌 2. C-Minus 프로그램을 Java 프로그램으로 변환

예제 프로그램(test1.c)을 컴파일:
```
.\compiler test1.c
```

그러면 출력:
```
Code generation finished. Output: test1_out.java

```
test1_out.java 파일이 생성됩니다.

## 📌 3. 생성된 Java 코드 컴파일
```
javac test1_out.java
```

성공하면:
```
test1_out.class
```

파일이 생깁니다.

## 📌 4. Java 실행
```
java test1_out
```
```
예) 입력

5


출력:

120
```
```
java test2_out
```
```
예) 입력
12
8

출력:
4
```
## 🎯 예제 프로그램

### 🔹 test1.c (factorial)
```
int fact(int x)
{
    if (x > 1)
        return x * fact(x - 1);
    else
        return 1;
}

void main(void)
{
    int x;
    x = input();
    if (x > 0)
        output(fact(x));
}
```
```
🔹 test2.c (gcd)
int gcd(int u, int v)
{
    if (v == 0)
        return u;
    else
        return gcd(v, u - u/v*v);
}

void main(void)
{
    int x; int y;
    x = input();
    y = input();
    output(gcd(x,y));
}
```
### 🚀 기능 요약

```
✔ DFA 기반 스캐너 구현
✔ 재귀 하강 구문 분석기
✔ AST(Abstract Syntax Tree) 생성
✔ 스코프 기반 Symbol Table
✔ 타입 검사 (정수 타입, return 타입, 비교 연산 등)
✔ Java 코드 생성기 (input/output 라이브러리 포함)
✔ fact(), gcd() 예제 변환 및 실행 가능
```
### 👥 팀 과제 체크리스트(보고서 요구 만족)
```
 어휘 분석기 작성

 구문 트리 노드 구조 설계

 구문 분석기 작성 및 AST 생성

 의미 분석 (변수/함수 선언 검사, 타입 검사)

 Java 코드 생성기 구현 ← 핵심 평가 항목

 예제 2개(fact, gcd) 코드 변환 및 실행

 전체 컴파일 파이프라인 완성
```
