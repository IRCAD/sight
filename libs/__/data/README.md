# sight::data

This is one of the most important libraries in _Sight_. It defines data objects that are going to be used in services.

_Sight_ relies on many third part libraries, and as such, it needs common data structures to communicate between them.
Basic data are provided, such as scalar values, string, geometric entities, as well as medical data used in computer
assisted surgery such as images, segmentations, etc...

## Data classes

The base data class is `sight::data::object`. It provides a default slot to warn subscribers when its content changes.
All concrete data classes implement this interface.

### Basic

- **ActivitySet**: activity container.
- **Array**: generic and dynamic multi-dimensional array.
- **Boolean**: boolean value.
- **Color**: RGBA color value.
- **Float**: float value.
- **Image**: defines an image.
- **Integer**: integer value.
- **Map**: contains a map of objects.
- **Object**: base class for each data object.
- **Set**: generic and dynamic set of elements.
- **series_set**: series container.
- **String**: character array.
- **TimeLine**: generic definition for collection of objects, each object being associated with a timestamp. It is intended to store lightweight objects.
- **Vector**: generic and dynamic 1D array.
- **GenericField**: generic objet containing a data.

### Medical

- **Activity**: contains information and data of an application activity.
- **dicom_series**: contains a DICOM series.
- **Equipment**: contains an equipment information.
- **histogram**: contains the histogram of a `sight::data::image`.
- **image_series**: a `sight::data::image` with the associated medical data.
- **Landmarks**: defines a set of spatial (3D) or color (4D) points.
- **model_series**: holds a medical data.
- **Patient**: holds a patient information.
- **Reconstruction**: defines a reconstruction object.
- **ReconstructionTraits**: defines Reconstruction traits containing an identifier, a mask a mesh and a structure traits associated to the reconstruction.
- **Resection**: defines a resection.
- **ResectionDB**: defines a resection container.
- **ROITraits**: defines ROI traits containing an identifier, an evaluated expression, a ROI mask node used for ROIand a structure traits associated to the ROI.
- **Series**: defines medical data.
- **series_set**: holds `sight::data::series`
- **structure_traits**: defines a structure traits containing various data all optional (type, category, class, color, native ROI expression,... )
- **StructureTraitsDictionary**: dictionary of `sight::data::structure_traits`.
- **StructureTraitsHelper**: helper of `sight::data::structure_traits`.
- **Study**: contains a medical study information.
- **TransferFunction**: defines a transfer function which associates color and values

### Computer vision

- **CalibrationInfo**: stores calibration images used to compute camera calibration.
- **Camera**: contains intrinsic and extrinsic parameters of a camera.
- **CameraSet**: stores a collection of cameras.
- **FrameTL**: defines a timeline of `sight::data::image`.
- **MarkerMap**: stores a map of optical markers (2D).
- **MarkerTL**: defines a timeline of 2D markers from their four corner positions.
- **MatrixTL**: defines a timeline of raw 4x4 matrices.
- **RawBufferTL**: defines a timeline of buffers.

### Geometry

- **Line**: line defined by two points.
- **Matrix4**: 4x4 transformation matrix.
- **Material**: material is represented by an ambient color and a diffuse color.
- **Mesh**: geometrical mesh composed of points, lines, triangles, quads or polygons.
- **Plane**: plane defined by three `sight::data::point`.
- **PlaneList**: list of `sight::data::plane`.
- **Point**: 3D point.
- **PointList**: list of 3D `sight::data::point`.
- **TransformationMatrix3D**: 4x4 transformation matrix.

### Graph

- **Edge**: represents an edge in a `sight::data::Graph`.
- **Graph**: graph representation.
- **Node**: represents a node in a `sight::data::Graph`.
- **Port**: represents a connection point in a `sight::data::Node`.

### Technical

- **Exception**: implements exception related to data.
- **generic_tl**: defines a timeline of template objects.

## How to use it

### CMake

```cmake
target_link_libraries( my_target <PUBLIC|PRIVATE> data)
```
