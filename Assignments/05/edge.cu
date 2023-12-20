
#include <sys/mman.h>

#include <iostream>
#include <fstream>
#include <span>
#include <sstream>
#include <string>

using Byte = unsigned char;

#define DEBUG 

#include "CudaCheck.h"
#include "Image.h"

#include "Kernels/Color.h"
#include "Kernels/greyscale.h"
#include "Kernels/convolve.h"
#include "Kernels/threshold.h"

int main(int argc, char* argv[]) {
    const char* Testfile = "Fox.ppm";
    const char* filename = argc > 1 ? argv[1] : Testfile;

    RGBImage img = readRGBImage(filename);

    const size_t width = img.width;
    const size_t height = img.height;
    const size_t numBytes = img.size();
    const size_t numPixels = img.numPixels(); 

    Color* gpuColor;
    CUDA_CHECK_CALL(cudaMalloc(&gpuColor, numBytes));
    CUDA_CHECK_CALL(cudaMemcpy(gpuColor, img.data(), numBytes, cudaMemcpyHostToDevice));

    Byte* gpuGreyscale;
    CUDA_CHECK_CALL(cudaMalloc(&gpuGreyscale, numPixels));

    dim3 blockDim(16, 16);
    dim3 numBlocks(width/blockDim.x + 1, height/blockDim.y + 1);
    greyscale<<<numBlocks, blockDim>>>(width, height, gpuColor, gpuGreyscale);

    Byte* gpuEdges;
    CUDA_CHECK_CALL(cudaMalloc(&gpuEdges, numPixels));
    convolve<<<numBlocks, blockDim>>>(width, height, gpuGreyscale, gpuEdges);

    const Byte thresholdValue = 40;
    threshold<<<numBlocks, blockDim>>>(width, height, gpuEdges, gpuColor, thresholdValue);

    Color* results = new Color[numPixels];
    CUDA_CHECK_CALL(cudaMemcpy(results, gpuColor, numBytes, cudaMemcpyDeviceToHost));

    std::ofstream outstream("Out.ppm");
    outstream << RGBImage(width, height, results);
}
