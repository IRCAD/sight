#ifndef _FWMATH_MESHFUNCTIONS_HPP_
#define _FWMATH_MESHFUNCTIONS_HPP_

#include <math.h>
#include <vector>

#include <fwCore/base.hpp>
#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/export.hpp"


namespace fwMath
{
FWMATH_API bool IsInclosedVolume(const fwVertexPosition _vertex, const fwVertexIndex _vertexIndex, const fwVec3d &_P );

//intersection du triangle compos� des vertex vert1, vert2, vert3  avec la droite parall�le � (Oz) passant par P.
// En cas de point d'intersection, la m�thode renvoie la valeur true (sinon, false) et t correspond � la distance orient�e de P au point d'intersction.
// u, et v correspondent aux coordonn�es  du point d'intersection dans le rep�re du triangle (O,u,v). Avec :
// O = vert1. u =  vert2 - O. v =  vert3 - O.
//"Fast, Minimum Storage Ray/Triangle Intersection", Tomas M�ller, Ben Trumbore.
FWMATH_API bool intersect_triangle(fwVec3d _orig, fwVec3d _dir, fwVec3d _vert0, fwVec3d _vert1, fwVec3d _vert2, double &_t, double &_u, double &_v);

/// test whatever a vertex is duplicated or not
FWMATH_API bool isBorderlessSurface( const fwVertexIndex _vertexIndex);

FWMATH_API void findBorderEdges( const fwVertexIndex _vertexIndex , std::vector< std::vector<  std::pair< int, int  > > > &contours);


FWMATH_API bool closeSurface( fwVertexPosition &_vertex, fwVertexIndex &_vertexIndex );

}

#endif /* _FWMATH_MESHFUNCTIONS_HPP_ */
