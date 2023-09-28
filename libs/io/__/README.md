# sight::io

Contains base classes & functions for input/output.

## Services

This module contains services that can write sight data to files, and read them back. 


### Reader

- **new**: internal mechanism: generic constructor with restrictions on objects construction.
- **detail**:  internal mechanism: provides the instances of the factory registry.
- **CsvReader**:  reads CSV file and returns parsed tokens.
- **ArrayReader**: reads `.raw` files and converts them into a `sight::data::Array`.
- **DictionaryReader**: reads `.dic` files and converts them into a `sight::data::StructureTraitsDictionary`.
- **GenericObjectReader**: generic reader which reads an object.
- **GzArrayReader**: reads `.raw.gz` files and converts them into a `sight::data::Array`.
- **GzBufferImageReader**: reads `.raw.gz` files and converts them into a `sight::data::Image`.
- **IObjectReader**: generic definition for readers, though is not a service unlike `sight::io::service::reader`.
- **Matrix4Reader**: reads `.trf` files and converts them into a `sight::data::Matrix4`.

### Service

Contains the base interface for reader and writer services. 

- **ioTypes**: defines different types of paths used by service readers/writers.
- **reader**: reader service API. It manages extension points definition and extension configuration.
- **writer**: writer service API. It manages extension points definition and extension configuration.

### Writer

- **new**: internal mechanism: generic constructor with restrictions on objects construction.
- **detail**:  internal mechanism: provides the instances of the factory registry.
- **ArrayWriter**: writes `sight::data::Array` into a `.raw` file.
- **GenericObjectWriter**: generic reader which reads an Object.
- **GzArrayWriter**: writes `sight::data::Array` into a `.raw.gz` file.
- **GzBufferImageWriter**: writes `sight::data::Image` into a `.raw.gz` file.
- **IObjectWriter**: generic definition for writer, though is not a service unlike `sight::io::service::writer`.
- **Matrix4Writer**: writes `sight::data::Matrix4` into a `.trf` file.

## How to use it

### CMake

```cmake
target_link_libraries(myTarget <PUBLIC|PRIVATE> io)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in XML configurations.

