/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SText.hpp"

#include <fwData/Color.hpp>
#include <fwData/GenericFieldBase.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwServices/macros.hpp>

#include <boost/lexical_cast.hpp>

#include <vtkActor2D.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

fwServicesRegisterMacro(::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SText);

namespace visuVTKAdaptor
{

SText::SText() :
    m_actor(vtkActor2D::New()),
    m_mapper(vtkTextMapper::New()),
    m_fontSize(20),
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

    const ConfigType srvconfig = this->getConfigTree().get_child("service");
    const ConfigType config    = srvconfig.get_child("config.<xmlattr>");

    std::string text = config.get<std::string>("text", "");
    if(!text.empty() && text[0] == '@')
    {
        OSLM_ASSERT("You shall set text attribute or <text> tag, not both", srvconfig.count("text") == 0);
        ::fwData::Object::csptr obj            = this->getInOut< ::fwData::Object >("object");
        ::fwData::GenericFieldBase::sptr field = ::fwDataCamp::getObject< ::fwData::GenericFieldBase >(obj, text);
        SLM_ASSERT("Camp path can't be cast to generic field", field);
        if(field)
        {
            text = field->toString();
        }
    }

    if(text.empty())
    {
        SLM_ASSERT("<text> tag must be defined.", srvconfig.count("text"));
        text = srvconfig.get<std::string>("text");
    }

    m_text = text;

    m_fontSize = config.get<unsigned int>("fontSize", 20);

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

    m_mapper->GetTextProperty()->SetFontSize( int(m_fontSize) );

    if( m_textColor[0] == '#')
    {
        ::fwData::Color::sptr color = ::fwData::Color::New();
        color->setRGBA(m_textColor);
        m_mapper->GetTextProperty()->SetColor(color->getRefRGBA()[0], color->getRefRGBA()[1],
                                              color->getRefRGBA()[2]);
    }
    else
    {
        // compatibility with "old" color
        double color = ::boost::lexical_cast<double> (m_textColor);
        m_mapper->GetTextProperty()->SetColor(color, color, color);
    }

    this->setText(m_text);

    this->addToRenderer(m_actor);
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

void SText::setText(const std::string& str)
{
    m_text = str;
    m_mapper->SetInput(m_text.c_str());
    this->setAlignment();
    this->setVtkPipelineModified();
}

//-----------------------------------------------------------------------------

} //namespace visuVTKAdaptor

