Description
===========

Extra repository for **FW4SPL**, a framework for fast and easy creation of applications, mainly in the medical imaging field.

This repository brings features for **Augmented Reality**:

* webcam, network video and local video playing based on [QtMultimedia](http://doc.qt.io/qt-5/qtmultimedia-index.html),
* mono and stereo camera calibration,
* [ArUco](https://sourceforge.net/projects/aruco/) optical markers tracking,
* [openIGTLink](http://openigtlink.org/) support through clients and servers services,
* TimeLine data, allowing to store buffers of various data (video, matrices, markers, etc...). These can be used to synchronize these data accross time.

For general information, see [documentation](http://fw4spl.readthedocs.org/) or go the [main repository](https://github.com/fw4spl-org/fw4spl).

Applications
============

**ARCalibration** is a user-friendly application to calibrate mono and stereo cameras. This software is a must-have since camera calibration is a mandatory step in any AR application.

Some examples can also be found in the *Samples/* directory. **Ex01VideoTracking** shows basic marker tracking on a video, **Ex02TimeLine** is a basic producer-consumer pattern sample with timeLine data, and **Ex03Igtl** shows some of the *openIGTLink* features.

Install
=======

See how to install fw4spl :
 - [stable version](http://fw4spl.readthedocs.org/en/master/Installation/index.html)
 - [latest version](http://fw4spl.readthedocs.org/en/dev/Installation/index.html)

To add this repository to your project build, you must add the path of the repository to the *ADDITIONAL_PROJECTS* CMake variable (multiple paths can be given by separating them with semicolons).

Documentation
=============

* **documentation**: http://fw4spl.readthedocs.org/
* **blog**: http://fw4spl-org.github.io/fw4spl-blog/

