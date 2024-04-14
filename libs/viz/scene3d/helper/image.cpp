/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "viz/scene3d/helper/image.hpp"

namespace sight::viz::scene3d::helper
{

//-----------------------------------------------------------------------------

Ogre::AxisAlignedBox image::compute_bounding_box_from_mask(const data::image::csptr _mask)
{
    // To compute the bounding box of the non-cropped area,
    // we browse the image per slice in the 6 directions x,-x,y,-y,z,-z until we find a visible voxel
    // So, the smaller the area, the more time the algorithm takes.

    const data::image::size_t size = _mask->size();

    const auto find_bound =
        [&_mask, &size](std::size_t _axis, bool _reverse)
        {
            const std::array bounds = {
                size[_axis % 3], size[(1 + _axis) % 3], size[(2 + _axis) % 3]
            };
            static const std::array<std::array<std::size_t, 3>, 3> s_PERM = {{{0, 1, 2}, {2, 0, 1}, {1, 2, 0}}};

            std::array<std::size_t, 3> uvw = {0, 0, 0};
            for(uvw[0] = 0 ; uvw[0] < bounds[0] ; ++uvw[0])
            {
                for(uvw[1] = 0 ; uvw[1] < bounds[1] ; ++uvw[1])
                {
                    for(uvw[2] = 0 ; uvw[2] < bounds[2] ; ++uvw[2])
                    {
                        const auto [x, y, z] =
                            std::tuple {uvw[s_PERM[_axis][0]], uvw[s_PERM[_axis][1]], uvw[s_PERM[_axis][2]]};
                        const auto val = _reverse
                                         ? _mask->at<std::uint8_t>(size[0] - x - 1, size[1] - y - 1, size[2] - z - 1)
                                         : _mask->at<std::uint8_t>(x, y, z);
                        if(val != 0)
                        {
                            return static_cast<float>(_reverse ? bounds[0] - uvw[0] - 1 : uvw[0]);
                        }
                    }
                }
            }

            return static_cast<float>(_reverse ? bounds[0] - 1 : 0);
        };

    const Ogre::Vector3 s = {static_cast<float>(size[0]), static_cast<float>(size[1]), static_cast<float>(size[2])};

    Ogre::AxisAlignedBox bounding_box;
    bounding_box.setMinimum(find_bound(0, false) / s[0], find_bound(1, false) / s[1], find_bound(2, false) / s[2]);
    bounding_box.setMaximum(find_bound(0, true) / s[0], find_bound(1, true) / s[1], find_bound(2, true) / s[2]);

    return bounding_box;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper
