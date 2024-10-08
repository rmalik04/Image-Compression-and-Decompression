/*
******************************************************************************
* Name: Krish Savla (ksavla01), Ranvir Malik (rmalik04)
* Assignment: HW4 - arith
* Date: 10/26/2022
* File name: quantize.h
*
* Header file that defines the quantize struct and its member variables
*
******************************************************************************
*/

typedef struct quantized *quantized;
struct quantized {
        signed b, c, d;
        unsigned a, PbAverage, PrAverage;
};      
