
#ifndef __THRESHOLD_H__
#define __THRESHOLD_H__

#include "Color.h"

__global__
void threshold(size_t width, size_t height, Byte* edges, Color* colors, Byte thresholdValue) {

    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= width || y >= height) return;

    auto index = y * width + x;

    auto value = edges[index];
    colors[index] = value > thresholdValue ? Color(255, 0,  0) : Color(value);

    __syncthreads();
}

#endif // __THRESHOLD_H__