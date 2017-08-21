/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SText.hpp"

#include <fwData/Color.hpp>
#include <fwData/GenericFieldBase.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwServices/macros.hpp>

#include <vtkActor2D.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

fwServicesRegisterMacro(::fwRenderVTK::IAdaptor, ::visuVTKARAdaptor::SText, ::fwData::Object);

namespace visuVTKARAdaptor
{

static const ::fwServices::IService::KeyType s_OBJECT_IN = "object";

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

void SText::configuring()
{
    this->configureParams();

    m_seshatPath = this->getConfigTree().get<std::string>("service.config.<xmlattr>.seshat", "");

    const ConfigType style = this->getConfigTree().get_child("service.config.style");

    m_textProperty.m_fontSize = style.get<size_t>("fontSize");

    const std::string fontFamily = style.get<std::string>("fontFamily");
    SLM_ASSERT("'fontFamily' value must be 'ARIAL', 'COURIER' or 'TIMES'",
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

    const std::string hAlign = style.get<std::string>("hAlign");
    SLM_ASSERT("'hAlign' value must be 'left', 'centered' or 'right'",
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

    const std::string vAlign = style.get<std::string>("vAlign");
    SLM_ASSERT("'vAlign' value must be 'top', 'centered' or 'bottom'",
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

    const std::string colorText = style.get<std::string>("color");
    ::fwData::Color::sptr color = ::fwData::Color::New();
    color->setRGBA(colorText);

    const ::fwData::Color::ColorArray& rgbaArray = color->getRefRGBA();
    std::copy(rgbaArray.begin(), --rgbaArray.end(), std::begin(m_textProperty.m_color));

    m_textProperty.m_italic  = style.get<bool>("italic");
    m_textProperty.m_bold    = style.get<bool>("bold");
    m_textProperty.m_shadow  = style.get<bool>("shadow");
    m_textProperty.m_opacity = style.get<double>("opacity");
}

//-----------------------------------------------------------------------------

void SText::starting()
{
    this->initialize();

    this->addToRenderer(m_actor);
    this->updating();
}

//-----------------------------------------------------------------------------

void SText::stopping()
{
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void SText::swapping()
{
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

void SText::updating()
{
    this->updateStyle();
    this->updateText();

    this->requestRender();
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
    ::fwData::Object::csptr obj = this->getInput< ::fwData::Object >(s_OBJECT_IN);
    ::fwData::GenericFieldBase::csptr field;

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

