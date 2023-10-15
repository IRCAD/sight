# sight::modules::service

Initializes service, service_config, appConfig & appConfigParameters extension points registries.
Contains also `.xsd` files to perform checks of each of the previously named configurations.

## Classes

- **Plugin**: initializes/cleans service, config, appConfig & appConfigParameters extension points.

## Resources

- **serviceFactory.xsd**: XML schema definition for service factory.
- **service_config.xsd**: XML schema definition for service config.
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
     <extension implements="sight::service::extension::factory">
        <type>sight::service::IXMLParser</type>
        <service>sight::service::parser::Color</service>
        <object>sight::data::color</object>
    </extension>
 ```