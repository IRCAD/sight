#include <boost/array.hpp>
#include <vector>

/// Type for a vector of tree double
typedef ::boost::array<double, 3> fwVec3d;

/// Type for a line defined by a position and a direction
typedef std::pair<fwVec3d, fwVec3d> fwLine;

/// Type for a plane defined by a normal and a distance.
/// the
typedef ::boost::array<double, 4> fwPlane;

/// Type for a matrix 4*4
typedef ::boost::array< ::boost::array< double,4 >, 4> fwMatrix4x4;

/// Type for vertex positions
typedef std::vector < std::vector <float> > fwVertexPosition;

/// Type for vertex index
typedef std::vector < std::vector <int> > fwVertexIndex;

