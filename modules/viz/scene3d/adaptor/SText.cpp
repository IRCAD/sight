/************************************************************************
 *
 * Copyright (C) 2019-2022 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SText.hpp"

#include <core/com/Slots.hxx>

#include <data/GenericFieldBase.hpp>
#include <data/tools/Color.hpp>

#include <viz/scene3d/helper/Font.hpp>
#include <viz/scene3d/IWindowInteractor.hpp>
#include <viz/scene3d/SRender.hpp>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::Slots::SlotKeyType s_SET_TEXT_SLOT = "setText";

static const std::string s_TEXT_CONFIG        = "text";
static const std::string s_FONT_SIZE_CONFIG   = "fontSize";
static const std::string s_FONT_SOURCE_CONFIG = "fontSource";
static const std::string s_H_ALIGN_CONFIG     = "hAlign";
static const std::string s_V_ALIGN_CONFIG     = "vAlign";
static const std::string s_X_CONFIG           = "x";
static const std::string s_Y_CONFIG           = "y";
static const std::string s_COLOR_CONFIG       = "color";

//----------------------------------------------------------------------------

SText::SText() noexcept
{
    newSlot(s_SET_TEXT_SLOT, &SText::setText, this);
}

//----------------------------------------------------------------------------

SText::~SText() noexcept
{
}

//----------------------------------------------------------------------------

void SText::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

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

    const auto hexaTextColor = config.get<std::string>(s_COLOR_CONFIG, "#FFFFFF");
    std::array<std::uint8_t, 4> textColor;
    data::tools::Color::hexaStringToRGBA(hexaTextColor, textColor.data());

    const auto divideBy255 = std::bind(std::divides<float>(), std::placeholders::_1, 255.f);
    std::transform(textColor.begin(), textColor.end(), m_textColor.ptr(), divideBy255);
}

//----------------------------------------------------------------------------

void SText::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    Ogre::OverlayContainer* textContainer = this->getLayer()->getOverlayTextPanel();

    const float dpi = this->getRenderService()->getInteractorManager()->getLogicalDotsPerInch();

    m_text = sight::viz::scene3d::Text::New(
        this->getID() + "_text",
        this->getSceneManager(),
        textContainer,
        m_fontSource,
        m_fontSize,
        dpi,
        this->getLayer()->getDefaultCamera()
    );

    m_text->setTextColor(m_textColor);

    this->updateText();
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SText::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_OBJECT_INPUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//----------------------------------------------------------------------------

void SText::updating()
{
    this->getRenderService()->makeCurrent();
    this->updateText();
}

//----------------------------------------------------------------------------

void SText::stopping()
{
    this->getRenderService()->makeCurrent();

    Ogre::SceneManager* const sm = this->getLayer()->getSceneManager();
    m_text->detachFromParent();
    sm->destroyMovableObject(m_text);
    m_text = nullptr;
}

//----------------------------------------------------------------------------

void SText::setText(std::string str)
{
    this->getRenderService()->makeCurrent();

    m_textString = str;
    m_text->setText(str);
    this->updatePositionFromAlignment();

    this->requestRender();
}

//----------------------------------------------------------------------------

void SText::updatePositionFromAlignment()
{
    const std::map<std::string, Ogre::TextAreaOverlayElement::Alignment> stringToHorizAlignmentMap {
        {"left", Ogre::TextAreaOverlayElement::Left},
        {"center", Ogre::TextAreaOverlayElement::Center},
        {"right", Ogre::TextAreaOverlayElement::Right}
    };

    const std::map<std::string, float> horizAlignToX {
        {"left", m_position.x},
        {"center", 0.5f + m_position.x},
        {"right", 1.f - m_position.x}
    };

    const std::map<std::string, Ogre::GuiVerticalAlignment> stringToVertAlignmentMap {
        {"bottom", Ogre::GVA_BOTTOM},
        {"center", Ogre::GVA_CENTER},
        {"top", Ogre::GVA_TOP}
    };

    const std::map<std::string, float> vertAlignToY {
        {"bottom", 1.f - m_position.y},
        {"center", 0.5f + m_position.y},
        {"top", m_position.y}
    };

    const auto hAlign = stringToHorizAlignmentMap.at(m_horizontalAlignment);
    const auto vAlign = stringToVertAlignmentMap.at(m_verticalAlignment);
    const float x     = horizAlignToX.at(m_horizontalAlignment);
    const float y     = vertAlignToY.at(m_verticalAlignment);

    m_text->setTextAlignment(hAlign, vAlign);
    m_text->setPosition(x, y);
}

//----------------------------------------------------------------------------

void SText::updateText()
{
    std::string textString = m_textString;

    const auto obj = m_object.lock();

    if(obj)
    {
        const data::GenericFieldBase::csptr field = data::GenericFieldBase::dynamicCast(obj.get_shared());

        if(field)
        {
            textString = field->toString();
        }
    }

    this->setText(textString);
}

} // namespace sight::module::viz::scene3d::adaptor.
