/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "visuVTKAdaptor/SText.hpp"

#include <fwData/Color.hpp>
#include <fwData/GenericFieldBase.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwServices/macros.hpp>

#include <vtkActor2D.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

fwServicesRegisterMacro(::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SText);

namespace visuVTKAdaptor
{

static const ::fwServices::IService::KeyType s_OBJECT_IN = "object";

SText::SText() :
    m_actor(vtkActor2D::New()),
    m_mapper(vtkTextMapper::New()),
    m_fontSize(20),
    m_fontFamily("courier"),
    m_italic(false),
    m_bold(false),
    m_shadow(false),
    m_vAlign("bottom"),
    m_hAlign("left"),
    m_textColor("#ffffffff")
{
    m_mapper->GetTextProperty()->SetFontFamilyToCourier(); // Fixed-width font
    m_mapper->GetTextProperty()->ShadowOn(); // better contrast
    m_mapper->GetTextProperty()->BoldOn();

    m_actor->SetMapper(m_mapper);
    m_actor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    m_actor->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
}

//-----------------------------------------------------------------------------

SText::~SText() noexcept
{
    m_actor->Delete();
    m_actor = nullptr;

    m_mapper->Delete();
    m_mapper = nullptr;
}

//-----------------------------------------------------------------------------

void SText::configuring()
{
    this->configureParams();

    const ConfigType srvconfig = this->getConfigTree();
    const ConfigType config    = srvconfig.get_child("config.<xmlattr>");

    std::string text = config.get<std::string>("text", "");

    if(text.empty())
    {
        text = srvconfig.get<std::string>("text", "");
    }
    else if(text[0] == '@')
    {
        m_path = text;
    }

    if(m_path.empty())
    {
        m_text = text;
    }

    m_fontSize   = config.get<unsigned int>("fontSize", 20);
    m_fontFamily = config.get<std::string>("fontFamily", "courier");

    m_italic = config.get<bool>("italic", false);
    m_bold   = config.get<bool>("bold", false);
    m_shadow = config.get<bool>("shadow", false);

    m_hAlign = config.get<std::string>("hAlign", "left");
    SLM_ASSERT("'hAlign' value must be 'left', 'center' or 'right'",
               m_hAlign == "left"
               || m_hAlign == "center"
               || m_hAlign == "right"
               );

    m_vAlign = config.get<std::string>("vAlign", "bottom");
    SLM_ASSERT("'vAlign' value must be 'top', 'center' or 'bottom'",
               m_vAlign == "top"
               || m_hAlign == "center"
               || m_vAlign == "bottom");

    m_textColor = config.get<std::string>("color", "#ffffffff");
}

//-----------------------------------------------------------------------------

void SText::starting()
{
    this->initialize();

    this->updateText();
    this->setStyle();
    this->setText(m_text);

    this->addToRenderer(m_actor);
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SText::updating()
{
    this->updateText();
    this->setText(m_text);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SText::stopping()
{
    this->removeAllPropFromRenderer();
}

//-----------------------------------------------------------------------------

void SText::setAlignment()
{
    vtkTextProperty* textprop = m_mapper->GetTextProperty();

    double x = 0.5;
    double y = 0.5;

    textprop->SetJustificationToCentered();
    textprop->SetVerticalJustificationToCentered();

    if(m_hAlign == "left")
    {
        textprop->SetJustificationToLeft();
        x = 0.001;
    }
    else if(m_hAlign == "right")
    {
        textprop->SetJustificationToRight();
        x = 0.99;
    }

    if(m_vAlign == "top")
    {
        textprop->SetVerticalJustificationToTop();
        y = 0.98;
    }
    else if(m_vAlign == "bottom")
    {
        textprop->SetVerticalJustificationToBottom();
        y = 0.02;
    }

    m_actor->SetPosition(x, y);
}

//-----------------------------------------------------------------------------

void SText::setStyle()
{
    vtkTextProperty* textprop = m_mapper->GetTextProperty();

    textprop->SetFontSize( int(m_fontSize) );
    textprop->SetItalic(m_italic);
    textprop->SetBold(m_bold);
    textprop->SetShadow(m_shadow);

    if( m_textColor[0] == '#')
    {
        ::fwData::Color::sptr color = ::fwData::Color::New();
        color->setRGBA(m_textColor);
        textprop->SetColor(color->getRGBA()[0], color->getRGBA()[1], color->getRGBA()[2]);
    }
    else
    {
        // compatibility with "old" color
        double color = std::stod(m_textColor);
        textprop->SetColor(color, color, color);
    }

    if(m_fontFamily == "arial")
    {
        textprop->SetFontFamilyToArial();
    }
    else if(m_fontFamily == "courier")
    {
        textprop->SetFontFamilyToCourier();
    }
    else if(m_fontFamily == "times")
    {
        textprop->SetFontFamilyToTimes();
    }
    else
    {
        OSLM_FATAL("Unknown font family : '" << m_fontFamily << "'");
    }
}

//-----------------------------------------------------------------------------

void SText::updateText()
{
    ::fwData::Object::csptr obj = this->getInput< ::fwData::Object >(s_OBJECT_IN);

    if(obj)
    {
        ::fwData::GenericFieldBase::csptr field;

        if(m_path.empty())
        {
            field = ::fwData::GenericFieldBase::dynamicCast(obj);
        }
        else
        {
            field = ::fwDataCamp::getObject< ::fwData::GenericFieldBase >(obj, m_path);
        }

        if(field)
        {
            m_text = field->toString();
        }
    }
}

//-----------------------------------------------------------------------------

void SText::setText(const std::string& str)
{
    m_text = str;
    m_mapper->SetInput(m_text.c_str());
    this->setAlignment();
    this->setVtkPipelineModified();
}

//-----------------------------------------------------------------------------

fwServices::IService::KeyConnectionsMap SText::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_OBJECT_IN, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

} //namespace visuVTKAdaptor
