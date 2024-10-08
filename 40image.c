/*
******************************************************************************
* Name: Krish Savla (ksavla01), Ranvir Malik (rmalik04)
* Assignment: HW4 - arith
* Date: 10/26/2022
* File name: 40images.c
* main function that contains the comman loop for the program. Based on the
* client's input, the program will compress or decompress and image. 
******************************************************************************
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress40.h"

static void (*compress_or_decompress)(FILE *input) = compress40;

int main(int argc, char *argv[])
{
        int i;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        /* calls compress function */
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        /* calls decompress function */
                        compress_or_decompress = decompress40;
                /* incorrect wrong inputs */
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1);    /* at most one file on command line */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }

        return EXIT_SUCCESS; 
}
