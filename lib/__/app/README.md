# sight::service

This is one of the most important libraries in _Sight_. It defines extension points for _appConfig_:
- **factory**: allows to define new services.
- **config**: application configuration that describe a set of data, services and connections. **This is the main way to create applications in _Sight_.**
- **parameters**: allows to pass parameters to an config instance, when launched by the app module.

## Classes

### Services

The library proposes different base classes to implement services. They bring a semantic on top of `IService`, which allows to query services that match an interface.

- **controller**: generic interface meant to define control services
- **object_parser**: generic interface meant to define services which build objects or associated services from an XML-based description
- **config_launcher**: starts/stops a template configuration

### _extension_

- **config**: registers all the configurations from an xml file element with the point extension '<appConfig>'
- **parameters**: registers all the configurations from an xml file element with the point extension '<parameters>'

### _helper_

- **config**: provides configuration element parsing tools.
- **config_launcher**: provides few methods to manage config (parsing, starting, stopping...) associated from an xml configuration file element with the point extension '<appConfig>'

### _parser_

- **boolean**: specialisation of GenericField parser for boolean
- **color**: parses an hexa color value from an xml-based description and set it to the color data (`sight::data::color`) object
- **map**: parses and constructs a map (`sight::data::map`) and its associated services from an XML-based description
- **float**: specialisation of GenericField parser for float
- **generic**: parses and constructs a GenericField and its associated services from an XML-based description
- **list**: parses and constructs a List (`sight::data::List`) and its associated services from an XML-based description
- **matrix4**: parses and constructs a square 4x4 Matrix (`sight::data::matrix4`) and its associated services from an XML-based description
- **object**: parses and constructs a generic sight Object (`sight::data::object`) from an XML-based description. Does nothing as no IService method is specialized
- **string**: specialisation of GenericField parser for string
- **transfer_function**: parses and constructs a TransferFunction (`sight::data::transfer_function`) and its associated services from an XML-based description

## How to use it

### CMake

```cmake
target_link_libraries( my_target <PUBLIC|PRIVATE> app)
```
