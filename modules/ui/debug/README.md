# sight::module::ui::debug

This module contains editors and actions for debugging purposes. 

## Services

- **DumpEditor**: dumps or restores selected buffer via an editor.
- **ComponentsTree**: shows module information via an action.
- **ClassFactoryRegistryInfo**: shows services registered in the factory via an action.

## CMake

```cmake

add_dependencies(myTarget
                 ...
                 module_ui_debug
)

```
