# sight::module::io::matrix

Contains services to read/write matrices from/to files.

## Services

* **SMatrixReader**
Reads a csv file, extracts matrices from it and push them into a ::sight::data::MatrixTL.

* **Matrix4ReaderService**
Read a ::sight::data::Matrix4 from a .trf file

* **SMatrixWriter**
Saves a timeline of matrices in a csv file.

* **Matrix4WriterService**
Writes a ::sight::data::Matrix4 into a .trf file.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_io_matrix ... )
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
