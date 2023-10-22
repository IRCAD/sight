# sight::io

Contains base classes & functions for input/output.

## Services

This module contains services that can write sight data to files, and read them back. 


### Reader

- **new**: internal mechanism: generic constructor with restrictions on objects construction.
- **detail**:  internal mechanism: provides the instances of the factory registry.
- **csv_reader**:  reads CSV file and returns parsed tokens.
- **array_reader**: reads `.raw` files and converts them into a `sight::data::array`.
- **dictionary_reader**: reads `.dic` files and converts them into a `sight::data::structure_traits_dictionary`.
- **generic_object_reader**: generic reader which reads an object.
- **gz_array_reader**: reads `.raw.gz` files and converts them into a `sight::data::array`.
- **GzBufferImageReader**: reads `.raw.gz` files and converts them into a `sight::data::image`.
- **object_reader**: generic definition for readers, though is not a service unlike `sight::io::service::reader`.
- **matrix4_reader**: reads `.trf` files and converts them into a `sight::data::matrix4`.

### Service

Contains the base interface for reader and writer services. 

- **ioTypes**: defines different types of paths used by service readers/writers.
- **reader**: reader service API. It manages extension points definition and extension configuration.
- **writer**: writer service API. It manages extension points definition and extension configuration.

### Writer

- **new**: internal mechanism: generic constructor with restrictions on objects construction.
- **detail**:  internal mechanism: provides the instances of the factory registry.
- **array_writer**: writes `sight::data::array` into a `.raw` file.
- **generic_object_writer**: generic reader which reads an Object.
- **gz_array_writer**: writes `sight::data::array` into a `.raw.gz` file.
- **gz_buffer_image_writer**: writes `sight::data::image` into a `.raw.gz` file.
- **object_writer**: generic definition for writer, though is not a service unlike `sight::io::service::writer`.
- **matrix4_writer**: writes `sight::data::matrix4` into a `.trf` file.

## How to use it

### CMake

```cmake
target_link_libraries(my_target <PUBLIC|PRIVATE> io)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in XML configurations.

