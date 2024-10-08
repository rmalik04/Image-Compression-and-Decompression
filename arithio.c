/*
******************************************************************************
* Name: Krish Savla (ksavla01), Ranvir Malik (rmalik04)
* Assignment: HW4 - arith
* Date: 10/26/2022
* File name: arithio.c
* Defines the implementation for the functions in arithio.h. Thes functions
* handle the input of files and writing of files to stdout.  
******************************************************************************
*/
#include <stdio.h>
#include <a2methods.h>
#include <uarray2.h>
#include <uarray.h>
#include <uarray2b.h>
#include <stdlib.h>
#include "assert.h"
#include "mem.h"
#include "pnm.h"
#include <a2plain.h>
#include "bitpack.h"
#include "arithio.h"
#include "constants.h"

/*
 * Name: read_image
 * Purpose: create a new Pnm_ppm object with information contained in the
            inputted ppm file. 
 * Parameters: the input file that the program needs to read from,
               and an A2Methods_T function set that the Pnm_ppm object can use
 * Returns: a Pnm_ppm object with the dimensions and pixels of the inputted
            image.
 * Effects: none
 * Error Cases: if the width and height are less than or equal to 0, 
                the program will terminate with a checked runtime error. 
                Additionally, if the input file is NULL, or the created
                Pnm_ppm object is NULL, the program will terminate with a 
                checked runtime error.
*/
Pnm_ppm read_image(FILE *inputfd, A2Methods_T methods) 
{
        assert(inputfd != NULL);
        assert(methods != NULL);

        Pnm_ppm pixmap = Pnm_ppmread(inputfd, methods);
        assert(pixmap != NULL);
        /* checking if the dimensions of the image are even*/
        if (pixmap->height % 2 != 0) {
                pixmap->height = pixmap->height - 1;
        }
        if (pixmap->width % 2 != 0) {
                pixmap->width = pixmap->width - 1;
        }
        A2Methods_UArray2 image = methods->new_with_blocksize(pixmap->width, 
                                  pixmap->height, sizeof(struct Pnm_rgb), 
                                  BLOCKSIZE);
        assert(image != NULL);
        /* copying the pixel structs in the Pnm object to the blocked array */
        for (unsigned row = 0; row < pixmap->height; row++) {
                for (unsigned col = 0; col < pixmap->width; col++) {
                        Pnm_rgb elem = methods->at(pixmap->pixels, col, row);
                        assert(elem != NULL);
                        Pnm_rgb elem_new = methods->at(image, col, row);
                        *elem_new = *elem;
                }
        }
        methods->free(&(pixmap->pixels));
        pixmap->pixels = image;
        return pixmap;
}
/*
 * Name: print_compressed
 * Purpose: prints the compressed image file to stdout
 * Parameters: an A2Methods_UArray2 containing the bits of the compressed
               image. 
 * Returns: nothing
 * Effects: none
 * Error Cases: if the inputted array is NULL, the program will terminate with
                a checked runtime error. Additionally if the height and width 
                of the array is less than or equal to 0, the program will 
                terminate with a checked runtime error. 
*/
void print_compressed(A2Methods_UArray2 bits) 
{
        assert(bits != NULL);

        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);
        /* getting width and height to output to file */
        int wid = methods->width(bits);
        int hei = methods->height(bits);
        printf("COMP40 Compressed image format 2\n%u %u", wid * BLOCKSIZE, 
               hei * BLOCKSIZE);
        printf("\n");
        /* write bits in inputted array to stdout */
        methods->map_default(bits, print_comp_apply, NULL);       
}
/*
 * Name: print_comp_apply
 * Purpose: apply function for the mapping methods function. It access 
            every byte within every 
 * Parameters: integers representing the current column and row, an array that
               stores the bits of the compressed image, two void pointers for 
               the current element in the array and the closure
 * Returns: nothing
 * Effects: none
 * Error Cases: if the inputted array is NULL, the program will terminate with
                a checked runtime error. 
*/
void print_comp_apply(int col, int row, A2Methods_UArray2 bits, void *elem, 
                      void *cl) 
{
        A2Methods_T methods = uarray2_methods_plain;
        uint64_t *word =  methods->at(bits, col, row);
        /*converting to 32 bits */
        uint32_t code = (uint32_t) *word;

        for (int index = 24; index >= 0; index = index - 8) {
                unsigned char c = code >> index;
                putchar(c);
        }
        (void) elem;
        (void) cl;
}
/*
 * Name: print_decompressed
 * Purpose: prints the decompressed image to stdout
 * Parameters: an array of pixels that is part of the Pnm_ppm object  
 * Returns: nothing
 * Effects: none
 * Error Cases: if the inputted array/object is NULL, the program 
                will terminate with a checked runtime error. 
*/
void print_decompressed(Pnm_ppm pixmap) 
{
        assert(pixmap != NULL);
        Pnm_ppmwrite(stdout, pixmap);
}
/*
 * Name: compressed_image_read
 * Purpose: reads in the compressed image bits from an inputted file
 * Parameters: a file that contains the bits of a compressed image
 * Returns: A2Methods_UArray2 containing the bits of the compressed image
 * Effects: allocates memory for a A2Methods_UArray2
 * Error Cases: if the inputted file is NULL, the program 
                will terminate with a checked runtime error. 
*/
A2Methods_UArray2 compressed_image_read(FILE *inputfd) 
{
        assert(inputfd != NULL);

        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);

        unsigned height, width;
        /* read in header of file, store dimensions into width and height */
        int read = fscanf(inputfd, "COMP40 Compressed image format 2\n%u %u",
                          &width, &height);
        width = width / BLOCKSIZE;
        height = height / BLOCKSIZE;
        assert(read == 2);
        int c = getc(inputfd);
        assert(c == '\n');
        /* create new 2d array and store each bit from file into array */
        A2Methods_UArray2 bits = methods->new(width, height, 
                                              sizeof(uint64_t));
        assert(bits != NULL);
        methods->map_default(bits, compressed_image_apply, inputfd);
        return bits;
        
}
/*
 * Name: compressed_image_apply
 * Purpose: apply function for the mapping method function called in 
            compressed_image_read. It iterates through every bit in a file
            and adds it to the array passed into the function.
 * Parameters: integers representing the current column and row, an array that
               stores the bits of the compressed image, two void pointers for 
               the current element in the array and the closure
 * Returns: nothing
 * Effects: allocates memory for a A2Methods_UArray2 
 * Error Cases: if the inputted array is NULL, the program will terminate
                with a checked runtime error.
*/
void compressed_image_apply(int col, int row, A2Methods_UArray2 bits, 
                            void *elem, void *cl) 
{
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);

        FILE *inputfd = (FILE *) cl;
        assert(inputfd != NULL);

        assert(!feof(inputfd));
        
        uint64_t word = 0;
        int c;
        /* read in 1 byte at a time */
        for (int index = 24; index >= 0; index = index - 8) {
                c = getc(inputfd);
                assert(c != -1);
                word = Bitpack_newu(word, 8, (unsigned) index, c);
        } 
        
        /* store bit into an array */
        *(uint64_t *)methods->at(bits, col, row) = word; 
        word = 0;
        (void) elem;
}

