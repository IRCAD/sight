/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "geometry/glm/mesh.hpp"

#include <core/spyLog.hpp>

#include <glm/glm.hpp>

#include <string>

namespace sight::geometry::glm
{

//-----------------------------------------------------------------------------

::glm::dvec3 toBarycentricCoord(const ::glm::dvec3& _P, const ::glm::dvec3& _A, const ::glm::dvec3& _B,
                                const ::glm::dvec3& _C)
{
    ::glm::dvec3 baryCoord;

    const ::glm::dvec3 v0 = _B - _A; // AB Vector
    const ::glm::dvec3 v1 = _C - _A; // AC Vector
    const ::glm::dvec3 v2 = _P - _A; // AP Vector

    // Precompute some dot products.
    const double d00 = ::glm::dot(v0, v0);
    const double d01 = ::glm::dot(v0, v1);
    const double d11 = ::glm::dot(v1, v1);
    const double d20 = ::glm::dot(v2, v0);
    const double d21 = ::glm::dot(v2, v1);

    const double div = ((d00 * d11) - (d01 * d01));

    // Don't test the case in release to avoid performance issue.
    SLM_ASSERT("Degenerate triangle case leads to zero division.", !(div >= -10e-16 && div <= 10e-16));

    // Inverse the denominator to speed up computation of v & w.
    const double invdenom = 1. / div;

    // Barycentric coordinates
    const double v = ((d11 * d20) - (d01* d21)) * invdenom;
    const double w = ((d00 * d21) - (d01 * d20)) * invdenom;
    const double u = 1. - v - w; // deduce last coordinate from the two others.

    baryCoord.x = u;
    baryCoord.y = v;
    baryCoord.z = w;

    return baryCoord;
}

//-----------------------------------------------------------------------------

::glm::dvec4 toBarycentricCoord(const ::glm::dvec3& _P, const ::glm::dvec3& _A, const ::glm::dvec3& _B,
                                const ::glm::dvec3& _C,
                                const ::glm::dvec3& _D)
{

    /*
       In general, a point with barycentric coordinates (u, v, w,h) is inside (or on) the tetrahedron(ABCD)
          if and only if 0 ≤ u, v, w, h ≤ 1, or alternatively
          if and only if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, 0 ≤ h ≤ 1, and v + w + h ≤ 1.

       The main idea of the barycentric volume coordinate is a proportionality with the ratio of the sub-tetrahedron
          volumes over the whole volume. Considering one of the four vertexes (_A, _B, _C, _D), the associated
             barycentric
          coordinate are equal to the volume of the tetrahedron build with the three other vertexes and P,
          divided by the total tetrahedron volume.

       As a result, the principle in the present algorithm, is to compute the three tetrahedron (_A,_B,_C,_P)
          (_A,_B,_D_P) (_A,_C,_D,_P) volume and the (_A,_B,_C,_D) volume. Then the ratio for respectively,
          _D, _C, _B vertexes are computed, and the last barycentric coordinate is obtained by the formula
          u + v + w + h = 1
     */

    ::glm::dvec4 baryCoord;

    const ::glm::dvec3 vab = _B - _A; // AB Vector
    const ::glm::dvec3 vac = _C - _A; // AC Vector
    const ::glm::dvec3 vad = _D - _A; // AD Vector

    const ::glm::dvec3 vap = _P - _A; // AP Vector

    const double volumeB = ::glm::dot(vap, ::glm::cross(vac, vad));
    const double volumeC = ::glm::dot(vap, ::glm::cross(vad, vab));
    const double volumeD = ::glm::dot(vap, ::glm::cross(vab, vac));

    const double volumeTot = ::glm::dot(vab, ::glm::cross(vac, vad));

    // Don't test the case in release to avoid performance issue.
    SLM_ASSERT("Degenerate triangle case leads to zero division.", volumeTot != 0.);

    // Inverse the denominator to speed up computation of v & w.
    const double invdenom = 1. / volumeTot;

    // Barycentric coordinates
    const double v = volumeB * invdenom;
    const double w = volumeC * invdenom;
    const double h = volumeD * invdenom;
    const double u = 1. - v - w - h; // deduce last coordinate from the two others.

    baryCoord[0] = u;
    baryCoord[1] = v;
    baryCoord[2] = w;
    baryCoord[3] = h;

    return baryCoord;

}

//-----------------------------------------------------------------------------

::glm::dvec3 fromBarycentricCoord(const ::glm::dvec3& _baryCoord, const ::glm::dvec3& _A, const ::glm::dvec3& _B,
                                  const ::glm::dvec3& _C)
{
    ::glm::dvec3 worldCoordinates;

    // Use standard notation for clarity.
    const double u = _baryCoord[0];
    const double v = _baryCoord[1];
    const double w = _baryCoord[2];

    [[maybe_unused]] const double sum = u + v + w; // Only used in the following assertion.

    // Don't test in release to avoid performance issue.
    SLM_ASSERT("Wrong barycentric coordinates.(u + v + w = " + std::to_string( sum ) + ")"
               , sum < 1. + 10e-9 && sum > 1. - 10e-9);

    worldCoordinates = u * _A + v * _B + w * _C;

    return worldCoordinates;
}

//-----------------------------------------------------------------------------

::glm::dvec3 fromBarycentricCoord(const ::glm::dvec4& _baryCoord, const ::glm::dvec3& _A, const ::glm::dvec3& _B,
                                  const ::glm::dvec3& _C,
                                  const ::glm::dvec3& _D)
{
    /*
       General formula (if [u, v, w, h] is normalized).
       x = (u * _A.x + v * _B.x + w * _C.x + h * _D.x)
       y = (u * _A.y + v * _B.y + w * _C.y + h * _D.y)
       z = (u * _A.z + v * _B.z + w * _C.z + h * _D.z)
     */

    // Use standard notation for clarity.
    const double u = _baryCoord[0];
    const double v = _baryCoord[1];
    const double w = _baryCoord[2];
    const double h = _baryCoord[3];

    [[maybe_unused]] const double sum = u + v + w + h; // Only used in the following assertion.

    // Don't test in release to avoid performance issue.
    SLM_ASSERT("Wrong barycentric coordinates.(u + v + w = " + std::to_string( sum ) + ")"
               , sum < 1. + 10e-9 && sum > 1. - 10e-9);

    return u * _A + v * _B + w * _C + h * _D;

}

//------------------------------------------------------------------------------

bool isInsideTetrahedron(const ::glm::dvec3& _P, const ::glm::dvec3& _A,
                         const ::glm::dvec3& _B, const ::glm::dvec3& _C, const ::glm::dvec3& _D)
{

    /*
       There are several ways to determine if a point is inside a tetrahedron.
       The present algorithm make use of the barycentric coordinates.
       It first computes the barycentric coordinate of the point inside the tetrahedron, and then checks if all of them
          are
          in between 0 and 1.
     */
    const ::glm::dvec4 barycentricCoord = toBarycentricCoord(_P, _A, _B, _C, _D);
    return isInsideTetrahedron(barycentricCoord);
}
//------------------------------------------------------------------------------

bool isInsideTetrahedron( const ::glm::dvec4 barycentricCoordPInsideABCD)
{

    /*
       There are several ways to determine if a point is inside a tetrahedron.
       The present algorithm make use of the barycentric coordinates.
       It checks if all of the barycentric coordinates are in between 0 and 1.

     */
    return 0 <= barycentricCoordPInsideABCD[0] &&  barycentricCoordPInsideABCD[0] <= 1
           &&  0 <= barycentricCoordPInsideABCD[1] &&  barycentricCoordPInsideABCD[1] <= 1
           &&  0 <= barycentricCoordPInsideABCD[2] &&  barycentricCoordPInsideABCD[2] <= 1
           &&  0 <= barycentricCoordPInsideABCD[3] &&  barycentricCoordPInsideABCD[3] <= 1;
}

//-----------------------------------------------------------------------------

} // namespace sight::geometry::glm
