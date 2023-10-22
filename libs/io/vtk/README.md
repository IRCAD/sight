# sight::io::vtk

Library for writing and reading data (image, mesh, ...) to and from many VTK formats like `.vtp`, `.ply` or `.vti`. Seed [VTK](https://vtk.org/) for more information.

## Classes

Most of the classes inherit from io::reader::generic_object_reader or io::writer::generic_object_writer. They all share almost the same API.

### Helper

- **Mesh**: converts Mesh to and from VTK data like `vtkPolyData` or `vtkUnstructuredGrid`
- **TransferFunction**: converts TransferFunction to and from VTK `vtkLookupTable`
- **vtk**: converts image to/from `vtkImageData`, Matrix4 to/from `vtkMatrix4x4`

### Reader / Writer

- **BitmapImageReader**: reads bitmap image from any supported format from `vtkImageReader2Collection`.
- **image[Read|Writ]er**: reads/writes `image` to/from legacy `.vtk`.
- **Mesh[Read|Writ]er**: reads/writes `image` to/from legacy `.vtk`.
- **MetaImage[Read|Writ]er**: reads/writes `image` to/from `.mhd`
- **ModelSeriesObjWriter**: writes `ModelSeries` to many `.obj`
- **ObjMesh[Read|Writ]er**: reads/writes `Mesh` to/from `.obj`.
- **PlyMesh[Read|Writ]er**: reads/writes `Mesh` to/from `.ply`.
- **series_setReader**: reads `series_set` from a collection of files. It basically uses other readers depending of the file extension.
- **StlMesh[Read|Writ]er**: reads/writes `Mesh` to/from `.stl`.
- **VtiImage[Read|Writ]er**: reads/writes `image` to/from `.vti`.
- **VtpMesh[Read|Writ]er**: reads/writes `Mesh` to/from `.vtp`.

## How to use it

All `readers` and `writers` which implements `io::reader::generic_object_reader` will share the same usage.

Here is a sample for `.vti` image:

### Writing

```c++
    auto writer = sight::io::vtk::VtiImageWriter::New();
    writer->set_object(image);
    writer->setFile(file);
    writer->write();
```

### Reading

```c++
    auto reader = sight::io::vtk::VtiImageReader::New();
    writer->set_object(image);
    writer->setFile(file);
    writer->read();
```

### CMake

```cmake
target_link_library(my_target <PUBLIC|PRIVATE> io_vtk)
```



