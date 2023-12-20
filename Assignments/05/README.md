# Basic Image Processing using GPU Computing

In this exercise, we'll use CUDA to do some simple image processing, in particular, highlight the edges in an image.

A number of techniques like this use a mathematical concept called a _convolution_.  Convolutions are just arrays of numbers (called **kernels** [just to be extra confusing] usually matching the dimensionality of the data you're working with; for us with an image, it'll be a 2D array of values).  Conceptually, a convolution "looks around" a particular data value (a pixel in this use case) and using information about the neighbors of that pixel, along with its data, generates a value.

For edge detection, a kernel looks for difference between pixel values.  If a collection of pixels all have the same values, there won't be any noticeable difference, so we might mark that with a zero (which conveniently maps to black).  If you consider a 2D grid and pick a particular point, you can look at its neighbors and see if there are any changes in the "neighborhood".  The changes in the neighborhood is called the _gradient_, and in our case the convolution measures the change in the gradient.

For a 2D data set, any particular point (except along the edges) will have eight neighbors.  This gives us eight values to use to determine how the values change in the neighborhood of our pixel.  If we're looking for changes in the horizontal direction, we'd likely have a kernel with columns of values all having the same values.  Similarly, if we're looking for vertical direction changes, we'd have values in the rows.  For this exercise, we're going to look for differences along any direction from our pixel.  In particular, our kernel is going to be:

$$
k = \left(
\begin{matrix}
-1 & -1 & -1 \\
-1 & 8 & -1 \\
-1 & -1 & -1 \\
\end{matrix}
\right)
$$

Note that the sum of all of the kernel elements equals zero.  This indicates that the kernel doesn't add or remove any "energy" from the image.  Not all convolution kernels are constructed in this way, but it's something to note.

Applying a kernel to data is pretty simple, although writing it as an equation can look complicated.  For our specific case (given it's a $3 \times 3$ kernel):

$$
p_{xy} = \sum_{i = -1}^{1} \sum_{j = -1}^{13} p_{(x+i)(y+j)} k_{ij}
$$ 

What that says (in case you're math phobic) is for the $3 \times 3$ set of pixels centered at location $(x,y)$ in the image
 1. "flip" them over in the vertical and horizontal directions
 2. overlay the kernel
 3. multiply each pair of values (one pixel and one kernel value)
 4. add up the products
 5. and that's your new value

## How we'll do this in CUDA

For this project, there are two sample images:
 * **Chanelly.ppm** who was one of my kitties
 * **Fox.ppm** which is a frame of video of a fox walking across my front doorway

The project includes the "driver" program (all the boring stuff) to load the PPM into an array of data, load it into the GPU, and even sets up calling the kernels.  Your role will be to complete the implementation of three kernels.

1. The first kernel, `greyscale` converts RGB pixels to a greyscale value.  Often we'll do this to make working with the gradient simpler.  This process is very simple: for each individual pixel, take the linear combination of the pixel's RGB values with the coefficients for mapping RGB to grey $(0.299, 0.587, 0.114)$.  You could get fancy, and convert the RGB value into a CUDA `dim3` value, make the above coefficients also into a `dim3` vector, and take the dot (inner) product.  Or, you could just compute $0.299 * red + 0.587 * green + 0.114 * blue$ and get grey.  It's a one-liner kernel, but very helpful.

2. The second convolution computes the convolution, and you're asked to write the summation statement in the loops that iterate across the kernel and pixels.  The complicated part of doing this in CUDA is that the image is presented as a one-dimensional array, and not a 2D image, so you'd need to convert from an $(x, y)$ value into a linear-array index.  But since I'm such a nice guy, I've done that for you, along with a magic Lambda function (named `index`) that will determine pixel locations around the center pixel correctly.<br/><br/>`index()` will return a negative value if the pixel's outside of the image, which we can use to determine if we're reading outside of the image (in which case, we just assume the pixel's color value is black [zero]).<br/><br/>Given all that, the kernel will look something like:

```c++
__global__
void convolve(size_t width, size_t height, Byte* greyscale, Byte* edges) {

    // Determine the pixel's coordinates
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    // Do bound checking
    if (x >= width || y >= height) return;

    // Define our kernel
    const int kernel[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };

    // Our index() function that takes an offset from
    //  the current (x, y) position, and returns either
    //  - the index of the pixel in the image
    //  - or -1 if the offset position would be outside the image
    //
    // (while this is a CUDA kernel, this is a C++ lambda that
    //   uses closures, just like in our CPU-based C++)
    //
    auto index = [=](int xOffset, int yOffset) -> int { 
        auto xPos = x + xOffset;
        auto yPos = y + yOffset;
        if (xPos < 0 || xPos > width)  return -1;
        if (yPos < 0 || yPos > height) return -1;

        return  (yPos * width) + xPos; 
    };

    // Our convolution computation
    int sum = 0;
    for (int j = -1; j <= 1; ++j) {
        for (int i = -1; i <= 1; ++i) {
            // Compute the offseted index required to be
            //   combined with the kernel
            int idx = index(i, j);

            // Make sure we're working with a valid pixel
            if (idx < 0) continue;

            // Compute the convolution sum.  You're asked
            //  to fill in the following line.  "idx" 
            //  can be used to index into the "greyscale"
            //  array, and with just a bit of cleverness
            //  you can figure out how to index into the
            //  kernel array. 
            sum += ...
        }
    }

    // Clamp the resulting values
    sum = sum < 0 ? 0: sum;
    sum = sum > 255 ? 255 : sum;

    // Store the results
    auto center = index(0, 0);
    edges[center] = sum;

    __syncthreads();
}
```

1. Finally, we'll threshold the resulting edges image.  Recall the edge convolution looks for changes between neighbors.  When two neighbors are close in values, the result is pretty much zero, so we want to emphasize where the differences (edges) are, and filter out the noise.  The `threshold` kernel does this (although, practically speaking all of these kernels could have been folded into a single kernel). <br/><br/>For this last step, find a threshold value (which is set in the main driver program **edge\.cu**) that controls the filtering value (in an aptly named variable `thresholdValue`).  Find a value you like.  Larger ones will show the most prominent edges; smaller values will show many more edges.

## Submitting for Credit

**edge\.cu** will output a PPM file named **Out.ppm** containing the resulting final image.

I will check your Github repository on Wednesday, 20 December before I submit final grades.  If you include this project in your repo with a final image, I'll award five extra credit points onto the project part of the grade computation.