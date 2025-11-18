/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the C- compiler   */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include <string.h>

/* DFA states */
typedef enum
{
    START,
    INNUM,
    INID,
    INLT,
    INGT,
    INEQ,
    INNEQ,
    INDIV,
    INCOMM,
    OUTCOMM,
    DONE
} StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];

#define BUFLEN 256
static char lineBuf[BUFLEN];
static int linepos = 0;
static int bufsize = 0;

static int getNextChar(void)
{
    if (!(linepos < bufsize))
    {
        lineno++;
        if (fgets(lineBuf, BUFLEN - 1, source))
        {
            if (EchoSource)
                fprintf(listing, "%4d: %s", lineno, lineBuf);
            bufsize = strlen(lineBuf);
            linepos = 0;
            return (unsigned char)lineBuf[linepos++];
        }
        else
            return EOF;
    }
    else
        return (unsigned char)lineBuf[linepos++];
}

static void ungetNextChar(void)
{
    if (linepos > 0)
        linepos--;
}

/* reserved words table */
static struct
{
    const char *str;
    TokenType tok;
} reservedWords[MAXRESERVED] = {
    {"else", ELSE}, {"if", IF}, {"int", INT}, {"return", RETURN}, {"void", VOID}, {"while", WHILE}};

static TokenType reservedLookup(char *s)
{
    for (int i = 0; i < MAXRESERVED; i++)
        if (!strcmp(s, reservedWords[i].str))
            return reservedWords[i].tok;
    return ID;
}

/* main scanner routine */
TokenType getToken(void)
{
    int tokenStringIndex = 0;
    TokenType currentToken = ERROR;
    StateType state = START;
    int save; // 문자인지 숫자인지 구별할 변수

    while (state != DONE)
    {
        int c = getNextChar();
        save = TRUE;

        switch (state)
        {
        case START:
            if (c == EOF)
            {
                state = DONE;
                save = FALSE;
                currentToken = ENDFILE;
            }
            else if (isdigit(c))
                state = INNUM;
            else if (isalpha(c))
                state = INID;
            else if (c == '<')
                state = INLT;
            else if (c == '>')
                state = INGT;
            else if (c == '=')
                state = INEQ;
            else if (c == '!')
                state = INNEQ;
            else if (c == '/')
                state = INDIV;
            else if ((c == ' ') || (c == '\t') || (c == '\n'))
                save = FALSE;
            else
            {
                state = DONE;
                switch (c)
                {
                case '+':
                    currentToken = PLUS;
                    break;
                case '-':
                    currentToken = MINUS;
                    break;
                case '*':
                    currentToken = TIMES;
                    break;
                case ';':
                    currentToken = SEMI;
                    break;
                case ',':
                    currentToken = COMMA;
                    break;
                case '(':
                    currentToken = LPAREN;
                    break;
                case ')':
                    currentToken = RPAREN;
                    break;
                case '{':
                    currentToken = LBRACE;
                    break;
                case '}':
                    currentToken = RBRACE;
                    break;
                case '[':
                    currentToken = LBRACKET;
                    break;
                case ']':
                    currentToken = RBRACKET;
                    break;
                default:
                    currentToken = ERROR;
                    break;
                }
            }
            break;

        case INNUM:
            if (!isdigit(c))
            {
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = NUM;
            }
            break;

        case INID:
            if (!isalpha(c) && !isdigit(c))
            {
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = ID;
            }
            break;

        case INEQ: /* '=' 읽은 뒤 다음이 '='이면 EQ, 아니면 ASSIGN */
            state = DONE;
            if (c == '=')
                currentToken = EQ;
            else
            {
                ungetNextChar();
                save = FALSE;
                currentToken = ASSIGN;
            }
            break;

        case INNEQ: /* '!' 읽은 뒤 다음이 '='이면 NEQ, 아니면 단독 '!'은 에러 처리 */
            state = DONE;
            if (c == '=')
                currentToken = NEQ;
            else
            {
                ungetNextChar();
                save = FALSE;
                currentToken = ERROR;
            }
            break;

        case INLT:
            state = DONE;
            if (c == '=')
                currentToken = LTE;
            else
            {
                ungetNextChar();
                save = FALSE;
                currentToken = LT;
            }
            break;

        case INGT:
            state = DONE;
            if (c == '=')
                currentToken = GTE;
            else
            {
                ungetNextChar();
                save = FALSE;
                currentToken = GT;
            }
            break;

        case INDIV: /* '/' 다음이 '*'이면 블록 주석 시작, 아니면 OVER */
            if (c == '*')
            {
                save = FALSE;
                state = INCOMM;
                tokenStringIndex = 0;
            }
            else
            {
                ungetNextChar();
                state = DONE;
                currentToken = OVER;
            }
            break;

        case INCOMM: /* 주석 내부: '*' 만나면 OUTCOMM로 */
            save = FALSE;
            if (c == EOF)
            {
                state = DONE;
                currentToken = ENDFILE;
            }
            else if (c == '*')
            {
                state = OUTCOMM;
            } /* else remain in INCOMM */
            break;

        case OUTCOMM:
            save = FALSE;
            if (c == '/') { 
                state = START;
                continue;
            }
            else if (c == '*') {
                state = OUTCOMM;
            }
            else {
                state = INCOMM;
            }
            break;

        case DONE:
        default:
            state = DONE;
            currentToken = ERROR;
            break;
        }

        if (save && (tokenStringIndex <= MAXTOKENLEN))
            tokenString[tokenStringIndex++] = (char)c;

        if (state == DONE)
        {
            tokenString[tokenStringIndex] = '\0';
            if (currentToken == ID)
                currentToken = reservedLookup(tokenString);
        }
    }

    if (TraceScan)
    {
        fprintf(listing, "\t%d: ", lineno);
        printToken(currentToken, tokenString);
    }

    return currentToken;
}
