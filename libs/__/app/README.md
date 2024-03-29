# sight::service

This is one of the most important libraries in _Sight_. It defines extension points for _services_ and _appConfig_:
- **factory**: allows to define new services.
- **config**: creates service configurations that can be shared between different services.
- **config**: application configuration that describe a set of data, services and connections. **This is the main way to create applications in _Sight_.**
- **parameters**: allows to pass parameters to an config instance, when launched by the app module.

## Classes

### Service interfaces

The library proposes different base classes to implement services. They bring a semantic on top of `IService`, which allows to query services that match an interface.

- **AppConfigManager**: provides an API to define configuration template
- **IAppConfigManager**: generic interface meant to define configurations template
Deprecated, this class is no longer supported, please  use `sight::service::AppConfigManager` instead.
- **IController**: generic interface meant to define control services
- **filter**: generic interface meant to define filter services
- **IGenerator**: generic interface meant to define generator services
- **grabber**: generic interface meant to manage video grabber services
- **has_services**: generic interface for objects that need to manage services
- **IOperator**: generic interface meant to define operator services
- **IRegisterer**: generic interface meant to define register services
- **rgbd_grabber**: generic interface meant to define RGBD images grabber services
- **IRegisterer**: generic interface meant to define services
- **ISynchronizer**: generic interface meant to define services which synchronize objects from timelines
- **ITracker**: generic interface meant to define tracker services
- **IXMLParser**: generic interface meant to define services which build objects or associated services from an XML-based description
- **macros**: defines macro which declare service to object associations
- **config_controller**: starts/stops a template configuration
- **ServiceFactoryRegistry**: creates internally the service factory and adds it to the FactoryRegistry

### _extension_

- **config**: registers all the configurations from an xml file element with the point extension '<appConfig>'
- **parameters**: registers all the configurations from an xml file element with the point extension '<parameters>'
- **config**: registers all the configurations from an xml file element with the point extension '<config>'
- **factory**: registry of all the services. It allows to retrieve the information necessary to instantiate the services

### _factory_

- **new**: factory service creator

### _helper_

- **config**: provides configuration element parsing tools.
- **config_launcher**: provides few methods to manage config (parsing, starting, stopping...) associated from an xml configuration file element with the point extension '<appConfig>'
- **proxy_connections**: provides helper class to register proxy connections

### _op_

- **Add**: creates and attaches services to object
- **Get**: provides acces to services

### _parser_

- **Boolean**: specialisation of GenericField parser for boolean
- **Color**: parses an hexa color value from an xml-based description and set it to the color data (`sight::data::color`) object
- **Composite**: parses and constructs a composite (`sight::data::composite`) and its associated services from an XML-based description
- **Float**: specialisation of GenericField parser for float
- **GenericField**: parses and constructs a GenericField and its associated services from an XML-based description
- **List**: parses and constructs a List (`sight::data::List`) and its associated services from an XML-based description
- **Matrix4**: parses and constructs a square 4x4 Matrix (`sight::data::matrix4`) and its associated services from an XML-based description
- **Object**: parses and constructs a generic sight Object (`sight::data::object`) from an XML-based description. Does nothing as no IService method is specialized
- **String**: specialisation of GenericField parser for string
- **TransferFunction**: parses and constructs a TransferFunction (`sight::data::transfer_function`) and its associated services from an XML-based description

### _registry_

- **ActiveWorkers**: registers active worker and creates a default worker if necessary.
Deprecated, please use `sight::core::thread` instead.
- **ObjectService**:  maintains the relation between objects and services
 The API of ObjectService should not be directly invoked, please make use of service/op methods (i.e. add, get,...) instead
- **Proxy**: defines a proxy for signal/slot connections between services

## How to use it

### CMake

```cmake
target_link_libraries( my_target <PUBLIC|PRIVATE> service)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of `sight::service `to learn more about its use in xml configurations.