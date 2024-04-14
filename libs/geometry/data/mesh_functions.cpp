/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "geometry/data/mesh_functions.hpp"

#include "geometry/data/vector_functions.hpp"

#include <boost/unordered_map.hpp>

#include <glm/glm.hpp>

#include <cmath>
#include <list>
#include <map>
#include <set>

namespace sight::geometry::data
{

//-----------------------------------------------------------------------------

bool intersect_triangle(
    fw_vec3d _orig,
    fw_vec3d _dir,
    fw_vec3d _vert0,
    fw_vec3d _vert1,
    fw_vec3d _vert2,
    double& _t,
    double& _u,
    double& _v
)
{
    const double epsilon = 0.000001;

    fw_vec3d edge1;
    fw_vec3d edge2;
    fw_vec3d tvec;
    fw_vec3d pvec;
    fw_vec3d qvec;

    /* find vectors for two edges sharing vert0 */
    edge1 = _vert1 - _vert0;
    edge2 = _vert2 - _vert0;

    /* begin calculating determinant - also used to calculate U parameter */
    pvec = geometry::data::cross(_dir, edge2);

    /* if determinant is near zero, ray lies in plane of triangle */
    const double det = geometry::data::dot(edge1, pvec);

    if(det > -epsilon && det < epsilon)
    {
        return false;
    }

    const double inv_det = 1.0 / det;

    /* calculate distance from vert0 to ray origin */
    tvec = _orig - _vert0;

    /* calculate U parameter and test bounds */
    _u = inv_det * geometry::data::dot(tvec, pvec);
    if(_u < 0.0 || _u > 1.0)
    {
        return false;
    }

    /* prepare to test V parameter */
    qvec = geometry::data::cross(tvec, edge1);

    /* calculate V parameter and test bounds */
    _v = inv_det * geometry::data::dot(_dir, qvec);
    if(_v < 0.0 || _u + _v > 1.0)
    {
        return false;
    }

    /* calculate t, ray intersects triangle */
    _t = inv_det * geometry::data::dot(edge2, qvec);
    return true;
}

//------------------------------------------------------------------------------

bool is_inclosed_volume(const fw_vertex_position& _vertex, const fw_vertex_index& _vertex_index, const fw_vec3d& _p)
{
    const unsigned int x          = 0;
    const unsigned int y          = 1;
    const unsigned int z          = 2;
    const std::size_t element_nbr = _vertex_index.size();
    if(element_nbr == 0)
    {
        return false;
    }

    // Check all mesh triangles
    unsigned int intersection_nbr = 0;
    for(std::size_t i = 0 ; i < element_nbr ; ++i)
    {
        // get triangle vertices.
        const fw_vec3d p1 =
        {_vertex[std::size_t(_vertex_index[i][0])][0], _vertex[std::size_t(_vertex_index[i][0])][1],
         _vertex[std::size_t(_vertex_index[i][0])][2]
        };
        const fw_vec3d p2 =
        {_vertex[std::size_t(_vertex_index[i][1])][0], _vertex[std::size_t(_vertex_index[i][1])][1],
         _vertex[std::size_t(_vertex_index[i][1])][2]
        };
        const fw_vec3d p3 =
        {_vertex[std::size_t(_vertex_index[i][2])][0], _vertex[std::size_t(_vertex_index[i][2])][1],
         _vertex[std::size_t(_vertex_index[i][2])][2]
        };

        // remove all triangles above point.
        if(!(p1[z] > _p[z] && p2[z] > _p[z] && p3[z] > _p[z])) //trianglePotentiallyWellPositionned
        {
            // We check vertices on each side of the 3 axis.
            // If P1[X] > P[X] So we need P2[X] < P[X] ou P3[X] < P[X], same way on the 2 other axis.
            // By doing so we can exclude points that are localized on axis.
            bool stop = false;
            for(unsigned int axe = x ; axe <= y && !stop ; ++axe)
            {
                const double delta1 = p1[axe] - _p[axe];
                const double delta2 = p2[axe] - _p[axe];
                const double delta3 = p3[axe] - _p[axe];

                if(delta1 >= 0.F && delta2 >= 0.F && delta3 >= 0.F)
                {
                    stop = true;
                    break;
                }

                if(delta1 < 0.F && delta2 < 0.F && delta3 < 0.F)
                {
                    stop = true;
                    break;
                }
            }

            if(!stop)
            {
                fw_vec3d orig = {_p[0], _p[1], _p[2]};

                fw_vec3d dir   = {0.F, 0.F, 1.F};
                fw_vec3d vert0 = {p1[0], p1[1], p1[2]};
                fw_vec3d vert1 = {p2[0], p2[1], p2[2]};
                fw_vec3d vert2 = {p3[0], p3[1], p3[2]};
                double t       = NAN;
                double u       = NAN;
                double v       = NAN;
                if(intersect_triangle(orig, dir, vert0, vert1, vert2, t, u, v))
                {
                    // We only keep points below _p (following Oz axis).
                    if(t < 0.F)
                    {
                        ++intersection_nbr;
                    }
                }
            }
        }
    }

    return intersection_nbr % 2 == 1;
}

//-----------------------------------------------------------------------------

bool is_borderless_surface(const fw_vertex_index& _vertex_index)
{
    using edge           = std::pair<int, int>; // always edge_t.first < edge_t.second !!
    using edge_histogram = boost::unordered_map<edge, int>;
    edge_histogram edges_histogram;
    bool is_borderless = true;

    for(const auto& vertex : _vertex_index)
    {
        SIGHT_ASSERT("Invalid vertex size: " << vertex.size(), vertex.size() > 2);
        ++edges_histogram[std::make_pair(std::min(vertex[0], vertex[1]), std::max(vertex[0], vertex[1]))];
        ++edges_histogram[std::make_pair(std::min(vertex[0], vertex[2]), std::max(vertex[0], vertex[2]))];
        ++edges_histogram[std::make_pair(std::min(vertex[2], vertex[1]), std::max(vertex[2], vertex[1]))];
    }

    for(const auto& h : edges_histogram)
    {
        if(h.second < 2)
        {
            is_borderless = false;
            break;
        }
    }

    return is_borderless;
}

//-----------------------------------------------------------------------------

// container of connected component
void find_border_edges(
    const fw_vertex_index& _vertex_index,
    std::vector<std::vector<std::pair<std::size_t, std::size_t> > >& _contours
)
{
    using edge_t    = std::pair<std::size_t, std::size_t>;
    using contour_t = std::vector<edge_t>; // at Border

    std::map<edge_t, int> edges_histogram;
    for(fw_vertex_index::value_type vertex : _vertex_index)
    {
        assert(vertex.size() > 2);
        int i1 = vertex[0];
        int i2 = vertex[1];
        int i3 = vertex[2];
        edges_histogram[std::make_pair(std::min(i1, i2), std::max(i1, i2))]++;
        edges_histogram[std::make_pair(std::min(i1, i3), std::max(i1, i3))]++;
        edges_histogram[std::make_pair(std::min(i3, i2), std::max(i3, i2))]++;
    }

    for(const auto& elt1 : edges_histogram)
    {
        if(elt1.second < 2) // an orphan found
        {
            contour_t contour;
            contour.reserve(1000);
            std::list<edge_t> fifo;
            edge_t orphan = elt1.first;

            fifo.push_back(orphan);
            while(!fifo.empty())
            {
                edge_t current = fifo.front();
                contour.push_back(current);
                fifo.pop_front();
                edges_histogram[current] = 2; // to mark it processed;

                // search neighbor at border and insert in fifo
                for(const auto& elt2 : edges_histogram)
                {
                    edge_t candidate = elt2.first;
                    if(elt2.second < 2) // at border
                    {
                        if(candidate.first == current.first || candidate.second == current.second // neighbor
                           || candidate.first == current.second || candidate.second == current.first)
                        {
                            edges_histogram[candidate] = 2; // mark processed;
                            fifo.push_back(candidate);
                        }
                    }
                }
            }

            // all neighbor processed
            _contours.push_back(contour);
        }
    }
}

//-----------------------------------------------------------------------------

bool close_surface(fw_vertex_position& _vertex, fw_vertex_index& _vertex_index)
{
    using edge_t     = std::pair<std::size_t, std::size_t>;
    using contour_t  = std::vector<edge_t>; // at Border
    using contours_t = std::vector<contour_t>;

    contours_t contours;
    find_border_edges(_vertex_index, contours);
    bool closure_performed = !contours.empty();
    // close each hole
    for(const auto& contour : contours)
    {
        std::size_t new_vertex_index = _vertex.size();
        // create gravity point & insert new triangle
        std::vector<float> mass_center(3, 0);

        for(const auto& edge : contour)
        {
            for(std::size_t i = 0 ; i < 3 ; ++i)
            {
                mass_center[i] += _vertex[edge.first][i];
                mass_center[i] += _vertex[edge.second][i];
            }

            // create new Triangle
            std::vector<int> triangle_index(3);
            triangle_index[0] = int(edge.first);
            triangle_index[1] = int(edge.second);
            triangle_index[2] = int(new_vertex_index);
            _vertex_index.push_back(triangle_index); // TEST
        }

        for(std::size_t i = 0 ; i < 3 ; ++i)
        {
            mass_center[i] /= float(contour.size() * 2);
        }

        _vertex.push_back(mass_center); // normalize barycenter
    }

    return closure_performed;
}

//-----------------------------------------------------------------------------

bool remove_orphan_vertices(fw_vertex_position& _vertex, fw_vertex_index& _vertex_index)
{
    fw_vertex_position new_vertex;
    new_vertex.reserve(_vertex.size());

    std::set<int> index_point_to_keep;

    for(const auto& vertex : _vertex_index)
    {
        index_point_to_keep.insert(vertex[0]);
        index_point_to_keep.insert(vertex[1]);
        index_point_to_keep.insert(vertex[2]);
    }

    bool orphan_found = index_point_to_keep.size() != _vertex.size();

    if(orphan_found)
    {
        // rebuild index table according to element suppression
        int idx = 0;
        std::map<int, int> translate; // map oldIndex -> newIndex (to take into account removal
        std::set<int>::iterator idx_iter;
        for(int index_pt : index_point_to_keep)
        {
            translate[index_pt] = idx++;
            new_vertex.push_back(_vertex[std::size_t(index_pt)]);
        }

        for(fw_vertex_index::value_type& vertex : _vertex_index)
        {
            vertex[0] = translate[vertex[0]];
            vertex[1] = translate[vertex[1]];
            vertex[2] = translate[vertex[2]];
        }

        _vertex.swap(new_vertex);
    }

    return orphan_found;
}

//-----------------------------------------------------------------------------

} // namespace sight::geometry::data
