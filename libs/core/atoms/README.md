# sight::atoms

Defines classes describing and wrapping different types, along with visitors and helper functions.

## Classes

- **Base**: defines the base class for all Atom classes.
- **Blob**: represents blob wrapping a buffer.
- **Boolean**: represents a Boolean value.
- **Exception**: implements atom exception.
- **Map**: implements a container for mapping representation.
- **Numeric**: stores a numeric value.
- **Object**: represents a sight::data::Object
- **Sequence**: represents a list of meta objects.
- **String**: represents a String.

## Structure

Here is the list of folders in this library:

- **conversion**: visitors and helper functions to convert between different formats.
- **factory**: atoms factory.
- **registry**: registry and macros.
- **test**: unit tests.

## How to use it

### CMake

```cmake
target_link_libraries( myTarget <PUBLIC|PRIVATE> atoms)
```
