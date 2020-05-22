/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
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

#include "fwRenderOgre/helper/Mesh.hpp"

#include "fwCore/spyLog.hpp"

#include <cstdint>

namespace fwRenderOgre
{

namespace helper
{

//-----------------------------------------------------------------------------

void Mesh::copyColors(::Ogre::RGBA* _dest, const std::uint8_t* _src, size_t _numPoints, size_t _numComponents)
{
    if(_numComponents == 3)
    {
        for (unsigned int i = 0; i < (_numPoints - 1); ++i)
        {
            // Fastest way to copy tested so far, take 1.0 in alpha as default
            ::Ogre::RGBA argb = 0xFF000000;
            argb             |= *reinterpret_cast< const ::Ogre::RGBA*>(_src);
            *_dest++          = argb;
            _src             += 3;
        }
        // The above loop reads 4 bytes at a time and will read one byte out of bounds on the last
        // vertex, so handle it as a special case.
        *_dest = 0xFF000000
                 | _src[0]
                 | static_cast<unsigned int>(_src[1]) << 8
                 | static_cast<unsigned int>(_src[2]) << 16;
    }
    else if(_numComponents == 4)
    {
        memcpy(_dest, _src, _numPoints * _numComponents);
    }
    else
    {
        SLM_FATAL("We only support RGB or RGBA vertex color");
    }
}

//-----------------------------------------------------------------------------

} // namespace helper

} // namespace fwRenderOgre
