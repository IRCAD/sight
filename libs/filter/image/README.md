# filter_image library

`filter_image` library is a library related to 'filtering' : producing an output by 'filtering' an input. It includes registration, matrix optimization, image filtering, drawing functions, etc.

The list of included classes:

- AutomaticRegistration
  > Performs automatic image registration using ITKv4 registration framework.

- BresenhamLine
  > Draws a Bresenham line.

- filters
  > various image filters (threshold, median, ...).

- Flipper
  > flips an image according to a given axis.

- Image
  > Applies a mask to an image, zeroing data outside the mask.

- ImageDiff
  > Computes difference between two images.

- ImageExtruder
  > Extrudes voxels from an image that are inside a given mesh.

- ItkImageCaster
  > Cast from/to different image type.

- Labeling
  > Filters image using labels

- LineDrawer
  > Draws line.

- MatrixRegressor
  > Computes an 'average' matrix from a matrix list. Uses PowellOptimizer.

- MinMaxPropagation
  > Flood fills an image as long as the neighboring voxels are greater than the smallest seed value.

- MIPMatchingRegistration
  > Fast approximate registration, made of pure translation roughly matching two 3-dimensional images

- Resampler
  > transforms and resamples an image.

- SpheroidExtraction
  > Extracts spheres centers in an image with a given radius and above a given intensity value.


## How to use it

### Cmake

```cmake

add_tartarget_link_libraries( myTarget 
    ...
    filter_image
)

```

