/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/TransferFunction.hpp"

#include <fwRenderOgre/Utils.hpp>

#include <GL/gl.h>

#include <OGRE/OgreTextureManager.h>

#include <OgreHardwarePixelBuffer.h>

#include <algorithm>
#include <cstdint>  // for std::uint_8

namespace fwRenderOgre
{

std::uint32_t TransferFunction::TEXTURE_SIZE;
std::uint32_t TransferFunction::TEXTURE_PIXEL_COUNT;

//-----------------------------------------------------------------------------

TransferFunction::TransferFunction() :
    m_sampleDistance(1.f)
{
    // Unluckily Ogre does not seem to give us the maximum texture size through the caps... :'(
    // So we have no other choice than asking OpenGL directly
    int max;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
    TEXTURE_SIZE        = static_cast<std::uint32_t>(max);
    TEXTURE_PIXEL_COUNT = static_cast<std::uint32_t>(max);

    OSLM_INFO("Use a 1D texture of size : " << TEXTURE_SIZE);
}

//-----------------------------------------------------------------------------

TransferFunction::~TransferFunction()
{
}

//-----------------------------------------------------------------------------

void TransferFunction::createTexture(const ::Ogre::String& _parentId)
{
    m_texture = ::Ogre::TextureManager::getSingleton().getByName(_parentId + "_tfTexture");
    if(!m_texture)
    {
        m_texture = ::Ogre::TextureManager::getSingleton().createManual(
            _parentId + "_tfTexture",                                   // name
            ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,  // resource groupe
            ::Ogre::TEX_TYPE_1D,                                        // type
            TEXTURE_SIZE, 1,                                            // width, height
            0,                                                          // number of mipmaps (depth)
            ::Ogre::PF_A8R8G8B8,                                        // pixel format
            ::Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);                 // usage
    }
}

//-----------------------------------------------------------------------------

void TransferFunction::removeTexture()
{
    ::Ogre::TextureManager::getSingleton().remove(m_texture->getHandle());
    m_texture.reset();
}

//-----------------------------------------------------------------------------

void TransferFunction::updateTexture(const ::fwData::TransferFunction::csptr& _tf)
{
    // Retrieves the pixel buffer from the texture
    ::Ogre::HardwarePixelBufferSharedPtr pixBuffer = m_texture->getBuffer();

    // Discards the entire buffer while locking so that we can easily refill it from scratch
    pixBuffer->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
    ::Ogre::PixelBox pixBox = pixBuffer->getCurrentLock();
    std::uint8_t* pDest = static_cast<std::uint8_t*>(pixBox.data);

    // Retrieves the transfer function's data map
    const ::fwData::TransferFunction::TFDataType tfData = _tf->getTFData();

    // Retrieves the transfer function's intensity window
    const ::fwData::TransferFunction::TFValuePairType intensityMinMax = _tf->getWLMinMax();

    const ::fwData::TransferFunction::TFValuePairType tfMinMax = _tf->getMinMaxTFValues();

    // Counter used to iterate through the texture buffer without exceeding its limit
    const double invWindow                                      = 1./_tf->getWindow();
    const ::fwData::TransferFunction::TFValueType intensityStep = (intensityMinMax.second - intensityMinMax.first) /
                                                                  TEXTURE_PIXEL_COUNT;

    ::fwData::TransferFunction::TFValueType i = intensityMinMax.first;
    for( std::uint32_t k = 0; k < TEXTURE_PIXEL_COUNT; ++k)
    {
        // Tf intensity to mapped color.
        auto value = (i - intensityMinMax.first) * (tfMinMax.second - tfMinMax.first) * invWindow + tfMinMax.first;

        ::fwData::TransferFunction::TFColor interpolatedColor = _tf->getInterpolatedColor(value);

        *pDest++ = static_cast<std::uint8_t>(interpolatedColor.b * 255);
        *pDest++ = static_cast<std::uint8_t>(interpolatedColor.g * 255);
        *pDest++ = static_cast<std::uint8_t>(interpolatedColor.r * 255);
        *pDest++ = static_cast<std::uint8_t>(interpolatedColor.a * 255);

        i += intensityStep;
    }

    ::Ogre::Image image;
    image.loadDynamicImage(static_cast<std::uint8_t*>(pixBox.data), TEXTURE_SIZE, 1, 1, ::Ogre::PF_A8R8G8B8);

    pixBuffer->unlock();

    image.save("zizi.png");
}

//-----------------------------------------------------------------------------

} // Namespace fwRenderOgre
