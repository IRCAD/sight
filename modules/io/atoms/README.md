# sight::module::io::atoms

Contains services for reading and writing atoms (`sight::atoms::Object`).
In the process of saving C++ structures from memory to hard drive, this module provides services to load and save `sight::atoms::Object`;

## Services
// TODO: add `FilterUnknownActivies`' s description.
- **SReader**: loads a `sight::atoms::Object` and converts it into `sight::data::Object`.
- **SWriter**: converts a `sight::data::Object` into `sight::atoms::Object` and saves it to hard drive.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_io_atoms ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
