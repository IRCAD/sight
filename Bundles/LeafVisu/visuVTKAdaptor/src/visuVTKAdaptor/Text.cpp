/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/Text.hpp"

#include <fwData/Color.hpp>
#include <fwData/GenericFieldBase.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwServices/macros.hpp>

#include <boost/lexical_cast.hpp>

#include <vtkActor2D.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

fwServicesRegisterMacro(::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::Text, ::fwData::Object);

namespace visuVTKAdaptor
{

Text::Text() :
    m_actor(vtkActor2D::New()),
    m_mapper(vtkTextMapper::New()),
    m_fontSize(20),
    m_vAlign("bottom"),
    m_hAlign("left")
{
    m_mapper->GetTextProperty()->SetFontFamilyToCourier(); // Fixed-width font
    m_mapper->GetTextProperty()->ShadowOn(); // better contrast
    m_mapper->GetTextProperty()->BoldOn();

    m_actor->SetMapper(m_mapper);
    m_actor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    m_actor->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
}

//-----------------------------------------------------------------------------

Text::~Text() noexcept
{
    m_actor->Delete();
    m_actor = nullptr;

    m_mapper->Delete();
    m_mapper = nullptr;
}

//-----------------------------------------------------------------------------

void Text::doConfigure()
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");

    std::string text = m_configuration->getAttributeValue("text");
    if(!text.empty() && text[0] == '@')
    {
        OSLM_ASSERT("You shall set text attribute or <text> tag, not both", m_configuration->find("text").empty());
        ::fwData::Object::sptr obj             = this->getObject();
        ::fwData::GenericFieldBase::sptr field = ::fwDataCamp::getObject< ::fwData::GenericFieldBase >(obj, text);
        SLM_ASSERT("Seshat path can't be cast to generic field", field);
        if(field)
        {
            text = field->toString();
        }
    }

    if(text.empty())
    {
        auto textVector = m_configuration->find("text");
        if(!textVector.empty())
        {
            OSLM_ASSERT("<text> tag shall be set at most once", textVector.size() <= 1);
            auto item = textVector[0];
            text = item->getValue();
        }

    }

    if(m_configuration->hasAttribute("fontSize"))
    {
        m_fontSize = ::boost::lexical_cast< unsigned int >(m_configuration->getAttributeValue("fontSize"));
    }

    m_mapper->GetTextProperty()->SetFontSize( int(m_fontSize) );

    if(m_configuration->hasAttribute("hAlign"))
    {
        m_hAlign = m_configuration->getAttributeValue("hAlign");
        SLM_ASSERT("'hAlign' value must be 'left', 'center' or 'right'",
                   m_hAlign == "left"
                   || m_hAlign == "center"
                   || m_hAlign == "right"
                   );
    }

    if(m_configuration->hasAttribute("vAlign"))
    {
        m_vAlign = m_configuration->getAttributeValue("vAlign");
        SLM_ASSERT("'vAlign' value must be 'top', 'center' or 'bottom'",
                   m_vAlign == "top"
                   || m_hAlign == "center"
                   || m_vAlign == "bottom");
    }

    if( m_configuration->hasAttribute("color") )
    {
        std::string colorText = m_configuration->getAttributeValue("color");
        if( colorText[0] == '#')
        {
            ::fwData::Color::sptr color = ::fwData::Color::New();
            color->setRGBA(colorText);
            m_mapper->GetTextProperty()->SetColor(color->getRefRGBA()[0], color->getRefRGBA()[1],
                                                  color->getRefRGBA()[2]);
        }
        else
        {
            // compatibility with "old" color
            double color = ::boost::lexical_cast<double> (colorText);
            m_mapper->GetTextProperty()->SetColor(color, color, color);
        }
    }

    this->setText(text);
}

//-----------------------------------------------------------------------------

void Text::doStart()
{
    this->addToRenderer(m_actor);
}

//-----------------------------------------------------------------------------

void Text::doStop()
{
    this->removeAllPropFromRenderer();
}

//-----------------------------------------------------------------------------

void Text::setAlignment()
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

void Text::setText(const std::string& str)
{
    m_text = str;
    m_mapper->SetInput(m_text.c_str());
    this->setAlignment();
    this->setVtkPipelineModified();
}

//-----------------------------------------------------------------------------

} //namespace visuVTKAdaptor

