/************************************************************************
 *
 * Copyright (C) 2015-2021 IRCAD France
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

#if defined(__APPLE__)
    #include <OpenGL/gl.h>
#else
#ifdef WIN32
// OpenGL on windows requires some types defined by the windows API such as WINGDIAPI and APIENTRY.
    #include <windows.h>
#endif
    #include <GL/gl.h>
#endif

#include <OGRE/OgreTextureManager.h>

#include <OgreHardwarePixelBuffer.h>

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

void TransferFunction::createTexture(const ::Ogre::String& _parentId)
{
    m_texture = ::Ogre::TextureManager::getSingleton().getByName(_parentId + "_tfTexture", RESOURCE_GROUP);
    if(!m_texture)
    {
        m_texture = ::Ogre::TextureManager::getSingleton().createManual(
            _parentId + "_tfTexture",     // name
            viz::scene3d::RESOURCE_GROUP, // resource groupe
            ::Ogre::TEX_TYPE_1D,          // type
            TEXTURE_SIZE,
            1,                   // width, height
            0,                   // number of mipmaps (depth)
            ::Ogre::PF_A8R8G8B8, // pixel format
            ::Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE
        ); // usage
    }
}

//-----------------------------------------------------------------------------

void TransferFunction::removeTexture()
{
    if(m_texture)
    {
        ::Ogre::TextureManager::getSingleton().remove(m_texture);
    }

    m_texture.reset();
}

//-----------------------------------------------------------------------------

void TransferFunction::updateTexture(const data::TransferFunction::csptr& _tf)
{
    using TFValueType     = data::TransferFunction::TFValueType;
    using TFValuePairType = data::TransferFunction::TFValuePairType;

    // Retrieves the pixel buffer from the texture
    ::Ogre::HardwarePixelBufferSharedPtr pixBuffer = m_texture->getBuffer();

    // Discards the entire buffer while locking so that we can easily refill it from scratch
    pixBuffer->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
    ::Ogre::PixelBox pixBox = pixBuffer->getCurrentLock();
    std::uint8_t* pDest     = static_cast<std::uint8_t*>(pixBox.data);

    // Retrieves the transfer function's intensity window
    const TFValuePairType intensityMinMax = _tf->getWLMinMax();

    const TFValuePairType tfMinMax = _tf->getMinMaxTFValues();

    // Counter used to iterate through the texture buffer without exceeding its limit
    const TFValueType invWindow     = 1. / _tf->getWindow();
    const TFValueType intensityStep = (intensityMinMax.second - intensityMinMax.first)
                                      / TEXTURE_PIXEL_COUNT;

    TFValueType i = intensityMinMax.first;
    for(std::uint32_t k = 0 ; k < TEXTURE_PIXEL_COUNT ; ++k)
    {
        // Tf intensity to mapped color.
        const TFValueType value = (i - intensityMinMax.first) * (tfMinMax.second - tfMinMax.first)
                                  * invWindow + tfMinMax.first;

        data::TransferFunction::TFColor interpolatedColor = _tf->getInterpolatedColor(value);

        *pDest++ = static_cast<std::uint8_t>(interpolatedColor.b * 255);
        *pDest++ = static_cast<std::uint8_t>(interpolatedColor.g * 255);
        *pDest++ = static_cast<std::uint8_t>(interpolatedColor.r * 255);
        *pDest++ = static_cast<std::uint8_t>(interpolatedColor.a * 255);

        i += intensityStep;
    }

    pixBuffer->unlock();

    m_isClamped = _tf->getIsClamped();

    const auto tfWLMinMax = _tf->getWLMinMax();
    m_tfWindow = ::Ogre::Vector2(float(tfWLMinMax.first), float(tfWLMinMax.second));
}

//-----------------------------------------------------------------------------

} // Namespace fwRenderOgre
