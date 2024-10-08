/*
******************************************************************************
* Name: Krish Savla (ksavla01), Ranvir Malik (rmalik04)
* Assignment: HW4 - arith
* Date: 10/26/2022
* File name: compress40.c
*
* This file defines functions that direct the compression and decompression
* proccesses of the program. They read in files from the command line or 
* stdin and will compress or decompress the image based on the user's input.
*
******************************************************************************
*/
#include <stdio.h>
#include <a2methods.h>
#include <stdio.h>
#include "assert.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "arithio.h"
#include "compressor.h"
#include "decompressor.h"

/*
 * Name: compress40
 * Purpose: function that will call helper functions to compress the image
            read in from the inputted file. It writes the compressed file
            to stdout
 * Parameters: a pointer to a file contianing the image
 * Returns: nothing
 * Effects: none
 * Error Cases: if the inputted file is NULL, or if any of the arrays created
                within the function are NULL, the program will terminate with
                a checked runtime error.        
*/
void compress40 (FILE *input) 
{
        A2Methods_T methodsB = uarray2_methods_blocked; 
        A2Methods_T methodsP = uarray2_methods_plain;
        assert(methodsB != NULL);
        assert(methodsP != NULL);

        Pnm_ppm pixmap = read_image(input, methodsB);
        assert(pixmap != NULL);
        /* call helper function that compresses the image and returns the 
           2d array of bits */
        A2Methods_UArray2 bits = compressor_main(pixmap);
        assert(bits != NULL);

        print_compressed(bits);
        methodsP->free(&bits);

}
/*
 * Name: decompress40
 * Purpose: function that will call helper functions to decompress the image
            read in from the inputted file. It will print the decompressed
            file to stdout
 * Parameters: a pointer to a file contianing the compressed image
 * Returns: nothing
 * Effects: none
 * Error Cases: if the inputted file is NULL, or if any of the arrays created
                within the function are NULL, the program will terminate with
                a checked runtime error.        
*/
void decompress40(FILE *input) 
{
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods != NULL);

        A2Methods_UArray2 bits = compressed_image_read(input);
        assert(bits != NULL);
        /* call helper function that decompresses the image and returns the 
           Pnm_ppm object of the decompressed image */
        Pnm_ppm decompressed = decompressor_main(bits);
        assert(decompressed != NULL);
        
        print_decompressed(decompressed);
        Pnm_ppmfree(&decompressed);

} 

