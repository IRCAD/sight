# sight::viz::scene3d

Contains all classes and helpers to manage Ogre3d renderer in sight.
This library links with `Ogre3d` and `OpenGL`.

## Classes

### Interfaces

- **IAdaptor**: provides behavior of Ogre adaptor services.
- **IGraphicsWorker**: interface to run gpu resource handling tasks in parallel.
- **ILight**: interface to manage the light adaptor.
- **IParameter**: contains common code to upload shader parameters from Sight data.
- **ITransformable**: provides behavior for all classes containing a transformation matrix.
- **IWindowInteractor**: manages ogreRenderWindowInteractor in a window.

### Base

- **Layer**: allows to render multiple scenes in the same render window with viewports.
- **Material**: manages a generic material.
- **Mesh**: manages a generic mesh, from `sight::data::Mesh` to an Ogre3d structure that can be rendered.
- **ogre**: defines a static variable for resource group name ("Sight").
- **Plane**: manages a plane mesh on which a slice texture will be applied.
- **R2VBRenderable**: implements a render-to-vertex-buffer (r2vb) process (GL_TRANSFORM_FEEDBACK).
- **SRender**: defines a generic scene service that shows adaptors in a 3D Ogre scene.
- **Text**: displays overlay text.
- **TransfertFunction**: manages a tranfer function from a GPU point of view.
- **Utils**: provides some Ogre3d general functions for Sight
- **WindowManager**: stores all Ogre render windows and manage their deletion (singleton).

### cmake

Contains the `ressource.cfg.in` file, this file is updated at cmake configure time and will contain path to ogre resources (glsl, compositors, ...).

### collisionTools

Contains the `collisionTools` class, used to do ray casts.

### compositor

Contains classes to deal with Ogre3d compositors.

### factory

Contains factory classes to build Ogre objects.

### helper

Contains helpers classes and functions.

### interactor

Contains interface and classes for interactions, clippingBox, trackball interactor and mesh picking.

### overlay

Contains the `ViewportListener`, that listens to render events triggered for a given viewport and activates the overlays enabled for that viewport.

### picker

Contains interface for pickers (`IPicker`).

### rc

Contains all resources needed by Ogre3d, such as glsl, materials and compositors scripts, some meshes and textures ...

### registry

### test

Contains unit-tests to test the library.

### ui

**Deprecated**

### vr

Contains classes related to volume rendering processes.

### widget

Holds the clipping widget, used to clip/move/scale a volume.

## How to use it

### CMake

```cmake
target_link_libraries( myTarget <PUBLIC|PRIVATE> viz_scene3d)
```
