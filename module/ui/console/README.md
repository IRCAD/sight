# sight::module::ui::console

Module used to launch a main loop for console applications, it does not contain any service.

To be started the module needs to be set as a cmake dependency of the console application **and** needs to be set as a xml requirement in the `plugin.xml` of the app (see [How to use it section](##How-to-use-it)).
## How to use it
### CMake

```cmake
add_dependencies(myApp 
                 module_ui_console
)
```
### XML

```xml
<plugin id="myApp" library="true" >

    <requirement id="sight::module::ui::console" />
    ...
```