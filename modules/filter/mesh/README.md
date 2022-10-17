# sight::module::filter::mesh

Contains services for mesh filtering. 

## Services

- **SVTKMesher**: generates a mesh from a mask in an image using the VTK library.
- **SNeedle**: generates a cylinder shape of `data::Mesh` type, using VTK.
- **SUltrasoundMesh**: generates a mesh used to display an ultrasound image.

## CMake

```cmake

add_dependencies(myTarget 
                ...
                 module_filter_mesh
)
```