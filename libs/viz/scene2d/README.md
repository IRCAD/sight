# sight::viz::scene2d

Contains the generic classes for the 2D scene display elements. 
The generic element cover background size, position, viewports, scenes and mouse interactions.

## Classes

* **adaptor**: root class for the 2D scene adaptors. Needs to be extended to get the sight::viz::scene2D elements. 

* **graphics_view**: contains the view dedicated element: viewport, as well as all the mouse interaction methods. 

* **render**: renders the QT scene. 
 
### Data

* **Axis**: defines an axis on the scene2D.

* **Coord**: defines a point on the scene2D.

* **Event**: manages the events on the scene2D.

* **InitQPen**: initialise the color of a QPen on the scene2D.

* **Size**: defines a 2D size data structure for scene2D elements.

* **Viewport**: defines a 2D viewport with its position and size for scene2D elements.
 
### Registry

* **adaptor**: Handles a map containing adaptor-render association for the sight registry.

 
## How to use it

### CMake

```cmake
add_dependencies(my_target viz_scene2d ... )
```

### Implementation

Define your specific scene2D class as an extension of the adaptor class. 


