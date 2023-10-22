/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "viz/scene3d/helper/mesh.hpp"

#include "core/spy_log.hpp"

#include <cstdint>

namespace sight::viz::scene3d::helper
{

//-----------------------------------------------------------------------------

void mesh::copyColors(Ogre::RGBA* _dest, const std::uint8_t* _src, std::size_t _num_points, std::size_t _num_components)
{
    if(_num_components == 3)
    {
        for(unsigned int i = 0 ; i < (_num_points - 1) ; ++i)
        {
            // Fastest way to copy tested so far, take 1.0 in alpha as default
            Ogre::RGBA argb = 0xFF000000;
            argb    |= *reinterpret_cast<const Ogre::RGBA*>(_src);
            *_dest++ = argb;
            _src    += 3;
        }

        // The above loop reads 4 bytes at a time and will read one byte out of bounds on the last
        // vertex, so handle it as a special case.
        *_dest = 0xFF000000
                 | _src[0]
                 | static_cast<unsigned int>(_src[1]) << 8
                 | static_cast<unsigned int>(_src[2]) << 16;
    }
    else if(_num_components == 4)
    {
        memcpy(_dest, _src, _num_points * _num_components);
    }
    else
    {
        SIGHT_FATAL("We only support RGB or RGBA vertex color");
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper
