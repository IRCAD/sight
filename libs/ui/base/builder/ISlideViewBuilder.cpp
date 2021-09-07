/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
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

#include <boost/lexical_cast.hpp>

namespace sight::ui::base
{

namespace builder
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

ISlideViewBuilder::ISlideViewBuilder()
{
}

//-----------------------------------------------------------------------------

ISlideViewBuilder::~ISlideViewBuilder()
{
}

//-----------------------------------------------------------------------------

void ISlideViewBuilder::initialize(core::runtime::ConfigurationElement::sptr _config)
{
    SIGHT_ASSERT(
        "Bad configuration name " + _config->getName() + ", must be 'slideView'",
        _config->getName() == "slideView"
    );

    if(_config->hasAttribute(s_H_ALIGN_CONFIG))
    {
        const std::string hAlign = _config->getExistingAttributeValue(s_H_ALIGN_CONFIG);
        if(hAlign == "left")
        {
            m_hAlignment = LEFT;
        }
        else if(hAlign == "right")
        {
            m_hAlignment = RIGHT;
        }
        else
        {
            SIGHT_FATAL(
                "Wrong value '" + hAlign + "' for '" + s_H_ALIGN_CONFIG
                + "' attribute (require 'left' or 'right')"
            );
        }
    }

    if(_config->hasAttribute(s_V_ALIGN_CONFIG))
    {
        const std::string vAlign = _config->getExistingAttributeValue(s_V_ALIGN_CONFIG);
        if(vAlign == "top")
        {
            m_vAlignment = TOP;
        }
        else if(vAlign == "bottom")
        {
            m_vAlignment = BOTTOM;
        }
        else
        {
            SIGHT_FATAL(
                "Wrong value '" + vAlign + "' for '" + s_V_ALIGN_CONFIG
                + "' attribute (require 'top' or 'bottom')"
            );
        }
    }

    if(_config->hasAttribute(s_WIDTH_CONFIG))
    {
        std::string width = _config->getExistingAttributeValue(s_WIDTH_CONFIG);
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

    if(_config->hasAttribute(s_HEIGHT_CONFIG))
    {
        std::string height = _config->getExistingAttributeValue(s_HEIGHT_CONFIG);
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

    if(_config->hasAttribute(s_H_OFFSET_CONFIG))
    {
        std::string offset = _config->getExistingAttributeValue(s_H_OFFSET_CONFIG);
        if(offset[offset.size() - 1] == '%')
        {
            m_percentHOffset = true;
            offset           = offset.substr(0, offset.size() - 1);
        }

        m_hOffset = std::stoi(offset);
    }

    if(_config->hasAttribute(s_V_OFFSET_CONFIG))
    {
        std::string offset = _config->getExistingAttributeValue(s_V_OFFSET_CONFIG);
        if(offset[offset.size() - 1] == '%')
        {
            m_percentVOffset = true;
            offset           = offset.substr(0, offset.size() - 1);
        }

        m_vOffset = std::stoi(offset);
    }

    if(_config->hasAttribute(s_OPACITY_CONFIG))
    {
        m_opacity = std::stod(_config->getExistingAttributeValue(s_OPACITY_CONFIG));
        SIGHT_ASSERT(
            "Opacity must be in [0 - 1]; actual: " + std::to_string(
                m_opacity
            ),
            m_opacity >= 0. && m_opacity <= 1.
        );
    }

    if(_config->hasAttribute(s_ANIMATABLE_CONFIG))
    {
        const std::string animatable = _config->getExistingAttributeValue(s_ANIMATABLE_CONFIG);
        if(animatable == "true")
        {
            m_animatable = true;
        }
        else if(animatable == "false")
        {
            m_animatable = false;
        }
        else
        {
            SIGHT_FATAL(
                "Wrong value '" + animatable + "' for '" + s_ANIMATABLE_CONFIG
                + "' attribute (require 'true' or 'false')"
            );
        }
    }

    if(_config->hasAttribute(s_ANIMATABLE_ALIGN_CONFIG))
    {
        const std::string align = _config->getExistingAttributeValue(s_ANIMATABLE_ALIGN_CONFIG);
        if(align == "top")
        {
            m_animatableAlignment = TOP_ANIMATION;
        }
        else if(align == "bottom")
        {
            m_animatableAlignment = BOTTOM_ANIMATION;
        }
        else if(align == "left")
        {
            m_animatableAlignment = LEFT_ANIMATION;
        }
        else if(align == "right")
        {
            m_animatableAlignment = RIGHT_ANIMATION;
        }
        else
        {
            SIGHT_FATAL(
                "Wrong value '" + align + "' for '" + s_ANIMATABLE_ALIGN_CONFIG
                + "' attribute (require 'left', 'right', 'top' or 'bottom')"
            );
        }
    }

    core::runtime::ConfigurationElement::csptr styleCfg = _config->findConfigurationElement(s_STYLE_SHEET_CONFIG);
    if(styleCfg)
    {
        m_styleSheet = styleCfg->getValue();
    }

    // Deprecated configuration.
    int size = 200;
    if(_config->hasAttribute("size"))
    {
        FW_DEPRECATED_MSG("::ui::base::builder::ISlideViewBuilder deprecated attribute 'size'", "21.0");
        size = std::stoi(_config->getExistingAttributeValue("size"));
    }

    if(_config->hasAttribute("align"))
    {
        FW_DEPRECATED_MSG("::ui::base::builder::ISlideViewBuilder deprecated attribute 'align'", "21.0");
        const std::string align = _config->getExistingAttributeValue("align");
        if(align == "top")
        {
            m_hAlignment    = LEFT;
            m_vAlignment    = TOP;
            m_height        = size;
            m_percentHeight = false;
        }
        else if(align == "bottom")
        {
            m_hAlignment    = LEFT;
            m_vAlignment    = BOTTOM;
            m_height        = size;
            m_percentHeight = false;
        }
        else if(align == "right")
        {
            m_hAlignment   = RIGHT;
            m_vAlignment   = TOP;
            m_width        = size;
            m_percentWidth = false;
        }
        else if(align == "left")
        {
            m_hAlignment   = LEFT;
            m_vAlignment   = TOP;
            m_width        = size;
            m_percentWidth = false;
        }
    }
}

//-----------------------------------------------------------------------------

ui::base::container::fwContainer::sptr ISlideViewBuilder::getContainer() const
{
    return m_container;
}

//-----------------------------------------------------------------------------

} // namespace builder.

} // namespace sight::ui::base.
