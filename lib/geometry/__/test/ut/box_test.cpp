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

#include <doctest/doctest.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace
{

// Identity orientation (axis-aligned box).
constexpr glm::dmat3 I(1.0);

// Rotation matrix around Z by angle (radians), columns = axes in world space.
glm::dmat3 rot_z(double _angle)
{
    const double c = std::cos(_angle);
    const double s = std::sin(_angle);
    // Column-major: col0=(c,s,0), col1=(-s,c,0), col2=(0,0,1)
    return {c, s, 0, -s, c, 0, 0, 0, 1};
}

} // namespace

TEST_SUITE("sight::geometry::oriented_box")
{
//------------------------------------------------------------------------------

    TEST_CASE("is_valid_identity")
    {
        const sight::geometry::oriented_box_t box = {.center = {0, 0, 0}, .extent = {1, 1, 1}, .orientation = I};
        CHECK(box.is_valid() == true);
    }

//------------------------------------------------------------------------------

    TEST_CASE("is_valid_arbitrary_positive_extents")
    {
        const sight::geometry::oriented_box_t box = {.center = {5, -3, 2}, .extent = {2, 0.5, 10}, .orientation = I};
        CHECK(box.is_valid() == true);
    }

//------------------------------------------------------------------------------

    TEST_CASE("is_valid_zero_extent")
    {
        {
            const sight::geometry::oriented_box_t box = {.center = {0, 0, 0}, .extent = {0, 1, 1}, .orientation = I};
            CHECK(box.is_valid() == false);
        }
        {
            const sight::geometry::oriented_box_t box = {.center = {0, 0, 0}, .extent = {1, 0, 1}, .orientation = I};
            CHECK(box.is_valid() == false);
        }
        {
            const sight::geometry::oriented_box_t box = {.center = {0, 0, 0}, .extent = {1, 1, 0}, .orientation = I};
            CHECK(box.is_valid() == false);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("is_valid_negative_extent")
    {
        {
            const sight::geometry::oriented_box_t box = {.center = {0, 0, 0}, .extent = {-1, 1, 1}, .orientation = I};
            CHECK(box.is_valid() == false);
        }
        {
            const sight::geometry::oriented_box_t box = {.center = {0, 0, 0}, .extent = {1, -1, 1}, .orientation = I};
            CHECK(box.is_valid() == false);
        }
        {
            const sight::geometry::oriented_box_t box = {.center = {0, 0, 0}, .extent = {1, 1, -1}, .orientation = I};
            CHECK(box.is_valid() == false);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("is_valid_non_unit_column")
    {
        // col0 has length 2 instead of 1
        constexpr glm::dmat3 non_unit {2, 0, 0, 0, 1, 0, 0, 0, 1};
        const sight::geometry::oriented_box_t box = {.center = {0, 0, 0}, .extent = {1, 1, 1}, .orientation = non_unit};
        CHECK(box.is_valid() == false);
    }

//------------------------------------------------------------------------------

    TEST_CASE("is_valid_non_orthogonal_columns")
    {
        // col0 and col1 both point along X: dot(col0, col1) == 1
        constexpr glm::dmat3 non_ortho {1, 0, 0, 1, 0, 0, 0, 0, 1};
        const sight::geometry::oriented_box_t box = {.center = {0, 0, 0
                                                     }, .extent = {1, 1, 1
                                                     },
                                                     .orientation = non_ortho
        };
        CHECK(box.is_valid() == false);
    }

//------------------------------------------------------------------------------

    TEST_CASE("obb_vs_obb_identical")
    {
        // Two coincident axis-aligned unit boxes always intersect.
        const sight::geometry::oriented_box_t a = {.center = {0, 0, 0}, .extent = {1, 1, 1}, .orientation = I};
        const sight::geometry::oriented_box_t b = {.center = {0, 0, 0}, .extent = {1, 1, 1}, .orientation = I};
        CHECK(sight::geometry::intersect(a, b) == true);
    }

//------------------------------------------------------------------------------

    TEST_CASE("obb_vs_obb_aabb_separated")
    {
        // Two axis-aligned boxes with a clear gap on X.
        sight::geometry::oriented_box_t a = {.center = {0, 0, 0}, .extent = {1, 1, 1}, .orientation = I};

        {
            // gap = 2.1 - 1 - 1 = 0.1
            sight::geometry::oriented_box_t b = {.center = {2.1, 0, 0}, .extent = {1, 1, 1}, .orientation = I};
            CHECK(sight::geometry::intersect(a, b) == false);
        }
        {
            // gap in Y
            sight::geometry::oriented_box_t b = {.center = {0, 2.1, 0}, .extent = {1, 1, 1}, .orientation = I};
            CHECK(sight::geometry::intersect(a, b) == false);
        }
        {
            // gap in Z
            sight::geometry::oriented_box_t b = {.center = {0, 0, 2.1}, .extent = {1, 1, 1}, .orientation = I};
            CHECK(sight::geometry::intersect(a, b) == false);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("obb_vs_obb_aabb_overlapping")
    {
        // Two axis-aligned boxes with a small overlap on X.
        sight::geometry::oriented_box_t a = {.center = {0, 0, 0}, .extent = {1, 1, 1}, .orientation = I};
        sight::geometry::oriented_box_t b = {.center = {1.9, 0, 0}, .extent = {1, 1, 1}, .orientation = I};
        // overlap = 1 + 1 - 1.9 = 0.1
        CHECK(sight::geometry::intersect(a, b) == true);
    }

//------------------------------------------------------------------------------

    TEST_CASE("obb_vs_obb_contained")
    {
        // B is fully inside A.
        sight::geometry::oriented_box_t a = {.center = {0, 0, 0}, .extent = {3, 3, 3}, .orientation = I};
        sight::geometry::oriented_box_t b = {.center = {0, 0, 0}, .extent = {1, 1, 1}, .orientation = I};
        CHECK(sight::geometry::intersect(a, b) == true);
    }

//------------------------------------------------------------------------------

    TEST_CASE("obb_vs_obb_rotated_separated")
    {
        // B rotated 45° around Z, placed far enough along X that A's x face-normal separates them.
        // A reach on X: 1.0
        // B projected on A's X-axis: e[0]*|cos45| + e[1]*|sin45| = sqrt(2) ≈ 1.414
        // Total: 1 + 1.414 = 2.414  →  center at X=2.5 gives |2.5| > 2.414, separated.
        const glm::dmat3 r45z             = rot_z(glm::radians(45.0));
        sight::geometry::oriented_box_t a = {.center = {0, 0, 0}, .extent = {1, 1, 1}, .orientation = I};
        sight::geometry::oriented_box_t b = {.center = {2.5, 0, 0}, .extent = {1, 1, 1}, .orientation = r45z};
        CHECK(sight::geometry::intersect(a, b) == false);
    }

//------------------------------------------------------------------------------

    TEST_CASE("obb_vs_obb_rotated_intersecting")
    {
        // Same setup but B closer: |2.0| < 2.414, no face-normal axis separates them → they intersect.
        const glm::dmat3 r45z             = rot_z(glm::radians(45.0));
        sight::geometry::oriented_box_t a = {.center = {0, 0, 0}, .extent = {1, 1, 1}, .orientation = I};
        sight::geometry::oriented_box_t b = {.center = {2.0, 0, 0}, .extent = {1, 1, 1}, .orientation = r45z};
        CHECK(sight::geometry::intersect(a, b) == true);
    }

//------------------------------------------------------------------------------

    TEST_CASE("obb_vs_obb_symmetry")
    {
        // intersect(a, b) == intersect(b, a).
        const glm::dmat3 r45z             = rot_z(glm::radians(45.0));
        sight::geometry::oriented_box_t a = {.center = {0, 0, 0}, .extent = {1, 2, 0.5}, .orientation = I};
        sight::geometry::oriented_box_t b = {.center = {1.5, 0.5, 0}, .extent = {0.5, 1, 2}, .orientation = r45z};

        CHECK(sight::geometry::intersect(a, b) == sight::geometry::intersect(b, a));
    }

//------------------------------------------------------------------------------
}
