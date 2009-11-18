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

//intersection du triangle composé des vertex vert1, vert2, vert3  avec la droite parallèle à (Oz) passant par P.
// En cas de point d'intersection, la méthode renvoie la valeur true (sinon, false) et t correspond à la distance orientée de P au point d'intersction.
// u, et v correspondent aux coordonnées  du point d'intersection dans le repère du triangle (O,u,v). Avec :
// O = vert1. u =  vert2 - O. v =  vert3 - O.
//"Fast, Minimum Storage Ray/Triangle Intersection", Tomas Möller, Ben Trumbore.
FWMATH_API bool intersect_triangle(fwVec3d _orig, fwVec3d _dir, fwVec3d _vert0, fwVec3d _vert1, fwVec3d _vert2, double &_t, double &_u, double &_v);
}

#endif /* _FWMATH_MESHFUNCTIONS_HPP_ */
