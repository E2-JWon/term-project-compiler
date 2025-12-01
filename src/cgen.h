/****************************************************/
/* File: cgen.h                                     */
/* The code generator interface to the C compiler   */
/****************************************************/

#ifndef _CGEN_H_
#define _CGEN_H_

#include "globals.h"

/* codeGen takes a syntax tree and generates code into a file */
void codeGen(TreeNode* syntaxTree, char* codefile);

#endif
