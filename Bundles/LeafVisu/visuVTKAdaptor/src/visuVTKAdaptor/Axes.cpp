/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/// FW4SPL Includes
#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/registry/ObjectService.hpp>

/// VTK Includes
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkProp3D.h>

#include "visuVTKAdaptor/Axes.hpp"

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Axes, ::fwTools::Object );

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

Axes::Axes() throw()
{
    m_axesActor = vtkAxesActor::New();
    m_length = 1;
    m_labelOn = true;
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

void Axes::doUpdate() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void Axes::doUpdate(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void Axes::configuring() throw(fwTools::Failed)
{
    assert( m_configuration->getName() == "config" );
    
    this->setRenderId( m_configuration->getAttributeValue("renderer") );

    if ( m_configuration->hasAttribute( "transform" ) )
    {
        this->setTransformId ( m_configuration->getAttributeValue ( "transform" ) );
    }
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
}

//------------------------------------------------------------------------------

void Axes::buildPipeline()
{
    m_axesActor->SetTotalLength( m_length, m_length, m_length );
    m_axesActor->SetShaftTypeToCylinder();
    m_axesActor->SetTipTypeToCone();

    if (!m_labelOn)
    {
        m_axesActor->SetXAxisLabelText( "x" );
        m_axesActor->SetYAxisLabelText( "y" );
        m_axesActor->SetZAxisLabelText( "z" );
        m_axesActor->AxisLabelsOff();
    }
    m_axesActor->SetUserTransform( this->getTransform() );
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
