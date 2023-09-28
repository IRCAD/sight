# sight::viz::qt3d

Contains all needed interfaces, helper classes and resources to render with the Qt3d API.

## Files

- **adaptor**: provides behavior of Qt3D adaptor services.
- **window_interactor**: defines an abstract class to manage Qt3D RenderWindow in a qt container.
- **SRender**: renders a scene with Qt3D API.

### _CMake_

Contains a template file configured by CMake that defining the location of resources (GLSL shaders).

### _core_

- **FrameGraph**: specifies how the scene will be rendered by managing a qt3d framegraph.
- **GenericScene**: manages the root component of the scene hierarchy. Entities to be drawn must be declared as children of a scene

### _data_

- **Material**: manages a qt3d material.
- **Mesh**: manages a generic mesh.

### _factory_

- **new**: used to restrict access to object construction.

### _rc/glsl_

Contains GLSL shaders.
* **VP**: vertex programs.
* **FP**: fragment programs.
* **GP**: geometry programs.

### _registry_
- **Adaptor**: registry of adaptors.
- **detail**: ?
- **macros**: ?

### _techniques_

- **Lighting**: manages a lighting technique.

## How to use it

### CMake

```cmake
target_link_libraries( myTarget <PUBLIC|PRIVATE> 
                        viz_qt3d
)
```
### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of `sight::viz::qt3d::SRender `to learn more about its use in xml configurations.