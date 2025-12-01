/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for the C- compiler        */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#define MAXRESERVED 6  /* number of reserved words */

/* Token types for C- */
typedef enum {
    /* book-keeping tokens */
    ENDFILE, ERROR,

    /* reserved words */
    ELSE, IF, INT, RETURN, VOID, WHILE,

    /* multicharacter tokens */
    ID, NUM,

    /* special symbols */
    PLUS, MINUS, TIMES, OVER,
    LT, LTE, GT, GTE, EQ, NEQ, ASSIGN,
    SEMI, COMMA,
    LPAREN, RPAREN,
    LBRACE, RBRACE,
    LBRACKET, RBRACKET
} TokenType;

/* files */
extern FILE* source;
extern FILE* listing;

/* source line number for listing */
extern int lineno;

/* trace flags */
extern int EchoSource;
extern int TraceScan;
extern int TraceParse;
extern int Error;

#endif
