# sight::module::io::zip

Contains a service to read an archive and output to the specified directory. The supported format is ZIP with ZSTD
compression. It can be password protected using AES256.

## Services

- **extract**: read an archive and output to the specified directory

## How to use it

### CMake

```cmake
add_dependencies(my_target module_io_zip ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
