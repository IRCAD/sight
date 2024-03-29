# sight::module::io::session

Contains services for reading and writing data objects (`sight::data::object`) to files stored on a filesystem.

The session file is indeed a standard "ZIP" archive, while the compression algorithm for files inside the session
archive is ZSTD. A standard archive reader could open a session file, if it is able to handle ZIP archive with ZSTD
compression.

The archive can be password protected using AES256 algorithm and the compression level is set individually, depending
of the type of data to serialize.

## Services

- **reader**: loads a session file and restore the saved `sight::data::object`.
- **writer**: saves a `sight::data::object` into a session file on a filesystem.

## How to use it

### CMake

```cmake
add_dependencies(my_target module_io_session ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
