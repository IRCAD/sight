# sight::io::dicom

Library containing classes to manage command history.

## Classes:

- **command**: defines a basic command.
- **ImageDiffCommand**: defines commands to deal with `filter::image::ImageDiff` which is a class memorizing pixel changes in a image.
- **UndoRedoManager**: keeps track of commands, undo/redo them.
## How to use it

### CMake

```cmake

target_link_libraries( myTarget <PUBLIC|PRIVATE> ui_history)

```

