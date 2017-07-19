/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SText.hpp"

#include <fwServices/macros.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwData/GenericFieldBase.hpp>
#include <fwData/Color.hpp>

#include <boost/lexical_cast.hpp>

#include <vtkActor2D.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

fwServicesRegisterMacro(::fwRenderVTK::IVtkAdaptorService, ::visuVTKARAdaptor::SText, ::fwData::Object);

namespace visuVTKARAdaptor
{

SText::SText() :
    m_actor(vtkActor2D::New()),
    m_mapper(vtkTextMapper::New())
{
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

void SText::doConfigure()
{
    SLM_ASSERT("Required 'config' element is missing.", m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );

    m_seshatPath = m_configuration->getAttributeValue("seshat");

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    std::vector < ConfigurationType > vectStyle = m_configuration->find("style");
    if(!vectStyle.empty())
    {
        ConfigurationType styleConfiguration = vectStyle.at(0);

        if(styleConfiguration->hasConfigurationElement("fontSize"))
        {
            ConfigurationType fontSizeConfig = styleConfiguration->findConfigurationElement("fontSize");
            m_textProperty.m_fontSize = ::boost::lexical_cast< size_t >(fontSizeConfig->getValue());
        }
        if(styleConfiguration->hasConfigurationElement("fontFamily"))
        {
            ConfigurationType fontFamilyConfig = styleConfiguration->findConfigurationElement("fontFamily");
            const std::string fontFamily       = fontFamilyConfig->getValue();
            SLM_ASSERT("'hAlign' value must be 'left' or 'right'",
                       fontFamily == "ARIAL" ||
                       fontFamily == "COURIER" ||
                       fontFamily == "TIMES");
            if(fontFamily == "TIMES")
            {
                m_textProperty.m_fontFamily = TIMES;
            }
            else if(fontFamily == "COURIER")
            {
                m_textProperty.m_fontFamily = COURIER;
            }
            else
            {
                m_textProperty.m_fontFamily = ARIAL;
            }
        }

        if(styleConfiguration->hasConfigurationElement("hAlign"))
        {
            ConfigurationType hAlignConfig = styleConfiguration->findConfigurationElement("hAlign");
            const std::string hAlign       = hAlignConfig->getValue();
            SLM_ASSERT("'hAlign' value must be 'left' or 'right'",
                       hAlign == "left" ||
                       hAlign == "centered" ||
                       hAlign == "right");
            if(hAlign == "right")
            {
                m_textProperty.m_hAlign = RIGHT;
            }
            else if(hAlign == "centered")
            {
                m_textProperty.m_hAlign = HCENTERED;
            }
            else
            {
                m_textProperty.m_hAlign = LEFT;
            }
        }

        if(styleConfiguration->hasConfigurationElement("vAlign"))
        {
            ConfigurationType vAlignConfig = styleConfiguration->findConfigurationElement("vAlign");
            const std::string vAlign       = vAlignConfig->getValue();
            SLM_ASSERT("'vAlign' value must be 'left' or 'right'",
                       vAlign == "top" ||
                       vAlign == "centered" ||
                       vAlign == "bottom");
            if(vAlign == "top")
            {
                m_textProperty.m_vAlign = TOP;
            }
            else if(vAlign == "centered")
            {
                m_textProperty.m_vAlign = VCENTERED;
            }
            else
            {
                m_textProperty.m_vAlign = BOTTOM;
            }
        }

        if(styleConfiguration->hasConfigurationElement("color"))
        {
            ConfigurationType colorConfig = styleConfiguration->findConfigurationElement("color");
            const std::string colorText   = colorConfig->getValue();
            ::fwData::Color::sptr color = ::fwData::Color::New();
            color->setRGBA(colorText);
            m_textProperty.m_color[0] = color->getRefRGBA()[0];
            m_textProperty.m_color[1] = color->getRefRGBA()[1];
            m_textProperty.m_color[2] = color->getRefRGBA()[2];
        }

        if(styleConfiguration->hasConfigurationElement("italic"))
        {
            ConfigurationType italicConfig = styleConfiguration->findConfigurationElement("italic");
            const std::string italic       = italicConfig->getValue();
            SLM_ASSERT("'italic' value must be 'true' or 'false'", italic == "true" || italic == "false");
            m_textProperty.m_italic = (italic == "true");
        }

        if(styleConfiguration->hasConfigurationElement("bold"))
        {
            ConfigurationType boldConfig = styleConfiguration->findConfigurationElement("bold");
            const std::string bold       = boldConfig->getValue();
            SLM_ASSERT("'bold' value must be 'true' or 'false'", bold == "true" || bold == "false");
            m_textProperty.m_bold = (bold == "true");
        }

        if(styleConfiguration->hasConfigurationElement("shadow"))
        {
            ConfigurationType shadowConfig = styleConfiguration->findConfigurationElement("shadow");
            const std::string shadow       = shadowConfig->getValue();
            SLM_ASSERT("'shadow' value must be 'true' or 'false'", shadow == "true" || shadow == "false");
            m_textProperty.m_shadow = (shadow == "true");
        }

        if(styleConfiguration->hasConfigurationElement("opacity"))
        {
            ConfigurationType opacityConfig = styleConfiguration->findConfigurationElement("opacity");
            const std::string opacity       = opacityConfig->getValue();
            m_textProperty.m_opacity = ::boost::lexical_cast< double >(opacity);
        }
    }
}

//-----------------------------------------------------------------------------

void SText::doStart()
{
    this->addToRenderer(m_actor);
    this->doUpdate();
}

//-----------------------------------------------------------------------------

void SText::doStop()
{
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void SText::doSwap()
{
    this->doStop();
    this->doStart();
}

//-----------------------------------------------------------------------------

void SText::doUpdate()
{
    this->updateStyle();
    this->updateText();
}

//-----------------------------------------------------------------------------

void SText::updateStyle()
{
    vtkTextProperty* textprop = m_mapper->GetTextProperty();

    textprop->SetOpacity(m_textProperty.m_opacity);
    textprop->SetFontSize(::boost::numeric_cast<int>(m_textProperty.m_fontSize));
    textprop->SetBold(m_textProperty.m_bold);
    textprop->SetItalic(m_textProperty.m_italic);
    textprop->SetShadow(m_textProperty.m_shadow);
    textprop->SetColor(m_textProperty.m_color);

    switch(m_textProperty.m_fontFamily)
    {
        case ARIAL:
            textprop->SetFontFamilyToArial();
            break;
        case COURIER:
            textprop->SetFontFamilyToCourier();
            break;
        case TIMES:
            textprop->SetFontFamilyToTimes();
            break;
    }

    double x = 0., y = 0.;
    switch(m_textProperty.m_vAlign)
    {
        case TOP:
            textprop->SetVerticalJustificationToTop();
            y = 0.98;
            break;
        case BOTTOM:
            textprop->SetVerticalJustificationToBottom();
            y = 0.02;
            break;
        case VCENTERED:
            textprop->SetVerticalJustificationToCentered();
            y = 0.5;
            break;
    }

    switch(m_textProperty.m_hAlign)
    {
        case LEFT:
            textprop->SetJustificationToLeft();
            x = 0.001;
            break;
        case RIGHT:
            textprop->SetJustificationToRight();
            x = 0.99;
            break;
        case HCENTERED:
            textprop->SetJustificationToCentered();
            x = 0.5;
            break;
    }

    m_actor->SetPosition(x, y);
}

//-----------------------------------------------------------------------------

void SText::updateText()
{
    ::fwData::Object::sptr obj = this->getObject();
    ::fwData::GenericFieldBase::sptr field;

    if(!m_seshatPath.empty())
    {
        field = ::fwDataCamp::getObject< ::fwData::GenericFieldBase >(obj, m_seshatPath);
    }
    else
    {
        field = ::fwData::GenericFieldBase::dynamicCast(obj);
    }

    SLM_ASSERT("Seshat path or fwData::Object can't be cast to generic field", field);
    if(field)
    {
        m_mapper->SetInput(field->toString().c_str());
        this->setVtkPipelineModified();
    }
}

//-----------------------------------------------------------------------------

} //namespace visuVTKARAdaptor

