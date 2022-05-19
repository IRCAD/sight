/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "geometry/data/MeshFunctions.hpp"

#include "geometry/data/VectorFunctions.hpp"

#include <boost/unordered_map.hpp>

#include <glm/glm.hpp>

#include <list>
#include <map>
#include <set>

namespace sight::geometry::data
{

//-----------------------------------------------------------------------------

bool intersect_triangle(
    fwVec3d _orig,
    fwVec3d _dir,
    fwVec3d _vert0,
    fwVec3d _vert1,
    fwVec3d _vert2,
    double& _t,
    double& _u,
    double& _v
)
{
    const double Epsilon = 0.000001;

    fwVec3d edge1, edge2, tvec, pvec, qvec;

    /* find vectors for two edges sharing vert0 */
    edge1 = _vert1 - _vert0;
    edge2 = _vert2 - _vert0;

    /* begin calculating determinant - also used to calculate U parameter */
    pvec = geometry::data::cross(_dir, edge2);

    /* if determinant is near zero, ray lies in plane of triangle */
    const double Det = geometry::data::dot(edge1, pvec);

    if(Det > -Epsilon && Det < Epsilon)
    {
        return false;
    }

    const double Inv_det = 1.0 / Det;

    /* calculate distance from vert0 to ray origin */
    tvec = _orig - _vert0;

    /* calculate U parameter and test bounds */
    _u = Inv_det * geometry::data::dot(tvec, pvec);
    if(_u < 0.0 || _u > 1.0)
    {
        return false;
    }

    /* prepare to test V parameter */
    qvec = geometry::data::cross(tvec, edge1);

    /* calculate V parameter and test bounds */
    _v = Inv_det * geometry::data::dot(_dir, qvec);
    if(_v < 0.0 || _u + _v > 1.0)
    {
        return false;
    }

    /* calculate t, ray intersects triangle */
    _t = Inv_det * geometry::data::dot(edge2, qvec);
    return true;
}

//------------------------------------------------------------------------------

bool IsInclosedVolume(const fwVertexPosition& _vertex, const fwVertexIndex& _vertexIndex, const fwVec3d& _p)
{
    const unsigned int X = 0, Y = 1, Z = 2;
    const std::size_t ElementNbr = _vertexIndex.size();
    if(ElementNbr == 0)
    {
        return false;
    }

    // Check all mesh triangles
    unsigned int intersectionNbr = 0;
    for(std::size_t i = 0 ; i < ElementNbr ; ++i)
    {
        // get triangle vertices.
        const fwVec3d P1 =
        {_vertex[std::size_t(_vertexIndex[i][0])][0], _vertex[std::size_t(_vertexIndex[i][0])][1],
         _vertex[std::size_t(_vertexIndex[i][0])][2]
        };
        const fwVec3d P2 =
        {_vertex[std::size_t(_vertexIndex[i][1])][0], _vertex[std::size_t(_vertexIndex[i][1])][1],
         _vertex[std::size_t(_vertexIndex[i][1])][2]
        };
        const fwVec3d P3 =
        {_vertex[std::size_t(_vertexIndex[i][2])][0], _vertex[std::size_t(_vertexIndex[i][2])][1],
         _vertex[std::size_t(_vertexIndex[i][2])][2]
        };

        // remove all triangles above point.
        if(!(P1[Z] > _p[Z] && P2[Z] > _p[Z] && P3[Z] > _p[Z])) //trianglePotentiallyWellPositionned
        {
            // We check vertices on each side of the 3 axis.
            // If P1[X] > P[X] So we need P2[X] < P[X] ou P3[X] < P[X], same way on the 2 other axis.
            // By doing so we can exclude points that are localized on axis.
            bool stop = false;
            for(unsigned int axe = X ; axe <= Y && !stop ; ++axe)
            {
                const double Delta1 = P1[axe] - _p[axe];
                const double Delta2 = P2[axe] - _p[axe];
                const double Delta3 = P3[axe] - _p[axe];

                if(Delta1 >= 0.f && Delta2 >= 0.f && Delta3 >= 0.f)
                {
                    stop = true;
                    break;
                }

                if(Delta1 < 0.f && Delta2 < 0.f && Delta3 < 0.f)
                {
                    stop = true;
                    break;
                }
            }

            if(!stop)
            {
                fwVec3d orig = {_p[0], _p[1], _p[2]};

                fwVec3d dir = {0.f, 0.f, 1.f};
                fwVec3d vert0 = {P1[0], P1[1], P1[2]};
                fwVec3d vert1 = {P2[0], P2[1], P2[2]};
                fwVec3d vert2 = {P3[0], P3[1], P3[2]};
                double t, u, v;
                if(intersect_triangle(orig, dir, vert0, vert1, vert2, t, u, v))
                {
                    // We only keep points below _p (following Oz axis).
                    if(t < 0.f)
                    {
                        ++intersectionNbr;
                    }
                }
            }
        }
    }

    return intersectionNbr % 2 == 1;
}

//-----------------------------------------------------------------------------

bool isBorderlessSurface(const fwVertexIndex& _vertexIndex)
{
    typedef std::pair<int, int> Edge; // always Edge.first < Edge.second !!
    typedef boost::unordered_map<Edge, int> EdgeHistogram;
    EdgeHistogram edgesHistogram;
    bool isBorderless = true;

    for(const fwVertexIndex::value_type& vertex : _vertexIndex)
    {
        SIGHT_ASSERT("Invalid vertex size: " << vertex.size(), vertex.size() > 2);
        ++edgesHistogram[std::make_pair(std::min(vertex[0], vertex[1]), std::max(vertex[0], vertex[1]))];
        ++edgesHistogram[std::make_pair(std::min(vertex[0], vertex[2]), std::max(vertex[0], vertex[2]))];
        ++edgesHistogram[std::make_pair(std::min(vertex[2], vertex[1]), std::max(vertex[2], vertex[1]))];
    }

    for(const EdgeHistogram::value_type& h : edgesHistogram)
    {
        if(h.second < 2)
        {
            isBorderless = false;
            break;
        }
    }

    return isBorderless;
}

//-----------------------------------------------------------------------------

// container of connected component
void findBorderEdges(
    const fwVertexIndex& _vertexIndex,
    std::vector<std::vector<std::pair<std::size_t, std::size_t> > >& contours
)
{
    typedef std::pair<std::size_t, std::size_t> Edge;
    typedef std::vector<Edge> Contour; // at Border

    std::map<Edge, int> edgesHistogram;
    for(fwVertexIndex::value_type vertex : _vertexIndex)
    {
        assert(vertex.size() > 2);
        int i1 = vertex[0];
        int i2 = vertex[1];
        int i3 = vertex[2];
        edgesHistogram[std::make_pair(std::min(i1, i2), std::max(i1, i2))]++;
        edgesHistogram[std::make_pair(std::min(i1, i3), std::max(i1, i3))]++;
        edgesHistogram[std::make_pair(std::min(i3, i2), std::max(i3, i2))]++;
    }

    for(const std::map<Edge, int>::value_type& elt1 : edgesHistogram)
    {
        if(elt1.second < 2) // an orphan found
        {
            Contour contour;
            contour.reserve(1000);
            std::list<Edge> fifo;
            Edge orphan = elt1.first;

            fifo.push_back(orphan);
            while(!fifo.empty())
            {
                Edge current = fifo.front();
                contour.push_back(current);
                fifo.pop_front();
                edgesHistogram[current] = 2; // to mark it processed;

                // search neighbor at border and insert in fifo
                for(const std::map<Edge, int>::value_type& elt2 : edgesHistogram)
                {
                    Edge candidate = elt2.first;
                    if(elt2.second < 2) // at border
                    {
                        if(candidate.first == current.first || candidate.second == current.second // neighbor
                           || candidate.first == current.second || candidate.second == current.first)
                        {
                            edgesHistogram[candidate] = 2; // mark processed;
                            fifo.push_back(candidate);
                        }
                    }
                }
            }

            // all neighbor processed
            contours.push_back(contour);
        }
    }
}

//-----------------------------------------------------------------------------

bool closeSurface(fwVertexPosition& _vertex, fwVertexIndex& _vertexIndex)
{
    typedef std::pair<std::size_t, std::size_t> Edge;
    typedef std::vector<Edge> Contour; // at Border
    typedef std::vector<Contour> Contours;

    Contours contours;
    findBorderEdges(_vertexIndex, contours);
    bool closurePerformed = !contours.empty();
    // close each hole
    for(const Contours::value_type& contour : contours)
    {
        std::size_t newVertexIndex = _vertex.size();
        // create gravity point & insert new triangle
        std::vector<float> massCenter(3, 0);

        for(const Contour::value_type& edge : contour)
        {
            for(std::size_t i = 0 ; i < 3 ; ++i)
            {
                massCenter[i] += _vertex[edge.first][i];
                massCenter[i] += _vertex[edge.second][i];
            }

            // create new Triangle
            std::vector<int> triangleIndex(3);
            triangleIndex[0] = int(edge.first);
            triangleIndex[1] = int(edge.second);
            triangleIndex[2] = int(newVertexIndex);
            _vertexIndex.push_back(triangleIndex); // TEST
        }

        for(std::size_t i = 0 ; i < 3 ; ++i)
        {
            massCenter[i] /= float(contour.size() * 2);
        }

        _vertex.push_back(massCenter); // normalize barycenter
    }

    return closurePerformed;
}

//-----------------------------------------------------------------------------

bool removeOrphanVertices(fwVertexPosition& _vertex, fwVertexIndex& _vertexIndex)
{
    fwVertexPosition newVertex;
    newVertex.reserve(_vertex.size());

    std::set<int> indexPointToKeep;

    for(const fwVertexIndex::value_type& vertex : _vertexIndex)
    {
        indexPointToKeep.insert(vertex[0]);
        indexPointToKeep.insert(vertex[1]);
        indexPointToKeep.insert(vertex[2]);
    }

    bool orphanFound = indexPointToKeep.size() != _vertex.size();

    if(orphanFound)
    {
        // rebuild index table according to element suppression
        int idx = 0;
        std::map<int, int> translate; // map oldIndex -> newIndex (to take into account removal
        std::set<int>::iterator idxIter;
        for(int indexPt : indexPointToKeep)
        {
            translate[indexPt] = idx++;
            newVertex.push_back(_vertex[std::size_t(indexPt)]);
        }

        for(fwVertexIndex::value_type& vertex : _vertexIndex)
        {
            vertex[0] = translate[vertex[0]];
            vertex[1] = translate[vertex[1]];
            vertex[2] = translate[vertex[2]];
        }

        _vertex.swap(newVertex);
    }

    return orphanFound;
}

//-----------------------------------------------------------------------------

} // namespace sight::geometry::data
