# sight::module::ui::debug

This module contains editors and actions for debugging purposes. 

## Services

- **DumpEditor**: dumps or restores selected buffer via an editor.
- **ComponentsTree**: shows module information via an action.
- **ClassFactoryRegistryInfo**: shows services registered in the factory via an action.

### action
This sub-folder contains classes defining some actions.

- **ClassFactoryRegistryInfo**: implements an action to show services registered in a factory.
- **ComponentsTree**: implements an action that shows a module's information.

## CMake

```cmake

add_dependencies(my_target
                 ...
                 module_ui_debug
)

```
