# sight::filter::vision

Library containing helper classes and computer vision algorithms.

## Classes:

- **Masker**: performs OpenCV's Expectation Maximization segmentation after a learning step of two color models.
  One for the foreground objects that we need to segment and a second one for the background.

- **Projection**: contains helpers to project/reproject 3D/2D points on images

## How to use it

### CMake

```cmake

target_link_libraries( myTarget <PUBLIC|PRIVATE> filter_vision)

```

