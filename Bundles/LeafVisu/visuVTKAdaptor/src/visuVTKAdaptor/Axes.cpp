/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/Axes.hpp"

#include <fwCom/Slots.hxx>
#include <fwServices/macros.hpp>

#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkProp3D.h>
#include <vtkCommand.h>

#include <boost/lexical_cast.hpp>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Axes, ::fwData::Object );

namespace visuVTKAdaptor
{
//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType Axes::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";

//------------------------------------------------------------------------------

Axes::Axes() throw() :
    m_axesActor(fwVtkAxesActor::New()),
    m_length(1.),
    m_labelOn(true),
    m_transformAxes(vtkTransform::New()),
    m_xLabel("x"),
    m_yLabel("y"),
    m_zLabel("z")

{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &Axes::updateVisibility, this);
}

//------------------------------------------------------------------------------

Axes::~Axes() throw()
{
    m_axesActor->Delete();
    m_axesActor = 0;
}

//------------------------------------------------------------------------------

void Axes::doStart() throw(fwTools::Failed)
{
    this->buildPipeline();
    this->addToRenderer( m_axesActor );
}

//------------------------------------------------------------------------------

void Axes::doStop() throw(fwTools::Failed)
{
    this->removeAllPropFromRenderer();
    this->getRenderer()->RemoveActor(m_axesActor);
}

//------------------------------------------------------------------------------

void Axes::doSwap() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void Axes::doUpdate() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void Axes::doConfigure() throw(fwTools::Failed)
{
    assert( m_configuration->getName() == "config" );

    if ( m_configuration->hasAttribute( "length" ) )
    {
        m_length = boost::lexical_cast<double>( m_configuration->getAttributeValue( "length" ) );
    }
    if ( m_configuration->hasAttribute( "label" ) )
    {
        std::string value = m_configuration->getAttributeValue( "label" );
        std::transform( value.begin(), value.end(), value.begin(), tolower );
        m_labelOn = ( value == "yes" );
    }
    if(m_configuration->hasAttribute( "xLabel" ))
    {
        m_xLabel = m_configuration->getAttributeValue( "xLabel" );
    }
    if(m_configuration->hasAttribute( "yLabel" ))
    {
        m_yLabel = m_configuration->getAttributeValue( "yLabel" );
    }
    if(m_configuration->hasAttribute( "zLabel" ))
    {
        m_zLabel = m_configuration->getAttributeValue( "zLabel" );
    }
}

//------------------------------------------------------------------------------

void Axes::buildPipeline()
{
    vtkTransform* transform = m_renderService.lock()->getOrAddVtkTransform(m_transformId);
    m_axesActor->SetTotalLength( m_length, m_length, m_length );
    m_axesActor->SetShaftTypeToCylinder();
    m_axesActor->SetTipTypeToCone();
    m_axesActor->SetXAxisLabelText( m_xLabel.c_str() );
    m_axesActor->SetYAxisLabelText( m_yLabel.c_str() );
    m_axesActor->SetZAxisLabelText( m_zLabel.c_str() );

    if (!m_labelOn)
    {
        m_axesActor->AxisLabelsOff();
    }

    m_axesActor->SetUserTransform(transform);

    this->setVtkPipelineModified();

}

//------------------------------------------------------------------------------

void Axes::updateVisibility(bool _isVisible)
{
    m_axesActor->SetVisibility(_isVisible);

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

#endif // ANDROID
