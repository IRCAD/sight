# sight::module::filter::image

Contains services for image processing. 
In other words it contains services to operate on a `sight::data::image`.

## Services

- **automatic_registration**
  Computes a rigid transform, matching an image to another.
- **bitwise_and**
  Implements the AND bitwise operator pixel-wise between two images. 
- **flip**
  Flips an input image along x or y or z axis.
- **image_center**
  Computes the center of a 3D image, it outputs the translation going from origin to the center.
- **imageExtruder**
  Sets all voxels of an image that are inside meshes to the lowest value of the image type.
- **images_substract**
  Computes the difference of two images.
- **label_geometry_image**
  Computes centroids of shapes from labeled image and adds them as a 'landmark' field to it.
- **label_image_to_binary_image**
  Extracts a binary mask from an image with labels.
- **matrix_regressor**
  Computes the matrix that best fits a list of matrices.
- **mip_matching_registration**
  Uses a fast registration algorithm to quickly register two images together if the transform between both is pure translation.
- **plane_slicer**
  Computes a 2D slice from a 3D image.
- **propagator**
  Places seeds and propagating from them in an image.
- **resampler**
  Applies a transform to an image and outputting the resampled image.
- **threshold**
  Applies a threshold on an image.

## CMake

```cmake

add_dependencies(myTarget 
                ...
                 module_filter_image
)

```