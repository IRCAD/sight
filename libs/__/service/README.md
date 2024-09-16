# sight::service

This is one of the most important libraries in _Sight_. It defines extension points for _services_:
- **factory**: allows to define new services.
- **config**: creates service configurations that can be shared between different services.

## Classes

### Service interfaces

The library proposes different base classes to implement services. They bring a semantic on top of `IService`, which allows to query services that match an interface.

- **filter**: generic interface meant to define filter services
- **generator**: generic interface meant to define generator services
- **grabber**: generic interface meant to manage video grabber services
- **has_services**: generic interface for objects that need to manage services
- **operator**: generic interface meant to define operator services
- **registerer**: generic interface meant to define register services
- **rgbd_grabber**: generic interface meant to define RGBD images grabber services
- **synchronizer**: generic interface meant to define services which synchronize objects from timelines
- **tracker**: generic interface meant to define tracker services
- **macros**: defines macro which declare service to object associations

### _extension_

- **config**: registers all the configurations from an xml file element with the point extension '<config>'
- **factory**: registry of all the services. It allows to retrieve the information necessary to instantiate the services

### _factory_

- **new**: factory service creator

### _op_

- **add**: creates and attaches services to object
- **get**: provides access to services

### _registry_

- **active_workers**: registers active worker and creates a default worker if necessary.
Deprecated, please use `sight::core::thread` instead.
- **object_service**:  maintains the relation between objects and services
 The API of ObjectService should not be directly invoked, please make use of service/op methods (i.e. add, get,...) instead
- **proxy**: defines a proxy for signal/slot connections between services

## How to use it

### CMake

```cmake
target_link_libraries( my_target <PUBLIC|PRIVATE> service)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of `sight::service `to learn more about its use in xml configurations.