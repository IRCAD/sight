# sight::viz::scene3d

Contains all classes and helpers to manage Ogre3d renderer in sight.
This library links with `Ogre3d` and `OpenGL`.

## Classes

### Interfaces

- **adaptor**: provides behaviour of Ogre adaptor services.
- **graphics_worker**: interface to run gpu resource handling tasks in parallel.
- **light_adaptor**: interface to manage the light adaptor.
- **parameter_adaptor**: contains common code to upload shader parameters from Sight data.
- **transformable**: provides behavior for all classes containing a transformation matrix.
- **window_interactor**: manages ogreRenderwindow_interactor in a window.

### Base

- **Layer**: allows to render multiple scenes in the same render window with viewports.
- **Material**: manages a generic material.
- **Mesh**: manages a generic mesh, from `sight::data::mesh` to an Ogre3d structure that can be rendered.
- **ogre**: defines a static variable for resource group name ("Sight").
- **Plane**: manages a plane mesh on which a slice texture will be applied.
- **r2vb_renderable**: implements a render-to-vertex-buffer (r2vb) process (GL_TRANSFORM_FEEDBACK).
- **render**: defines a generic scene service that shows adaptors in a 3D Ogre scene.
- **Text**: displays overlay text.
- **TransfertFunction**: manages a transfer function from a GPU point of view.
- **utils**: provides some Ogre3d general functions for Sight
- **window_manager**: stores all Ogre render windows and manage their deletion (singleton).

### cmake

Contains the `ressource.cfg.in` file, this file is updated at CMake configure time and will contain path to ogre resources (GLSL, compositors, ...).

### collisionTools


- **collision_tools**: detects collision with shapes for ray casting.

### compositor

Contains classes to deal with Ogre3d compositors.

- **chain_manager**: manages the compositors in a layer view.
- **core**: manages principal compositor for a layer's 3D scene.
- **material_mgr_listener**: generates transparency techniques at runtime.
- **sao_listener**: subclass of `Ogre::CompositorInstance::Listener`. Reimplements its functions for compatibility.
- **types**: defines `enum transparencyTechnique`, holding OIT (Order Independent Transparency) techniques supported.

#### listener

- **auto_stereo**: implements `Ogre::MaterialManager::Listener`.
- **ray_exit_depth**: Listener used in mixed surface/volume rendering. Creates volume entry point computation techniques for meshes.


### factory

Contains factory classes to build Ogre objects.

### helper

Contains helpers classes and functions.

- **Camera**: contains functions to compute view and projection matrices, to convert screen space to view space, etc.
- **Font**: contains functions to generate fonts and materials for text rendering.
- **manual_object**: contains functions to generate simple Ogre shapes (cylinders, cones, cubes, spheres)
- **Mesh**: implements `copyColor`, function copying the colour layer of a mesh.
- **scene**: implements `getNodeById`, function looking for a given node in a scene manager.
- **shading**: contains various functions to retrieve information about shaders.
- **technique**: implements `copyToMaterial`, function copying a technique to an existing material.


### interactor

Contains interface and classes for interactions, clippingBox, trackball interactor and mesh picking.

- **clipping_box_interactor**: Picks volume rendering widgets and updates clipping cube.
- **base**: interface implementation for all the interaction with the mouse
- **mesh_picker_interactor**: runs picking queries when the user clicks in the scene.
- **trackball_interactor**: trackball camera interactions. Lets the user move the camera around a point of interest when moving the mouse.

### overlay

- **ViewportListener**: listens to render events triggered for a given viewport and activates the overlays enabled for that viewport.

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

- **grid_proxy_geometry**: proxy geometry used for volume ray tracing.
- **illum_ambient_occlusion_sat**: computes the illumination volume for extinction based shading.
- **volume_renderer**: interface for volume renderers.
- **pre_integration_table**: computes the table for pre-integrated rendering.
- **ray_entry_compositor**: creates a compositor to compute volume ray entry points.
- **ray_tracing_volume_renderer**: implements a simple GPU ray-tracing renderer.
- **summed_area_table**: summed area table of a 3D image.


### widget

- **clipping_box**: holds the clipping widgets, used to clip/move/scale the volume.

## How to use it

### CMake

```cmake
target_link_libraries( my_target <PUBLIC|PRIVATE> viz_scene3d)
```

