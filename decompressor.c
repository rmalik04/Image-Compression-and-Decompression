/*
******************************************************************************
* Name: Krish Savla (ksavla01), Ranvir Malik (rmalik04)
* Assignment: HW4 - arith
* Date: 10/26/2022
* File name: decompressor.c
*
* Defines all the function that declared in decompressorg. These functions 
* deal with all the decompression steps of the program. 
*
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
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "new_pnm.h"
#include "decompressor.h"
#include "constants.h"

void free_data_decomp(A2Methods_UArray2 quant_pixels, A2Methods_UArray2 
              scaled_pixels, A2Methods_UArray2 compressed_pixels,
              A2Methods_UArray2 comp_vid_pixels, A2Methods_UArray2 bits);
void video_convert_apply(int col, int row, 
                         A2Methods_UArray2 compressed_pixels, void *elem,
                         void *cl);

/*
 * Name: decompressor_main
 * Purpose: calls all the functions necessary for decompressing an inputted
            compressed image and returns an array of compressed bits
 * Parameters: an array that contains the bits of the compressed image
 * Returns: a 2d array of bits that represents the compressed image
 * Effects: allocates and frees memory for multiple arrays used to store
            pixels of the image.  
 * Error Cases: if the array of bits is NULL, if either of the A2Methods 
                is NULL, or if any newly created array is NULL, the program 
                will exit with a checked runtime error. 
*/
Pnm_ppm decompressor_main(A2Methods_UArray2 bits) 
{
        assert(bits != NULL);
        
        A2Methods_T methodsB = uarray2_methods_blocked;
        A2Methods_T methodsP = uarray2_methods_plain;
        assert(methodsB != NULL);
        assert(methodsP != NULL);

        int wid = methodsP->width(bits);
        int hei = methodsP->height(bits); 

        /* creates a new 2d array to store the quantized pixels */
        A2Methods_UArray2 quant_pixels = methodsP->new(wid, hei, 
                                                    sizeof(struct quantized));
        assert(quant_pixels != NULL);
        /* converts each bit into quantized pixels and stores those pixels in
           the 2d array created above */
        methodsP->map_default(bits, unbitpack_apply, quant_pixels);

        /* creates a new 2d array to store the compressed pixels */
        A2Methods_UArray2 compressed_pixels = methodsP->new(wid, hei, 
                                              sizeof(struct compressed_pix));
        assert(compressed_pixels != NULL);
        /* converts each bit into compressed pixels and stores those pixels in
           the 2d array created above */
        methodsP->map_default(quant_pixels, dequantize_apply, 
                              compressed_pixels);

        /* creates a 2d blocked array to store the compnent video pixels */
        /* converts the compressed pixels to component video pixels */
        A2Methods_UArray2 comp_video_pixels = video_convert(compressed_pixels);
        assert(comp_video_pixels != NULL);
        
        wid = BLOCKSIZE * wid;
        hei = BLOCKSIZE * hei;
        /* creates a 2d blocked array to store the float rgb pixels */
        A2Methods_UArray2 float_rgb_pixels = methodsB->new_with_blocksize(wid, 
                                             hei, sizeof(struct float_rgb),
                                             BLOCKSIZE);
        assert(float_rgb_pixels != NULL);
        /* converts the component video pixels to float rgb pixel and stores
           them into the blocked array created above */
        methodsB->map_default(comp_video_pixels, comp_video_to_rgb_apply, 
                              float_rgb_pixels);

        /* creates a 2d blocked array to store the rgb pixels */
        A2Methods_UArray2 pnm_pixels = methodsB->new_with_blocksize(wid, hei,
                                       sizeof(struct Pnm_rgb), BLOCKSIZE);
        /* convert each float_rgb_pixel to a normal pnm_ppm rgb pixel 
           and store it into the blocked array created above*/
        methodsB->map_default(float_rgb_pixels, float_rgb_to_pnm_apply,
                              pnm_pixels);
        assert(pnm_pixels != NULL);
        
        free_data_decomp(quant_pixels, float_rgb_pixels, compressed_pixels, 
                         comp_video_pixels, bits);
        /* create a new Pnm_ppm object for the decompressed image */
        Pnm_ppm decompressed = new_pnm(pnm_pixels);

        return decompressed;
        
}
/*
 * Name: free_data_decomp
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
void free_data_decomp(A2Methods_UArray2 quant_pixels, A2Methods_UArray2 
              scaled_pixels, A2Methods_UArray2 compressed_pixels,
              A2Methods_UArray2 comp_vid_pixels, A2Methods_UArray2 bits) 
{
        assert(bits != NULL);
        assert(scaled_pixels != NULL);
        assert(comp_vid_pixels != NULL);
        assert(quant_pixels != NULL);
        assert(compressed_pixels != NULL);

        A2Methods_T methodsB = uarray2_methods_blocked;
        A2Methods_T methodsP = uarray2_methods_plain;
        assert(methodsB != NULL);
        assert(methodsP != NULL);

        methodsP->free(&quant_pixels);
        methodsP->free(&bits);
        methodsB->free(&scaled_pixels);
        methodsP->free(&compressed_pixels);
        methodsB->free(&comp_vid_pixels);
}



/*
 * Name: video_convert
 * Purpose: convert each compressed pixel in the inputted 2d array to 
            component video pixels and store those pixels in a new 
            blocked 2d array
 * Parameters: a 2d array that represents the compressed pixels of the
               compressed image
 * Returns: an A2Methods_UArray2 array of component video pixels
 * Effects: none
 * Error Cases: if any of the array inputted into the function is NULL
                the program will terminate with a checked runtime error.       
*/
A2Methods_UArray2 video_convert(A2Methods_UArray2 compressed_pixels) 
{
        A2Methods_T methodsB = uarray2_methods_blocked;
        A2Methods_T methodsP = uarray2_methods_plain;
        assert(methodsB != NULL);
        assert(methodsP != NULL);

        unsigned wid = methodsP->width(compressed_pixels) * BLOCKSIZE;
        unsigned hei = methodsP->height(compressed_pixels) * BLOCKSIZE;
        /* create array to store component video pixels */
        A2Methods_UArray2 comp_vid_pixels = methodsB->new_with_blocksize(wid,
                                            hei, sizeof(struct comp_video), 
                                            BLOCKSIZE);
        assert(comp_vid_pixels != NULL);

        methodsP->map_default(compressed_pixels, video_convert_apply, 
                              comp_vid_pixels);

        return comp_vid_pixels;
}
/*
 * Name: video_convert_apply
 * Purpose: apply function called in video_convert that populates an
            blocked 2d array with component video pixels
 * Parameters: two integer representing the coordinates (col, row) of the
               current element, an A2Methods_UArray2 that holds the compressed
               pixels of the image, void pointers for the current element and
               the closure variable.
 * Returns: nothing
 * Effects: none
 * Error Cases: if any of the array inputted into the function is NULL
                the program will terminate with a checked runtime error.       
*/
void video_convert_apply(int col, int row, A2Methods_UArray2 compressed_pixels,
                         void *elem, void *cl)
{
        assert(compressed_pixels != NULL);
        A2Methods_UArray2 comp_vid_pixels = (A2Methods_UArray2) cl;
        assert(comp_vid_pixels != NULL);
        compressed_pix curr_pix = (compressed_pix) elem;
        populate_comp_vid(comp_vid_pixels, curr_pix, col, row);
}
/*
 * Name: populate_comp_vid
 * Purpose: helper function called in video_convert that populates a blocked
            2d array with component video pixels
 * Parameters: a blocked 2d array that will hold the component video pixels
               the current compressed pixel that needs to be converted, and
               the location of the pixel in the array
 * Returns: an A2Methods_UArray2 array of component video pixels
 * Effects: none
 * Error Cases: if any of the array inputted into the function is NULL
                the program will terminate with a checked runtime error.
                
*/
void populate_comp_vid(A2Methods_UArray2 comp_video_pixels, 
                       compressed_pix curr_pix, int col, int row) 
{
        A2Methods_T methodsB = uarray2_methods_blocked;
        assert(methodsB != NULL);
        
        UArray_T comp_vid_block = compressed_to_vid(curr_pix);
        assert(comp_vid_block != NULL);
        int counter = 0;
        /* Populate blocked array with comp_video structs */
        for (int block_row = 0; block_row < BLOCKSIZE; block_row++) {
                for (int block_col = 0; block_col < BLOCKSIZE; block_col++) {

                        comp_video blank_vid = 
                                (comp_video)methodsB->at(comp_video_pixels, 
                                           BLOCKSIZE * col + block_col, 
                                           BLOCKSIZE * row + block_row);

                        comp_video curr_vid = UArray_at(comp_vid_block, counter);
                        *blank_vid = *curr_vid;
                        counter++;
                }  
        }
        UArray_free(&comp_vid_block);
}

