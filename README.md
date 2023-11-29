# Sight

| Branch |    Status |
|--------|-----------|
| Dev    | [![pipeline status](https://git.ircad.fr/Sight/sight/badges/dev/pipeline.svg)](https://git.ircad.fr/Sight/sight/commits/dev) |
| Master | [![pipeline status](https://git.ircad.fr/Sight/sight/badges/master/pipeline.svg)](https://git.ircad.fr/Sight/sight/commits/master) |

## Description
[//]: # (cspell: disable)
**Sight**, the **S**urgical **I**mage **G**uidance and **H**ealthcare **T**oolkit aims to ease the creation of
applications based on medical imaging.
[//]: # (cspell: enable)

It includes various functionalities such as 2D and 3D digital image processing, visualization, augmented reality and
medical interaction simulation. It runs on Microsoft Windows and Linux, is written in C++, and features rapid interface
design using XML files. It is freely available under the LGPL.

**Sight** is mainly developed by the Surgical Data Sciences Team of [IRCAD France](https://www.ircad.fr), where it is
used everyday to develop innovative applications for the operating room and medical research centers.

Many **tutorials** and **examples**, which can help you to learn smoothly how to use **Sight**, are located in the
`tutorials` and `examples` directories.
Detailed steps are described [here](https://sight.pages.ircad.fr/sight-doc/Tutorials/index.html).

### Features

- 2D/3D visualization of medical images, meshes, and many widgets.
- Import / export medical data from various formats (DICOM, [VTK](https://www.vtk.org/), ...) and sources
  (files, devices, PACS, ...).
- Playing, recording, processing videos (webcams, network streams, Intel RealSense devices, ...).
- Easy GUI configuration and customization (XML description and stylesheets support).
- Timeline, allowing to store various data (video, matrices, markers, etc...) and synchronize these data across time.
- Mono and stereo camera calibration,
- [ArUco](https://sourceforge.net/projects/aruco/) optical markers tracking,
- [openIGTLink](http://openigtlink.org/) support through client and server services,
- Advanced memory management to support large data. Unused data can be offloaded to disk, saving memory for foreground
  tasks.
- Work session or any part of it, can be saved and restored on disk. The data itself can be encrypted using AES256 to
  ensure a high level of security and privacy


### Hardware / Operating System / Compiler support

**Sight** is written in standard C++17 and use [CMake](https://cmake.org/) as its build system, which means that Sight
should at least compile on any operating system that provide support for a decent C++17 compiler, CMake, **AND** Sight's
dependencies (see [Install](#install) for a list of dependencies for Linux platform). However, we currently have access
to a limited set of hardware/OS/compiler combinations where the code is actually tested on a regular basis.

Such combination includes:
-  [Debian 11 stable on AMD64 with GCC 10.2.1](https://www.debian.org/ports/amd64)
-  [Ubuntu 21.04 on AMD64 with GCC 10.3.0 or CLang 12](https://releases.ubuntu.com/21.04/)
-  [Microsoft Windows 10 on AMD64 with VisualStudio 2019](https://www.microsoft.com/windows/)

> If your platform is not listed, that *doesn't* mean **Sight** will not work, just we cannot guarantee that it is well
> tested. If you are on this kind of platform and are able to build and use **Sight**, feel free to share with us your
> success !

> We use some fine tuned compiler flags (like `/arch:AVX2`) to optimize and generate code specifically for CPUs that
> were released around 2013 and later. It means, if your CPU is too old, **Sight** will crash at runtime because some
> CPU instructions are not implemented. In such situation, you can modify hidden cmake variable `SIGHT_ARCH` at
> configuring time or modify the default compiler flag directly in **Sight** CMake code.

## Applications

### sight_viewer

**sight_viewer** is a full featured medical image and mesh viewer with advanced rendering features such as volume
rendering. It supports most medical image formats, and can also retrieve DICOM files from a PACS. It demonstrates many
useful features of Sight.

<div align=center style="text-align: center; display: flex; flex-flow: row wrap; justify-content: space-around;">
<figure>
    <img src="https://git.ircad.fr/sight/sight-doc/-/raw/dev/Introduction/media/SightViewer01.gif">
    <figcaption>
        <b><i>MPR view of a medical 3D image with additional volume rendering</i></b>
    </figcaption>
</figure>
<figure>
    <img src="https://git.ircad.fr/sight/sight-doc/-/raw/dev/Introduction/media/SightViewer02.gif">
    <figcaption>
        <b><i>Volume rendering and transfer function tuning</i></b>
    </figcaption>
</figure>
<figure>
    <img src="https://git.ircad.fr/sight/sight-doc/-/raw/dev/Introduction/media/mixed_vr_reconstructions.gif">
    <figcaption>
        <b><i>Volume rendering mixed with 3D surfacic meshes</i></b>
    </figcaption>
</figure>
</div>

### DicomXplorer

**DicomXplorer** is a simple medical image viewer that can connect to a PACS to retrieve DICOM data. It supports CT-scan
and MRI images.

<div align=center style="text-align: center; display: flex; flex-flow: row wrap; justify-content: space-around;">
<figure>
    <img src="https://git.ircad.fr/sight/sight-doc/-/raw/dev/Introduction/media/DicomXplorer01.gif">
    <figcaption>
        <b><i>DICOM and medical image files navigation</i></b>
    </figcaption>
</figure>
<figure>
    <img src="https://git.ircad.fr/sight/sight-doc/-/raw/dev/Introduction/media/DicomXplorer02.gif">
    <figcaption>
        <b><i>MPR view of a medical 3D image</i></b>
    </figcaption>
</figure>
</div>

### sight_calibrator

**sight_calibrator** is a user-friendly application to calibrate mono and stereo cameras.
This software is a must-have since camera calibration is a mandatory step in any AR application.

<div align=center style="text-align: center; display: flex; flex-flow: row wrap; justify-content: space-around;">
<figure style="">
    <img src="https://git.ircad.fr/sight/sight-doc/-/raw/dev/Introduction/media/SightCalibrator01.gif">
    <figcaption>
        <b><i>Intrinsic & extrinsic calibration of mono/stereo cameras with live reprojection error display</i></b>
    </figcaption>
</figure>
</div>

## Install

See [detailed install instructions](https://sight.pages.ircad.fr/sight-doc/Installation/index.html) for Windows and
Linux.

## Documentation

* [Documentation](https://sight.pages.ircad.fr/sight-doc)
* [Tutorials](https://sight.pages.ircad.fr/sight-doc/Tutorials/index.html)
* [Doxygen](https://sight.pages.ircad.fr/sight)

## Support

Please note that our GitLab is currently only available in read-only access for external developers and users. This is a
restriction because of the licensing model of GitLab. Since we use an EE version, we would be forced to pay for every
community user, and unfortunately we cannot afford it. This licensing model might change in the future
https://gitlab.com/gitlab-org/gitlab-ee/issues/4382 though.

Until then, we gently ask our community users to use our GitHub mirror to
[report any issues](https://github.com/IRCAD/sight/issues) or propose
[contributions](https://github.com/IRCAD/sight/pulls).

You can also get live community support on the [gitter chat room](https://gitter.im/IRCAD-IHU/sight-support).

