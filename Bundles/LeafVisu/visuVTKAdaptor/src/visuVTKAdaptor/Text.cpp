/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>

#include <fwData/Color.hpp>

#include <vtkRenderer.h>
#include <vtkActor2D.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

#include "visuVTKAdaptor/Text.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Text, ::fwTools::Object ) ;

namespace visuVTKAdaptor
{

Text::Text() throw()
{
    m_mapper = vtkTextMapper::New();
//    m_mapper->GetTextProperty()->SetFontFamilyToArial();
    m_mapper->GetTextProperty()->SetFontFamilyToCourier(); // Fixed-width font
    m_mapper->GetTextProperty()->SetFontSize(20);
    m_mapper->GetTextProperty()->ShadowOn(); // better constast
    m_mapper->GetTextProperty()->BoldOn();
//    m_mapper->GetTextProperty()->ItalicOn();

    m_actor = vtkActor2D::New();
    m_actor->SetMapper(m_mapper);
    m_actor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    m_actor->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
    m_actor->SetPosition(0.001,0.02);
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
    this->setText( m_configuration->getAttributeValue("text") );

    if( m_configuration->hasAttribute("color") )
    {
        std::string colorText = m_configuration->getAttributeValue("color");
        if( colorText[0] == '#')
        {
            ::fwData::Color color;
            color.setRGBA(colorText);
            m_mapper->GetTextProperty()->SetColor(color.getRefRGBA()[0], color.getRefRGBA()[1], color.getRefRGBA()[2]);
        }
        else
        {
            // compatibility with "old" color
            double color = ::boost::lexical_cast<double> (colorText);
            m_mapper->GetTextProperty()->SetColor(color, color, color);
        }
    }
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

void Text::setText(std::string str)
{
    m_text = str;
    m_mapper->SetInput(m_text.c_str());
    this->setVtkPipelineModified();
}

//-----------------------------------------------------------------------------


} //namespace visuVTKAdaptor
