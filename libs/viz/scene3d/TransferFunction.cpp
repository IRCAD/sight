/************************************************************************
 *
 * Copyright (C) 2015-2022 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

#include "viz/scene3d/TransferFunction.hpp"

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/Utils.hpp>

#ifdef WIN32
// OpenGL on windows requires some types defined by the windows API such as WINGDIAPI and APIENTRY.
#include <windows.h>
#endif

#include <GL/gl.h>

#include <OGRE/OgreTextureManager.h>

#include <OgreHardwarePixelBuffer.h>
#include <core/Profiling.hpp>
#include <algorithm>
#include <cstdint> // for std::uint_8

namespace sight::viz::scene3d
{

std::uint32_t TransferFunction::TEXTURE_SIZE;
std::uint32_t TransferFunction::TEXTURE_PIXEL_COUNT;

//-----------------------------------------------------------------------------

TransferFunction::TransferFunction()
{
    // Unluckily Ogre does not seem to give us the maximum texture size through the caps... :'(
    // So we have no other choice than asking OpenGL directly
    int max;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
    TEXTURE_SIZE        = static_cast<std::uint32_t>(max);
    TEXTURE_PIXEL_COUNT = static_cast<std::uint32_t>(max);

    SIGHT_INFO("Use a 1D texture of size : " << TEXTURE_SIZE);
}

//-----------------------------------------------------------------------------

TransferFunction::~TransferFunction()
{
    this->removeTexture();
}

//-----------------------------------------------------------------------------

void TransferFunction::createTexture(const Ogre::String& _parentId)
{
    m_texture = Ogre::TextureManager::getSingleton().getByName(_parentId + "_tfTexture", RESOURCE_GROUP);
    if(!m_texture)
    {
        m_texture = Ogre::TextureManager::getSingleton().createManual(
            _parentId + "_tfTexture",     // name
            viz::scene3d::RESOURCE_GROUP, // resource groupe
            Ogre::TEX_TYPE_1D,            // type
            TEXTURE_SIZE,
            1,                 // width, height
            0,                 // number of mipmaps (depth)
            Ogre::PF_A8R8G8B8, // pixel format
            Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE
        ); // usage
    }
}

//-----------------------------------------------------------------------------

void TransferFunction::removeTexture()
{
    if(m_texture)
    {
        Ogre::TextureManager::getSingleton().remove(m_texture);
    }

    m_texture.reset();
}

//-----------------------------------------------------------------------------

void TransferFunction::updateTexture(const data::TransferFunction::csptr& _tf)
{
    using value_t   = data::TransferFunction::value_t;
    using min_max_t = data::TransferFunction::min_max_t;

    // Retrieves the pixel buffer from the texture
    Ogre::HardwarePixelBufferSharedPtr pixBuffer = m_texture->getBuffer();

    // Discards the entire buffer while locking so that we can easily refill it from scratch
    pixBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    Ogre::PixelBox pixBox = pixBuffer->getCurrentLock();
    std::uint8_t* pDest   = static_cast<std::uint8_t*>(pixBox.data);

    // Retrieves the transfer function's intensity window
    const min_max_t tfWLMinMax = _tf->windowMinMax();

    // Here we will sample the transfer function in its window to benefit of the whole texture range
    // We want the minimum and the maximum to be included (so we need +1 extra sample)
    // But we also want one extra point outside each bound window to sample the default color if the tf is clamped,
    // so we need at the end 1+2=3 extra samples
    const value_t intensityStep = (tfWLMinMax.second - tfWLMinMax.first) / (TEXTURE_PIXEL_COUNT - 3);

    value_t i = tfWLMinMax.first - intensityStep;
    for(std::uint32_t k = 0 ; k < TEXTURE_PIXEL_COUNT ; ++k)
    {
        const auto color = _tf->sample(i) * 255.0;

        *pDest++ = static_cast<std::uint8_t>(color.b);
        *pDest++ = static_cast<std::uint8_t>(color.g);
        *pDest++ = static_cast<std::uint8_t>(color.r);
        *pDest++ = static_cast<std::uint8_t>(color.a);

        i += intensityStep;
    }

    pixBuffer->unlock();

    m_tfWindow = Ogre::Vector2(float(tfWLMinMax.first), float(tfWLMinMax.second));
}

//-----------------------------------------------------------------------------

} // Namespace fwRenderOgre
