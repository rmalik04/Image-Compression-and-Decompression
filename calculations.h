/*
******************************************************************************
* Name: Krish Savla (ksavla01), Ranvir Malik (rmalik04)
* Assignment: HW4 - arith
* Date: 10/26/2022
* File name: calculations.h
*
* Declares all the functions defined in the calculations.c module. These
* functions handle all the arithmetic for converting pixels during compression
* and decompression
*
******************************************************************************
*/
#include <stdio.h>
#include <uarray2b.h>
#include <a2methods.h>
#include "uarray.h"
#include "compressed_pix.h"
#include "float_rgb.h"
#include "comp_video.h"
#include "quantized.h"


extern void scale_float_apply(int col, int row, A2Methods_UArray2 pixels, 
                              void *elem, void *cl);
extern void rgb_to_vid_apply(int col, int row, 
                             A2Methods_UArray2 scaled_pixels, void *elem, 
                             void *cl);
extern compressed_pix vid_to_compress(UArray_T block);
extern void bit_to_compressed_apply(int col, int row, A2Methods_UArray2 bit32,
                                    void *elem, void *cl);
extern UArray_T compressed_to_vid(compressed_pix compressed_pixel);
extern comp_video comp_vid_populate(compressed_pix compressed_pixel, 
                                    int num_pix);
extern void comp_video_to_rgb_apply(int col, int row, 
                                    A2Methods_UArray2 comp_video_pixels, 
                                    void *elem, void *cl);
extern void float_rgb_to_pnm_apply(int col, int row, 
                                   A2Methods_UArray2 float_rgb_pixels, 
                                   void *elem, void *cl);
extern void quantize_apply(int col, int row, A2Methods_UArray2 comp_pixels, 
                           void *elem, void *cl);
extern void dequantize_apply(int col, int row, A2Methods_UArray2 quant_pixels,
                             void *elem, void *cl);
extern void bitpack_apply(int col, int row, A2Methods_UArray2 quant_pixels, 
                          void *elem, void *cl);
extern void unbitpack_apply(int col, int row, A2Methods_UArray2 bits, 
                            void *elem, void *cl);

