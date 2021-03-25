# sight::io::patch

Library dealing with semantic and structural patches for ::sight::data.

## Semantic patches

- **PatchLoader.hpp**: loads semantic patches by processing .versions and .graphlink files.

## Structural patches

- **PatchLoader.hpp**: loads structural patches.

## Test

Contains tests ensuring version upgrades are working fine for relevant ::sight::data.

## How to use it

### CMake

```cmake
target_link_libraries( myTarget <PUBLIC|PRIVATE> io_patch)
```
