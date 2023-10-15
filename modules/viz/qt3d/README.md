# sight::module::viz::qt3d

**EXPERIMENTAL**

Module that allows to visualize a 3D scene using Qt3D. This module is not mature yet to build any real-world application and is a preview of what we could achieve with Qt3D. For now, please stick with `sight::module::viz::scene3d` which is stable and complete.

## Services

- **material**: binds a `sight::data::material` in a Qt3D scene.
- **mesh**: displays a `sight::data::mesh` in a Qt3D scene.
- **render**: renders a 3D scene using Qt3D.

## Classes

- **window_interactor**: manages an interactive `Qt3DExtras::Qt3DWindow` from `ui::container::widget::sptr `.

## How to use it

### CMake

```cmake

target_link_libraries(myTarget <PUBLIC|PRIVATE> module_viz_qt3d)

```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
