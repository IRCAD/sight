# sight::io::igtl

Library which implements OpenIGTLink network communication protocol.  
Please refer to [OpenIGTLink official website](http://openigtlink.org/) for more detailed information about this network protocol.

## Classes

### Service interfaces

The library proposes different base classes to implement services. They bring a semantic on top of `IService`, which allows to query services that match an interface.

- **network**: generic interface meant to handle the protocol for both client and server sides
- **Server**: defines a network igtl server which supports sight native data transfer
- **Client**: defines a network igtl client which supports sight native data transfer
- **Exception**: defines the igtl network exceptions

## Archiver

These classes are used for the lib internal mechanism:

- **MemoryReadArchive**: reads the data from an archive 
- **MemoryWriteArchive**: writes some data in an archive

## Converters

The purpose of the library is to facilitate the transfer through igtl network protocol of sight native data formats. 
The present converters transform (forward and backward) various sight native data toward igtl specific data format.

- **base**: generic interface meant to convert `sight::data` to `::igtl::MessageBase`
- **TrackingStartConverter**: starts the conversion between the sight data and the igtl message
- **TrackingStopConverter**: tops the conversion between the sight data and the igtl message
- **AtomConverter**: manages the conversion between `data::object` and `igtl::RawMessage` (contain serialized atom)
- **MapConverter**: manages the conversion between `data::map` and `igtl::TrackingDataMessage`
- **ImageConverter**: manages the conversion between `data::image` and `igtl::ImageMessage`
- **LineConverter**: manages the conversion between `data::line` and `igtl::PositionMessage`
- **MatrixConverter**: manages the conversion between `data::matrix4` and `igtl::TransformationMessage`
- **MeshConverter**: manages the conversion between `data::mesh` and `igtl::PolyDataMessage`
- **PointListConverter**: manages the conversion between `data::point_list` and `igtl::PointMessage`
- **ScalarConverter**: manages the conversion between `data::integer` of Float and `igtl::RawMessage`
- **StringConverter**: manages the conversion between `data::string` and `igtl::StringMessage`

## Exception

- **exception**: is thrown when a conversion between sight data and igtl message is not possible 

## Helper

- **ScalarToBytes**: converts bytes to scalar and scalar to bytes

## Details

- **DataConverter**:  handles the conversion between igtl data and fwData
  Every converter should register to this class, as the DataConverter pattern is a singleton and needs to be called by DataConverter::getInstance()
- **ImageTypeConverter**: handles image type conversion between igtl data and sight native data
- **MessageFactory**: creates and registers igtl messages in the factory.
- **RawMessage**: OpenIGTLink message in which raw data can be stored


### application configuration

## How to use it

Here is a sample of forward and backward data conversion:

```c++
    data::mesh::sptr mesh  
    DataConverter::sptr converter = DataConverter::getInstance();
    ::igtl::MessageBase::Pointer msg = converter->fromFwObject(mesh);

    data::object::sptr obj = converter->fromIgtlMessage(msg);
    data::mesh::sptr mesh2 = data::mesh::dynamicCast(obj);
```

### CMake

```cmake
target_link_library(my_target <PUBLIC|PRIVATE> io_igtl  )
```



