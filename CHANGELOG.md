# fw4spl-ogre 17.2.0

## Refactor:

### SVolumeRender

*Store clipping matrices the same way VTK does.*

Now clipping box transforms are stored in world space instead of texture space. Clipping transforms can be passed from/to VTK that way.

Removes the **broken** slice based volume renderer.

### plugins

*Remove the freeimage plugin.*

### textures

*Convert all png and tga textures to dds.*

DDS is supported natively by ogre without the freeimage plugin.

The freeimage BinPkg is awful to maintain and should be considered as deprecated from now on.

### cmake

*Removed racy backward compatibility.*

## New features:

### *

*Update ogre to 1.11.*

This brings a bunch of fixes following API changes. Among them :

* default light direction is set to the camera's view direction, was implicitly the case in ogre 1.10 but changed in 1.11
* `Codec_FreeImage` plugin loaded to support common image file formats
* plugin config parsing was modified to be able to load multiple plugins
* `::Ogre::Affine3` replaces `::Ogre::Matrix4` when we need to decompose a matrix
* colour masks are enabled when computing volume ray entry points




# fw4spl-ogre 17.1.0

## Bug fixes:

### fwRenderOgre

*Correct valgrind errors and leaks.*

Memory errors were fixed and memory leaks detected by valgrind (memcheck) on the test suite:
* One out of bounds in read `fwRenderOgre::helper::Mesh`
* Memory leaks on Ogre root destruction

### fwRenderOgre

*Missing headers.*

### fwRenderOgre

*Remove clang specific hack about OpenMP.*

Remove a clang specific OpenMP hack in our CMake code.

### R2VBRenderable

*Clear vertex declaration before filling it.*

This caused the varying to be duplicated, and thus the program link to fail.

### Mesh

*Generate normals each time the mesh is modified.*

For triangle based meshes, when we don't have normals, we generate them. The problem was that it was only done on the first update of the mesh. If points were added to the mesh, the corresponding normals were not computed accordingly, thus the normal layer ended to be be shorter than the position layer. This led eventually to crash at some  point...

## New features:

### fwRenderOgre

*Add a helper to convert pixel to view space position.*

The function `convertPixelToViewSpace` translates a pixel coordinates to view space coordinates.

### SNegato2D,3D

*Use the transparency of the transfer function (optionally).*

A new option was added to use the transparency of the transfer function.

### SAxis

*Add a label configurable option.*

SAxis now has an option `label` that can be set to `true` or `false`
to display or hide the axis labels (`true` by default).

### SRender

*Add a 'sync' renderMode.*

In the following of our recent rework of the synchronization for real-time augmented-reality, this new mode allows to make the Ogre generic scene compatible with the approach. The example ExSimpleARCVOgre was reworked to use the new sync mechanism and proves that this works.

## Documentation:

### visuOgreAdaptor

*Update some documentation.*

The documentation of several adaptors were fixed.




# fw4spl-ogre 17.0.0

## Bug fixes:

### Mesh

*Do not compute normals with point based meshes.*

We are not supposed to compute normals when displaying a point based mesh only, however the condition testing this was wrong in the code.

### fwRenderOgre

*Missing headers.*

### material

*Ensure Common.{program,materials} are parsed first.*

Depending on your file system, the `Common.program` could be parsed after the `Video.program`, causing it to fail because it needs `TransferFunction_FP`, which lies inside Common.program to be declared first.

### IDVR

*Compute the countersink geometry in world space.*

We changed the way the MImP IDVR countersink geometry (CSG) is defined/computed:

* CSG used to have a fixed viewport radius, it now has a fixed angle and isn't resized when zooming with the camera.
* Depth lines now start at the importance zone and are in the same unit as the image's spacing.
* The CSG border had to be removed because we couldn't easily adapt it to this new method :crying_cat_face:
* Greyscale CSG and modulation are now separate.

### SMesh

*Build error with GCC 5.4.0.*

### SAxis

*Make the visibility changeable and fix adaptor stop.*

### RayTracingVolumeRenderer

*Do not delay the resize of the viewport.*

Delaying the resize of the entry points textures broke the auto-stereoscopic rendering. This was introduced recently in 6e2946 but was actually not necessary and did not fix anything.

## New features:

### SLandmarks

*Add adaptor to display landmarks.*

The new adaptor SLandmarks displays landmarks with Ogre generic scene.

### SLine

*Allow length update via a slot.*

A `updateLength()` slot was implemented to update of the length of the rendered line.

## Refactor:

### Interactors

*Allow adaptors to be an interactor implementation.*

This is a first step in the refactor of interactors. We plan to implement interactors directly instead of using the only one SInteractorStyle that instantiates sub-classes. It is actually more complicated than if the interactor does the job directly.

In ARPerfusion, we had to create a new interactor to select regions (ARPerfusion!10). We wanted to implement it as an adaptor, which allows us to test if the design works. So we modify the inheritance to allow adaptors to behave as an interactor directly. Consider this as a temporary step in the migration of interactors, where both solutions are possible.

Besides this, there are some changes that might seem unrelated but they were necessary for our new interactor. There is a first fix to allow all kind of meshes to be displayed with a SPointList adaptor. Then there is a second commit to fix the cell color textures, which were not correctly fetched from the texture used to store them.


