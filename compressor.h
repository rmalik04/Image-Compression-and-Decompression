/*
******************************************************************************
* Name: Krish Savla (ksavla01), Ranvir Malik (rmalik04)
* Assignment: HW4 - arith
* Date: 10/26/2022
* File name: compressor.h
*
* Declares all the functions that are used in the compressor module.
*
******************************************************************************
*/

#include "pnm.h"


extern A2Methods_UArray2 compressor_main(Pnm_ppm pixmap);
extern A2Methods_UArray2 block_to_pix(A2Methods_UArray2 vid, Pnm_ppm pixmap);
extern void free_data(A2Methods_UArray2 quant_pixels, A2Methods_UArray2 
                      scaled_pixels, A2Methods_UArray2 compressed_pixels,
                      A2Methods_UArray2 vid, Pnm_ppm pixmap);