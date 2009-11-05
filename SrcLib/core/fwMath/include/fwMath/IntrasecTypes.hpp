#include <boost/array.hpp>

/// Type for a vector of tree double
typedef ::boost::array<double, 3> fwVec3d;

/// Type for a line defined by a position and a direction
typedef std::pair<fwVec3d, fwVec3d> fwLine;

/// Type for a plane defined by a normal and a distance.
/// the
typedef ::boost::array<double, 4> fwPlane;

/// Type for a matrix 4*4
typedef ::boost::array< ::boost::array< double,4 >, 4> fwMatrix;
