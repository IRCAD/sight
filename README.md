| Branch |    Status |
|--------|-----------|
| Dev    | [![pipeline status](https://git.ircad.fr/Sight/sight/badges/dev/pipeline.svg)](https://git.ircad.fr/Sight/sight/commits/dev) |
| Master | [![pipeline status](https://git.ircad.fr/Sight/sight/badges/master/pipeline.svg)](https://git.ircad.fr/Sight/sight/commits/master) |


# Description

**Sight**, the **S**urgical **I**mage **G**uidance and **H**ealthcare **T**oolkit aims to ease the creation of applications based on medical imaging.
It includes various features such as 2D and 3D digital image processing, visualization, augmented reality and medical interaction simulation. It runs on many different environments (Windows, linux, macOS), is written in C++, and features rapid interface design using XML files. It is freely available under the LGPL.

**Sight** is mainly developed by the research and development team of [IRCAD France](https://www.ircad.fr) and [IHU Strasbourg](https://www.ihu-strasbourg.eu), where it is used everyday to develop experimental applications for the operating room.

**Sight** was formerly known as [FW4SPL](https://github.com/fw4spl-org/fw4spl). It was renamed in 2018, firstly to make its purpose clearer, and secondly as part of a major change in design and in the governance of the development team. For instance, FW4SPL was made of several repositories, whereas Sight now contains all open-source features in a single one, for the sake of simplicity.

Lots of **tutorials** and **examples** can also be found in the `Samples` directory. The tutorials can help you to learn smoothly how to use **Sight**, detailed steps are described [here](https://sight.pages.ircad.fr/sight-doc/Tutorials/index.html).

## Features

* 2D/3D visualization of images, meshes, and many widgets, either with [VTK](https://www.vtk.org/) or [Ogre3D](https://www.ogre3d.org/),
* Configurable GUI
* Advanced memory management to support large data
* webcam, network video and local video playing based on [QtMultimedia](http://doc.qt.io/qt-5/qtmultimedia-index.html),
* mono and stereo camera calibration,
* [ArUco](https://sourceforge.net/projects/aruco/) optical markers tracking,
* [openIGTLink](http://openigtlink.org/) support through client and server services,
* TimeLine data, allowing to store buffers of various data (video, matrices, markers, etc...). These can be used to synchronize these data across time.
* Data serialization in xml/json/zip

# Applications

## VRRender

**Sight** comes with **VRRender**, a medical image and segmentation viewer. It supports many import formats including DICOM and VTK.

![VRRender main page](https://sight.pages.ircad.fr/sight-doc/_images/SDB.png)
![MPR](https://sight.pages.ircad.fr/sight-doc/_images/MPR.png)
![3D](https://sight.pages.ircad.fr/sight-doc/_images/3D.png)


## ARCalibration

**ARCalibration** is a user-friendly application to calibrate mono and stereo cameras. This software is a must-have since camera calibration is a mandatory step in any AR application.

![ARCalibration](https://sight.pages.ircad.fr/sight-doc/_images/calibration.png)
![ARCalibration](https://sight.pages.ircad.fr/sight-doc/_images/calibrationExt.png)

# Install

See [detailed install instructions](https://sight.pages.ircad.fr/sight-doc/Installation/index.html) for each supported platforms :
* [Windows](https://sight.pages.ircad.fr/sight-doc/Installation/src/WindowsInstall.html)
* [Linux](https://sight.pages.ircad.fr/sight-doc/Installation/src/LinuxInstall.html)
* [MacOs](https://sight.pages.ircad.fr/sight-doc/Installation/src/MacOSXInstall.html)

# Documentation

* [Documentation](https://sight.pages.ircad.fr/sight-doc)
* [Tutorials](https://sight.pages.ircad.fr/sight-doc/Tutorials/index.html)
* [Doxygen](https://sight.pages.ircad.fr/sight)
* Former FW4SPL [Blog](http://fw4spl-org.github.io/fw4spl-blog/) (new website coming soon)

# Support

Please note that our GitLab is currently only available in read-only access
for external developers and users. This is a restriction because of the licensing
model of GitLab. Since we use an EE version, we would be forced to pay for every
community user, and unfortunately we cannot afford it. This licensing
model might change in the
future https://gitlab.com/gitlab-org/gitlab-ee/issues/4382 though.

Until then, we gently ask our community users to use our GitHub mirror to [report any issues](https://github.com/IRCAD-IHU/sight/issues)
or propose [contributions](https://github.com/IRCAD-IHU/sight/pulls).

You can also get live community support on the [gitter chat room](https://gitter.im/IRCAD-IHU/sight-support).

# Annex

* [sight-deps](https://git.ircad.fr/Sight/sight-deps): contains the scripts to compile the external libraries used by **Sight** (Boost, VTK, ITK, Qt, ...)
