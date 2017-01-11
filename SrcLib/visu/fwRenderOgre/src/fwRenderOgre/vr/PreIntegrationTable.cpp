/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/vr/PreIntegrationTable.hpp"
#include <fwRenderOgre/Utils.hpp>

#include <fwCore/Profiling.hpp>

#include <fwDataTools/helper/Image.hpp>

#include <fwMemory/BufferManager.hpp>
#include <fwMemory/BufferObject.hpp>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreVector4.h>

#include <boost/algorithm/clamp.hpp>

#include <algorithm>

namespace fwRenderOgre
{

namespace vr
{

//-----------------------------------------------------------------------------

PreIntegrationTable::PreIntegrationTable() :
    m_table(nullptr),
    m_integralTable(nullptr),
    m_textureSize(0)
{

}

//-----------------------------------------------------------------------------

PreIntegrationTable::~PreIntegrationTable()
{

}

//-----------------------------------------------------------------------------

void PreIntegrationTable::createTexture(const std::string& _parentId)
{
    m_tableTexture = ::Ogre::TextureManager::getSingleton().create(
        _parentId + "_PreIntTableTexture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);
}

//-----------------------------------------------------------------------------

void PreIntegrationTable::removeTexture()
{
    ::Ogre::TextureManager::getSingleton().remove(m_tableTexture->getHandle());
    m_tableTexture.setNull();
}

//-----------------------------------------------------------------------------

void PreIntegrationTable::imageUpdate(const fwData::Image::sptr& _img, const ::fwData::TransferFunction::sptr& _tf,
                                      float _samplingRate)
{
    FW_PROFILE("TF Init")
    {
        ::Ogre::PixelFormat pixelFormat = ::fwRenderOgre::Utils::getPixelFormatOgre(_img);

        ::fwDataTools::helper::Image imgHelper(_img);

        size_t nbPixels = _img->getDataArray()->getNumberOfElements();

        switch(pixelFormat)
        {
            case ::Ogre::PF_L8: //uint8
            {
                uint8_t* ucharImgBuffer = static_cast<uint8_t*>(imgHelper.getBuffer());
                auto minMax             = std::minmax_element(ucharImgBuffer, ucharImgBuffer + nbPixels);

                m_valueInterval.first  = *minMax.first;
                m_valueInterval.second = *minMax.second;

                break;
            }
            case ::Ogre::PF_L16: //int16
            {
                int16_t* ushortImgBuffer = static_cast<int16_t*>(imgHelper.getBuffer());
                auto minMax              = std::minmax_element(ushortImgBuffer, ushortImgBuffer + nbPixels);

                m_valueInterval.first  = *minMax.first;
                m_valueInterval.second = *minMax.second;

                break;
            }
            default:
                OSLM_FATAL("Invalid pixel format for pre-integration, pixels must be integers");
        }

        unsigned textureSize = m_valueInterval.second - m_valueInterval.first;

        if(textureSize != m_textureSize)
        {
            m_textureSize = textureSize;

            if(m_table)
            {
                delete m_table;
                delete m_integralTable;
            }

            m_table         = new TablePixel[m_textureSize * m_textureSize];
            m_integralTable = new IntegralPixel[m_textureSize];

            Utils::allocateTexture(
                m_tableTexture.get(), m_textureSize, m_textureSize, 1, ::Ogre::PF_A8R8G8B8, ::Ogre::TEX_TYPE_2D,
                true);

            tfUpdate(_tf, _samplingRate);
        }
    }
}

//-----------------------------------------------------------------------------

void PreIntegrationTable::tfUpdate(const fwData::TransferFunction::sptr& _tf, float _sampleDistance)
{
    if(!m_table)
    {
        return;
    }

    FW_PROFILE("PreIntegration")
    {
        const ::fwData::TransferFunction::TFValuePairType intensityMinMax = _tf->getWLMinMax();

        const ::fwData::TransferFunction::TFValuePairType tfMinMax = _tf->getMinMaxTFValues();

        const double invWindow = 1./_tf->getWindow();

        ::glm::vec4 tmp(0.f);

        _tf->setIsClamped(false);

        for(int k = 0; k < (int)m_textureSize; ++k)
        {
            ::fwData::TransferFunction::TFValueType value = k + m_valueInterval.first;

            value = (value - intensityMinMax.first) * (tfMinMax.second - tfMinMax.first) * invWindow + tfMinMax.first;

            ::fwData::TransferFunction::TFColor interpolatedColor = _tf->getInterpolatedColor(value);

            // We use associated colours.
            double alpha = interpolatedColor.a;
            tmp += ::glm::vec4(alpha * interpolatedColor.r, alpha * interpolatedColor.g, alpha * interpolatedColor.b,
                               alpha);

            m_integralTable[k] = tmp;
        }

        // Inverse of the sampling accounted by the TF.
        const float samplingAdjustementFactor = 200.f;

        #pragma omp parallel for schedule(dynamic)
        for(int sb = 0; sb < static_cast<int>(m_textureSize); ++sb)
        {
            for(int sf = 0; sf < static_cast<int>(m_textureSize); ++sf)
            {
                ::glm::vec4 res(0.f);

                const float d = (_sampleDistance * samplingAdjustementFactor) / static_cast<float>(sb - sf);

                if(sb != sf)
                {
                    const float opacity = 1.f - std::exp( -d * (m_integralTable[sb].a - m_integralTable[sf].a));

                    const ::glm::vec3 colour =
                        (d * (::glm::vec3(m_integralTable[sb]) - ::glm::vec3(m_integralTable[sf]))) / opacity;

                    res = ::glm::vec4(colour, opacity);
                }
                else
                {
                    ::fwData::TransferFunction::TFValueType value = sb + m_valueInterval.first;

                    // intensity --> transfer function
                    value = (value - intensityMinMax.first) * (tfMinMax.second - tfMinMax.first) * invWindow +
                            tfMinMax.first;

                    ::fwData::TransferFunction::TFColor interpolatedColor = _tf->getInterpolatedColor(value);

                    res =
                        ::glm::vec4(interpolatedColor.r, interpolatedColor.g, interpolatedColor.b,
                                    interpolatedColor.a);

                    res.a = 1.f - std::pow(1.f - res.a, _sampleDistance * samplingAdjustementFactor);
                }

                res = ::glm::clamp(res, 0.f, 1.f);

                m_table[sb * m_textureSize + sf] = {
                    static_cast<uint8_t>(res.b * 255.f),
                    static_cast<uint8_t>(res.g * 255.f),
                    static_cast<uint8_t>(res.r * 255.f),
                    static_cast<uint8_t>(res.a * 255.f)
                };
            }
        }

        // Store table in texture buffer.
        ::Ogre::HardwarePixelBufferSharedPtr pixBuffer = m_tableTexture->getBuffer();

        // Discards the entire buffer while locking so that we can easily refill it from scratch
        pixBuffer->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
        ::Ogre::PixelBox pixBox = pixBuffer->getCurrentLock();
        std::uint8_t* pDest = static_cast<std::uint8_t*>(pixBox.data);

        std::memcpy(pDest, m_table, m_textureSize * m_textureSize * sizeof(TablePixel));

        pixBuffer->unlock();
    }
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace fwRenderOgre
