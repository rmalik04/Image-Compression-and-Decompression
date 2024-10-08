/*
******************************************************************************
* Name: Krish Savla (ksavla01), Ranvir Malik (rmalik04)
* Assignment: HW4 - arith
* Date: 10/26/2022
* File name: new_pnm.c
*
* This file defines a function that creates an empty Pnm_ppm object
* and populates it using the inputted array
*
******************************************************************************
*/
#include <a2methods.h>
#include "a2blocked.h"
#include "pnm.h"
#include "new_pnm.h"
#include "mem.h"
#include <stdlib.h>
#include "constants.h"
#include "assert.h"

/*
 * Name: new_pnm.c
 * Purpose: creates and populates a new Pnm_ppm object based on the inputted
            array of pixels 
 * Parameters: an A2Methods_UArray2 containing the pixels of the image 
 * Returns: a populated Pnm_ppm object
 * Effects: allocates memory for a new Pnm_ppm object
 * Error Cases: If the inputted array is NULL, if the created methods suite
                is NULL, or if  the created Pnm_ppm object is NULL, the
                program will terminate with a checked runtime error.    
*/
Pnm_ppm new_pnm(A2Methods_UArray2 pnm_pixels) 
{
        assert(pnm_pixels != NULL);
        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        Pnm_ppm original_image = malloc(sizeof(struct Pnm_ppm));
        assert(original_image != NULL);
        /* populate the Pnm_ppm object using information from the inputted 
           array */
        original_image->width = methods->width(pnm_pixels);
        original_image->height = methods->height(pnm_pixels), 
        original_image->denominator = DENOMINATOR;
        original_image->pixels = pnm_pixels; 
        original_image->methods = methods;
        return original_image;
}
