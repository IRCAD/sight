# sight::module::app

Module containing the mechanism to parse xml configuration of an application

## Classes:

- **Plugin**: parses the xml configuration provided as configuration of the application.

## How to use it

### CMake

In the CMakeLists of your application you need to add the dependency, and provide the configuration files as parameters of this module as follow:

```cmake
add_dependencies(my_target module_app ... )


module_param(
        sight::module_app
    PARAM_LIST
        config parameters
    PARAM_VALUES
        YOUR_APP_CONFIG YOUR_APP_CONFIG_PARAMETER
)
```


