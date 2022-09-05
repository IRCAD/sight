/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "viz/scene3d/vr/PreIntegrationTable.hpp"

#include <core/memory/BufferManager.hpp>
#include <core/memory/BufferObject.hpp>
#include <core/Profiling.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/Utils.hpp>

#include <boost/algorithm/clamp.hpp>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreVector4.h>

#include <algorithm>

namespace sight::viz::scene3d::vr
{

//-----------------------------------------------------------------------------

PreIntegrationTable::PreIntegrationTable()
= default;

//-----------------------------------------------------------------------------

PreIntegrationTable::~PreIntegrationTable()
= default;

//-----------------------------------------------------------------------------

void PreIntegrationTable::createTexture(const std::string& _parentId)
{
    m_tableTexture = Ogre::TextureManager::getSingleton().create(
        _parentId + "_PreIntTableTexture",
        viz::scene3d::RESOURCE_GROUP,
        true
    );
}

//-----------------------------------------------------------------------------

void PreIntegrationTable::removeTexture()
{
    Ogre::TextureManager::getSingleton().remove(m_tableTexture->getHandle());
    m_tableTexture.reset();
}

//-----------------------------------------------------------------------------

void PreIntegrationTable::imageUpdate(
    const data::Image::csptr& _img,
    const data::TransferFunction::csptr& _tf,
    float _samplingRate
)
{
    FW_PROFILE("TF Init")
    {
        Ogre::PixelFormat pixelFormat = viz::scene3d::Utils::getPixelFormatOgre(_img);

        const auto dumpLock = _img->dump_lock();

        const std::size_t nbPixels = _img->numElements();

        switch(pixelFormat)
        {
            case Ogre::PF_L8: //uint8
            {
                const auto* ucharImgBuffer = static_cast<const uint8_t*>(_img->getBuffer());
                auto minMax                = std::minmax_element(ucharImgBuffer, ucharImgBuffer + nbPixels);

                m_valueInterval.first  = *minMax.first;
                m_valueInterval.second = *minMax.second;

                break;
            }

            case Ogre::PF_L16: //int16
            {
                const auto* ushortImgBuffer = static_cast<const int16_t*>(_img->getBuffer());
                auto minMax                 = std::minmax_element(ushortImgBuffer, ushortImgBuffer + nbPixels);

                m_valueInterval.first  = *minMax.first;
                m_valueInterval.second = *minMax.second;

                break;
            }

            case ::Ogre::PF_R32_SINT: //int32
            {
                const auto* uintImgBuffer = static_cast<const int32_t*>(_img->getBuffer());
                auto minMax               = std::minmax_element(uintImgBuffer, uintImgBuffer + nbPixels);

                m_valueInterval.first  = *minMax.first;
                m_valueInterval.second = *minMax.second;

                break;
            }

            default:
                SIGHT_FATAL("Invalid pixel format for pre-integration, pixels must be integers");
        }

        auto textureSize = static_cast<unsigned>(m_valueInterval.second - m_valueInterval.first);

        if(textureSize != m_textureSize)
        {
            m_textureSize = textureSize;

            if(m_table != nullptr)
            {
                delete m_table;
                delete m_integralTable;
            }

            m_table         = new TablePixel[static_cast<std::size_t>(m_textureSize) * m_textureSize];
            m_integralTable = new IntegralPixel[m_textureSize];

            Utils::allocateTexture(
                m_tableTexture.get(),
                m_textureSize,
                m_textureSize,
                1,
                Ogre::PF_A8R8G8B8,
                Ogre::TEX_TYPE_2D,
                true
            );

            tfUpdate(_tf, _samplingRate);
        }
    }
}

//-----------------------------------------------------------------------------

void PreIntegrationTable::tfUpdate(const data::TransferFunction::csptr& _tf, float _sampleDistance)
{
    if(m_table == nullptr)
    {
        return;
    }

    FW_PROFILE("PreIntegration")
    {
        glm::vec4 tmp(0.F);

        for(int k = 0 ; k < static_cast<int>(m_textureSize) ; ++k)
        {
            data::TransferFunction::value_t value             = k + m_valueInterval.first;
            data::TransferFunction::color_t interpolatedColor = _tf->sample(value);

            // We use associated colours.
            double alpha = interpolatedColor.a;
            tmp += glm::vec4(
                alpha * interpolatedColor.r,
                alpha * interpolatedColor.g,
                alpha * interpolatedColor.b,
                alpha
            );

            m_integralTable[k] = tmp;
        }

        // Inverse of the sampling accounted by the TF.
        const float samplingAdjustmentFactor = 200.F;

        #pragma omp parallel for schedule(dynamic)
        for(int sb = 0 ; sb < static_cast<int>(m_textureSize) ; ++sb)
        {
            for(int sf = 0 ; sf < static_cast<int>(m_textureSize) ; ++sf)
            {
                glm::vec4 res(0.F);

                const float d = (_sampleDistance * samplingAdjustmentFactor) / static_cast<float>(sb - sf);

                if(sb != sf)
                {
                    const float opacity = 1.F - std::exp(-d * (m_integralTable[sb].a - m_integralTable[sf].a));

                    const glm::vec3 colour =
                        (d * (glm::vec3(m_integralTable[sb]) - glm::vec3(m_integralTable[sf]))) / opacity;

                    res = glm::vec4(colour, opacity);
                }
                else
                {
                    data::TransferFunction::value_t value             = sb + m_valueInterval.first;
                    data::TransferFunction::color_t interpolatedColor = _tf->sample(value);

                    res =
                        glm::vec4(
                            interpolatedColor.r,
                            interpolatedColor.g,
                            interpolatedColor.b,
                            interpolatedColor.a
                        );

                    res.a = 1.F - std::pow(1.F - res.a, _sampleDistance * samplingAdjustmentFactor);
                }

                res = glm::clamp(res, 0.F, 1.F);

                m_table[static_cast<unsigned>(sb) * m_textureSize + static_cast<unsigned>(sf)] = {
                    static_cast<uint8_t>(res.b * 255.F),
                    static_cast<uint8_t>(res.g * 255.F),
                    static_cast<uint8_t>(res.r * 255.F),
                    static_cast<uint8_t>(res.a * 255.F)
                };
            }
        }

        // Store table in texture buffer.
        Ogre::HardwarePixelBufferSharedPtr pixBuffer = m_tableTexture->getBuffer();

        // Discards the entire buffer while locking so that we can easily refill it from scratch
        pixBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        Ogre::PixelBox pixBox = pixBuffer->getCurrentLock();
        auto* pDest           = static_cast<std::uint8_t*>(pixBox.data);

        std::memcpy(pDest, m_table, static_cast<std::size_t>(m_textureSize) * m_textureSize * sizeof(TablePixel));

        pixBuffer->unlock();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
