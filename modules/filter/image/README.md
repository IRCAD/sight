# sight::module::filter::image

This module contains services for image processing. 
In other words it contains services to operate on a `sight::data::Image`.

## CMake

```cmake

add_dependencies(myTarget 
                ...
                 module_filter_image
)

```
## Services

- **SAutomaticRegistration**
  Computes a rigid transform, matching an image to another.
- **SBitwiseAnd**
  Implements the AND bitwise operator pixel-wise between two images. 
- `sight::module::filter::image::SFlip`
- `sight::module::filter::image::SImageCenter`
- `sight::module::filter::image::SImageExtruder`
- `sight::module::filter::image::SImagesSubstract`
- `sight::module::filter::image::SLabelGeometryImage`
- `sight::module::filter::image::SLabelImageToBinaryImage`
- `sight::module::filter::image::SMatrixRegressor`
- `sight::module::filter::image::SMIPMatchingRegistration`
- `sight::module::filter::image::SPlaneSlicer`
- `sight::module::filter::image::SPropagator`
- `sight::module::filter::image::SResampler`
- `sight::module::filter::image::SThreshold`

## Service description

- SAutomaticRegistration

> Computes a rigid transform, matching an image to another.

### SBitwiseAnd

> Implements the AND bitwise operator pixel-wise between two images.

### SFlip

> Flips an input image along x or y or z axis.

### SImageCenter

> Computes the center of a 3D image, it outputs the translation going from origin to the center.

### SImageExtruder

> Sets all voxels of an image that are inside meshes to the lowest value of the image type.

### SImagesSubstract

> Computes the difference of two images.

### SLabelGeometryImage

> Computes centroids of shapes from labeled image and adds them as a 'landmark' field to it.

### SLabelImageToBinaryImage

> Extracts a binary mask from an image with labels.

### SMatrixRegressor

> Computes the matrix that best fits a list of matrices.

### SMIPMatchingRegistration

> Uses a fast registration algorithm to quickly register two images together if the transform between both is pure translation.

### SPlaneSlicer

> Computes a 2D slice from a 3D image.

### SPropagator

> Places seeds and propagating from them in an image.

### SResampler

> Applies a transform to an image and outputting the resampled image.

### SThreshold

> Applies a threshold on an image.
