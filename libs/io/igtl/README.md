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
- **AtomConverter**: manages the conversion between `data::Object` and `igtl::RawMessage` (contain serialized atom)
- **CompositeConverter**: manages the conversion between `data::Composite` and `igtl::TrackingDataMessage`
- **ImageConverter**: manages the conversion between `data::Image` and `igtl::ImageMessage`
- **LineConverter**: manages the conversion between `data::Line` and `igtl::PositionMessage`
- **MatrixConverter**: manages the conversion between `data::Matrix4` and `igtl::TransformationMessage`
- **MeshConverter**: manages the conversion between `data::Mesh` and `igtl::PolyDataMessage`
- **PointListConverter**: manages the conversion between `data::PointList` and `igtl::PointMessage`
- **ScalarConverter**: manages the conversion between `data::Integer` of Float and `igtl::RawMessage`
- **StringConverter**: manages the conversion between `data::String` and `igtl::StringMessage`

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


### Application configuration

## How to use it

Here is a sample of forward and backward data conversion:

```c++
    data::Mesh::sptr mesh  
    DataConverter::sptr converter = DataConverter::getInstance();
    ::igtl::MessageBase::Pointer msg = converter->fromFwObject(mesh);

    data::Object::sptr obj = converter->fromIgtlMessage(msg);
    data::Mesh::sptr mesh2 = data::Mesh::dynamicCast(obj);
```

### CMake

```cmake
target_link_library(myTarget <PUBLIC|PRIVATE> io_igtl  )
```



