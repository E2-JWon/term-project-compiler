/****************************************************/
/* File: cgen.c                                     */
/* The code generator implementation (to Java)      */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "cgen.h"

/* 들여쓰기 관리 */
static int indent = 0;

/* 코드 생성 파일 포인터 */
static FILE* code;

/* 들여쓰기 함수 */
static void printIndent(void) {
    for (int i = 0; i < indent; i++) fprintf(code, "    "); /* 4 spaces */
}

/* 전방 선언 */
static void cGen(TreeNode* tree);
static void genDec(TreeNode* tree);
static void genStmt(TreeNode* tree);
static void genExp(TreeNode* tree);

/* 형제 노드 리스트 순회 (주로 선언 리스트용) */
static void genList(TreeNode* tree) {
    TreeNode* t = tree;
    while (t != NULL) {
        cGen(t);
        t = t->sibling;
    }
}

/* 메인 코드 생성 디스패처 */
static void cGen(TreeNode* tree) {
    if (tree == NULL) return;
    switch (tree->nodekind) {
    case StmtK: genStmt(tree); break;
    case ExpK:  genExp(tree);  break;
    case DeclK: genDec(tree);  break;
    default: break;
    }
}

/* 선언부 생성 (변수, 함수, 파라미터) */
static void genDec(TreeNode* tree) {
    switch (tree->kind.decl) {

    case VarK:
        printIndent();
        if (tree->type == Integer) fprintf(code, "int ");
        else                       fprintf(code, "void ");
        fprintf(code, "%s;\n", tree->attr.name);
        break;

    case FunK: {
        fprintf(code, "\n");

        /* main 함수 특별 처리: public static void main(String[] args) */
        if (strcmp(tree->attr.name, "main") == 0) {
            printIndent();
            fprintf(code, "public static void main(String[] args)\n");
        }
        else {
            printIndent();
            if (tree->type == Integer) fprintf(code, "static int ");
            else                       fprintf(code, "static void ");
            fprintf(code, "%s(", tree->attr.name);

            /* 파라미터 리스트 처리 (child[0]) */
            TreeNode* p = tree->child[0];
            if (p != NULL) {
                while (p != NULL) {
                    if (p->type == Integer) fprintf(code, "int ");
                    else                     fprintf(code, "void ");
                    fprintf(code, "%s", p->attr.name);
                    if (p->sibling != NULL) fprintf(code, ", ");
                    p = p->sibling;
                }
            }
            fprintf(code, ")\n");
        }

        /* 함수 본문 (child[1]) */
        cGen(tree->child[1]);
        break;
    }

    case ParamK:
        /* FunK에서 직접 처리하므로 여기서는 사용 안 함 */
        break;
    }
}

/* 문장 생성 */
static void genStmt(TreeNode* tree) {
    TreeNode *p1, *p2, *p3;

    switch (tree->kind.stmt) {

    case IfK:
        printIndent();
        fprintf(code, "if (");
        p1 = tree->child[0];
        cGen(p1);
        fprintf(code, ")\n");

        /* then-part */
        p2 = tree->child[1];
        if (p2->nodekind == ExpK) {
            printIndent();
            genExp(p2);
            fprintf(code, ";\n");
        } else {
            cGen(p2);
        }

        /* else-part */
        p3 = tree->child[2];
        if (p3 != NULL) {
            printIndent();
            fprintf(code, "else\n");
            if (p3->nodekind == ExpK) {
                printIndent();
                genExp(p3);
                fprintf(code, ";\n");
            } else {
                cGen(p3);
            }
        }
        break;

    case WhileK:
        printIndent();
        fprintf(code, "while (");
        p1 = tree->child[0];
        cGen(p1);
        fprintf(code, ")\n");

        p2 = tree->child[1];
        if (p2->nodekind == ExpK) {
            printIndent();
            genExp(p2);
            fprintf(code, ";\n");
        } else {
            cGen(p2);
        }
        break;

    case ReturnK:
        printIndent();
        fprintf(code, "return");
        p1 = tree->child[0];
        if (p1 != NULL) {
            fprintf(code, " ");
            cGen(p1);
        }
        fprintf(code, ";\n");
        break;

    case CmpdK:
        printIndent();
        fprintf(code, "{\n");
        indent++;

        /* 지역 변수 선언 (child[0]) */
        p1 = tree->child[0];
        genList(p1);

        /* 문장 리스트 (child[1]) */
        p2 = tree->child[1];
        while (p2 != NULL) {
            if (p2->nodekind == ExpK) {
                printIndent();
                genExp(p2);
                fprintf(code, ";\n");
            } else {
                cGen(p2);
            }
            p2 = p2->sibling;
        }

        indent--;
        printIndent();
        fprintf(code, "}\n");
        break;
    }
}

/* 표현식 생성 */
static void genExp(TreeNode* tree) {
    TreeNode *p1, *p2;

    switch (tree->kind.exp) {

    case OpK:
        /* 대입 연산은 괄호 없이 x = input() 형태로 출력 */
        if (tree->attr.op == ASSIGN) {
            p1 = tree->child[0];
            p2 = tree->child[1];
            cGen(p1);
            fprintf(code, " = ");
            cGen(p2);
        } else {
            /* 나머지 연산은 (a + b) 처럼 괄호로 감싸기 */
            fprintf(code, "(");
            p1 = tree->child[0];
            cGen(p1);

            switch (tree->attr.op) {
            case PLUS:   fprintf(code, " + ");  break;
            case MINUS:  fprintf(code, " - ");  break;
            case TIMES:  fprintf(code, " * ");  break;
            case OVER:   fprintf(code, " / ");  break;
            case LT:     fprintf(code, " < ");  break;
            case LTE:    fprintf(code, " <= "); break;
            case GT:     fprintf(code, " > ");  break;
            case GTE:    fprintf(code, " >= "); break;
            case EQ:     fprintf(code, " == "); break;
            case NEQ:    fprintf(code, " != "); break;
            default: break;
            }

            p2 = tree->child[1];
            cGen(p2);
            fprintf(code, ")");
        }
        break;

    case ConstK:
        fprintf(code, "%d", tree->attr.val);
        break;

    case IdK:
        fprintf(code, "%s", tree->attr.name);
        break;

    case CallK:
        fprintf(code, "%s(", tree->attr.name);
        p1 = tree->child[0];
        while (p1 != NULL) {
            cGen(p1);
            if (p1->sibling != NULL) fprintf(code, ", ");
            p1 = p1->sibling;
        }
        fprintf(code, ")");
        break;
    }
}

/* 코드 생성 메인 함수: Java 코드 생성 */
void codeGen(TreeNode* syntaxTree, char* codefile) {
    /* codefile: main에서 넘긴 출력 파일 이름 (예: test1_out.java) */
    char* s = (char*)malloc(strlen(codefile) + 1);
    if (s == NULL) {
        fprintf(listing, "Out of memory for code file name\n");
        return;
    }
    strcpy(s, codefile);

    /* class 이름은 파일 이름에서 확장자(.java) 제거한 것 */
    char className[128];
    size_t len;
    char* dot = strrchr(s, '.');
    if (dot != NULL && dot > s) {
        len = (size_t)(dot - s);
    } else {
        len = strlen(s);
    }
    if (len >= sizeof(className)) len = sizeof(className) - 1;
    memcpy(className, s, len);
    className[len] = '\0';

    code = fopen(s, "w");
    if (code == NULL) {
        fprintf(listing, "Unable to open %s for writing\n", s);
        free(s);
        return;
    }

    /* 1. Java preamble + 클래스 선언 */
    fprintf(code, "/* C-Minus Compilation to Java */\n");
    fprintf(code, "/* Program: %s */\n", s);
    fprintf(code, "import java.util.*;\n\n");

    fprintf(code, "public class %s {\n", className);
    indent = 1;

    /* 2. 런타임 라이브러리 (input / output) */
    printIndent();
    fprintf(code, "static Scanner sc = new Scanner(System.in);\n\n");

    printIndent();
    fprintf(code, "static int input() {\n");
    indent++;
    printIndent();
    fprintf(code, "System.out.print(\"input: \");\n");
    printIndent();
    fprintf(code, "return sc.nextInt();\n");
    indent--;
    printIndent();
    fprintf(code, "}\n\n");

    printIndent();
    fprintf(code, "static void output(int x) {\n");
    indent++;
    printIndent();
    fprintf(code, "System.out.println(x);\n");
    indent--;
    printIndent();
    fprintf(code, "}\n");

    /* 3. AST 순회하며 코드 생성 (프로그램 = declaration list) */
    fprintf(code, "\n    /* Generated code */\n");
    genList(syntaxTree);

    /* 4. 클래스 닫기 */
    indent = 0;
    fprintf(code, "}\n");

    fclose(code);
    fprintf(listing, "Code generation finished. Output: %s\n", s);
    free(s);
}
