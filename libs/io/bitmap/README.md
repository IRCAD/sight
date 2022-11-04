# sight::io::bitmap

A "fast" 2D image writer (`sight::io::bitmap::Writer`). The writer uses NVidia CUDA accelerated JPEG / JPEG2000 encoding library: [nvJPEG](https://developer.nvidia.com/nvjpeg), if support has been compiled in and if a suitable GPU has been found. Alternatively, `libjpeg-turbo`, `openJPEG`, `libtiff` or `libPNG` can be used as CPU fallback. The performance should be better than VTK or even OpenCV because of direct API calls and avoided unneeded buffer copy.

`detail` contains the real implementation with direct calls to image libraries.

## Classes

### Writer

- **Writer**: writes a 2D image to a file or a stream in the selected format (.jpg, .tiff, .png, j2k).

## How to use it

### Writing

```c++
    auto image = sight::data::Image::New();
    // fill the image...
    ...

    auto writer = io::bitmap::Writer::New();

    // Configure the session writer
    sessionWriter->setObject(image);

    // Write with DEFAULT backend (tiff) and DEFAULT mode (fast)
    sessionWriter->setFile("image.png");
    sessionWriter->write();

    // Write with backend "nvJPEG2000" (will except if not available) and "best" mode
    // .jp2 will be added to the "image" filename
    sessionWriter->setFile("image");
    sessionWriter->write(io::bitmap::Writer::Backend::NVJPEG2K, io::bitmap::Writer::MODE::BEST);
```

### CMake

```cmake
target_link_library(myTarget <PUBLIC|PRIVATE> io_bitmap)
```



