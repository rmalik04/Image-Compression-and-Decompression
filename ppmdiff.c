#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress40.h"
#include <string.h>
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include <math.h>
#include <stdbool.h>

double compare_pix(Pnm_ppm pixmap_1, Pnm_ppm pixmap_2);
bool image_checker(Pnm_ppm pixmap_1, Pnm_ppm pixmap_2);
int main(int argc, char *argv[]) 
{
        if (argc <= 1) {
                fprintf(stderr, "Not enough arguments\n");
        }
        else if (argc > 1 && argc < 4) {
                /* default to UArray2 methods */
                A2Methods_T methods = uarray2_methods_plain; 
                assert(methods);
                /* default to best map */
                A2Methods_mapfun *map = methods->map_default; 
                assert(map);

                FILE *inputfd_1;
                FILE *inputfd_2;
                if (strcmp(argv[1],"-") == 0) {
                        inputfd_1 = stdin;
                        assert(inputfd_1 != NULL);
                        inputfd_2 = fopen(argv[2], "r");
                        assert(inputfd_2 != NULL);
                }
                else if (strcmp(argv[2],"-") == 0) {
                        inputfd_1 = fopen(argv[1], "r");
                        assert(inputfd_1 != NULL);
                        inputfd_2 = stdin;
                        assert(inputfd_2 != NULL);
                }
                else {
                        inputfd_1 = fopen(argv[1], "r");
                        assert(inputfd_1 != NULL);
                        inputfd_2 = fopen(argv[2], "r");
                        assert(inputfd_2 != NULL);
                }
                Pnm_ppm pixmap_1 = Pnm_ppmread(inputfd_1, methods);
                Pnm_ppm pixmap_2 = Pnm_ppmread(inputfd_2, methods);
                assert(pixmap_1 != NULL);
                assert(pixmap_2 != NULL);
                bool dimension_checker = image_checker(pixmap_1, pixmap_2);
                if (dimension_checker == false) {
                        fprintf(stderr, "dimensions are too different");
                        exit(1);
                }
                double diff = compare_pix(pixmap_1, pixmap_2);
                printf("%f\n", diff);
                Pnm_ppmfree(&pixmap_1);
                Pnm_ppmfree(&pixmap_2);
                fclose(inputfd_1);
                fclose(inputfd_2);
                
        }
        else {
                fprintf(stderr, "Too many arguments\n");
        }
        return 0;
}

bool image_checker(Pnm_ppm pixmap_1, Pnm_ppm pixmap_2) {
        int diff_height = (int) pixmap_1->height - (int) pixmap_2->height;
        int diff_width = (int) pixmap_1->width - (int) pixmap_2->width;

        if ((abs(diff_height)) > 1 || ((abs(diff_width) > 1))) {
                return false;
        }
        else {
                return true;
        }

}
double compare_pix(Pnm_ppm pixmap_1, Pnm_ppm pixmap_2) 
{
        unsigned int height = 0;
        unsigned int width = 0;

        if (pixmap_1->height > pixmap_2->height) {
                height = pixmap_2->height;
        }
        else {
                height = pixmap_1->height;
        }
        if (pixmap_1->width > pixmap_2->width) {
                width = pixmap_2->width;
        }
        else {
              width = pixmap_1->width;  
        }
        double sum = 0.0;
        for (unsigned int row = 0; row < height; row++) {
                for (unsigned int col = 0; col < width; col++) {
                        Pnm_rgb pixel_1 = (pixmap_1->methods)->at(pixmap_1->pixels, 
                        col, row);
                        Pnm_rgb pixel_2 = (pixmap_2->methods)->at(pixmap_2->pixels, 
                        col, row);
                        double red = (double) pixel_1->red / 255 - (double) pixel_2->red / 255;
                        double green = (double) pixel_1->green / 255 - (double) pixel_2->green / 255;
                        double blue = (double) pixel_1->blue / 255 - (double) pixel_2->blue / 255;
                        sum = sum + (red * red) + (green * green) + 
                        (blue * blue);
                
                }
        }
        sum = sum / (3 * pixmap_1->height * pixmap_1->width);
        return sqrt(sum);

}
