/*
******************************************************************************
* Name: Krish Savla (ksavla01), Ranvir Malik (rmalik04)
* Assignment: HW4 - arith
* Date: 10/26/2022
* File name: arithio.h
*
* Contains the function headers for all the function defined in arithio.c 
*
******************************************************************************
*/
#include <stdio.h>
#include <pnm.h>
#include <uarray2b.h>
#include <a2methods.h>


extern Pnm_ppm read_image(FILE *inputfd, A2Methods_T methods);
extern Pnm_ppm compressed_image_reader(FILE *inputfd, A2Methods_T methods);
extern void print_compressed(A2Methods_UArray2 bits);
extern void print_decompressed(Pnm_ppm pixmap);
extern A2Methods_UArray2 compressed_image_read(FILE *inputfd);
extern void print_comp_apply(int col, int row, A2Methods_UArray2 bits, 
                             void *elem, void *cl);
extern void compressed_image_apply(int col, int row, A2Methods_UArray2 bits, 
                                   void *elem, void *cl);