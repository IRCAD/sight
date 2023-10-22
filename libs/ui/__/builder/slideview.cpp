/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

#include "ui/__/builder/slideview.hpp"

namespace sight::ui::builder
{

const std::string slideview::REGISTRY_KEY = "::ui::slideview";

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

void slideview::initialize(const ui::config_t& _config)
{
    if(const auto h_align = _config.get_optional<std::string>("<xmlattr>." + s_H_ALIGN_CONFIG); h_align.has_value())
    {
        if(*h_align == "left")
        {
            m_hAlignment = LEFT;
        }
        else if(*h_align == "right")
        {
            m_hAlignment = RIGHT;
        }
        else
        {
            SIGHT_FATAL(
                std::string(
                    "Wrong value '"
                ) + *h_align + "' for '" + s_H_ALIGN_CONFIG + "' attribute (require 'left' or 'right')"
            );
        }
    }

    if(const auto v_align = _config.get_optional<std::string>("<xmlattr>." + s_V_ALIGN_CONFIG); v_align.has_value())
    {
        if(*v_align == "top")
        {
            m_vAlignment = TOP;
        }
        else if(*v_align == "bottom")
        {
            m_vAlignment = BOTTOM;
        }
        else
        {
            SIGHT_FATAL(
                std::string(
                    "Wrong value '"
                ) + *v_align + "' for '" + s_V_ALIGN_CONFIG + "' attribute (require 'top' or 'bottom')"
            );
        }
    }

    if(const auto width_cfg = _config.get_optional<std::string>("<xmlattr>." + s_WIDTH_CONFIG); width_cfg.has_value())
    {
        std::string width = width_cfg.value();
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

    if(const auto height_cfg = _config.get_optional<std::string>("<xmlattr>." + s_HEIGHT_CONFIG);
       height_cfg.has_value())
    {
        std::string height = height_cfg.value();
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

    if(const auto off_cfg = _config.get_optional<std::string>("<xmlattr>." + s_H_OFFSET_CONFIG); off_cfg.has_value())
    {
        std::string offset = off_cfg.value();
        if(offset[offset.size() - 1] == '%')
        {
            m_percentHOffset = true;
            offset           = offset.substr(0, offset.size() - 1);
        }

        m_hOffset = std::stoi(offset);
    }

    if(const auto off_cfg = _config.get_optional<std::string>("<xmlattr>." + s_V_OFFSET_CONFIG); off_cfg.has_value())
    {
        std::string offset = off_cfg.value();
        if(offset[offset.size() - 1] == '%')
        {
            m_percentVOffset = true;
            offset           = offset.substr(0, offset.size() - 1);
        }

        m_vOffset = std::stoi(offset);
    }

    if(const auto opacity_cfg = _config.get_optional<double>("<xmlattr>." + s_OPACITY_CONFIG); opacity_cfg.has_value())
    {
        m_opacity = opacity_cfg.value();
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

ui::container::widget::sptr slideview::getContainer() const
{
    return m_container;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::builder
