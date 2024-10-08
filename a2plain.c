/*
******************************************************************************

* Name: Krish Savla (ksavla01), Lauren Langbort (llangb01)
* Assignment: HW3 - locality
* Date: 07/10/2022
* File name: a2plain.c
* abstract or virtual function pointers that allows us to implement inheritance
* without an OOP supporting language.

******************************************************************************
*/
#include <string.h>

#include <a2plain.h>
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

/*
 * Name: new
 * Purpose: creates a new A2Methods_UArray2
 * Parameters: an integer width, an integer height, and an integer size
 * Returns: a new empty A2Methods_UArray2 with the given width, height, and 
 * size
 * Effects: initializes a new UArray2 that is pointed to and used by 
 * A2Methods_UArray2 
 * Error Cases: handled in UArray2_new: it asserts the width, height, and size 
 * so if the size, width, or height are negative it will assert it
*/
static A2Methods_UArray2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}

/*
 * Name: new_with_blocksize
 * Purpose: creates a new A2Methods_UArray2
 * Parameters: an integer width, an integer height, an integer block size
 * and an integer size
 * Returns: a new empty UArray2 with the given width, height, and size
 * Effects: initializes a new UArray2 that is pointed to and used by 
 * A2Methods_UArray2, voids the block size since it is not applicable for 
 * a normal UArray2
 * Error Cases: handled in UArray2_new it asserts the width, height, and size 
 * so if the size, width, or height are negative it will assert it, voids block
 * size so it does not crash immediately
*/
static A2Methods_UArray2 new_with_blocksize(int width, int height, int size, 
                                            int blocksize)
{
        (void) blocksize;
        return UArray2_new(width, height, size);
}

                                
/*
 * Name: map_row_major
 * Purpose: goes through each row of the A2Methods_UArray2 and calls 
 * an apply function at each index of the A2Methods_UArray2
 * Parameters: A2Methods_UArray2 object called uarray2 and 
 * A2Methods_applyfun apply function, and a void pointer to cl/closure
 * Returns: nothing 
 * Effects: calls the map_row_major function in UArray2 so it will go through 
 * all the indexes of the UArray2 starting at the first row and call the apply
 * function on each index
 * Error Cases: handled in UArray2_map_row_major, if the UArray2 is  
 * NULL
*/
static void map_row_major(A2Methods_UArray2 uarray2,A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_row_major(uarray2, (UArray2_applyfun*) apply, cl);
}


/*
 * Name: map_col_major
 * Purpose: goes through each column of the A2Methods_UArray2 and calls 
 * an apply function at each index of the A2Methods_UArray2
 * Parameters: A2Methods_UArray2 object called uarray2 and 
 * A2Methods_applyfun apply function, and a void pointer to cl/closure
 * Returns: nothing 
 * Effects: calls the map_col_major function in UArray2 so it will go through 
 * all the indexes of the UArray2 starting at the first col and call the apply
 * function on each index
 * Error Cases: handled in UArray2_map_col_major, if the UArray2 is  
 * NULL
*/
static void map_col_major(A2Methods_UArray2 uarray2, A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2, (UArray2_applyfun*) apply, cl);
}


/*
 * Struct: small closure 
 * Purpose: holds the apply function and a pointer to closure 
 * so it can be passed in as the closure when called by a 
 * mapping function
 * Returns: A struct called small_closure
*/
struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void *cl;
};

/*
 * Name: apply_small
 * Purpose: creates a simple apply function that can be called by 
 * the mapping function
 * Parameters: an integer I, integer j, UArray2_T object called uarray2 
 * a void pointer called elem, and a void pointer called vcl
 * Effects: when the map function is called it will apply this function 
 * to every index of the uarray2
 * Error Cases: handled in the mapping functions, if the uarray2 is 
 * NULL and if the index is out of bounds
*/
static void apply_small(int i, int j, UArray2_T uarray2, void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}

/*
 * Name: small_map_row_major
 * Purpose: applies the small apply function to each index 
 * in row order
 * Parameters: an A2Methods_UArray2 object called a2, an 
 * A2Methods_smallapplyfun apply function, and a void pointer to 
 * cl/closure
 * Effects: calls the apply function in each index starting from the 
 * first row and going through the entire UArray2 using 
 * UArray2_map_row_major
 * Error Cases: handled in UArray2_map_row_major, if the uarray2 is 
 * NULL and if the index is out of bounds
*/
static void small_map_row_major(A2Methods_UArray2 a2, A2Methods_smallapplyfun 
                                apply, void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

/*
 * Name: small_map_col_major
 * Purpose: applies the small apply function to each index 
 * in col order
 * Parameters: an A2Methods_UArray2 object called a2, an 
 * A2Methods_smallapplyfun apply function, and a void pointer to 
 * cl/closure
 * Effects: calls the apply function in each index starting from the 
 * first col and going through the entire UArray2 using 
 * UArray2_map_col_major
 * Error Cases: handled in UArray2_map_col_major, if the uarray2 is 
 * NULL and if the index is out of bounds
*/
static void small_map_col_major(A2Methods_UArray2 a2, A2Methods_smallapplyfun 
                                apply, void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

/*
 * Name: a2free
 * Purpose: frees the A2Methods_UArray2
 * Parameters: pointer to A2Methods_UArray2 object called array2 
 * Returns: nothing 
 * Effects: calls the UArray2_free function in order to free/recycle 
 * the memory allocated for the A2Methods_UArray2/uarray2
 * Error Cases: handled in UArray2_free, if the UArray2 is  
 * NULL
*/
static void a2free(A2Methods_UArray2 * array2)
{
        UArray2_free((UArray2_T *) array2);
} 

/*
 * Name: width
 * Purpose: tells the width of the A2Methods_UArray2
 * Parameters: A2Methods_UArray2 object called array2 
 * Returns: an integer that holds the value of width
 * Effects: calls the UArray2_width function in order to gain
 * access to the width of the A2Methods_UArray2/uarray2
 * Error Cases: handled in UArray2_free, if the UArray2 is  
 * NULL
*/
static int width(A2Methods_UArray2 array2)
{
        return UArray2_width(array2);
}

/*
 * Name: height
 * Purpose: tells the height of the A2Methods_UArray2
 * Parameters: A2Methods_UArray2 object called array2 
 * Returns: an integer that holds the value of height 
 * Effects: calls the UArray2_height function n order to gain
 * access to the width of the A2Methods_UArray2/uarray2
 * Error Cases: handled in UArray2_free, if the UArray2 is  
 * NULL
*/
static int height(A2Methods_UArray2 array2)
{
        return UArray2_height(array2);
}

/*
 * Name: size
 * Purpose: tells the size of the elements in A2Methods_UArray2
 * Parameters: A2Methods_UArray2 object called array2 
 * Returns: an integer that holds the value of the size of the 
 * elements
 * Effects: calls the UArray2_size function n order to gain
 * access to the size of the elements in A2Methods_UArray2/uarray2
 * Error Cases: handled in UArray2_size, if the UArray2 is  
 * NULL
*/
static int size(A2Methods_UArray2 array2)
{
        return UArray2_size(array2);
}        

/*
 * Name: at
 * Purpose: gains access to the elements at a certain point of the
 * A2Methods_UArray2 given the index
 * Parameters: A2Methods_UArray2 object called array2, an integer
 * I and an integer j
 * Returns: a pointer to an A2Methods_Object
 * Effects: uses the UArray2_at function and passes in the array2 and
 * the integers to gain access to the elements or initialize elements at
 * a certain index
 * Error Cases: handled in UArray2_size, if the UArray2 is  
 * NULL and if the row/col passed in is out of the bounds of the 
 * UArray2
*/
static A2Methods_Object *at(A2Methods_UArray2 array2, int i, int j)
{
        return UArray2_at(array2, i, j);
}

/*
 * Name: blocksize
 * Purpose: returns the block size used in the A2Methods_UArray2
 * Parameters: A2Methods_UArray2 object called array2
 * Returns: an integer of the block size
 * Effects: since the array2 is using the UArray2 implementation 
 * there is no actual block size, so the block size defaults to 1
 * Error Cases: handled in UArray2_size, if the UArray2 is  
 * NULL 
*/
static int blocksize(A2Methods_UArray2 array2)
{
        (void) array2;
        return 1;
        
}

/*
 * Struct: uarray2_methods_plain_struct
 * Purpose: a struct of the type A2Methods_T that holds all 
 * the variables/functions used in a2plain to create the 
 * abstract pointer that can be used for a2plain and a2blocked
 * Returns: An A2Methods_T uarray2_methods_plain_struct
*/
static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize,
        at,
        map_row_major,                  
        map_col_major,                  
        NULL,
        map_row_major,  /* map_default */
        small_map_row_major,                    
        small_map_col_major,                    
        NULL,
        small_map_col_major     /* small_map_default */
};

/* here is the exported pointer to the struct */

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
