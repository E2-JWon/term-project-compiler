/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the C- compiler    */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* 현재 토큰을 저장 */

/* -------------------------------------------------- */
/* 모든 파싱 함수에 대한 선언(forward declaration) */
/* -------------------------------------------------- */
static TreeNode* parse_program(void);
static TreeNode* parse_declaration_list(void);
static TreeNode* parse_declaration(void);
static TreeNode* parse_var_declaration(void);
// static TreeNode * parse_fun_declaration(void); // parse_declaration에 통합됨
static TreeNode* parse_params(void);
static TreeNode* parse_param_list(void);
static TreeNode* parse_param(void);
static TreeNode* parse_compound_stmt(void);
static TreeNode* parse_local_declarations(void);
static TreeNode* parse_statement_list(void);
static TreeNode* parse_statement(void);
static TreeNode* parse_expression_stmt(void);
static TreeNode* parse_selection_stmt(void);
static TreeNode* parse_iteration_stmt(void);
static TreeNode* parse_return_stmt(void);
static TreeNode* parse_expression(void);
// static TreeNode * parse_var(void); // parse_factor에 통합됨
static TreeNode* parse_simple_expression(void);
static TreeNode* parse_additive_expression(void);
static TreeNode* parse_term(void);
static TreeNode* parse_factor(void);
// static TreeNode * parse_call(void); // parse_factor에 통합됨
static TreeNode* parse_args(void);
static TreeNode* parse_arg_list(void);


/* * 헬퍼 함수: 토큰이 일치하는지 확인하고 다음 토큰을 가져옴
 */
static void match(TokenType expected) {
    if (token == expected) {
        token = getToken();
    }
    else {
        // syntax error 처리 
        fprintf(listing, "Syntax error: unexpected token -> ");
        printToken(token, tokenString);
        Error = TRUE; // 에러 플래그 설정
    }
}

/*
 * program -> declaration-list
 */
static TreeNode* parse_program(void) {
    return parse_declaration_list();
}

/*
 * declaration-list -> declaration { declaration }
 * (문법의 좌순환을 루프로 변경)
 */
static TreeNode* parse_declaration_list(void) {
    TreeNode* t = parse_declaration();
    TreeNode* p = t;

    // 다음 토큰이 int나 void이면 (선언의 시작) 루프 계속
    while (token == INT || token == VOID) {
        TreeNode* q = parse_declaration();
        if (q != NULL) {
            if (t == NULL) {
                t = p = q;
            }
            else {
                p->sibling = q; // 형제 노드로 연결
                p = q;          // 리스트의 꼬리(p) 이동
            }
        }
    }
    return t; // 리스트의 헤드(t) 반환
}

/*
 * declaration -> var-declaration | fun-declaration
 * var-declaration -> type-specifier ID ;  (배열 제외 )
 * fun-declaration -> type-specifier ID ( params ) compound-stmt
 */
static TreeNode* parse_declaration(void) {
    TreeNode* t = NULL;
    ExpType type;
    char* name;

    // 1. type-specifier 파싱
    if (token == INT) type = Integer;
    else if (token == VOID) type = Void;
    else return NULL; // 선언이 아님
    match(token);

    // 2. ID 파싱
    if (token != ID) {
        fprintf(listing, "Syntax error: ID expected\n");
        return NULL;
    }
    name = copyString(tokenString); // ID 이름 복사
    match(ID);

    // 3. '(' 이면 함수 선언, 아니면 변수 선언
    if (token == LPAREN) { // fun-declaration
        t = newDeclNode(FunK);
        t->attr.name = name;
        t->type = type;
        match(LPAREN);
        t->child[0] = parse_params();
        match(RPAREN);
        t->child[1] = parse_compound_stmt();
    }
    else if (token == SEMI) { // var-declaration
        t = newDeclNode(VarK);
        t->attr.name = name;
        t->type = type;
        match(SEMI);
    }
    // (배열 [ ] 관련 코드는 에 따라 제외)
    else {
        fprintf(listing, "Syntax error: unexpected token -> ");
        printToken(token, tokenString);
    }

    return t;
}

/*
 * var-declaration -> type-specifier ID ;
 * (지역 변수 선언 시 호출됨)
 */
static TreeNode* parse_var_declaration(void) {
    TreeNode* t = newDeclNode(VarK);

    if (token == INT) t->type = Integer;
    else if (token == VOID) t->type = Void;
    else return NULL;
    match(token);

    if (token == ID) {
        t->attr.name = copyString(tokenString);
        match(ID);
    }
    else {
        fprintf(listing, "Syntax error: ID expected\n");
        return NULL;
    }

    match(SEMI);
    return t;
}


/*
 * params -> param-list | void
 * (가정: param-list는 INT로, void는 VOID로 시작 [cite: 15, 22, 27])
 */
static TreeNode* parse_params(void) {
    if (token == VOID) {
        match(VOID);
        return NULL; // (void) - 파라미터 없음
    }
    else { // INT로 시작하는 param-list라고 가정
        return parse_param_list();
    }
}

/*
 * param-list -> param { , param }
 * (문법의 좌순환을 루프로 변경)
 */
static TreeNode* parse_param_list(void) {
    TreeNode* t = parse_param();
    TreeNode* p = t;
    while (token == COMMA) {
        match(COMMA);
        TreeNode* q = parse_param();
        if (q != NULL) {
            p->sibling = q;
            p = q;
        }
    }
    return t;
}

/*
 * param -> type-specifier ID
 * (가정: type-specifier는 INT [cite: 15, 22])
 */
static TreeNode* parse_param(void) {
    TreeNode* t = newDeclNode(ParamK);

    // (void x) 형태를 지원하지 않는다고 가정
    if (token == INT) {
        t->type = Integer;
        match(INT);
    }
    else {
        fprintf(listing, "Syntax error: 'int' type expected for param\n");
    }

    if (token == ID) {
        t->attr.name = copyString(tokenString);
        match(ID);
    }
    else {
        fprintf(listing, "Syntax error: ID expected for param\n");
    }

    // (배열 [ ] 관련 코드는 에 따라 제외)
    return t;
}

/*
 * compound-stmt -> { local-declarations statement-list }
 */
static TreeNode* parse_compound_stmt(void) {
    TreeNode* t = newStmtNode(CmpdK);
    match(LBRACE);
    t->child[0] = parse_local_declarations();
    t->child[1] = parse_statement_list();
    match(RBRACE);
    return t;
}

/*
 * local-declarations -> { var-declaration }
 * (문법의 'empty'를 루프 0번으로 처리)
 */
static TreeNode* parse_local_declarations(void) {
    TreeNode* t = NULL;
    TreeNode* p = NULL;
    // 지역 변수 선언은 'int' 또는 'void'로 시작
    while (token == INT || token == VOID) {
        TreeNode* q = parse_var_declaration();
        if (q != NULL) {
            if (t == NULL) t = p = q;
            else {
                p->sibling = q;
                p = q;
            }
        }
    }
    return t;
}

/*
 * statement-list -> { statement }
 * (문법의 'empty'를 루프 0번으로 처리)
 */
static TreeNode* parse_statement_list(void) {
    TreeNode* t = NULL;
    TreeNode* p = NULL;
    // '}'가 나오기 전까지 statement 파싱
    while (token != RBRACE) {
        TreeNode* q = parse_statement();
        if (q != NULL) {
            if (t == NULL) t = p = q;
            else {
                p->sibling = q;
                p = q;
            }
        }
        else {
            // 에러 발생 시 무한 루프 방지
            break;
        }
    }
    return t;
}

/*
 * statement -> expression-stmt | compound-stmt | selection-stmt
 * | iteration-stmt | return-stmt
 */
static TreeNode* parse_statement(void) {
    TreeNode* t = NULL;
    switch (token) {
    case IF:
        t = parse_selection_stmt();
        break;
    case WHILE:
        t = parse_iteration_stmt();
        break;
    case RETURN:
        t = parse_return_stmt();
        break;
    case LBRACE: // {
        t = parse_compound_stmt();
        break;
    case ID:
    case NUM:
    case LPAREN: // (
    case SEMI:   // ;
        t = parse_expression_stmt();
        break;
    default:
        fprintf(listing, "Syntax error: unexpected token -> ");
        printToken(token, tokenString);
        token = getToken(); // 에러 복구 (토큰 하나 버림)
        break;
    }
    return t;
}

/*
 * expression-stmt -> expression ; | ;
 */
static TreeNode* parse_expression_stmt(void) {
    TreeNode* t = NULL;
    if (token == SEMI) {
        match(SEMI); // 비어있는 문장 ';'
        return NULL;
    }
    else {
        t = parse_expression();
        match(SEMI);
    }
    return t;
}

/*
 * selection-stmt -> if ( expression ) statement
 * | if ( expression ) statement else statement
 */
static TreeNode* parse_selection_stmt(void) {
    TreeNode* t = newStmtNode(IfK);
    match(IF);
    match(LPAREN);
    t->child[0] = parse_expression();
    match(RPAREN);
    t->child[1] = parse_statement(); // 'then'
    if (token == ELSE) {
        match(ELSE);
        t->child[2] = parse_statement(); // 'else'
    }
    return t;
}

/*
 * iteration-stmt -> while ( expression ) statement
 */
static TreeNode* parse_iteration_stmt(void) {
    TreeNode* t = newStmtNode(WhileK);
    match(WHILE);
    match(LPAREN);
    t->child[0] = parse_expression();
    match(RPAREN);
    t->child[1] = parse_statement();
    return t;
}

/*
 * return-stmt -> return ; | return expression ;
 */
static TreeNode* parse_return_stmt(void) {
    TreeNode* t = newStmtNode(ReturnK);
    match(RETURN);
    if (token == SEMI) {
        match(SEMI);
        // t->child[0]은 NULL
    }
    else {
        t->child[0] = parse_expression();
        match(SEMI);
    }
    return t;
}

/*
 * expression -> var = expression | simple-expression
 * (var -> ID )
 * (ID = ... 인지 ID ( ... ) 인지 ID + ... 인지 구분)
 */
static TreeNode* parse_expression(void) {
    TreeNode* t = parse_simple_expression();

    // '=' (ASSIGN)는 우선순위가 가장 낮음
    if (token == ASSIGN) {
        // 좌변(L-value)이 ID(변수)인지 확인
        if (t != NULL && t->nodekind == ExpK && t->kind.exp == IdK) {
            TreeNode* p = newExpNode(OpK);
            p->attr.op = ASSIGN;
            p->child[0] = t; // 좌변
            match(ASSIGN);
            p->child[1] = parse_expression(); // 우변
            return p;
        }
        else {
            fprintf(listing, "Syntax error: assignment to non-variable\n");
        }
    }
    return t; // 할당문이 아니면 simple-expression 반환
}

/*
 * simple-expression -> additive-expression
 * | additive-expression relop additive-expression
 */
static TreeNode* parse_simple_expression(void) {
    TreeNode* t = parse_additive_expression();
    if (token == LT || token == LTE || token == GT || token == GTE || token == EQ || token == NEQ) {
        TreeNode* p = newExpNode(OpK);
        p->attr.op = token; // <, <=, >, >=, ==, !=
        p->child[0] = t;
        match(token);
        p->child[1] = parse_additive_expression();
        return p;
    }
    return t;
}

/*
 * additive-expression -> term { addop term }
 * (좌순환을 루프로 변경, 좌결합)
 */
static TreeNode* parse_additive_expression(void) {
    TreeNode* t = parse_term(); // 첫 번째 term
    while (token == PLUS || token == MINUS) {
        TreeNode* p = newExpNode(OpK);
        p->attr.op = token; // + 또는 -
        p->child[0] = t; // t는 이전까지의 누적 결과
        match(token);
        p->child[1] = parse_term();
        t = p; // t를 새 연산 노드로 업데이트 (좌결합)
    }
    return t;
}

/*
 * term -> factor { mulop factor }
 * (좌순환을 루프로 변경, 좌결합)
 */
static TreeNode* parse_term(void) {
    TreeNode* t = parse_factor(); // 첫 번째 factor
    while (token == TIMES || token == OVER) {
        TreeNode* p = newExpNode(OpK);
        p->attr.op = token; // * 또는 /
        p->child[0] = t;
        match(token);
        p->child[1] = parse_factor();
        t = p; // t를 새 연산 노드로 업데이트 (좌결합)
    }
    return t;
}

/*
 * factor -> ( expression ) | var | call | NUM
 * var -> ID  (배열 제외 )
 * call -> ID ( args )
 */
static TreeNode* parse_factor(void) {
    TreeNode* t = NULL;
    switch (token) {
    case NUM:
        t = newExpNode(ConstK);
        t->attr.val = atoi(tokenString);
        match(NUM);
        break;
    case ID:
    { // 새 변수 선언을 위한 블록
        char* name = copyString(tokenString);
        match(ID);
        if (token == LPAREN) { // call
            t = newExpNode(CallK);
            t->attr.name = name;
            match(LPAREN);
            t->child[0] = parse_args();
            match(RPAREN);
        }
        else { // var
            t = newExpNode(IdK);
            t->attr.name = name;
        }
    }
    break;
    case LPAREN:
        match(LPAREN);
        t = parse_expression();
        match(RPAREN);
        break;
    default:
        fprintf(listing, "Syntax error: unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    }
    return t;
}

/*
 * args -> arg-list | empty
 */
static TreeNode* parse_args(void) {
    // ')'가 바로 나오면 'empty'
    if (token == RPAREN) {
        return NULL; // 인수 없음
    }
    else {
        return parse_arg_list();
    }
}

/*
 * arg-list -> expression { , expression }
 * (좌순환을 루프로 변경)
 */
static TreeNode* parse_arg_list(void) {
    TreeNode* t = parse_expression();
    TreeNode* p = t;
    while (token == COMMA) {
        match(COMMA);
        TreeNode* q = parse_expression();
        if (q != NULL) {
            p->sibling = q;
            p = q;
        }
    }
    return t;
}

/* * 최상위 파싱 함수 (시작점)
 */
TreeNode* parse(void) {
    TreeNode* t;
    token = getToken(); // 첫 번째 토큰 가져오기
    t = parse_program();
    if (token != ENDFILE) {
        fprintf(listing, "Syntax error: Code ends before file\n");
    }
    return t;
}
