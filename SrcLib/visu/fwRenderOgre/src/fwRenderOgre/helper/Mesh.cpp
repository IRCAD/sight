/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
        for (unsigned int i = 0; i < _numPoints; ++i)
        {
            // Fastest way to copy tested so far, take 1.0 in alpha as default
            ::Ogre::RGBA argb = 0xFF000000;
            argb             |= *reinterpret_cast< const ::Ogre::RGBA*>(_src);
            *_dest++          = argb;
            _src             += 3;
        }
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
