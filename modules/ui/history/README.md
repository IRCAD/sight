 
# sight::module::ui::history

Contains service to handle a command history. Used for instance to undo or redo some command (see `ui::history::command`).

## Services

- **SCommandHistory**: manages a command history, see also `ui::history::command` for more details about which command can be stored.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_ui_history ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.