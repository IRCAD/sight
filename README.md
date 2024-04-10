# Sight

| Branch |    Status |
|--------|-----------|
| Dev    | [![pipeline status](https://git.ircad.fr/Sight/sight/badges/dev/pipeline.svg)](https://git.ircad.fr/Sight/sight/commits/dev) |
| Master | [![pipeline status](https://git.ircad.fr/Sight/sight/badges/master/pipeline.svg)](https://git.ircad.fr/Sight/sight/commits/master) |

## Description
<!-- (cspell: disable) -->
**Sight**, the **S**urgical **I**mage **G**uidance and **H**ealthcare **T**oolkit aims to ease the creation of
applications based on medical imaging.
<!-- (cspell: enable) -->

It encompasses a range of functionalities, including 2D and 3D digital image processing, visualization, augmented
reality, and medical interaction simulation. Compatible with both Linux and Windows operating systems,
it is coded in C++ and offers swift interface design through XML files. Moreover, it is freely accessible under the
LGPL license.

**Sight** is developed by the Surgical Data Science Team of [IRCAD France](https://www.ircad.fr), where it is
used everyday to develop innovative applications for the operating room and medical research centers.

Numerous **tutorials** and **examples**, housed within the `tutorials` and `examples` directories, are available to
facilitate your smooth learning journey with Sight.

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

**Sight** is written in standard C++20 and use [CMake](https://cmake.org/) as its build system, which means that Sight
should compile on any operating system that provides support for a decent C++20 compiler, CMake, **AND** Sight's
dependencies (see [Install](#install) for a list of dependencies for Linux platform). However, we currently have access
to a limited set of hardware/OS/compiler combinations where the code is actually tested on a regular basis.

Such combination includes:
-  [Debian 12 stable on AMD64 with GCC 12](https://www.debian.org/ports/amd64)
-  [Ubuntu 22.04 on AMD64 with GCC 11/12 or CLang 17](https://releases.ubuntu.com/22.04/)
-  [Microsoft Windows 10/11 on AMD64 with VisualStudio 2022](https://www.microsoft.com/windows/)

> If your platform is not listed, that *doesn't* imply **Sight** will not build, just we cannot guarantee it.
> If you're using such a platform and manage to build and utilize **Sight**, please feel free to share your success
> with us!

## Applications

### Sight Viewer

**Sight Viewer** is a full featured medical image and mesh viewer with advanced rendering features such as volume
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

### Sight Calibrator

**Sight Calibrator** is a user-friendly application to calibrate mono and stereo cameras.
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

You can also get live community support on the
[gitter chat room](https://matrix.to/#/#IRCAD-IHU_sight-support:gitter.im).

