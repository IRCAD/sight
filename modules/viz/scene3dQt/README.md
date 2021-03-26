# sight::modules::viz::scene3dQt

Contains an Ogre Render windows based on QT which serves as main window for the sight applications

## Services

- **OffScreenWindowInteractor**: manages an offscreen render window that renders in an Ogre texture.
- **OpenGLContext**: ddefines a common shared OpenGl context for all render windows
- **OpenGLRunner**: runs OpenGL resource handling tasks in a separate thread. Note that itcan not be used for parallel rendering as Ogre does not support it
- **Window**: initialise and render a window, as well as it interaction mechanism
- **WindowInteractor**: defines the interaction mechanism with the displayed windows


## How to use it

### CMake

```cmake

target_link_libraries(myTarget <PUBLIC|PRIVATE> module_viz_qt3d)

```

### XML

This module needs to be set in the application (or activity) xml <requirement\> in order to use a Ogre scene in an application using Qt:

```xml

    <requirement id="sight::module::viz::scene3dQt" />
    
 ```





