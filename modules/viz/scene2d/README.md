 # sight::module::viz::scene2D

This module contains services to display various 2D elements.
The services are implementations of the adaptor present in libs/viz/scene2D

## Services

- **axis**
  Renders an axis on the scene2d
- **grid2d**
  Renders a grid on the scene2d
- **histogram**
  Displays the histogram of an image
- **line**
  Draws a line on the scene2D
- **transfer_function**
  Displays a composite of TF and interact with them.
- **negato**
  Displays one slice of an 3D image.
- **square**
  Draws a square on the scene2D.
- **transfer_function**
  Displays and edits a transfer function from a medical image.
- **viewport_interactor**
  Manages the camera on the scene2D view
- **viewport_range_selector**
  Allows to select a delimited range of a viewport.
  It uses a graphical delimiter (called shutter) that can be moved from both left to right
  and right to left directions (in those cases, shutter's width is changing).
- **viewport_updater**
  Manages the camera on the view (by updating the viewport object)

## How to use it

## CMake

```cmake

add_dependencies(myTarget
                ...
                 module_viz_scene2D
)

```

