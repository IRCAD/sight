/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "ui/base/builder/ISlideViewBuilder.hpp"

namespace sight::ui::base::builder
{

const std::string ISlideViewBuilder::REGISTRY_KEY = "::ui::base::SlideViewBuilder";

static const std::string s_H_ALIGN_CONFIG          = "hAlign";
static const std::string s_V_ALIGN_CONFIG          = "vAlign";
static const std::string s_H_OFFSET_CONFIG         = "hOffset";
static const std::string s_V_OFFSET_CONFIG         = "vOffset";
static const std::string s_WIDTH_CONFIG            = "width";
static const std::string s_HEIGHT_CONFIG           = "height";
static const std::string s_OPACITY_CONFIG          = "opacity";
static const std::string s_ANIMATABLE_CONFIG       = "animatable";
static const std::string s_ANIMATABLE_ALIGN_CONFIG = "animatableAlignment";
static const std::string s_STYLE_SHEET_CONFIG      = "styleSheet";

//-----------------------------------------------------------------------------

void ISlideViewBuilder::initialize(const ui::base::config_t& _config)
{
    if(const auto hAlign = _config.get_optional<std::string>("<xmlattr>." + s_H_ALIGN_CONFIG); hAlign.has_value())
    {
        if(*hAlign == "left")
        {
            m_hAlignment = LEFT;
        }
        else if(*hAlign == "right")
        {
            m_hAlignment = RIGHT;
        }
        else
        {
            SIGHT_FATAL(
                std::string(
                    "Wrong value '"
                ) + *hAlign + "' for '" + s_H_ALIGN_CONFIG + "' attribute (require 'left' or 'right')"
            );
        }
    }

    if(const auto vAlign = _config.get_optional<std::string>("<xmlattr>." + s_V_ALIGN_CONFIG); vAlign.has_value())
    {
        if(*vAlign == "top")
        {
            m_vAlignment = TOP;
        }
        else if(*vAlign == "bottom")
        {
            m_vAlignment = BOTTOM;
        }
        else
        {
            SIGHT_FATAL(
                std::string(
                    "Wrong value '"
                ) + *vAlign + "' for '" + s_V_ALIGN_CONFIG + "' attribute (require 'top' or 'bottom')"
            );
        }
    }

    if(const auto widthCfg = _config.get_optional<std::string>("<xmlattr>." + s_WIDTH_CONFIG); widthCfg.has_value())
    {
        std::string width = widthCfg.value();
        if(width[width.size() - 1] == '%')
        {
            width = width.substr(0, width.size() - 1);
        }
        else
        {
            m_percentWidth = false;
        }

        m_width = std::stoi(width);
        SIGHT_ASSERT("Height must be upper to 0", m_width >= 0);
    }

    if(const auto heightCfg = _config.get_optional<std::string>("<xmlattr>." + s_HEIGHT_CONFIG); heightCfg.has_value())
    {
        std::string height = heightCfg.value();
        if(height[height.size() - 1] == '%')
        {
            height = height.substr(0, height.size() - 1);
        }
        else
        {
            m_percentHeight = false;
        }

        m_height = std::stoi(height);
        SIGHT_ASSERT("Height must be upper to 0", m_height >= 0);
    }

    if(const auto offCfg = _config.get_optional<std::string>("<xmlattr>." + s_H_OFFSET_CONFIG); offCfg.has_value())
    {
        std::string offset = offCfg.value();
        if(offset[offset.size() - 1] == '%')
        {
            m_percentHOffset = true;
            offset           = offset.substr(0, offset.size() - 1);
        }

        m_hOffset = std::stoi(offset);
    }

    if(const auto offCfg = _config.get_optional<std::string>("<xmlattr>." + s_V_OFFSET_CONFIG); offCfg.has_value())
    {
        std::string offset = offCfg.value();
        if(offset[offset.size() - 1] == '%')
        {
            m_percentVOffset = true;
            offset           = offset.substr(0, offset.size() - 1);
        }

        m_vOffset = std::stoi(offset);
    }

    if(const auto opacityCfg = _config.get_optional<double>("<xmlattr>." + s_OPACITY_CONFIG); opacityCfg.has_value())
    {
        m_opacity = opacityCfg.value();
        SIGHT_ASSERT(
            "Opacity must be in [0 - 1]; actual: " + std::to_string(
                m_opacity
            ),
            m_opacity >= 0. && m_opacity <= 1.
        );
    }

    m_animatable = _config.get<bool>(s_ANIMATABLE_CONFIG, m_animatable);

    if(const auto align = _config.get_optional<std::string>("<xmlattr>." + s_ANIMATABLE_ALIGN_CONFIG);
       align.has_value())
    {
        if(*align == "top")
        {
            m_animatableAlignment = TOP_ANIMATION;
        }
        else if(*align == "bottom")
        {
            m_animatableAlignment = BOTTOM_ANIMATION;
        }
        else if(*align == "left")
        {
            m_animatableAlignment = LEFT_ANIMATION;
        }
        else if(*align == "right")
        {
            m_animatableAlignment = RIGHT_ANIMATION;
        }
        else
        {
            SIGHT_FATAL(
                std::string("Wrong value '") + *align + "' for '" + s_ANIMATABLE_ALIGN_CONFIG
                + "' attribute (require 'left', 'right', 'top' or 'bottom')"
            );
        }
    }

    m_styleSheet = _config.get<std::string>(s_STYLE_SHEET_CONFIG, "");
}

//-----------------------------------------------------------------------------

ui::base::container::fwContainer::sptr ISlideViewBuilder::getContainer() const
{
    return m_container;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base::builder
