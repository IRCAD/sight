# Image module

The `image` module contains services for image processing. 
In other words it contains services to operate on a "image" data type.

Plugin id is `sight::module::filter::image`

## CMake

```cmake

add_dependencies(myTarget 
                ...
                 module_filter_image
)

```
## List of available services

- `::sight::module::filter::image::SAutomaticRegistration`
- `::sight::module::filter::image::SBitwiseAnd`
- `::sight::module::filter::image::SFlip`
- `::sight::module::filter::image::SImageCenter`
- `::sight::module::filter::image::SImageExtruder`
- `::sight::module::filter::image::SImagesSubstract`
- `::sight::module::filter::image::SLabelGeometryImage`
- `::sight::module::filter::image::SLabelImageToBinaryImage`
- `::sight::module::filter::image::SMatrixRegressor`
- `::sight::module::filter::image::SMIPMatchingRegistration`
- `::sight::module::filter::image::SPlaneSlicer`
- `::sight::module::filter::image::SPropagator`
- `::sight::module::filter::image::SResampler`
- `::sight::module::filter::image::SThreshold`

## Service description

### ::sight::module::filter::image::SAutomaticRegistration

Service computing a rigid transform, matching an image to another.

### ::sight::module::filter::image::SBitwiseAnd

Service that implements the AND bitwise operator pixel-wise between two images.

### ::sight::module::filter::image::SFlip

Service that flips an input image along x or y or z axis.

### ::sight::module::filter::image::SImageCenter

Service computing the center of a 3D image, it outputs the translation going from origin to the center.

### ::sight::module::filter::image::SImageExtruder

Service that sets all voxels of an image that are inside meshes to the lowest value of the image type.

### ::sight::module::filter::image::SImagesSubstract

Service that computes the difference of two images.

### ::sight::module::filter::image::SLabelGeometryImage

Service that computes centroids of shapes from labeled image and adds them as a 'landmark' field to it.

### ::sight::module::filter::image::SLabelImageToBinaryImage

Service extracting a binary mask from an image with labels.

### ::sight::module::filter::image::SMatrixRegressor

Service computing the matrix that best fits a list of matrices.

### ::sight::module::filter::image::SMIPMatchingRegistration

service that uses a fast registration algorithm to quickly register two images together if the transform between both is pure translation.

### ::sight::module::filter::image::SPlaneSlicer

Service computing a 2D slice from a 3D image.

### ::sight::module::filter::image::SPropagator

Service placing seeds and propagating from them in an image.

### ::sight::module::filter::image::SResampler

Service applying a transform to an image and outputting the resampled image.

### ::sight::module::filter::image::SThreshold

Service that applies a threshold on an image.
