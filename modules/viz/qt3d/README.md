# sight::module::viz::qt3d

Sight-to-Qt3d adaptor module.

## Services

- **SMaterial**: Convenience class that converts data::Material to Qt3D.
- **SMesh**: Convenience class that converts data::Mesh to Qt3D.
- **SRender**: Renders a generic scene with Qt3D API.

## Classes

- **WindowInteractor**: Convenience class to manage an interactive `Qt3DExtras::Qt3DWindow` from `ui::base::container::fwContainer::sptr `.

## How to use it

### CMake

```cmake

target_link_libraries(myTarget <PUBLIC|PRIVATE> module_viz_qt3d)

```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
