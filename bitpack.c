/*
******************************************************************************
* Name: Krish Savla (ksavla01), Ranvir Malik (rmalik04)
* Assignment: HW4 - arith
* Date: 10/26/2022
* File name: bitpack.c
*
* Defines all the function necessary for the step of compressions that
* requires bitpacking 
*
******************************************************************************
*/

#include "bitpack.h"
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include "except.h"

Except_T Bitpack_Overflow = { "Overflow packing bits" };
/*
 * Name: shift_right
 * Purpose: shifts the inputted bitstring right by the inputted
            number of spaces
 * Parameters: two unsigned int 64 bits, the first which is the
               bitstring containing the data, and the other which is the
               number of spaces the bit needs to be shifted to the right
 * Returns: The shifted unsigned int 64 bit
 * Effects: none
 * Error Cases: if the inputted bit that represents the number of spaces to 
                shift the bit is greater than or equal to 64, the program 
                will terminate with a checked runtime error. 
*/
uint64_t shift_right(uint64_t n, unsigned units) 
{
        assert(units <= 64);
        if (units == 64)
                return 0;
        else
                return n >> units;
}
/*
 * Name: shift_left
 * Purpose: shifts the inputted bitstring left by the inputted
            number of spaces
 * Parameters: two unsigned int 64 bits, the first which is the
               bitstring containing the data, and the other which is the
               number of spaces the bit needs to be shifted to the left
 * Returns: The shifted unsigned int 64 bit
 * Effects: none
 * Error Cases: if the inputted bit that represents the number of spaces to 
                shift the bit is greater than or equal to 64, the program 
                will terminate with a checked runtime error. 
*/
uint64_t shift_left(uint64_t n, unsigned units) 
{
        assert(units <= 64);
        if (units == 64)
                return 0;
        else
                return n << units;
}
/*
 * Name: Bitpack_fitsu
 * Purpose: checks if the unsigned number entered would fit in the 
            number of bits entered ("width")
 * Parameters: an unsigned 64 bit integer and an unsigned width that
               represents the number of bits
 * Returns: a boolean that indicates whether the inputted 64 bit integer can
            fit within the inputted number of bits
 * Effects: none
 * Error Cases: if the width is greater than 64, then the program will
                terminate with a checked runtime error
*/
bool Bitpack_fitsu(uint64_t n, unsigned width) 
{
        assert(width <= 64);
        if (width == 0) {
                return false;
        }
        /* if we shift to the right by width, everything should go away and we
        should be left with only 0s */
        bool fit = (shift_right(n, width) == 0);
        return fit;
}
/*
 * Name: Bitpack_fitss
 * Purpose: checks if the signed number entered would fit in the 
            number of bits entered ("width")
 * Parameters: an signed 64 bit integer and an unsigned width that
               represents the number of bits
 * Returns: a boolean that indicates whether the inputted 64 bit integer can
            fit within the inputted number of bits
 * Effects: none
 * Error Cases: if the width is greater than 64, then the program will
                terminate with a checked runtime error
*/
bool Bitpack_fitss(int64_t n, unsigned width) 
{
        assert(width <= 64);
        if (width == 0) {
                return false;
        }
        /* shifting the entire bit to the right so we have all 0s or all 1s*/
        int64_t limit = n >> (width - 1); 
        
        /* if the number is positive check if the bits after right shifting 
        are equal to zero */
        if (n >= 0) {
                if (limit == 0) {
                        return true;
                }
        }
        /* if the number is negative check if the bits after right shifting 
        are equal to -1 */
        if (n < 0) {
                if (limit == -1) {
                        return true;
                }
        }
        return false;
}
/*
 * Name: Bitpack_getu
 * Purpose: extracts an unsigned value from the inputted word given the width
            and least significant bit
 * Parameters: an unsigned 64 bit code word, an unsigned width (# of bits), 
               and an unsigned value representing the lsb
 * Returns: an unsigned 64 bit integer that reperesents the value extracted
            from the code word
 * Effects: none
 * Error Cases: if the width is greater than 64, then the program will
                terminate with a checked runtime error. Additionally, if the
                calculated shift is greater than 64, the program will
                terminate with a checked runtime error
*/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb) 
{
        assert(width <= 64);
        if (width == 0) 
                return 0;
        uint64_t shift = width + lsb;
        assert(shift <= 64);
        /* make the entire value go to the left, there is nothing to the left
        */
        uint64_t data = shift_left(word, 64 - shift);
        /* make the entire value go to the right, removing any data on the 
        right */
        data = shift_right(data, 64 - width);
        
        return data;
}
/*
 * Name: Bitpack_gets
 * Purpose: extracts an signed value from the inputted word given the width
            and least significant bit
 * Parameters: an unsigned 64 bit code word, an unsigned width (# of bits), 
               and an unsigned value representing the lsb
 * Returns: a signed 64 bit integer that reperesents the value extracted
            from the code word
 * Effects: none
 * Error Cases: if the width is greater than 64, then the program will
                terminate with a checked runtime error. Additionally, if the
                calculated shift is greater than 64, the program will
                terminate with a checked runtime error
*/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb) 
{
        assert(width <= 64);
        if (width == 0) 
                return 0;
        uint64_t shift = width + lsb;
        assert(shift <= 64);
        
        int64_t data_s; /* signed int to be returned */
        /* getting the required data */
        uint64_t data_u = Bitpack_getu(word, width, lsb);
        /* getting the msb */
        uint64_t msb = shift_left(1, width - 1);
        if (data_u < msb) {
                /* if positive */
                data_s = (int64_t) data_u;
        }
        else {
                /* if negative */
                data_s = (int64_t) (data_u - shift_left(1, width));
        }
        return data_s;

}
/*
 * Name: Bitpack_newu
 * Purpose: mutating a field in an unsigned bit so that the rest of the bit
            is unchanged.
 * Parameters: an unsigned 64 bit code word, an unsigned width (# of bits), 
               and an unsigned value representing the lsb, an unsigned 64 bit
               int that represents the value the user wants to store
 * Returns: an unsigned 64 bit integer that reperesents the new 64 bit int
            after the value is updated
 * Effects: none
 * Error Cases: if the width is greater than 64, then the program will
                terminate with a checked runtime error. Additionally, if the
                calculated shift is greater than 64, the program will
                terminate with a checked runtime error. If the value does fit
                in the number of bits inputted (width), the program will raise
                an exception. 
*/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                      uint64_t value)
{
        assert(width <= 64);
        unsigned shift = lsb + width;
        assert(shift <= 64);
        if (Bitpack_fitsu(value, width) == false)
                RAISE(Bitpack_Overflow);
        
        /* removing all information to the right of the required value, thus
        getting the starting or left index to store the word */
        uint64_t new_word = shift_right(word, shift);
        new_word = shift_left(new_word, shift);

        /* shifts all the information to the left of the lsb, thus getting the
        right index of the information*/
        uint64_t info = shift_left(word, 64 - lsb);
        info = shift_right(word, 64 - lsb);

        /* combining the two to get the place where word needs to go in the
        bit */
        new_word = new_word | info;

        /* puts the actual value that needs to be stored in the right location
        of the bit */
        uint64_t value_at_pos = shift_left(value, lsb);

        new_word = new_word | value_at_pos;

        return new_word;
        
}
/*
 * Name: Bitpack_news
 * Purpose: mutating a field in a signed bit so that the rest of the bit
            is unchanged.
 * Parameters: an unsigned 64 bit code word, an unsigned width (# of bits), 
               and an unsigned value representing the lsb, a signed 64 bit
               int that represents the value the user wants to store
 * Returns: an usigned 64 bit integer that reperesents the new 64 bit int
            after the value is updated
 * Effects: none
 * Error Cases: if the width is greater than 64, then the program will
                terminate with a checked runtime error. Additionally, if the
                calculated shift is greater than 64, the program will
                terminate with a checked runtime error. If the value does fit
                in the number of bits inputted (width), the program will raise
                an exception. 
*/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                      int64_t value)
{
        assert(width <= 64);
        unsigned shift = lsb + width;
        assert(shift <= 64);
        if (Bitpack_fitss(value, width) == false)
                RAISE(Bitpack_Overflow);
        
        /* getting all the values in width, including the signed bit */
        uint64_t data = Bitpack_getu(value, width, 0);

        /* simply putting in the signed value into the bit and returning the 
        entire new word, we treat the signed bit as a regular bit */
        uint64_t new_word = Bitpack_newu(word, width, lsb, data);
        return new_word;
}