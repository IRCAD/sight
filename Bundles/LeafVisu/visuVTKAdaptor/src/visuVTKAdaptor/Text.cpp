/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/Text.hpp"

#include <fwDataCamp/getObject.hpp>

#include <fwServices/macros.hpp>

#include <fwData/GenericFieldBase.hpp>
#include <fwData/Color.hpp>

#include <boost/lexical_cast.hpp>

#include <vtkActor2D.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

fwServicesRegisterMacro(::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Text, ::fwData::Object);

namespace visuVTKAdaptor
{

Text::Text() : m_fontSize(20), m_vAlign("bottom"), m_hAlign("left")
{
    m_mapper = vtkTextMapper::New();
    m_mapper->GetTextProperty()->SetFontFamilyToCourier(); // Fixed-width font
    m_mapper->GetTextProperty()->ShadowOn(); // better contrast
    m_mapper->GetTextProperty()->BoldOn();

    m_actor = vtkActor2D::New();
    m_actor->SetMapper(m_mapper);
    m_actor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    m_actor->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
}

//-----------------------------------------------------------------------------

Text::~Text() throw()
{
    m_actor->Delete();
    m_actor = 0;

    m_mapper->Delete();
    m_mapper = 0;
}

//-----------------------------------------------------------------------------

void Text::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );

    std::string text = m_configuration->getAttributeValue("text");
    if(!text.empty() && text[0] == '@')
    {
        ::fwData::Object::sptr obj = this->getObject();
        ::fwData::GenericFieldBase::sptr field = ::fwDataCamp::getObject< ::fwData::GenericFieldBase >(obj, text);
        SLM_ASSERT("Seshat path can't be cast to generic field", field);
        if(field)
        {
            text = field->toString();
        }
    }

    if(m_configuration->hasAttribute("fontSize"))
    {
        m_fontSize = ::boost::lexical_cast< unsigned int >(m_configuration->getAttributeValue("fontSize"));
    }

    m_mapper->GetTextProperty()->SetFontSize(m_fontSize);

    if(m_configuration->hasAttribute("hAlign"))
    {
        m_hAlign = m_configuration->getAttributeValue("hAlign");
        SLM_ASSERT("'hAlign' value must be 'left' or 'right'", m_hAlign == "left" || m_hAlign == "right");
    }

    if(m_configuration->hasAttribute("vAlign"))
    {
        m_vAlign = m_configuration->getAttributeValue("vAlign");
        SLM_ASSERT("'vAlign' value must be 'top' or 'bottom'", m_vAlign == "top" || m_vAlign == "bottom");
    }

    if( m_configuration->hasAttribute("color") )
    {
        std::string colorText = m_configuration->getAttributeValue("color");
        if( colorText[0] == '#')
        {
            ::fwData::Color::sptr color = ::fwData::Color::New();
            color->setRGBA(colorText);
            m_mapper->GetTextProperty()->SetColor(color->getRefRGBA()[0], color->getRefRGBA()[1], color->getRefRGBA()[2]);
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

void Text::doStart() throw(fwTools::Failed)
{
    this->addToRenderer(m_actor);
}

//-----------------------------------------------------------------------------

void Text::doStop() throw(fwTools::Failed)
{
    this->removeAllPropFromRenderer();
}

//-----------------------------------------------------------------------------

void Text::setAlignment()
{
    vtkTextProperty* textprop = m_mapper->GetTextProperty();

    if(m_hAlign == "left")
    {
        textprop->SetJustificationToLeft();

        if(m_vAlign == "top")
        {
            textprop->SetVerticalJustificationToTop();
            m_actor->SetPosition(0.001, 0.98);
        }
        else
        {
            textprop->SetVerticalJustificationToBottom();
            m_actor->SetPosition(0.001,0.02);
        }
    }
    else
    {
        textprop->SetJustificationToRight();

        if(m_vAlign == "top")
        {
            textprop->SetVerticalJustificationToTop();
            m_actor->SetPosition(0.99, 0.98);
        }
        else
        {
            textprop->SetVerticalJustificationToBottom();
            m_actor->SetPosition(0.99,0.02);
        }
    }
}

//-----------------------------------------------------------------------------

void Text::setText(std::string str)
{
    m_text = str;
    m_mapper->SetInput(m_text.c_str());
    this->setAlignment();
    this->setVtkPipelineModified();
}

//-----------------------------------------------------------------------------

} //namespace visuVTKAdaptor

