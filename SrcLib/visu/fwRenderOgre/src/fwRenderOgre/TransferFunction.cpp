/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/TransferFunction.hpp"

#include <fwRenderOgre/Utils.hpp>

#include <OGRE/OgreTextureManager.h>

#include <OgreHardwarePixelBuffer.h>

#include <cstdint>  // for std::uint_8
#include <math.h>   // for log

namespace fwRenderOgre
{

static const size_t TEXTURE_SIZE = 256;

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
    if(m_texture.isNull())
    {
        m_texture = ::Ogre::TextureManager::getSingleton().createManual(
            _parentId + "_tfTexture",                                   // name
            ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,  // resource groupe
            ::Ogre::TEX_TYPE_2D,                                        // type
            TEXTURE_SIZE, TEXTURE_SIZE,                                 // width, height
            0,                                                          // number of mipmaps (depth)
            ::Ogre::PF_FLOAT32_RGBA,                                    // pixel format
            ::Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);                 // usage
    }
}

//-----------------------------------------------------------------------------

void TransferFunction::removeTexture()
{
    ::Ogre::TextureManager::getSingleton().remove(m_texture->getHandle());
    m_texture.setNull();
}

//-----------------------------------------------------------------------------

void TransferFunction::updateTexture(const ::fwData::TransferFunction::csptr& _tf)
{
    // Retrieves the pixel buffer from the texture
    ::Ogre::HardwarePixelBufferSharedPtr pixBuffer = m_texture->getBuffer();

    // Discards the entire buffer while locking so that we can easily refill it from scratch
    pixBuffer->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
    ::Ogre::PixelBox pixBox = pixBuffer->getCurrentLock();
    float* pDest = static_cast<float*>(pixBox.data);

    // Retrieves the transfer function's data map
    const ::fwData::TransferFunction::TFDataType tfData = _tf->getTFData();

    // Retrieves the transfer function's intensity window
    const ::fwData::TransferFunction::TFValuePairType intensityMinMax = _tf->getWLMinMax();

    const ::fwData::TransferFunction::TFValuePairType tfMinMax = _tf->getMinMaxTFValues();

    // Used to convert intensity values to texture values
    const double halfTextureSize = (TEXTURE_SIZE * TEXTURE_SIZE) / 2;

    // Boundary between first uninterpolated range and the TF nodes group
    const size_t lScaledBoundary = static_cast<size_t>(intensityMinMax.first + halfTextureSize);

    // If the transfer function is clamped, we have to force the extremity colors to black
    bool isTFClamped = _tf->getIsClamped();
    ::fwData::TransferFunction::TFColor black(0.f, 0.f, 0.f, 1.f);

    // We need first and last colors defined in the TF in order to fill the uninterpolated ranges (left and right)
    const ::fwData::TransferFunction::TFColor lBoundaryColor = isTFClamped ? black : tfData.cbegin()->second;
    const ::fwData::TransferFunction::TFColor rBoundaryColor = isTFClamped ? black : tfData.crbegin()->second;

    const double lBoundaryExtinction = -std::log(1.f - lBoundaryColor.a) / m_sampleDistance;
    const double rBoundaryExtinction = -std::log(1.f - rBoundaryColor.a) / m_sampleDistance;

    // Counter used to iterate through the texture buffer without exceeding its limit
    size_t k = 0;

    // LEFT BOUNDARY
    for(; k < lScaledBoundary; ++k)
    {
        *pDest++ = static_cast<float>(lBoundaryColor.r);
        *pDest++ = static_cast<float>(lBoundaryColor.g);
        *pDest++ = static_cast<float>(lBoundaryColor.b);
        *pDest++ = static_cast<float>(lBoundaryExtinction);
    }

    // Retrieves intensity value for each TF value
    const ::fwData::TransferFunction::TFValueVectorType intensityValues = _tf->getScaledValues();

    const double invWindow = 1./_tf->getWindow();

    // DEFINED VALUES
    if(!intensityValues.empty())
    {
        size_t lIntensityValue = static_cast<size_t>(*intensityValues.begin() + halfTextureSize);
        size_t rIntensityValue = static_cast<size_t>(*intensityValues.rbegin() + halfTextureSize);

        // We need to compute the extinction values for every interpolated colors.
        double extinction(0.);

        // For each couple of TF nodes, we generate interpolated colors for a range in the texture starting from a left
        // scaled value to a right one
        for(k = lIntensityValue; k < rIntensityValue; ++k)
        {
            // texture --> intensity
            ::fwData::TransferFunction::TFValueType value = k - halfTextureSize;

            // intensity --> transfer function
            value = (value - intensityMinMax.first) * (tfMinMax.second - tfMinMax.first) * invWindow + tfMinMax.first;

            ::fwData::TransferFunction::TFColor interpolatedColor = _tf->getInterpolatedColor(value);
            extinction                                            = -std::log(1.f - interpolatedColor.a) /
                                                                    m_sampleDistance;

            *pDest++ = static_cast<float>(interpolatedColor.r);
            *pDest++ = static_cast<float>(interpolatedColor.g);
            *pDest++ = static_cast<float>(interpolatedColor.b);
            *pDest++ = static_cast<float>(extinction);
        }
    }

    // RIGHT BOUNDARY
    for(; k < TEXTURE_SIZE * TEXTURE_SIZE; ++k)
    {
        *pDest++ = static_cast<float>(rBoundaryColor.r);
        *pDest++ = static_cast<float>(rBoundaryColor.g);
        *pDest++ = static_cast<float>(rBoundaryColor.b);
        *pDest++ = static_cast<float>(rBoundaryExtinction);
    }

    pixBuffer->unlock();
}

//-----------------------------------------------------------------------------

} // Namespace fwRenderOgre
