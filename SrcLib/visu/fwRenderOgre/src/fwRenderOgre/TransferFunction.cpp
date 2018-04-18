/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/TransferFunction.hpp"

#include <fwRenderOgre/Utils.hpp>

#include <OGRE/OgreTextureManager.h>

#include <OgreHardwarePixelBuffer.h>

#include <algorithm>
#include <cstdint>  // for std::uint_8

namespace fwRenderOgre
{

static const std::int32_t TEXTURE_SIZE         = 256;
static const std::int32_t TEXTURE_PIXEL_COUNT  = TEXTURE_SIZE * TEXTURE_SIZE;
static const std::int32_t TEXTURE_CENTER_INDEX = TEXTURE_PIXEL_COUNT / 2;

//-----------------------------------------------------------------------------

TransferFunction::TransferFunction() :
    m_sampleDistance(1.f)
{
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
            ::Ogre::TEX_TYPE_2D,                                        // type
            TEXTURE_SIZE, TEXTURE_SIZE,                                 // width, height
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

    // Index corresponding to the first tf value.
    const std::int32_t minIntensityIndex = intensityToBufferIndex(intensityMinMax.first);

    // If the transfer function is clamped, we have to force the extremity colors to black
    const bool isTFClamped = _tf->getIsClamped();
    const ::fwData::TransferFunction::TFColor black(0., 0., 0., 1.);

    // We need first and last colors defined in the TF in order to fill the uninterpolated ranges (left and right)
    const ::fwData::TransferFunction::TFColor lBoundaryColor = isTFClamped ? black : tfData.cbegin()->second;
    const ::fwData::TransferFunction::TFColor rBoundaryColor = isTFClamped ? black : tfData.crbegin()->second;

    // Counter used to iterate through the texture buffer without exceeding its limit
    std::int32_t k = 0;

    // LEFT BOUNDARY
    for(; k < minIntensityIndex; ++k)
    {
        *pDest++ = static_cast<std::uint8_t>(lBoundaryColor.b * 255);
        *pDest++ = static_cast<std::uint8_t>(lBoundaryColor.g * 255);
        *pDest++ = static_cast<std::uint8_t>(lBoundaryColor.r * 255);
        *pDest++ = static_cast<std::uint8_t>(lBoundaryColor.a * 255);
    }

    const double invWindow = 1./_tf->getWindow();

    // Index corresponding to the last tf value.
    const std::int32_t maxIntensityIndex = intensityToBufferIndex(intensityMinMax.second);

    for(; k < maxIntensityIndex; ++k)
    {
        // Buffer index to tf intensity.
        ::fwData::TransferFunction::TFValueType value = static_cast<double>(k) - TEXTURE_CENTER_INDEX;

        // Tf intensity to mapped color.
        value = (value - intensityMinMax.first) * (tfMinMax.second - tfMinMax.first) * invWindow + tfMinMax.first;

        ::fwData::TransferFunction::TFColor interpolatedColor = _tf->getInterpolatedColor(value);

        *pDest++ = static_cast<std::uint8_t>(interpolatedColor.b * 255);
        *pDest++ = static_cast<std::uint8_t>(interpolatedColor.g * 255);
        *pDest++ = static_cast<std::uint8_t>(interpolatedColor.r * 255);
        *pDest++ = static_cast<std::uint8_t>(interpolatedColor.a * 255);
    }

    // RIGHT BOUNDARY
    for(; k < TEXTURE_PIXEL_COUNT; ++k)
    {
        *pDest++ = static_cast<std::uint8_t>(rBoundaryColor.b * 255);
        *pDest++ = static_cast<std::uint8_t>(rBoundaryColor.g * 255);
        *pDest++ = static_cast<std::uint8_t>(rBoundaryColor.r * 255);
        *pDest++ = static_cast<std::uint8_t>(rBoundaryColor.a * 255);
    }

    pixBuffer->unlock();
}

//-----------------------------------------------------------------------------

std::int32_t TransferFunction::intensityToBufferIndex(double intensity)
{
    std::int32_t bufferIndex = static_cast<std::int32_t>(intensity) + TEXTURE_CENTER_INDEX;

    // Clamp the buffer index.
    if(bufferIndex < 0 || bufferIndex >= TEXTURE_PIXEL_COUNT)
    {
        SLM_WARN("GPU TFs only handle intensities between -32768 and 32767 but the given intensity"
                 " is outside that range.");
        bufferIndex = std::min(::std::max(bufferIndex, 0), TEXTURE_PIXEL_COUNT);
    }

    return bufferIndex;
}

//-----------------------------------------------------------------------------

} // Namespace fwRenderOgre
