/*
******************************************************************************
* Name: Krish Savla (ksavla01), Ranvir Malik (rmalik04)
* Assignment: HW4 - arith
* Date: 10/26/2022
* File name: decompressor.h
*
* Declares all the functions that are used in the decompressor module.
*
******************************************************************************
*/


#include <a2methods.h>
#include <uarray2b.h>
#include "a2plain.h"
#include "a2blocked.h"
#include "calculations.h"


extern Pnm_ppm decompressor_main(A2Methods_UArray2 bits);
extern A2Methods_UArray2 video_convert(A2Methods_UArray2 compressed_pixels);
extern void populate_comp_vid(A2Methods_UArray2 comp_video_pixels, 
                              compressed_pix curr_pix, int col, int row);