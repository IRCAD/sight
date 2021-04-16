# sight::io::base



## Services

This module mainly contains services that can writte sight data to files, and read them back. 


### Reader

- **new**: internal mechanism:generic constructor with restrictions on objects construction.
- **detail**:  internal mechanism: provides the instances of the factory registry.
- **ArrayReader**: reads file with .raw extension into a `sight::data::Array`.
- **DictionaryReader**: reads file with .dic extension into a `sight::data::StructureTraitsDictionary`.
- **GenericObjectReader**: generic reader wich reads an Object.
- **GzArrayReader**: reads file with .raw.gz extension into a `sight::data::Array`.
- **GzBufferImageReader**: reads file with .raw.gz extension into a `sight::data::Image`.
- **IObjectReader**: generic definition for readers, though is not a service unlike `sight::io::base::service::IReader`.
- **Matrix4Reader**: reads file with .trf extension into a `sight::data::Matrix4`.

### Service

Contains the base interface for reader and writer services. 

- **ioTypes**: defines different type of paths used by service readers/writers.
- **IReader**: reader service API. It manages extension points definition and extension configuration.
- **IWriter**: writer service API. It manages extension points definition and extension configuration.

### Writer

- **new**: internal mechanism:generic constructor with restrictions on objects construction.
- **detail**:  internal mechanism: provides the instances of the factory registry.
- **ArrayWriter**: writes `sight::data::Array` into a file with .raw extension.
- **GenericObjectWriter**: generic reader wich reads an Object.
- **GzArrayWriter**: writes `sight::data::Array` into a file with .raw.gz extension.
- **GzBufferImageWriter**: writes `sight::data::Image` into a file with .raw.gz extension.
- **IObjectWriter**: generic definition for writter, though is not a service unlike `sight::io::base::service::IWriter`.
- **Matrix4Writer**: writes `sight::data::Matrix4` into a file with .trf extension.

## How to use it

### CMake

```cmake
add_dependencies(myTarget io_base ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.

