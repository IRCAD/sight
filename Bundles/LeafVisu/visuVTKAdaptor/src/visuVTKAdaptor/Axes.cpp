/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/Axes.hpp"

#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

#include <vtkCommand.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3D.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>

#include <boost/lexical_cast.hpp>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Axes, ::fwData::Object );

namespace visuVTKAdaptor
{
// ------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType Axes::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";

// ------------------------------------------------------------------------------

Axes::Axes() throw() :
    m_axesActor(fwVtkAxesActor::New()),
    m_length(1.),
    m_labelOn(true),
    m_transformAxes(vtkTransform::New()),
    m_xLabel("x"),
    m_yLabel("y"),
    m_zLabel("z"),
    m_sphereOn(false),
    m_sphereActor(vtkSmartPointer< vtkActor>::New())

{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &Axes::updateVisibility, this);
}

// ------------------------------------------------------------------------------

Axes::~Axes() throw()
{
    m_axesActor->Delete();
    m_axesActor = 0;
    if(m_sphereOn)
    {
        m_sphereActor = 0;
    }
}

// ------------------------------------------------------------------------------

void Axes::doStart() throw(fwTools::Failed)
{
    this->buildPipeline();
    this->addToRenderer( m_axesActor );
    if(m_sphereOn)
    {
        this->addToRenderer( m_sphereActor );
    }
}

// ------------------------------------------------------------------------------

void Axes::doStop() throw(fwTools::Failed)
{
    this->removeAllPropFromRenderer();
    this->getRenderer()->RemoveActor(m_axesActor);
    if(m_sphereOn)
    {
        this->getRenderer()->RemoveActor(m_sphereActor);
    }
}

// ------------------------------------------------------------------------------

void Axes::doSwap() throw(fwTools::Failed)
{
}

// ------------------------------------------------------------------------------

void Axes::doUpdate() throw(fwTools::Failed)
{
}

// ------------------------------------------------------------------------------

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
    if ( m_configuration->hasAttribute( "marker" ) )
    {
        std::string value = m_configuration->getAttributeValue( "marker" );
        std::transform( value.begin(), value.end(), value.begin(), tolower );
        m_sphereOn = ( value == "yes" );
    }
    if ( m_configuration->hasAttribute( "markerColor" ) )
    {
        std::string strColor = m_configuration->getAttributeValue("markerColor");

        m_color = ::fwData::Color::New();
        m_color->setRGBA(strColor);
    }
    else
    {
        m_color = ::fwData::Color::New(0.2f, 1.2f, 1.3f, 1.0f);
    }
}

// ------------------------------------------------------------------------------

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

    if(m_sphereOn)
    {
        // build a vtkSphereSource
        vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetRadius(m_length/4.0);
        vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(sphereSource->GetOutputPort());

        m_sphereActor->SetMapper(mapper);
        m_sphereActor->GetProperty()->SetColor(m_color->red(), m_color->green(), m_color->blue());
        m_sphereActor->GetProperty()->SetOpacity(m_color->alpha());
        m_sphereActor->SetUserTransform(transform);
    }

    m_axesActor->SetUserTransform(transform);

    this->setVtkPipelineModified();

}

// ------------------------------------------------------------------------------

void Axes::updateVisibility(bool _isVisible)
{
    m_axesActor->SetVisibility(_isVisible);
    if(m_sphereOn)
    {
        m_sphereActor->SetVisibility(_isVisible);
    }

    this->setVtkPipelineModified();
    this->requestRender();
}

// ------------------------------------------------------------------------------

} // namespace visuVTKAdaptor

#endif // ANDROID
