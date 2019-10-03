/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "visuOgreAdaptor/SText.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/GenericFieldBase.hpp>

#include <fwDataTools/Color.hpp>

#include <fwRenderOgre/helper/Font.hpp>
#include <fwRenderOgre/IRenderWindowInteractorManager.hpp>
#include <fwRenderOgre/SRender.hpp>

namespace visuOgreAdaptor
{

static const ::fwServices::IService::KeyType s_OBJECT_INPUT = "object";

static const ::fwCom::Slots::SlotKeyType s_SET_TEXT_SLOT = "setText";

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

    const ConfigType srvconfig = this->getConfigTree();
    const ConfigType config    = srvconfig.get_child("config.<xmlattr>");

    m_textString = srvconfig.get<std::string>("text", "");

    m_fontSource = config.get("fontSource", m_fontSource);
    m_fontSize   = config.get<size_t>("fontSize", m_fontSize);

    m_horizontalAlignment = config.get<std::string>("hAlign", "left");
    SLM_ASSERT("'hAlign' must be 'left', 'center' or 'right'",
               m_horizontalAlignment == "left"
               || m_horizontalAlignment == "center"
               || m_horizontalAlignment == "right" );

    m_verticalAlignment = config.get<std::string>("vAlign", "bottom");
    SLM_ASSERT("'vAlign' must be 'top', 'center' or 'bottom'",
               m_verticalAlignment == "top"
               || m_verticalAlignment == "center"
               || m_verticalAlignment == "bottom");

    m_position.x = config.get<float>("x", m_position.x);
    m_position.y = config.get<float>("y", m_position.y);

    const auto hexaTextColor = config.get<std::string>("color", "#ffffff");
    std::array< std::uint8_t, 4 > textColor;
    ::fwDataTools::Color::hexaStringToRGBA(hexaTextColor, textColor.data());

    const auto divideBy255 = std::bind(std::divides<float>(), std::placeholders::_1, 255.f);
    std::transform(textColor.begin(), textColor.end(), m_textColor.ptr(), divideBy255);
}

//----------------------------------------------------------------------------

void SText::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    ::Ogre::OverlayContainer* textContainer = this->getLayer()->getOverlayTextPanel();

    const float dpi = this->getRenderService()->getInteractorManager()->getLogicalDotsPerInch();

    m_text = ::fwRenderOgre::Text::New(this->getID() + "_text",
                                       this->getSceneManager(),
                                       textContainer,
                                       m_fontSource, m_fontSize, dpi,
                                       this->getLayer()->getDefaultCamera());

    m_text->setTextColor(m_textColor);

    this->updateText();
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

    ::Ogre::SceneManager* const sm = this->getLayer()->getSceneManager();
    m_text->detachFromParent();
    sm->destroyMovableObject(m_text);
    m_text = nullptr;
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SText::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_OBJECT_INPUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//----------------------------------------------------------------------------

void SText::setText(std::string str)
{
    this->getRenderService()->makeCurrent();

    m_textString = str;
    m_text->setText(str);
    this->updatePositionFromAlignment();
}

//----------------------------------------------------------------------------

void SText::updatePositionFromAlignment()
{
    const float textHeight = m_text->getTextHeight();

    const std::map< std::string, ::Ogre::TextAreaOverlayElement::Alignment > stringToHorizAlignmentMap {
        { "left",  ::Ogre::TextAreaOverlayElement::Left },
        { "center", ::Ogre::TextAreaOverlayElement::Center },
        { "right", ::Ogre::TextAreaOverlayElement::Right }
    };

    const std::map< std::string, float > horizAlignToX {
        { "left", m_position.x },
        { "center", 0.5f + m_position.x },
        { "right", 1.f - m_position.x }
    };

    const std::map< std::string, ::Ogre::GuiVerticalAlignment > stringToVertAlignmentMap {
        { "bottom", ::Ogre::GVA_BOTTOM },
        { "center", ::Ogre::GVA_CENTER },
        { "top", ::Ogre::GVA_TOP }
    };

    const std::map< std::string, float > vertAlignToY {
        { "bottom", 1.f - m_position.y },
        { "center", 0.5f + m_position.y },
        { "top", m_position.y }
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
    ::fwData::Object::csptr obj = this->getInput< ::fwData::Object >(s_OBJECT_INPUT);
    std::string textString = m_textString;

    if(obj)
    {
        const ::fwData::GenericFieldBase::csptr field = ::fwData::GenericFieldBase::dynamicCast(obj);

        if(field)
        {
            textString = field->toString();
        }
    }

    this->setText(textString);
}

} // namespace visuOgreAdaptor