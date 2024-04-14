# sight::io::zip

Library for dealing with archives, either compressed as a zip archive or uncompressed as simple directory on filesystem.

## Classes:

- **read_archive**, **write_archive**: defines abstract functions for reading / writing files in an archive.
- **read_dir_archive**, **write_dir_archive**: reads / writes files in a directory.
- **read_zip_archive**, **write_zip_archive**: reads / writes files in a zip archive. They use `minizip` backend.

- **minizip**: contains `minizip` fork from [minizip-ng](https://github.com/zlib-ng/minizip-ng). This fork differs from original [minizip](https://www.winimage.com/zLibDll/minizip.html) by the addition of bug and security fixes and additional features like encryption. The sources are mainly untouched and are based on `1.2`  branch.

## How to use it

### CMake

```cmake
target_link_libraries( my_target <PUBLIC|PRIVATE> io_zip)
```