/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <geometry/__/box.hpp>

#include <glm/glm.hpp>

#include <array>
#include <cmath>
#include <limits>

namespace sight::geometry
{

// OBB-vs-OBB separating axis theorem (Ericson, Real-Time Collision Detection §4.4).
// Tests 15 axes: 3 face normals from A, 3 from B, 9 edge cross-products.
// r[i][j] = dot(a.orientation[i], b.orientation[j])  (both indexed by column = axis index).
bool intersect(const oriented_box_t& _a, const oriented_box_t& _b)
{
    // Small bias on absolute-value terms to handle parallel-edge degeneracies.
    constexpr double eps = std::numeric_limits<double>::epsilon();

    const glm::dvec3 t = _b.center - _a.center;

    // Rotation of B expressed in A's frame, and its absolute value.
    std::array<std::array<double, 3>, 3> r {};
    std::array<std::array<double, 3>, 3> abs_r {};
    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        for(std::size_t j = 0 ; j < 3 ; ++j)
        {
            const auto gi = static_cast<int>(i);
            const auto gj = static_cast<int>(j);
            r[i][j]     = glm::dot(_a.orientation[gi], _b.orientation[gj]);
            abs_r[i][j] = std::abs(r[i][j]) + eps;
        }
    }

    // Translation expressed in A's frame.
    std::array<double, 3> ta {};
    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        ta[i] = glm::dot(t, _a.orientation[static_cast<int>(i)]);
    }

    // ── A's face normals ────────────────────────────────────────────────────
    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        const double ra = _a.extent[static_cast<int>(i)];
        const double rb = _b.extent[0] * abs_r[i][0] + _b.extent[1] * abs_r[i][1] + _b.extent[2] * abs_r[i][2];
        if(std::abs(ta[i]) > ra + rb)
        {
            return false;
        }
    }

    // ── B's face normals ────────────────────────────────────────────────────
    for(std::size_t j = 0 ; j < 3 ; ++j)
    {
        const double ra = _a.extent[0] * abs_r[0][j] + _a.extent[1] * abs_r[1][j] + _a.extent[2] * abs_r[2][j];
        const double rb = _b.extent[static_cast<int>(j)];
        if(std::abs(glm::dot(t, _b.orientation[static_cast<int>(j)])) > ra + rb)
        {
            return false;
        }
    }

    // ── 9 edge cross-product axes ────────────────────────────────────────────
    // L = a.axes[0] x b.axes[0]
    if(std::abs(ta[2] * r[1][0] - ta[1] * r[2][0])
       > _a.extent[1] * abs_r[2][0] + _a.extent[2] * abs_r[1][0]
       + _b.extent[1] * abs_r[0][2] + _b.extent[2] * abs_r[0][1])
    {
        return false;
    }

    // L = a.axes[0] x b.axes[1]
    if(std::abs(ta[2] * r[1][1] - ta[1] * r[2][1])
       > _a.extent[1] * abs_r[2][1] + _a.extent[2] * abs_r[1][1]
       + _b.extent[0] * abs_r[0][2] + _b.extent[2] * abs_r[0][0])
    {
        return false;
    }

    // L = a.axes[0] x b.axes[2]
    if(std::abs(ta[2] * r[1][2] - ta[1] * r[2][2])
       > _a.extent[1] * abs_r[2][2] + _a.extent[2] * abs_r[1][2]
       + _b.extent[0] * abs_r[0][1] + _b.extent[1] * abs_r[0][0])
    {
        return false;
    }

    // L = a.axes[1] x b.axes[0]
    if(std::abs(ta[0] * r[2][0] - ta[2] * r[0][0])
       > _a.extent[0] * abs_r[2][0] + _a.extent[2] * abs_r[0][0]
       + _b.extent[1] * abs_r[1][2] + _b.extent[2] * abs_r[1][1])
    {
        return false;
    }

    // L = a.axes[1] x b.axes[1]
    if(std::abs(ta[0] * r[2][1] - ta[2] * r[0][1])
       > _a.extent[0] * abs_r[2][1] + _a.extent[2] * abs_r[0][1]
       + _b.extent[0] * abs_r[1][2] + _b.extent[2] * abs_r[1][0])
    {
        return false;
    }

    // L = a.axes[1] x b.axes[2]
    if(std::abs(ta[0] * r[2][2] - ta[2] * r[0][2])
       > _a.extent[0] * abs_r[2][2] + _a.extent[2] * abs_r[0][2]
       + _b.extent[0] * abs_r[1][1] + _b.extent[1] * abs_r[1][0])
    {
        return false;
    }

    // L = a.axes[2] x b.axes[0]
    if(std::abs(ta[1] * r[0][0] - ta[0] * r[1][0])
       > _a.extent[0] * abs_r[1][0] + _a.extent[1] * abs_r[0][0]
       + _b.extent[1] * abs_r[2][2] + _b.extent[2] * abs_r[2][1])
    {
        return false;
    }

    // L = a.axes[2] x b.axes[1]
    if(std::abs(ta[1] * r[0][1] - ta[0] * r[1][1])
       > _a.extent[0] * abs_r[1][1] + _a.extent[1] * abs_r[0][1]
       + _b.extent[0] * abs_r[2][2] + _b.extent[2] * abs_r[2][0])
    {
        return false;
    }

    // L = a.axes[2] x b.axes[2]
    if(std::abs(ta[1] * r[0][2] - ta[0] * r[1][2])
       > _a.extent[0] * abs_r[1][2] + _a.extent[1] * abs_r[0][2]
       + _b.extent[0] * abs_r[2][1] + _b.extent[1] * abs_r[2][0])
    {
        return false;
    }

    return true;
}

} // namespace sight::geometry
