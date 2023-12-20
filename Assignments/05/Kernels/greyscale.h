
#ifndef __GREYSCALE_H__
#define __GREYSCALE_H__

#include "Color.h"

__global__
void greyscale(size_t width, size_t height, Color* color, Byte* greyscale) {

    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= width || y >= height) return;

    int index = x + width * y;

    Color& c = color[index];
    greyscale[index] = (c.r * .299) + (c.g * .587) + (c.b * .114); //

    __syncthreads();
}

#endif // __GREYSCALE_H__
