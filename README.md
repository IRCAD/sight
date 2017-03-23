Description
===========

Extra repository for **FW4SPL**, a framework for fast and easy creation of applications, mainly in the medical imaging field.

This repository brings a new 3D rendering backend using [Ogre3D](http://www.ogre3d.org/):

* regular surfacic meshes rendering for reconstruction,
* 2D and 3D negato medical image display with transfer function support,
* Order-independent transparency (several techniques implemented such as Depth-peeling, Weighted-blended order independent transparency, and Hybrid Transparency),
* customizable shaders and parameters edition.

For general information, see [documentation](http://fw4spl.readthedocs.org/) or go the [main repository](https://github.com/fw4spl-org/fw4spl).

Applications
============

**OgreViewer** is a demonstration application to show the rendering features of the Ogre3D backend.

Install
=======

See how to install *FW4SPL* [here](http://fw4spl.readthedocs.org/en/dev/Installation/index.html).

To be able to compile the code in this repository, you need first to build Ogre3D library. So in your *fw4spl-deps* build, you must set the *ENABLED_OGRE_DEPS* CMake variable to *ON* and call *'make ogre'*. Then to add this repository to your project build, you must add the path of the repository to the *ADDITIONAL_PROJECTS* CMake variable (multiple paths can be given by separating them with semicolons).

Documentation
=============

* **documentation**: http://fw4spl.readthedocs.org/
* **blog**: http://fw4spl-org.github.io/fw4spl-blog/

