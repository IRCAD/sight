/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "fwMath/MeshFunctions.hpp"

#include "fwMath/VectorFunctions.hpp"

#include <boost/unordered_map.hpp>

#include <glm/glm.hpp>

#include <list>
#include <map>
#include <set>

namespace fwMath
{

//-----------------------------------------------------------------------------

bool intersect_triangle(fwVec3d _orig, fwVec3d _dir, fwVec3d _vert0,
                        fwVec3d _vert1, fwVec3d _vert2,
                        double& _t, double& _u, double& _v)
{
    const double Epsilon = 0.000001;

    fwVec3d edge1, edge2, tvec, pvec, qvec;

    /* find vectors for two edges sharing vert0 */
    edge1 = _vert1 - _vert0;
    edge2 = _vert2 - _vert0;

    /* begin calculating determinant - also used to calculate U parameter */
    pvec = ::fwMath::cross(_dir, edge2);

    /* if determinant is near zero, ray lies in plane of triangle */
    const double Det = ::fwMath::dot(edge1, pvec);

    if (Det > -Epsilon && Det < Epsilon)
    {
        return false;
    }
    const double Inv_det = 1.0 / Det;

    /* calculate distance from vert0 to ray origin */
    tvec = _orig - _vert0;

    /* calculate U parameter and test bounds */
    _u = Inv_det * ::fwMath::dot(tvec, pvec);
    if (_u < 0.0 || _u > 1.0)
    {
        return false;
    }

    /* prepare to test V parameter */
    qvec = ::fwMath::cross(tvec, edge1);

    /* calculate V parameter and test bounds */
    _v = Inv_det * ::fwMath::dot(_dir, qvec);
    if (_v < 0.0 || _u + _v > 1.0)
    {
        return false;
    }

    /* calculate t, ray intersects triangle */
    _t = Inv_det * ::fwMath::dot(edge2, qvec);
    return true;
}

//------------------------------------------------------------------------------

bool IsInclosedVolume(const fwVertexPosition& _vertex, const fwVertexIndex& _vertexIndex, const fwVec3d& _p)
{
    const unsigned int X = 0, Y = 1, Z = 2;
    const size_t ElementNbr = _vertexIndex.size();
    if ( ElementNbr == 0 )
    {
        return false;
    }

    // on regarde tous les triangles du maillage
    unsigned int intersectionNbr = 0;
    for ( size_t i = 0; i < ElementNbr; ++i )
    {
        //recuperation des trois sommets du triangle
        const fwVec3d P1 =
        {_vertex[ _vertexIndex[i][0] ][0], _vertex[ _vertexIndex[i][0] ][1], _vertex[ _vertexIndex[i][0] ][2]};
        const fwVec3d P2 =
        {_vertex[ _vertexIndex[i][1] ][0], _vertex[ _vertexIndex[i][1] ][1], _vertex[ _vertexIndex[i][1] ][2]};
        const fwVec3d P3 =
        {_vertex[ _vertexIndex[i][2] ][0], _vertex[ _vertexIndex[i][2] ][1], _vertex[ _vertexIndex[i][2] ][2]};

        //on enleve les triangles s'ils sont situes au dessus du point
        OSLM_TRACE(
            "Trg : " << i << " with Z = [" << P1[Z]  << "][" << P2[Z]  << "][" << P3[Z]  << "] compare with " <<
                _p[Z] );

        if ( !(P1[Z] > _p[Z] && P2[Z] > _p[Z] && P3[Z] > _p[Z] ) ) //trianglePotentiallyWellPositionned
        {
            //on teste la presence des vertex de part et d'autre des 3 axes.
            //Si P1[X] > P[X] alors il faut necessairement P2[X] < P[X] ou P3[X] < P[X], idem pour les 2 autres axes
            //En outre cela permet d'exclure les points qui sont situes sur les axes
            bool stop = false;
            for ( unsigned int axe = X; axe <= Y && !stop; ++axe )
            {
                const double Delta1 = P1[axe] - _p[axe];
                const double Delta2 = P2[axe] - _p[axe];
                const double Delta3 = P3[axe] - _p[axe];

                OSLM_TRACE("d1 : " << Delta1 << "d2 : " << Delta2 << "d3 : " << Delta3 );

                if ( Delta1 >= 0.f && Delta2 >= 0.f && Delta3 >= 0.f )
                {
                    stop = true; break;
                }
                if ( Delta1 < 0.f && Delta2 < 0.f && Delta3 < 0.f )
                {
                    stop = true; break;
                }
            }
            if ( !stop )
            {
                OSLM_TRACE("The face(" << i << ") is interesting to find a point in volume");

                fwVec3d orig = {_p[0], _p[1], _p[2]};

                fwVec3d dir = { 0.f, 0.f, 1.f};
                fwVec3d vert0 = { P1[0], P1[1], P1[2]};
                fwVec3d vert1 = { P2[0], P2[1], P2[2]};
                fwVec3d vert2 = { P3[0], P3[1], P3[2]};
                double t, u, v;
                if ( intersect_triangle(orig, dir, vert0, vert1, vert2, t, u, v) )
                {
                    //on ne garde que les points situes en dessous du point _p selon l'axe (Oz)
                    if (t < 0.f)
                    {
                        OSLM_TRACE(" t = " << t << " u = " << u << " v = " << v);
                        ++intersectionNbr;
                    }
                }
            }
        }
    }
    OSLM_TRACE("Nb intersection : " << intersectionNbr);
    return ( intersectionNbr%2 == 1 );
}

//-----------------------------------------------------------------------------

bool isBorderlessSurface(const fwVertexIndex& _vertexIndex)
{
    typedef std::pair< int, int >  Edge; // always Edge.first < Edge.second !!
    typedef ::boost::unordered_map< Edge, int >  EdgeHistogram;
    EdgeHistogram edgesHistogram;
    bool isBorderless = true;

    for(const fwVertexIndex::value_type& vertex :  _vertexIndex)
    {
        OSLM_ASSERT("Invalid vertex size: "<< vertex.size(), vertex.size() > 2 );
        ++edgesHistogram[std::make_pair(std::min(vertex[0], vertex[1]), std::max(vertex[0], vertex[1]) )];
        ++edgesHistogram[std::make_pair(std::min(vertex[0], vertex[2]), std::max(vertex[0], vertex[2]) )];
        ++edgesHistogram[std::make_pair(std::min(vertex[2], vertex[1]), std::max(vertex[2], vertex[1]) )];
    }

    for(const EdgeHistogram::value_type& histo :  edgesHistogram)
    {
        if (histo.second < 2)
        {
            isBorderless = false;
            break;
        }
    }

    return isBorderless;
}

//-----------------------------------------------------------------------------

// container of connected component
void findBorderEdges( const fwVertexIndex& _vertexIndex,
                      std::vector< std::vector<  std::pair< int, int  > > >& contours)
{
    typedef std::pair< int, int  >  Edge;
    typedef std::vector< Edge > Contour; // at Border
    typedef std::vector< Contour> Contours;

    std::map< Edge, int > edgesHistogram;
    for ( fwVertexIndex::value_type vertex : _vertexIndex)
    {
        assert(vertex.size() > 2 );
        int i1 = vertex[0];
        int i2 = vertex[1];
        int i3 = vertex[2];
        edgesHistogram[std::make_pair(std::min(i1, i2), std::max(i1, i2) )]++;
        edgesHistogram[std::make_pair(std::min(i1, i3), std::max(i1, i3) )]++;
        edgesHistogram[std::make_pair(std::min(i3, i2), std::max(i3, i2) )]++;
    }

    for ( const std::map< Edge, int >::value_type& elt1 : edgesHistogram )
    {
        if (elt1.second < 2) // an orphan found
        {
            Contour contour;
            contour.reserve(1000);
            std::list< Edge > fifo;
            Edge orphan = elt1.first;

            fifo.push_back(orphan);
            while( !fifo.empty() )
            {
                Edge current = fifo.front();
                contour.push_back( current );
                fifo.pop_front();
                edgesHistogram[current] = 2; // to mark it processed;

                // search neighbor at border and insert in fifo
                for ( const std::map< Edge, int >::value_type& elt2 : edgesHistogram )
                {
                    Edge candidate = elt2.first;
                    if ( elt2.second < 2 ) // at border
                    {
                        if ( candidate.first == current.first ||  candidate.second == current.second || // neighbor
                             candidate.first == current.second ||  candidate.second == current.first
                             )
                        {
                            edgesHistogram[candidate] = 2; // mark processed;
                            fifo.push_back( candidate );
                        }
                    }
                }
            }
            // all neighbor processed
            contours.push_back( contour );
        }
    }
}

//-----------------------------------------------------------------------------

bool closeSurface(  fwVertexPosition& _vertex, fwVertexIndex& _vertexIndex )
{
    typedef std::pair< int, int  >  Edge;
    typedef std::vector< Edge > Contour; // at Border
    typedef std::vector< Contour> Contours;

    Contours contours;
    findBorderEdges( _vertexIndex, contours);
    bool closurePerformed = !contours.empty();
    // close each hole
    for( const Contours::value_type& contour : contours )
    {
        size_t newVertexIndex = _vertex.size();
        // create gravity point & insert new triangle
        std::vector< float > massCenter(3, 0);

        for ( const Contour::value_type& edge : contour )
        {
            for (int i = 0; i < 3; ++i )
            {
                massCenter[i] += _vertex[edge.first][i];
                massCenter[i] += _vertex[edge.second][i];
            }
            // create new Triangle
            std::vector< int > triangleIndex(3);
            triangleIndex[0] = edge.first;
            triangleIndex[1] = edge.second;
            triangleIndex[2] = newVertexIndex;
            _vertexIndex.push_back( triangleIndex ); // TEST
        }
        for (int i = 0; i < 3; ++i )
        {
            massCenter[i] /= contour.size()*2;
        }
        _vertex.push_back( massCenter ); // normalize barycenter
    }
    return closurePerformed;
}

//-----------------------------------------------------------------------------

bool removeOrphanVertices( fwVertexPosition& _vertex, fwVertexIndex& _vertexIndex )
{
    fwVertexPosition newVertex;
    newVertex.reserve(  _vertex.size() );

    std::set< int > indexPointToKeep;

    for( const fwVertexIndex::value_type& vertex : _vertexIndex )
    {
        indexPointToKeep.insert( vertex[0] );
        indexPointToKeep.insert( vertex[1] );
        indexPointToKeep.insert( vertex[2] );
    }

    bool orphanFound = indexPointToKeep.size() != _vertex.size();

    if (orphanFound)
    {
        // rebuild index table according to element suppression
        int idx = 0;
        std::map< int, int > translate; // map oldIndex -> newIndex (to take into account removal
        std::set< int >::iterator idxIter;
        for (int indexPt : indexPointToKeep )
        {
            translate[ indexPt ] = idx++;
            newVertex.push_back(  _vertex[ indexPt ] );
        }

        for (fwVertexIndex::value_type& vertex : _vertexIndex )
        {
            vertex[0] = translate[ vertex[0]  ];
            vertex[1] = translate[ vertex[1]  ];
            vertex[2] = translate[ vertex[2]  ];
        }
        _vertex.swap(newVertex);
    }
    return orphanFound;
}

//-----------------------------------------------------------------------------

fwVec3d toBarycentricCoord(const fwVec3d& _P, const fwVec3d& _A, const fwVec3d& _B, const fwVec3d& _C)
{

    // From Christer Ericson Real-Time Collision Detection.

    /*
       In general, a point with barycentric coordinates (u, v, w) is inside (or on) the triangle(ABC) if and only if
       0 ≤ u, v, w ≤ 1, or alternatively if and only if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1.

       That barycentric coordinates actually parameterize the plane follows from P = uA + vB + wC really just being a
       reformulation of _P = _A + v(_B − _A) + w(_C − _A), with v and w arbitrary, as

       _P = _A + v(_B − _A) + w(_C − _A) = (1 − v − w)_A + v * _B + w * _C. [1].

       In [1], the two independent direction vectors AB and AC form a coordinate system with origin _A,
       allowing any point P in the plane to be parameterized in terms of v and w alone.

       Clearly, barycentric  coordinates is a redundant representation in that the third component can be expressed in
       terms of the first two. It is kept for reasons of symmetry.

       To solve for the barycentric coordinates, the expression:
       _P = _A + v(_B − _A) + w(_C − _A)

       or equivalently:
       v(_B − _A) + w(_C − _A) = _P − _A

       can be written as:
       v * v0 + w * v1 = v2

       where:
       v0 = B − A,
       v1 = C − A,
       and
       v2 = P − A.

       Now, a 2 × 2  system of linear equations can be formed by taking the dot product of both sides
       with both v0 and v1:
       (v v0 + w v1) · v0 = v2 dot·v0

       and
       (v v0 + w v1) · v1 = v2 dot·v1

       Because the dot product is a linear operator, these expressions are equivalent to:
       v (v0 dot v0) + w (v1 dot v0) = v2 dot v0

       and
       v (v0 dot v1) + w (v1 dot v1) = v2 dot v1.

       This system can be solved using Cramer’s rule.
     */

    fwVec3d baryCoord;

    const fwVec3d v0 = _B - _A; // AB Vector
    const fwVec3d v1 = _C - _A; // AC Vector
    const fwVec3d v2 = _P - _A; // AP Vector

    // Precompute some dot products.
    const double d00 = ::fwMath::dot(v0, v0);
    const double d01 = ::fwMath::dot(v0, v1);
    const double d11 = ::fwMath::dot(v1, v1);
    const double d20 = ::fwMath::dot(v2, v0);
    const double d21 = ::fwMath::dot(v2, v1);

    const double div = ((d00 * d11) - (d01 * d01));

    // Don't test the case in release to avoid performance issue.
    SLM_ASSERT("Degenerate triangle case leads to zero division.", div != 0.);

    // Inverse the denominator to speed up computation of v & w.
    const double invdenom = 1. / div;

    // Barycentric coordinates
    const double v = ((d11 * d20) - (d01* d21)) * invdenom;
    const double w = ((d00 * d21) - (d01 * d20)) * invdenom;
    const double u = 1. - v - w; // deduce last coordinate from the two others.

    baryCoord[0] = u;
    baryCoord[1] = v;
    baryCoord[2] = w;

    return baryCoord;

}

//-----------------------------------------------------------------------------

::glm::dvec4 toBarycentricCoord(const ::glm::dvec3& _P, const ::glm::dvec3& _A, const ::glm::dvec3& _B,
                                const ::glm::dvec3& _C,
                                const ::glm::dvec3& _D)
{

    /*
       In general, a point with barycentric coordinates (u, v, w,h) is inside (or on) the tetrahedron(ABCD) if and only
          if
       0 ≤ u, v, w, h ≤ 1, or alternatively if and only if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, 0 ≤ h ≤ 1, and v + w + h ≤ 1.

       The main idea of the volumic baricentric coordinate is a proportionality with the sub-tetrahedron volumes ratio
          over the whole volume.
       Considering one of the four vertex (_A, _B, _C, _D) v, the associated baricentric coordinate is equal to the
          volume of the tetrahedron build with the three other vertexes and P, divided by the total tetrahedron volume.

       As a result, the principle in the present algorithm, is to compute the three tetrahedron (_A,_B,_C,_P)
          (_A,_B,_D_P) (_A,_C,_D,_P) volume and the (_A,_B,_C,_D) volume.
       Then the ratio for respectivemy, _D, _C, _B vertexes are computed, and the last baricentric coordinate is
          obtained by the formula u + v + w + h = 1
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

fwVec3d fromBarycentricCoord(const fwVec3d& _baryCoord, const fwVec3d& _A, const fwVec3d& _B, const fwVec3d& _C)
{
    /*
       General formula (if [u, v, w] is normalized).
       x = (u * _A.x + v * _B.x + w * _C.x)
       y = (u * _A.y + v * _B.y + w * _C.y)
       z = (u * _A.z + v * _B.z + w * _C.z)
     */

    fwVec3d worldCoordinates;

    // Use standard notation for clarity.
    const double u = _baryCoord[0];
    const double v = _baryCoord[1];
    const double w = _baryCoord[2];

    [[maybe_unused]] const double sum = u + v + w; // Only used in the following assertion.

    // Don't test in release to avoid performance issue.
    SLM_ASSERT("Wrong barycentric coordinates.(u + v + w = " + std::to_string( sum ) + ")"
               , sum < 1. + 10e-9 && sum > 1. - 10e-9);

    const double x = (u * _A[0] + v * _B[0] + w * _C[0]);
    const double y = (u * _A[1] + v * _B[1] + w * _C[1]);
    const double z = (u * _A[2] + v * _B[2] + w * _C[2]);

    worldCoordinates[0] = x;
    worldCoordinates[1] = y;
    worldCoordinates[2] = z;

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

bool isInsideThetrahedron(const ::glm::dvec3& _P, const ::glm::dvec3& _A,
                          const ::glm::dvec3& _B, const ::glm::dvec3& _C, const ::glm::dvec3& _D)
{

    /*
       There are several wais to determine if a point is inside a tetrahedron.
       The present lgorithm make use of the barycentric coordinates
       It first the baricentric coordinate of the point inside the tetrahedron, and then checks if all of them are
          inbetween 0 and 1
     */
    const ::glm::dvec4 barycentricCoord = toBarycentricCoord(_P, _A, _B, _C, _D);
    return 0 <= barycentricCoord[0] &&  barycentricCoord[0] <= 1
           &&  0 <= barycentricCoord[1] &&  barycentricCoord[1] <= 1
           &&  0 <= barycentricCoord[2] &&  barycentricCoord[2] <= 1
           &&  0 <= barycentricCoord[3] &&  barycentricCoord[3] <= 1;
}

//-----------------------------------------------------------------------------

} // namespace fwMath
