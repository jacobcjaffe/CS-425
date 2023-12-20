
#ifndef __CONVOLVE_H__
#define __CONVOLVE_H__

#include "Color.h"

__global__
void convolve(size_t width, size_t height, Byte* greyscale, Byte* edges) {

    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= width || y >= height) return;

    const int kernel[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };

    auto index = [=](int xOffset, int yOffset) -> int { 
        auto xPos = x + xOffset;
        auto yPos = y + yOffset;
        if (xPos < 0 || xPos > width)  return -1;
        if (yPos < 0 || yPos > height) return -1;

        return  (yPos * width) + xPos; 
    };

    int sum = 0;
    for (int j = -1; j <= 1; ++j) {
        for (int i = -1; i <= 1; ++i) {
            int idx = index(i, j);
            if (idx < 0) continue;
            sum += greyscale[idx] * kernel[i + 1][j + 1]; // code here
        }
    }

    sum = sum < 0 ? 0: sum;
    sum = sum > 255 ? 255 : sum;

    auto pos =  x + y * width;
    edges[pos] = sum;

    __syncthreads();
}

#endif // __CONVOLVE_H__
