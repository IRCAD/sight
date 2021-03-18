# sight::viz::scene2d

Contains the generic classes for the 2D scene display elements. 
The generic element cover backgroun size, position, viewports, scenes and mouse interactions.

## Classes

* **IAdaptor**
 Root class for the 2D Scene adaptors. Needs to be extended to get the sight::viz::scene2D elements 

* **Scene2DGraphicsView**
 Contains the view dedicated element: viewport, as well as all the mouse interaction methods. 

* **SRender**
 Renders the QT scene 
 
### Data
* **Axis**
 Defines an axis on the scene2D
* **Coord**
 Defines a point on the scene2D
* **Event**
 Manages the events on the scene2D
* **InitQPen**
 Initialise the color of a QPen on the scene2D
* **Size**
 Defines a 2D size data structure for scene2D elements
* **Viewport**
 Defines a 2D viewport with its position and size for scene2D elements
 
### Registry
* **Adaptor**
 Handles a map containing Adaptor-SRender association for the sight registry

 
## How to use it

### CMake

```cmake
add_dependencies(myTarget viz_scene2d ... )
```

### Implementation

Define your specific scened2D class as an extension of the IAdaptor class. 


