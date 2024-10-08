/*
******************************************************************************
* Name: Krish Savla (ksavla01), Ranvir Malik (rmalik04)
* Assignment: HW4 - arith
* Date: 10/26/2022
* File name: calculations.c
*
* Houses all the calculations for converting image pixels to different types.
* This includes quantizing, scaling, and averaging values.
*
******************************************************************************
*/

#include <stdio.h>
#include <a2methods.h>
#include <uarray2.h>
#include <uarray.h>
#include <uarray2b.h>
#include "calculations.h"
#include <stdlib.h>
#include "assert.h"
#include "mem.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "arith40.h"
#include <math.h>
#include "bitpack.h"
#include "constants.h"

/* private functions defined in this file */
void quantize(compressed_pix comp_pix, quantized quant_pix);
void dequantize(compressed_pix comp_pix, quantized quant_pix);
void pixel_adjuster(compressed_pix curr_pix);
void rgb_adjuster(float_rgb curr_pix);
void comp_video_adjuster(comp_video curr_vid);
UArray_T compressed_to_vid(compressed_pix compressed_pixel);

/*
 * Name: scale_float_apply
 * Purpose: apply function that converts each pixel iterated over to
            a scaled float pixel (multiplied by the denomenator)
 * Parameters: two integer representing the coordinates (col, row) of the
               current element, an A2Methods_UArray2 that holds the Pnm_ppm
               pixels of the image, void pointers for the current element and
               the closure variable.
 * Returns: nothing
 * Effects: none
 * Error Cases: if any of the array inputted into the function is NULL
                the program will terminate with a checked runtime error.
                Additionally, if the created methods suit is NULL, the program
                will terminate with a checked runtime error.       
*/
void scale_float_apply(int col, int row, A2Methods_UArray2 pixels,      
                       void *elem, void *cl) 
{
        assert(pixels != NULL);

        A2Methods_T methods =  uarray2_methods_blocked; 
        assert(methods != NULL);

        A2Methods_UArray2 scaled_pixels = (A2Methods_UArray2) cl; 
        assert(scaled_pixels != NULL);

        Pnm_rgb curr_pixel = elem;
        assert(curr_pixel != NULL);
        /* store struct of scaled pixels in newly created array */
        float_rgb curr_scaled_pixel = methods->at(scaled_pixels, col, row);
        assert(curr_scaled_pixel != NULL);
        /* populate each float_rgb struct with the proper values */
       curr_scaled_pixel->r = (float) curr_pixel->red / (float) DENOMINATOR;
       curr_scaled_pixel->g = (float) curr_pixel->green / (float) DENOMINATOR;
       curr_scaled_pixel->b = (float) curr_pixel->blue / (float) DENOMINATOR;
        /* force the rgb values to be in the proper range */
        rgb_adjuster(curr_scaled_pixel);
       *((float_rgb)methods->at(scaled_pixels, col, row)) = 
                                                        *curr_scaled_pixel;    
}
/*
 * Name: rgb_to_vid_apply
 * Purpose: apply function that converts each pixel iterated over to
            a component video pixel
 * Parameters: two integer representing the coordinates (col, row) of the
               current element, an A2Methods_UArray2 that holds the scaled
               pixels of the image, void pointers for the current element and
               the closure variable.
 * Returns: nothing
 * Effects: none
 * Error Cases: if any of the array inputted into the function is NULL
                the program will terminate with a checked runtime error.
                Additionally, if the created methods suit is NULL, the program
                will terminate with a checked runtime error.        
*/
void rgb_to_vid_apply(int col, int row, A2Methods_UArray2 scaled_pixels, 
                      void *elem, void *cl) 
{
        assert(scaled_pixels != NULL);

        A2Methods_T methods =  uarray2_methods_blocked; 
        assert(methods != NULL);

        A2Methods_UArray2 vid = (A2Methods_UArray2) cl;
        assert(vid != NULL);

        float_rgb curr_scaled_pixel = elem;
        assert(curr_scaled_pixel != NULL);
        /* get component video pixel */
        comp_video curr_vid_pixel = methods->at(vid, col, row);
        assert(curr_vid_pixel != NULL);

        /* populate compnent video pixel */
        curr_vid_pixel->y = 0.299 * curr_scaled_pixel->r + 
                            0.587 * curr_scaled_pixel->g + 
                            0.114 * curr_scaled_pixel->b;

        curr_vid_pixel->pb = -0.168736 * curr_scaled_pixel->r - 
                              0.331264 * curr_scaled_pixel->g + 
                              0.5 * curr_scaled_pixel->b;

        curr_vid_pixel->pr = 0.5 * curr_scaled_pixel->r - 
                             0.418688 * curr_scaled_pixel->g - 
                             0.081312 * curr_scaled_pixel->b;

        comp_video_adjuster(curr_vid_pixel);
        *((comp_video)methods->at(vid, col, row)) = *curr_vid_pixel; 
}

/*
 * Name: vid_to_compress
 * Purpose: converts 4 component video pixels (2 x 2 block) to a compressed
            pixel structs 
 * Parameters: A UArray that contains 4 component video pixels
 * Returns: a compressed_pix struct
 * Effects: none
 * Error Cases: if the UArray that is passed in is NULL, the program will 
                terminate with a checked runtime error. Additionally, if 
                the created struct is NULL, the program will terminate with
                a checked runtime error.        
*/
compressed_pix vid_to_compress(UArray_T block) 
{
        assert(block != NULL);
        /* array that will store y-values of the 4 pixels */
        float y[4];
        float num_blocks = BLOCKSIZE * BLOCKSIZE;
        compressed_pix curr_pix = malloc(sizeof(struct compressed_pix));
        assert(curr_pix != NULL);

        curr_pix->PbAverage = 0;
        curr_pix->PrAverage = 0;
        comp_video curr_vid;
        /* populate the PbAverage and PrAverage fields of the struct */
        for (int i = 0; i < UArray_length(block); i++) {
                curr_vid = (comp_video)UArray_at(block, i);
                assert(curr_vid != NULL);
                y[i] = curr_vid->y;
                curr_pix->PbAverage = curr_pix->PbAverage + curr_vid->pb;
                curr_pix->PrAverage = curr_pix->PrAverage + curr_vid->pr;
        }
        /* calculate and store the coefficients */
        curr_pix->PbAverage = curr_pix->PbAverage / num_blocks;
        curr_pix->PrAverage = curr_pix->PrAverage / num_blocks;
        curr_pix->a = (y[3] + y[2] + y[1] + y[0]) / num_blocks;
        curr_pix->b = (y[3] + y[2] - y[1] - y[0]) / num_blocks;
        curr_pix->c = (y[3] - y[2] + y[1] - y[0]) / num_blocks;
        curr_pix->d = (y[3] - y[2] - y[1] + y[0]) / num_blocks;
        /* force any values contained in the struct to be within the
           specified range*/
        pixel_adjuster(curr_pix);
        return curr_pix;
}
/*
 * Name: quantize_apply
 * Purpose: apply function that iterates through each compressed pixel
            and quantizes the values so that they are all within range
 * Parameters: two integer representing the coordinates (col, row) of the
               current element, an A2Methods_UArray2 that holds the compressed
               pixels of the image, void pointers for the current element and
               the closure variable.
 * Returns: nothing
 * Effects: none
 * Error Cases: if the array inputted into the function is NULL
                the program will terminate with a checked runtime error.
                Additionally, if the created methods suit is NULL, the program
                will terminate with a checked runtime error. Lastly, if any of 
                the structs are NULL, the program will terminate with a
                checked runtime error.      
*/
void quantize_apply(int col, int row, A2Methods_UArray2 comp_pixels, 
                    void *elem, void *cl) 
{
        assert(comp_pixels != NULL);

        A2Methods_T methods =  uarray2_methods_plain; 
        assert(methods != NULL);

        A2Methods_UArray2 quant_pixels = (A2Methods_UArray2) cl;
        assert(quant_pixels != NULL);
        /* get the quantized pixel */
        quantized curr_pix = methods->at(quant_pixels, col, row);
        assert(curr_pix != NULL);

        compressed_pix comp_pix = elem;
        assert(comp_pix != NULL);
        /* quantize the members of the struct */
        quantize(comp_pix, curr_pix);
}
/*
 * Name: dequantize_apply
 * Purpose: apply function that iterates through each quantized pixel
            and dequantizes the values so that they are all within range
 * Parameters: two integer representing the coordinates (col, row) of the
               current element, an A2Methods_UArray2 that holds the quantized
               pixels of the image, void pointers for the current element and
               the closure variable.
 * Returns: nothing
 * Effects: none
 * Error Cases: if the array inputted into the function is NULL
                the program will terminate with a checked runtime error.
                Additionally, if the created methods suit is NULL, the program
                will terminate with a checked runtime error. Lastly, if any of 
                the structs are NULL, the program will terminate with a
                checked runtime error.      
*/
void dequantize_apply(int col, int row, A2Methods_UArray2 quant_pixels, 
                      void *elem, void *cl) 
{
        assert(quant_pixels != NULL);
        
        A2Methods_T methods =  uarray2_methods_plain; 
        assert(methods != NULL);

        A2Methods_UArray2 comp_pixels = (A2Methods_UArray2) cl;
        assert(comp_pixels != NULL);
        /* get compressed pixel */
        compressed_pix comp_pix = methods->at(comp_pixels, col, row);
        assert(comp_pix != NULL);

        quantized curr_pix = elem;
        assert(curr_pix != NULL);
        /* dequantize the members of the struct */
        dequantize(comp_pix, curr_pix);
}
/*
 * Name: bitpack_apply
 * Purpose: apply function that iterates through each quantized pixel
            and converts the values and packs them into 32 bitstrings
 * Parameters: two integer representing the coordinates (col, row) of the
               current element, an A2Methods_UArray2 that holds the quantized
               pixels of the image, void pointers for the current element and
               the closure variable.
 * Returns: nothing
 * Effects: none
 * Error Cases: if the array inputted into the function is NULL
                the program will terminate with a checked runtime error.
                Additionally, if the created methods suit is NULL, the program
                will terminate with a checked runtime error. Lastly, if any of 
                the structs are NULL, the program will terminate with a
                checked runtime error.      
*/
void bitpack_apply(int col, int row, A2Methods_UArray2 quant_pixels, 
                   void *elem, void *cl) 
{
        assert(quant_pixels != NULL);

        A2Methods_T methods =  uarray2_methods_plain; 
        assert(methods != NULL);

        A2Methods_UArray2 bits = (A2Methods_UArray2) cl;
        assert(bits != NULL);

        uint64_t comp_pix = 0; 
        quantized quant_pix = elem;
        assert(quant_pix != NULL);

        /* create a new bit that contains all the values stored in the 
           compressed pixels */
        comp_pix = Bitpack_newu(comp_pix, A_WIDTH, A_LSB, quant_pix->a);
        comp_pix = Bitpack_news(comp_pix, B_WIDTH, B_LSB, quant_pix->b);
        comp_pix = Bitpack_news(comp_pix, C_WIDTH, C_LSB, quant_pix->c);
        comp_pix = Bitpack_news(comp_pix, D_WIDTH, D_LSB, quant_pix->d);
        comp_pix = Bitpack_newu(comp_pix, PB_WIDTH, PB_LSB, 
                                quant_pix->PbAverage);
        comp_pix = Bitpack_newu(comp_pix, PR_WIDTH, PR_LSB, 
                                quant_pix->PrAverage);
        *(uint64_t *) methods->at(bits, col, row) = comp_pix;
}
/*
 * Name: unbitpack_apply
 * Purpose: apply function that iterates through each 32 bitstring
            and gets the quantized values of the information packed into the
            bitstring
 * Parameters: two integer representing the coordinates (col, row) of the
               current element, an A2Methods_UArray2 that holds the bits
               of the compressed image, void pointers for the current element
               and the closure variable.
 * Returns: nothing
 * Effects: none
 * Error Cases: if the array inputted into the function is NULL
                the program will terminate with a checked runtime error.
                Additionally, if the created methods suit is NULL, the program
                will terminate with a checked runtime error. Lastly, if any of 
                the structs are NULL, the program will terminate with a
                checked runtime error.      
*/
void unbitpack_apply(int col, int row, A2Methods_UArray2 bits, void *elem, 
                     void *cl) 
{
        assert(bits != NULL);

        A2Methods_T methods =  uarray2_methods_plain; 
        assert(methods != NULL);

        A2Methods_UArray2 quant_pixels = (A2Methods_UArray2) cl;
        assert(quant_pixels != NULL);

        uint64_t *comp_pix = elem;
        assert(comp_pix != NULL);
        /* get the location of the quantized pixel */
        quantized quant_pix = methods->at(quant_pixels, col, row);
        assert(quant_pix != NULL);
        /* get each quantized value from the current bitstring 
           and store it into the quantized pixel */
        quant_pix->a = Bitpack_getu(*comp_pix, A_WIDTH, A_LSB);
        quant_pix->b = Bitpack_gets(*comp_pix, B_WIDTH, B_LSB);
        quant_pix->c = Bitpack_gets(*comp_pix, C_WIDTH, C_LSB);
        quant_pix->d = Bitpack_gets(*comp_pix, D_WIDTH, D_LSB);
        quant_pix->PbAverage = Bitpack_getu(*comp_pix, PB_WIDTH, PB_LSB);
        quant_pix->PrAverage = Bitpack_getu(*comp_pix, PR_WIDTH, PR_LSB);
}
/*
 * Name: quantized 
 * Purpose: quantize the coefficients and the Pb Average and Pr Average values
            of the compressed_pix struct 
 * Parameters: a compressed pixel struct and the quantized pixel struct.
 * Returns: nothing
 * Effects: none
 * Error Cases: if any of the structs are NULL, the program will terminate
                with a checked runtime error.      
*/
void quantize(compressed_pix comp_pix, quantized quant_pix) 
{
        assert(comp_pix != NULL);
        assert(quant_pix != NULL);
        /* quantizing each value and storing it into the inputted quantized 
           pixel struct */
        quant_pix->a = round(comp_pix->a * A_SCALE);
        quant_pix->b = round(comp_pix->b * BCD_SCALE);
        quant_pix->c = round(comp_pix->c * BCD_SCALE);
        quant_pix->d = round(comp_pix->d * BCD_SCALE);
        quant_pix->PbAverage = Arith40_index_of_chroma(comp_pix->PbAverage);
        quant_pix->PrAverage = Arith40_index_of_chroma(comp_pix->PrAverage);
}
/*
 * Name: dequantize 
 * Purpose: dequantize the coefficients and the Pb Average and Pr Average 
            values of a quantized pixel 
 * Parameters: a compressed pixel struct and the quantized pixel struct.
 * Returns: nothing
 * Effects: none
 * Error Cases: if any of the structs are NULL, the program will terminate
                with a checked runtime error.      
*/
void dequantize(compressed_pix comp_pix, quantized quant_pix) 
{
        assert(comp_pix != NULL);
        assert(quant_pix != NULL);
        /* dequantize the values stored in the quantized struct and store
           them into the inputted compressed_pix struct */
        comp_pix->a = (float) quant_pix->a / A_SCALE;
        comp_pix->b = (float) quant_pix->b / BCD_SCALE;
        comp_pix->c = (float) quant_pix->c / BCD_SCALE;
        comp_pix->d = (float) quant_pix->d / BCD_SCALE;
        comp_pix->PbAverage = Arith40_chroma_of_index(quant_pix->PbAverage);
        comp_pix->PrAverage = Arith40_chroma_of_index(quant_pix->PrAverage);
}
/*
 * Name: rgb_adjuster 
 * Purpose: ensure that the values within the float_rgb struct fall 
            within the range 0 - 1
 * Parameters: a float_rgb struct
 * Returns: nothing
 * Effects: none
 * Error Cases: if the inputted struct is NULL, the program will terminate
                with a checked runtime error.      
*/
void rgb_adjuster(float_rgb curr_pix) 
{
        assert(curr_pix != NULL);
        if(curr_pix->r > RED_U_LIM) {
                curr_pix->r = RED_U_LIM;
        }
        if (curr_pix->r < RED_L_LIM) {
                curr_pix->r = RED_L_LIM;
        }
        if(curr_pix->g > GREEN_U_LIM) {
                curr_pix->g = GREEN_U_LIM;
        }
        if (curr_pix->g < GREEN_L_LIM) {
                curr_pix->g = GREEN_L_LIM;
        }
        if(curr_pix->b > BLUE_U_LIM) {
                curr_pix->b = BLUE_U_LIM;
        }
        if (curr_pix->b < BLUE_L_LIM) {
                curr_pix->b = BLUE_L_LIM;
        }
}
/*
 * Name: pixel_adjuster 
 * Purpose: ensure that the values within the compressed_pixel struct fall 
            within the range -0.3 to 0.3 for b, c, and d and 0 - 1 for a
 * Parameters: a compressed_pixel struct
 * Returns: nothing
 * Effects: none
 * Error Cases: if the inputted struct is NULL, the program will terminate
                with a checked runtime error.      
*/
void pixel_adjuster(compressed_pix curr_pix) 
{
        assert(curr_pix != NULL);
        if(curr_pix->a > A_HIGH)
                curr_pix->a = A_HIGH;
        if(curr_pix->a < A_LOW)
                curr_pix->a = A_LOW;
        if(curr_pix->b > B_HIGH) 
                curr_pix->b = B_HIGH;
        if(curr_pix->b < B_LOW) 
                curr_pix->b = B_LOW;
        if(curr_pix->c > C_HIGH) 
                curr_pix->c = C_HIGH;
        if(curr_pix->c < C_LOW) 
                curr_pix->c = C_LOW;
        if(curr_pix->d > D_HIGH) 
                curr_pix->d = D_HIGH;
        if(curr_pix->d < D_LOW) 
                curr_pix->d = D_LOW;
}
/*
 * Name: comp_video_adjuster 
 * Purpose: ensure that the values within the comp_video struct fall 
            within the range -0.5 to 0.5 for Pb and Pr and 0 - 1 for Y
 * Parameters: a comp_video struct
 * Returns: nothing
 * Effects: none
 * Error Cases: if the inputted struct is NULL, the program will terminate
                with a checked runtime error.      
*/
void comp_video_adjuster(comp_video curr_vid) 
{
        assert(curr_vid != NULL);
        if (curr_vid->y > Y_HIGH) {
                curr_vid->y = Y_HIGH;
        }  
        if (curr_vid->y < Y_LOW) {
                curr_vid->y = Y_LOW;   
        }
        if (curr_vid->pr > PR_HIGH) {
                curr_vid->pr = PR_HIGH;
        }
        if (curr_vid->pr < PR_LOW) {
                curr_vid->pr = PR_LOW;
        }
        if (curr_vid->pb > PB_HIGH) { 
                curr_vid->pb = PB_HIGH;
        }
        if (curr_vid->pb < PB_LOW) {
                curr_vid->pb = PB_LOW;
        }
}
/*
 * Name: compressed_to_vid
 * Purpose: converts the inputted compressed_pixel to 4 component video pixels
 * Parameters: a compressed_pixel struct
 * Returns: a UArray of 4 component video pixels
 * Effects: none
 * Error Cases: if the inputted struct is NULL, the program will terminate
                with a checked runtime error. Additionally, if the created
                UArray is NUll, the program will terminate with a checked
                runtime error     
*/
UArray_T compressed_to_vid(compressed_pix compressed_pixel) 
{
        assert(compressed_pixel != NULL);

        UArray_T vid_pixel = UArray_new(4, sizeof(struct comp_video));
        assert(vid_pixel != NULL);
        /* create an array of comp_video structs and populate that array
           with 4 comp_video structs */
        comp_video curr_vid_pix[4];
        curr_vid_pix[0] = comp_vid_populate(compressed_pixel, 1);
        curr_vid_pix[1] = comp_vid_populate(compressed_pixel, 2);
        curr_vid_pix[2] = comp_vid_populate(compressed_pixel, 3);
        curr_vid_pix[3] = comp_vid_populate(compressed_pixel, 4);
        /* store the structs in the array into the created UArray*/
        for (int pix_index = 0; pix_index < 4; pix_index++) {

                comp_video curr_pix = (comp_video)UArray_at(vid_pixel, 
                                      pix_index);

                assert(curr_pix != NULL);

                *curr_pix = *(curr_vid_pix[pix_index]);
                free(curr_vid_pix[pix_index]);
        }
        return vid_pixel;
}
/*
 * Name: comp_to_vid_populate
 * Purpose: creates a component video pixel from the inputted compressed pixel
 * Parameters: a compressed_pixel struct, integer representing the location 
               of the pixel in the 2 x 2 block
 * Returns: a comp_video struct representing the component video pixel
 * Effects: allocates memory for a comp_video struct 
 * Error Cases: if the inputted struct is NULL, the program will terminate
                with a checked runtime error. Additionally, if the created
                comp_video is NUll, the program will terminate with a checked
                runtime error     
*/
comp_video comp_vid_populate(compressed_pix compressed_pixel, int num_pix) 
{
        assert(compressed_pixel != NULL);

        comp_video curr_pixel = malloc(sizeof(struct comp_video));
        assert(curr_pixel != NULL);
        /* populate created comp_video struct*/
        curr_pixel->pb = compressed_pixel->PbAverage;
        curr_pixel->pr = compressed_pixel->PrAverage;
        /* calculate the correct y-value depending on the location of the
           pixel in the 2x2 block */
        if (num_pix == 1) {
                curr_pixel->y = compressed_pixel->a - compressed_pixel->b - 
                                compressed_pixel->c + compressed_pixel->d;      
        }
        else if (num_pix == 2) {
                curr_pixel->y = compressed_pixel->a - compressed_pixel->b + 
                                compressed_pixel->c - compressed_pixel->d;
        }
        else if (num_pix == 3) {
                curr_pixel->y = compressed_pixel->a + compressed_pixel->b - 
                                compressed_pixel->c - compressed_pixel->d;
        }
        else {
                curr_pixel->y = compressed_pixel->a + compressed_pixel->b + 
                                compressed_pixel->c + compressed_pixel->d;
        }
        comp_video_adjuster(curr_pixel);
        return curr_pixel;
        
}
/*
 * Name: comp_video_to_rgb_apply
 * Purpose: apply function that converts each pixel iterated over to
            a float_rgb pixel
 * Parameters: two integer representing the coordinates (col, row) of the
               current element, an A2Methods_UArray2 that holds the component
               video pixels of the image, void pointers for the current 
               element and the closure variable.
 * Returns: nothing
 * Effects: none
 * Error Cases: if the array inputted into the function is NULL
                the program will terminate with a checked runtime error.
                Additionally, if the created methods suit is NULL, the program
                will terminate with a checked runtime error. Lastly, if any of
                the structs are NULL, the program will terminate with a
                checked runtime error.       
*/
void comp_video_to_rgb_apply(int col, int row, 
                             A2Methods_UArray2 comp_video_pixels, void *elem,
                             void *cl) 
{
        assert(comp_video_pixels != NULL);
        
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);

        A2Methods_UArray2 float_rgb_pixels = (A2Methods_UArray2) cl;
        assert(float_rgb_pixels != NULL);

        comp_video curr_vid_pixel = elem;
        assert(curr_vid_pixel != NULL);

        float_rgb curr_float_pixel = methods->at(float_rgb_pixels, col, row);
        assert(curr_float_pixel != NULL);
        /* populate float pixel by converting the values of the component
           video pixel to RGB values */
        curr_float_pixel->r = 1.0 * curr_vid_pixel->y + 
                              0.0 * curr_vid_pixel->pb + 
                              1.402 * curr_vid_pixel->pr;

        curr_float_pixel->g = 1.0 * curr_vid_pixel->y - 
                              0.344136 * curr_vid_pixel->pb - 
                              0.714136 * curr_vid_pixel->pr;

        curr_float_pixel->b = 1.0 * curr_vid_pixel->y + 
                              1.772 * curr_vid_pixel->pb + 
                              0.0 * curr_vid_pixel->pr;

        /* make sure that the RGB values are within the range 0 - 1 */
        rgb_adjuster(curr_float_pixel);
        /* store the populated float_pixel struct */
        *(float_rgb)methods->at(float_rgb_pixels, 
                                col, row) = *curr_float_pixel; 
}
/*
 * Name: float_rgb_to_pnm_apply
 * Purpose: apply function that converts each pixel iterated over to
            a Pnm_rgb pixel
 * Parameters: two integer representing the coordinates (col, row) of the
               current element, an A2Methods_UArray2 that holds the float
               rgb pixels of the image, void pointers for the current 
               element and the closure variable.
 * Returns: nothing
 * Effects: none
 * Error Cases: if the array inputted into the function is NULL
                the program will terminate with a checked runtime error.
                Additionally, if the created methods suit is NULL, the program
                will terminate with a checked runtime error. Lastly, if any of
                the structs are NULL, the program will terminate with a
                checked runtime error.       
*/
void float_rgb_to_pnm_apply(int col, int row, 
                            A2Methods_UArray2 float_rgb_pixels, void *elem, 
                            void *cl) 
{
        assert(float_rgb_pixels != NULL);

        A2Methods_T methods =  uarray2_methods_blocked;
        assert(methods != NULL);

        A2Methods_UArray2 pnm_rgb_pixels = (A2Methods_UArray2) cl;
        assert(pnm_rgb_pixels != NULL);
         
        float_rgb curr_float_pixel = elem;

        Pnm_rgb curr_pixel = methods->at(pnm_rgb_pixels, col, row);
        /* populate Pnm_rgb struct with unscaled RGB values */
        curr_pixel->red = curr_float_pixel->r * (unsigned) DENOMINATOR;
        curr_pixel->blue = curr_float_pixel->b * (unsigned) DENOMINATOR;
        curr_pixel->green = curr_float_pixel->g * (unsigned) DENOMINATOR;
        /* store the populated Pnm_rgb struct into the array */
        *((Pnm_rgb)methods->at(pnm_rgb_pixels, col, row)) = *curr_pixel;
        (void) float_rgb_pixels;
}