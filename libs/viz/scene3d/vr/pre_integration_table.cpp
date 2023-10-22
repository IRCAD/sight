/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "viz/scene3d/vr/pre_integration_table.hpp"

#include <core/memory/buffer_manager.hpp>
#include <core/memory/buffer_object.hpp>
#include <core/profiling.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgrePrerequisites.h>
#include <OGRE/OgreTextureManager.h>

#include <algorithm>

// Usual nolint comment does not work for an unknown reason (clang 17)
// cspell:ignore Wunknown
#ifdef __clang_analyzer__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif

namespace sight::viz::scene3d::vr
{

//-----------------------------------------------------------------------------

pre_integration_table::pre_integration_table()
= default;

//-----------------------------------------------------------------------------

pre_integration_table::~pre_integration_table()
= default;

//-----------------------------------------------------------------------------

void pre_integration_table::createTexture(const std::string& _parent_id)
{
    m_tableTexture = Ogre::TextureManager::getSingleton().create(
        _parent_id + "_PreIntTableTexture",
        viz::scene3d::RESOURCE_GROUP,
        true
    );
}

//-----------------------------------------------------------------------------

void pre_integration_table::removeTexture()
{
    Ogre::TextureManager::getSingleton().remove(m_tableTexture->getHandle());
    m_tableTexture.reset();
}

//-----------------------------------------------------------------------------

void pre_integration_table::imageUpdate(
    const data::image::csptr& _img,
    const data::transfer_function::csptr& _tf,
    float _sampling_rate
)
{
    FW_PROFILE("TF Init")
    {
        Ogre::PixelFormat pixel_format = viz::scene3d::utils::getPixelFormatOgre(*_img);

        const auto dump_lock = _img->dump_lock();

        const std::size_t nb_pixels = _img->numElements();

        switch(pixel_format)
        {
            case Ogre::PF_L8: //uint8
            {
                const auto* uchar_img_buffer = static_cast<const uint8_t*>(_img->buffer());
                auto min_max                 = std::minmax_element(uchar_img_buffer, uchar_img_buffer + nb_pixels);

                m_valueInterval.first  = *min_max.first;
                m_valueInterval.second = *min_max.second;

                break;
            }

            case Ogre::PF_L16: //int16
            {
                const auto* ushort_img_buffer = static_cast<const int16_t*>(_img->buffer());
                auto min_max                  = std::minmax_element(ushort_img_buffer, ushort_img_buffer + nb_pixels);

                m_valueInterval.first  = *min_max.first;
                m_valueInterval.second = *min_max.second;

                break;
            }

            case ::Ogre::PF_R32_SINT: //int32
            {
                const auto* uint_img_buffer = static_cast<const int32_t*>(_img->buffer());
                auto min_max                = std::minmax_element(uint_img_buffer, uint_img_buffer + nb_pixels);

                m_valueInterval.first  = *min_max.first;
                m_valueInterval.second = *min_max.second;

                break;
            }

            default:
                SIGHT_FATAL("Invalid pixel format for pre-integration, pixels must be integers");
        }

        auto texture_size = static_cast<unsigned>(m_valueInterval.second - m_valueInterval.first);

        if(texture_size != m_textureSize)
        {
            m_textureSize = texture_size;

            if(m_table != nullptr)
            {
                delete[] m_table;
                delete[] m_integralTable;
            }

            m_table         = new TablePixel[static_cast<std::size_t>(m_textureSize) * m_textureSize];
            m_integralTable = new IntegralPixel[m_textureSize];

            utils::allocateTexture(
                m_tableTexture.get(),
                m_textureSize,
                m_textureSize,
                1,
                Ogre::PF_A8R8G8B8,
                Ogre::TEX_TYPE_2D,
                true
            );

            tfUpdate(_tf, _sampling_rate);
        }
    }
}

//-----------------------------------------------------------------------------

void pre_integration_table::tfUpdate(const data::transfer_function::csptr& _tf, float _sample_distance)
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
            data::transfer_function::value_t value              = k + m_valueInterval.first;
            data::transfer_function::color_t interpolated_color = _tf->sample(value);

            // We use associated colours.
            double alpha = interpolated_color.a;
            tmp += glm::vec4(
                alpha * interpolated_color.r,
                alpha * interpolated_color.g,
                alpha * interpolated_color.b,
                alpha
            );

            m_integralTable[k] = tmp;
        }

        // Inverse of the sampling accounted by the TF.
        const float sampling_adjustment_factor = 200.F;

        // NOLINTNEXTLINE(clang-diagnostic-unknown-pragmas)
        #pragma omp parallel for schedule(dynamic)
        for(int sb = 0 ; sb < static_cast<int>(m_textureSize) ; ++sb)
        {
            for(int sf = 0 ; sf < static_cast<int>(m_textureSize) ; ++sf)
            {
                glm::vec4 res(0.F);

                const float d = (_sample_distance * sampling_adjustment_factor) / static_cast<float>(sb - sf);

                if(sb != sf)
                {
                    const float opacity = 1.F - std::exp(-d * (m_integralTable[sb].a - m_integralTable[sf].a));

                    const glm::vec3 colour =
                        (d * (glm::vec3(m_integralTable[sb]) - glm::vec3(m_integralTable[sf]))) / opacity;

                    res = glm::vec4(colour, opacity);
                }
                else
                {
                    data::transfer_function::value_t value              = sb + m_valueInterval.first;
                    data::transfer_function::color_t interpolated_color = _tf->sample(value);

                    res =
                        glm::vec4(
                            interpolated_color.r,
                            interpolated_color.g,
                            interpolated_color.b,
                            interpolated_color.a
                        );

                    res.a = 1.F - std::pow(1.F - res.a, _sample_distance * sampling_adjustment_factor);
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
        Ogre::HardwarePixelBufferSharedPtr pix_buffer = m_tableTexture->getBuffer();

        // Discards the entire buffer while locking so that we can easily refill it from scratch
        pix_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        Ogre::PixelBox pix_box = pix_buffer->getCurrentLock();
        auto* p_dest           = static_cast<std::uint8_t*>(pix_box.data);

        std::memcpy(p_dest, m_table, static_cast<std::size_t>(m_textureSize) * m_textureSize * sizeof(TablePixel));

        pix_buffer->unlock();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr

#ifdef __clang_analyzer__
#pragma clang diagnostic pop
#endif
