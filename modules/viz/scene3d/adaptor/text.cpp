/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/text.hpp"

#include <core/com/slots.hxx>

#include <data/generic_field_base.hpp>

#include <viz/scene3d/render.hpp>
#include <viz/scene3d/window_interactor.hpp>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t SET_TEXT_SLOT = "setText";

//----------------------------------------------------------------------------

text::text() noexcept
{
    new_slot(SET_TEXT_SLOT, &text::setText, this);
}

//----------------------------------------------------------------------------

void text::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    static const std::string s_TEXT_CONFIG        = "text";
    static const std::string s_FONT_SIZE_CONFIG   = s_CONFIG + "fontSize";
    static const std::string s_FONT_SOURCE_CONFIG = s_CONFIG + "fontSource";
    static const std::string s_H_ALIGN_CONFIG     = s_CONFIG + "hAlign";
    static const std::string s_V_ALIGN_CONFIG     = s_CONFIG + "vAlign";
    static const std::string s_X_CONFIG           = s_CONFIG + "x";
    static const std::string s_Y_CONFIG           = s_CONFIG + "y";
    static const std::string s_COLOR_CONFIG       = s_CONFIG + "color";

    m_textString = config.get<std::string>(s_TEXT_CONFIG, "");

    m_fontSource = config.get(s_FONT_SOURCE_CONFIG, m_fontSource);
    m_fontSize   = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_fontSize);

    m_horizontalAlignment = config.get<std::string>(s_H_ALIGN_CONFIG, "left");
    SIGHT_ASSERT(
        "'hAlign' must be 'left', 'center' or 'right'",
        m_horizontalAlignment == "left"
        || m_horizontalAlignment == "center"
        || m_horizontalAlignment == "right"
    );

    m_verticalAlignment = config.get<std::string>(s_V_ALIGN_CONFIG, "bottom");
    SIGHT_ASSERT(
        "'vAlign' must be 'top', 'center' or 'bottom'",
        m_verticalAlignment == "top"
        || m_verticalAlignment == "center"
        || m_verticalAlignment == "bottom"
    );

    m_position.x = config.get<float>(s_X_CONFIG, m_position.x);
    m_position.y = config.get<float>(s_Y_CONFIG, m_position.y);

    m_textColor = config.get<std::string>(s_COLOR_CONFIG, "#FFFFFF");
}

//----------------------------------------------------------------------------

void text::starting()
{
    this->initialize();

    auto renderSrv = this->getRenderService();

    m_text = sight::viz::scene3d::IText::make(this->getLayer());

    m_text->setFontSize(m_fontSize);
    m_text->setTextColor(m_textColor);

    this->updateText();
}

//-----------------------------------------------------------------------------

service::connections_t text::auto_connections() const
{
    connections_t connections;
    connections.push(s_OBJECT_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//----------------------------------------------------------------------------

void text::updating()
{
    this->getRenderService()->makeCurrent();
    this->updateText();
}

//----------------------------------------------------------------------------

void text::stopping()
{
    m_text = nullptr;
}

//----------------------------------------------------------------------------

void text::setText(std::string str)
{
    m_textString = str;
    m_text->setText(str);
    m_text->setTextColor(m_textColor);
    m_text->setTextAlignment(m_horizontalAlignment, m_verticalAlignment);
}

//----------------------------------------------------------------------------

void text::updateText()
{
    std::string textString = m_textString;

    const auto obj = m_object.lock();

    if(obj)
    {
        const auto field = std::dynamic_pointer_cast<const data::generic_field_base>(obj.get_shared());

        if(field)
        {
            textString = field->toString();
        }
    }

    this->setText(textString);
}

} // namespace sight::module::viz::scene3d::adaptor.
