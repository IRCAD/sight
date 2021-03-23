# sight::module::geometry::generator

Contains services used to generate meshes of a specific shapes 
(for example, a needle's shape, or the cone shaped ultrasound image).

## Services

- **SNeedle**: generates a cylinder shape of `data::Mesh` type, using VTK.
- **SUltrasoundMesh**: generates a cone shape that represents the ultrasound image. 

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_geometry_generator ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.