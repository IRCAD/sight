# sight::module::filter::mesh

Contains services for mesh filtering. 

## Services

- **vtk_mesher**: generates a mesh from a mask in an image using the VTK library.
- **ultrasound_mesh**: generates a mesh used to display an ultrasound image.

## CMake

```cmake

add_dependencies(my_target 
                ...
                 module_filter_mesh
)
```
