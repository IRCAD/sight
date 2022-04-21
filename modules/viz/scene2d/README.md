 # sight::module::viz::scene2D

This module contains services to display various 2D elements.
The services are implementations of the IAdaptor present in libs/viz/scene2D

## Services

- **SAxis**
  Renders an axis on the scene2d
- **SGrid2D**
  Renders a grid on the scene2d
- **SHistogram**
  Displays the histogram of an image
- **SLine**
  Draws a line on the scene2D
- **SMultipleTF**
  Displays a composite of TF and interact with them.
- **SNegato**
  Displays one slice of an 3D image.
- **SSquare**
  Draws a square on the scene2D
- **STransferFunction**
  Applies a transfer function on the image displayed
- **SViewportInteractor**
  Manages the camera on the scene2D view
- **SViewportRangeSelector**
  Allows to select a delimited range of a viewport.
  It uses a graphical delimiter (called shutter) that can be moved from both left to right
  and right to left directions (in those cases, shutter's width is changing).
- **SViewportUpdater**
  Manages the camera on the view (by updating the viewport object)

## How to use it

## CMake

```cmake

add_dependencies(myTarget
                ...
                 module_viz_scene2D
)

```

