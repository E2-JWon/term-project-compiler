/****************************************************/
/* File: scan.h                                     */
/* The scanner interface for the C- compiler        */
/****************************************************/

#ifndef _SCAN_H_
#define _SCAN_H_

#define MAXTOKENLEN 40
#include "globals.h"
extern char tokenString[MAXTOKENLEN + 1];

TokenType getToken(void);

#endif
