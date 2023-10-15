# sight::io::bitmap

A "fast" 2D image writer (`sight::io::bitmap::Writer`) and reader (`sight::io::bitmap::Reader`). The writer and the reader uses NVidia CUDA accelerated JPEG / JPEG2000 encoding library: [nvJPEG](https://developer.nvidia.com/nvjpeg), if support has been compiled in and if a suitable GPU has been found. Alternatively, `libjpeg-turbo`, `openJPEG`, `libtiff` or `libPNG` can be used as CPU fallback. The performance should be better than VTK or even OpenCV because of direct API calls and avoided unneeded buffer copy.

The image pixel format supported depends of the codec. For instance, while TIFF or PNG supports almost everything from float gray scales images to 32 bits RGBA images, JPEG2000 supports only unsigned 8 bits and 16 bits RGB or gray scale images and JPEG only unsigned 8 bits.

To sum up:
- `libJPEG`: supports unsigned 8 bits RGB, RGBA, BGR, BGRA or GRAYSCALE images.
- `libPNG`: supports unsigned 8, 16 bits RGB, RGBA, BGR, BGRA or GRAYSCALE images.
- `libTIFF`: supports unsigned, signed, float, 8 or 16 bits RGB, RGBA, BGR, BGRA or GRAYSCALE images.
  > Note: libTIFF signed and float image can be written and read back, but are rare and most third party software won't read them.
- `openJPEG`: supports unsigned, signed 8, 16 bits RGB, RGBA or GRAYSCALE images.
- `nvJPEG`: supports unsigned 8 bits RGB, BGR images.
- `nvJPEG2K`: supports unsigned 8, 16 bits RGB, BGR or GRAYSCALE images.

`detail` contains the real implementation with direct calls to image libraries.

## Classes

### Writer

- **Writer**: writes a 2D image to a file or a stream in the selected format (.jpg, .tiff, .png, j2k).

### Reader

- **Reader**: reads a 2D image to a file or a stream in the selected format (.jpg, .tiff, .png, j2k).

## How to use it

### Writing

```c++
    auto image = sight::data::image::New();
    // fill the image...
    ...

    auto writer = io::bitmap::Writer::New();

    // Configure the session writer
    writer->set_object(image);

    // Write with DEFAULT backend (tiff) and DEFAULT mode (fast)
    writer->setFile("image.tiff");
    sessionwriterWriter->write();

    // Write with backend "nvJPEG2000" (will except if not available) and "best" mode
    // .jp2 will be added to the "image" filename
    writer->setFile("image");
    writer->write(io::bitmap::Writer::Backend::NVJPEG2K, io::bitmap::Writer::MODE::BEST);
```

### Reading

```c++
    auto image = sight::data::image::New();
    // fill the image...
    ...

    auto reader = io::bitmap::Reader::New();

    // Configure the session writer
    reader->set_object(image);

    // Read by guessing right backend (use the extension)
    reader->setFile("image.png");
    reader->read();

    // Read with backend "nvJPEG2000" (will except if not available)
    reader->setFile("image.j2k");
    reader->read(io::bitmap::Writer::Backend::NVJPEG2K);
```

### CMake

```cmake
target_link_library(myTarget <PUBLIC|PRIVATE> io_bitmap)
```



