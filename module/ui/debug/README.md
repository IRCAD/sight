# sight::module::ui::debug

This module contains editors and actions for debugging purposes. 

## Services

- **dump_editor**: dumps or restores selected buffer via an editor.
- **components_tree**: shows module information via an action.
- **class_factory_registry_info**: shows services registered in the factory via an action.

### action
This sub-folder contains classes defining some actions.

- **class_factory_registry_info**: implements an action to show services registered in a factory.
- **components_tree**: implements an action that shows a module's information.

## CMake

```cmake

add_dependencies(my_target
                 ...
                 module_ui_debug
)

```
