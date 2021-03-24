 # sight::service

This is one of the most important library in _Sight_. It defines extension points for _services_ and _appConfig_:
- **Factory**: allows to define new services.
- **Config**: creates service configurations that can be shared between different services.
- **AppConfig**: application configuration that describe a set of data, services and connections. **This is the main way to create applications in _Sight_.**
- **AppConfigParameters**: allows to pass parameters to an AppConfig instance, when launched by the appXml module.

## Classes

### Service interfaces

The library proposes different base classes to implement services. They bring a semantic on top of `IService`, which allows to query services that match an interface.

- **ICalibration**: calibrates a device.
- **IController**: generic interface meant to control data as in the MVC paradigm.
- **IGenerator**: generates a new data.
- **IGrabber**: grabs data from a device or a resource.
- **IHasServices**: provides utility functions to manage sub-services.
- **IOperator**: transforms a data into another one.
- **IParametersService**: provides slots to connect to a service that modifies simple values such as `sight::module::ui::qt::SParameters`.
- **IRegisterer**: registers an entity in another space.
- **IRGBDGrabber**: grabs data from a RGBD camera.
- **ISimulator**: performs a simulation.
- **ISynchronizer**: synchronizes data or signals between them.
- **ITracker**: tracks an object.
- **IXMLParser**: provides a parser to initialize a data in a XML _AppConfig._ 

### Application configuration

## How to use it

### CMake

```cmake
target_link_library(myTarget <PUBLIC|PRIVATE> service ... )
```

### XML
