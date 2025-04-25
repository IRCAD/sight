# sight::module::viz::scene3d_qt

Contains an Ogre Render windows based on QT which serves as main window for the sight applications

## Services

- **offscreen_window_interactor**: manages an offscreen render window that renders in an Ogre texture.
- **OpenGLContext**: defines a common shared OpenGl context for all render windows
- **OpenGLRunner**: runs OpenGL resource handling tasks in a separate thread. Note that it can not be used for parallel rendering as Ogre does not support it
- **Window**: initializes and renders a window, as well as its interaction mechanism
- **window_interactor**: defines the interaction mechanism with the displayed windows


## How to use it

### CMake

```cmake

target_link_libraries(my_target <PUBLIC|PRIVATE> module_viz_qt3d)

```

### XML

This module needs to be set in the application (or activity) xml <requirement\> in order to use an Ogre scene in an application using Qt:

```xml

    <requirement id="sight::module::viz::scene3d_qt" />
    
 ```





