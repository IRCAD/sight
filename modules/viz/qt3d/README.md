# sight::module::viz::qt3d

**EXPERIMENTAL**

Module that allows to visualize a 3D scene using Qt3D. This module is not mature yet to build any real-world application and is a preview of what we could achieve with Qt3D. For now, please stick with `sight::module::viz::scene3d` which is stable and complete.

## Services

- **SMaterial**: binds a `sight::data::Material` in a Qt3D scene.
- **SMesh**: displays a `sight::data::Mesh` in a Qt3D scene.
- **SRender**: renders a 3D scene using Qt3D.

## Classes

- **WindowInteractor**: manages an interactive `Qt3DExtras::Qt3DWindow` from `ui::base::container::fwContainer::sptr `.

## How to use it

### CMake

```cmake

target_link_libraries(myTarget <PUBLIC|PRIVATE> module_viz_qt3d)

```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
