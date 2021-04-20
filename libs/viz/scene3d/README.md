# sight::viz::scene3d

Contains all classes and helpers to manage Ogre3d renderer in sight.
This library links with `Ogre3d` and `OpenGL`.

## Classes

### Interfaces

- **IAdaptor**: provides behaviour of Ogre adaptor services.
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

Contains the `ressource.cfg.in` file, this file is updated at CMake configure time and will contain path to ogre resources (GLSL, compositors, ...).

### collisionTools


- **CollisionTools**: detects collision with shapes for ray casting.

### compositor

Contains classes to deal with Ogre3d compositors.

- **ChainManager**: manages the compositors in a layer view.
- **Core**: manages principal compositor for a layer's 3D scene.
- **MaterialMgrListener**: generates transparency techniques at runtime.
- **SaoListener**: subclass of `Ogre::CompositorInstance::Listener`. Reimplements its functions for compatibility.
- **types**: defines `enum transparencyTechnique`, holding OIT (Ordrer Independent Transparency) techniques supported.

#### listener

- **AutoStereoCompositorListener**: implements `Ogre::MaterialManager::Listener`.
- **RayExitDepthListener**: Listener used in mixed surface/volume rendering. Creates volume entry point computation techniques for meshes.


### factory

Contains factory classes to build Ogre objects.

### helper

Contains helpers classes and functions.

- **Camera**: contains functions to compute view and projection matrices, to convert screen space to view space, etc.
- **Font**: contains functions to generate fonts and materials for text rendering.
- **ManualObject**: contains functions to generate simple Ogre shapes (cylinders, cones, cubes, spheres)
- **Mesh**: implements `copyColor`, function copying the colour layer of a mesh.
- **Scene**: implements `getNodeById`, function looking for a given node in a scene manager.
- **Shading**: contains various functions to retrieve information about shaders.
- **Technique**: implements `copyToMaterial`, function copying a technique to an existing material.


### interactor

Contains interface and classes for interactions, clippingBox, trackball interactor and mesh picking.

- **ClippingBoxInteractor**: Picks volume rendering widgets and updates clipping cube.
- **IInteractor**: interface implementation for all the interaction with the mouse
- **MeshPickerInteractor**: runs picking queries when the user clicks in the scene.
- **TrackballInteractor**: trackball camera interactions. Lets the user move the camera around a point of interest when moving the mouse.

### overlay

- **ViewportListener**: listens to render events triggered for a given viewport and activates the overlays enabled for that viewport.

### picker

- **IPicker**: interface for pickers.

### rc

Contains all resources needed by Ogre3d, such as GLSL, materials and compositors scripts, some meshes and textures ...

### registry

Internal mechanism: provides the instances of the factory registry.

### test

Contains unit-tests to test the library.

### ui

**Deprecated**

### vr

Contains classes related to volume rendering processes.

- **GridProxyGeometry**: proxy geometry used for volume ray tracing.
- **IllumAmbientOcclusionSAT**: computes the illumination volume for extinction based shading.
- **IVolumeRenderer**: interface for volume renderers.
- **PreIntegrationTable**: computes the table for pre-integrated rendering.
- **RayEntryCompositor**: creates a compositor to compute volume ray entry points.
- **RayTracingVolumeRenderer**: implements a simple GPU ray-tracing renderer.
- **SummedAreaTable**: summed area table of a 3D image.


### widget

- **ClippingBox**: holds the clipping widgets, used to clip/move/scale the volume.

## How to use it

### CMake

```cmake
target_link_libraries( myTarget <PUBLIC|PRIVATE> viz_scene3d)
```

