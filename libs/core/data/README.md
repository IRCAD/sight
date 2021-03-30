 # sight::data

This is one of the most important library in _Sight_. It defines data object that are going to be used in services.

_Sight_ relies on many third part libraries, and as such, it needs common data structures to communicate between them.
Basic data are provided, such as scalar values, string, geometric entities, as well as medical data used in computer 
assisted surgery such as images, segmentations, etc...

## Data classes

The base data class is `::sight::Data::Object`. It provides a default slot to warn subscribers when its content changes.
All concrete data classes implements this interface.

### Basic

- **Array**: generic and dynamic multi-dimensional array. 
- **Boolean**: boolean value.
- **Color**: RGBA color value.
- **Composite**: contains a map of objects.
- **Float**: float value.
- **Integer**: integer value.
- **List**: generic and dynamic list of elements.
- **String**: character array.
- **Vector**: generic and dynamic 1D array.

### Medical

- **ActivitySeries**: contains information and data of an application activity.
- **DicomSeries**: contains a DICOM series.
- **Equipment**: 
- **Histogram**:
- **Image**:
- **ImageSeries**:
- **Landmarks**:
- **ModelSeries**:
- **Patient**:
- **Reconstruction**:
- **ReconstructionTraits**:
- **Resection**:
- **ResectionDB**:
- **Series**:
- **SeriesDB**:
- **StructureTraits**:
- **StructureTraitsDictionary**:
- **Study**:
- **TransferFunction**:
  
### Computer vision

- **CalibrationInfo**: stores calibration images used to compute camera calibration.
- **Camera**: contains intrinsic and extrinsic parameters of a camera.
- **CameraSeries**: stores a collection of cameras.
- **FrameTL**:
- **MarkerMap**:
- **MarkerTL**:
- **MatrixTL**:

### Geometry

- **Line**:
- **Matrix4**: 4x4 transformation matrix.
- **Material**:
- **Mesh**:
- **Plane**:
- **PlaneList**:
- **Point**: 3D point.
- **PointList**: list of 3D points.

### Graph

- **Edge**: represents an edge in a `sight::data::Graph`.
- **Graph**: graph representation.
- **Node**: represents a node in a `sight::data::Graph`.
- **Port**: represents a connection point in a `sight::data::Node`.

### Obsolete

- **ProcessObject**: 
- **Tag**:
  