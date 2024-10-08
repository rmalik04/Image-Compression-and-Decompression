/*
******************************************************************************
* Name: Krish Savla (ksavla01), Ranvir Malik (rmalik04)
* Assignment: HW4 - arith
* Date: 10/26/2022
* File name: compressor.c
*
* the module that acts as a main for the compressor part of the program. It
* calls functions that are neccsessary for compressing the image. 
*
******************************************************************************
*/

#include <stdio.h>
#include <a2methods.h>
#include <a2blocked.h>
#include <a2plain.h>
#include <uarray2.h>
#include <uarray2b.h>
#include <stdlib.h>
#include "assert.h"
#include "mem.h"
#include "pnm.h"
#include "compressor.h"
#include "calculations.h"
#include "constants.h"

compressed_pix get_pix(A2Methods_UArray2 vid, int col, int row);
void get_pix_apply(int col, int row, A2Methods_UArray2 compressed_pixels, void 
                   *elem, void *cl);

/*
 * Name: compressor_main
 * Purpose: calls all the functions necessary for compressing an inputted 
            image and returns an array of compressed bits
 * Parameters: a Pnm_ppm object that reperesents the pixels in the image
 * Returns: a 2d array of bits that represents the compressed image
 * Effects: allocates and frees memory for multiple arrays used to store
            pixels of the image.  
 * Error Cases: if the pixmap is NULL, if either of the A2Methods is NULL, 
                or if any newly created array is NULL, the program will exit
                with a checked runtime error. 
*/
A2Methods_UArray2 compressor_main(Pnm_ppm pixmap) 
{
        assert(pixmap != NULL);
        
        A2Methods_T methodsB = uarray2_methods_blocked;
        A2Methods_T methodsP = uarray2_methods_plain;
        assert(methodsB != NULL);
        assert(methodsP != NULL);
        
        int wid = pixmap->width;
        int hei = pixmap->height;
        /* create blocked array to store the scaled pixels*/
        A2Methods_UArray2 scaled_pixels = methodsB->new_with_blocksize(wid, 
                                                    hei, 
                                                    sizeof(struct float_rgb), 
                                                    BLOCKSIZE);
        assert(scaled_pixels != NULL);
        /* convert each rgb pixel to a scaled pixel and store it into the
           array created above */
        methodsB->map_default(pixmap->pixels, scale_float_apply, 
                              scaled_pixels);

        /* create blocked array to store the component video pixels */
        A2Methods_UArray2 vid = methodsB->new_with_blocksize(wid, hei, 
                                                sizeof(struct comp_video), 
                                                BLOCKSIZE);
        assert(vid != NULL);
        /* convert each scaled pixel to a component video pixel and store it
           into the array created above */
        methodsB->map_default(scaled_pixels, rgb_to_vid_apply, vid);

        /* create 2d array to store the compressed pixels */
        /* convert each pixel into compressed pixel structs and store them
           in the array */
        A2Methods_UArray2 compressed_pixels = block_to_pix(vid, pixmap);
        assert(compressed_pixels != NULL);

        /* create blocked array to store the quanitzed compressed pixels */
        A2Methods_UArray2 quant_pixels = methodsP->new(wid / BLOCKSIZE, 
                                                       hei / BLOCKSIZE, 
                                              sizeof(struct quantized));
        assert(quant_pixels != NULL);
        /* convert each pixel to a quantized pixel and store it in the array
           created above */
        methodsP->map_default(compressed_pixels, quantize_apply, 
                              quant_pixels);

        /* create blocked array to store bits of the compressed image */
        A2Methods_UArray2 bits = methodsP->new(wid / BLOCKSIZE, 
                                               hei / BLOCKSIZE, 
                                               sizeof(uint64_t));
        assert(bits != NULL);
        /* convert each quantized but to a 32 bitstring and store the
           bitstring into the array created above */
        methodsP->map_default(quant_pixels, bitpack_apply, bits);

        /* free allocated memory */
        free_data(quant_pixels, scaled_pixels, compressed_pixels, vid, 
                  pixmap);

        return bits;
}
/*
 * Name: block_to_pix
 * Purpose: for each 2 x 2 block of compnenet video pixels, this function
            converts those blocks into compressed  pixel structs
 * Parameters: a Pnm_ppm object that represents the original pixel array
               and an A2Methods_UArray2 array that is the array that holds
               component video pixels
 * Returns: a 2d array of compressed_pix structs
 * Effects: allocates memory for array that holds the compressed_pix
            structs 
 * Error Cases: if the pixmap is NULL, if the A2Methods or the A2Methids array 
                is NULL, or if any newly created array is NULL, the program 
                will exit with a checked runtime error. 
*/
A2Methods_UArray2 block_to_pix(A2Methods_UArray2 vid, Pnm_ppm pixmap) 
{
        assert(vid != NULL);
        assert(pixmap != NULL);
        A2Methods_T methodsP = uarray2_methods_plain;
        unsigned width = pixmap->width / BLOCKSIZE;
        unsigned height = pixmap->height / BLOCKSIZE;
        A2Methods_UArray2 compressed_pixels = methodsP->new(width, height, 
                                              sizeof(struct compressed_pix));
        assert(compressed_pixels != NULL);
        methodsP->map_default(compressed_pixels, get_pix_apply, vid);
        return compressed_pixels;
}

/*
 * Name: get_pix_apply
 * Purpose: apply function that creates a UArray of 4 component video pixels
            and then converts those 4 pixels into 1 compressed pixel
 * Parameters: two integers that represent the coordinates of the component
               video pixel, an A2Methods_UArray2 that holds all the
               compressed pixels, and two void pointers for the current
               element and a closure variable
 * Returns: nothing
 * Effects: allocates memory for an array that contains 4 comp_video structs
 * Error Cases: if the A2Methods suite, A2Methods array, or the created
                UArray is NULL, the program will terminate with a checked
                runtime error.
*/
void get_pix_apply(int col, int row, A2Methods_UArray2 compressed_pixels, 
                   void *elem, void *cl) 
{
        A2Methods_T methodsB = uarray2_methods_blocked;
        A2Methods_T methodsP = uarray2_methods_plain;
        assert(methodsB != NULL);
        assert(methodsP != NULL);

        A2Methods_UArray2 vid = (A2Methods_UArray2) cl;
        assert(vid != NULL);

        UArray_T block = UArray_new(BLOCKSIZE * BLOCKSIZE, 
                                    sizeof(struct comp_video));
        assert(block != NULL);

        int counter = 0; 
        int vid_col = BLOCKSIZE * col;
        int vid_row = BLOCKSIZE * row;
        /* store a 2x2 block of comp_video struts into created UArray */  
        for (int block_row = 0; block_row < BLOCKSIZE; block_row++) {
                for (int block_col = 0; block_col < BLOCKSIZE; block_col++) {
                        comp_video curr_vid = 
                                       (comp_video) UArray_at(block, counter);

                        comp_video old_vid = 
                                        (comp_video)methodsB->at(vid,
                                        vid_col + block_col, vid_row + 
                                        block_row);

                        *curr_vid = *old_vid;
                        counter++;
                }
        }
        compressed_pix pix = vid_to_compress(block);
        UArray_free(&block);
        compressed_pix curr_pix = (compressed_pix)methodsP->at(
                                   compressed_pixels, col, row);
        *curr_pix = *pix;
        free(pix);
        (void) elem;
}





/*
 * Name: free_data
 * Purpose: free the memory allocated for any arrays that have not been
            freed already
 * Parameters: a Pnm_ppm object that represents the original pixel array
               an A2Methods_UArray2 arrays that represent arrays that hold
               component video pixels, compressed_pixels, scaled pixels, 
               and quantized pixels
 * Returns: nothing
 * Effects: frees the memory allocated for the inputted arrays
 * Error Cases: if any of the arrays inputted into the function are NULL
                the program will terminate with a checked runtime error.
                
*/
void free_data(A2Methods_UArray2 quant_pixels, A2Methods_UArray2 
              scaled_pixels, A2Methods_UArray2 compressed_pixels,
              A2Methods_UArray2 vid, Pnm_ppm pixmap) 
{
        assert(pixmap != NULL);
        assert(scaled_pixels != NULL);
        assert(vid != NULL);
        assert(quant_pixels != NULL);
        assert(compressed_pixels != NULL);

        A2Methods_T methodsB = uarray2_methods_blocked;
        A2Methods_T methodsP = uarray2_methods_plain;
        assert(methodsB != NULL);
        assert(methodsP != NULL);

        methodsP->free(&quant_pixels);
        Pnm_ppmfree(&pixmap);
        methodsB->free(&scaled_pixels);
        methodsP->free(&compressed_pixels);
        methodsB->free(&vid);
}