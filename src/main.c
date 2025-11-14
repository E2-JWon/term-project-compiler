/****************************************************/
/* File: main.c                                     */
/* Main program for C- scanner test                 */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

int lineno = 0;
FILE *source;
FILE *listing;
int EchoSource = TRUE;
int TraceScan = TRUE;
int Error = FALSE;

int main(int argc, char *argv[])
{
    char pgm[120];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    strcpy(pgm, argv[1]);
    if (strchr(pgm, '.') == NULL)
        strcat(pgm, ".c-");

    source = fopen(pgm, "r");
    if (source == NULL) {
        fprintf(stderr, "File %s not found\n", pgm);
        exit(1);
    }

    listing = stdout;
    fprintf(listing, "\nC-Minus SCANNER TEST: %s\n", pgm);

    while (getToken() != ENDFILE)
        ;

    fprintf(listing, "\nScanning completed.\n");
    fclose(source);
    return 0;
}
