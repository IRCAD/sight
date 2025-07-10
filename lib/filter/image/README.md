# sight::filter::image

Library containing functions producing an output by 'filtering' an input. This covers registration, 
matrix optimization, image filtering, drawing functions, etc...

## Classes:

- **automatic_registration**
  Performs automatic image registration using ITKv4 registration framework.

- **bresenham_line**
  Draws a Bresenham line.

- **filters**
  Various image filters (threshold, median, ...).

- **flipper**
  Flips an image according to a given axis.

- **image**
  Applies a mask to an image, zeroing data outside the mask.

- **image_diff**
  Computes difference between two images.

- **image_extruder**
  Extrudes voxels from an image that are inside a given mesh.

- **itk_image_caster**
  Casts from/to different image type.

- **labeling**
  Filters image using labels

- **line_drawer**
  Draws line.

- **matrix_regressor**
  Computes an 'average' matrix from a matrix list. Uses powell_optimizer.

- **min_max_propagation**
  Flood fills an image as long as the neighboring voxels are greater than the smallest seed value.

- **mip_matching_registration**
  Fast approximate registration, made of pure translation roughly matching two 3-dimensional images

- **resampler**
  Transforms and resamples an image.

- **spheroid_extraction**
  Extracts spheres centers in an image with a given radius and above a given intensity value.


## How to use it

### CMake

```cmake

target_link_libraries( my_target <PUBLIC|PRIVATE> filter_image)

```

