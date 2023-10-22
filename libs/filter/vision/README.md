# sight::filter::vision

Library containing helper classes and computer vision algorithms.

## Classes:

- **masker**: performs OpenCV's Expectation Maximization segmentation after a learning step of two color models.
  One for the foreground objects that we need to segment and a second one for the background.

- **projection**: contains helpers to project/reproject 3D/2D points on images

## How to use it

### CMake

```cmake

target_link_libraries( my_target <PUBLIC|PRIVATE> filter_vision)

```

