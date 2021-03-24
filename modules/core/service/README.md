 # sight::modules::service

Initializes service, serviceConfig, appConfig & appConfigParameters extension points registries.
Contains also `.xsd` files to perform checks of each of the previously named configurations.

## Classes

- **Plugin**: initializes/cleans service, config, appConfig & appConfigParameters extension points.

## Resources

- **serviceFactory.xsd**: XML schema definition for service factory.
- **serviceConfig.xsd**: XML schema definition for service config.
- **serviceFactory.xsd**: XML schema definition for appConfig.
- **serviceFactory.xsd**: XML schema definition for appConfigParameters.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_service ... )
```

### XML

example of service factory:

 ```xml
     <extension implements="::sight::service::extension::Factory">
        <type>sight::service::IXMLParser</type>
        <service>sight::service::parser::Color</service>
        <object>sight::data::Color</object>
    </extension>
 ```