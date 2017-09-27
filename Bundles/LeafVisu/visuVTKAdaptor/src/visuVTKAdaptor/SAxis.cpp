/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/SAxis.hpp"

#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

#include <boost/lexical_cast.hpp>

#include <vtkCommand.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3D.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SAxis);

namespace visuVTKAdaptor
{
//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SAxis::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";

//------------------------------------------------------------------------------

SAxis::SAxis() noexcept :
    m_axisActor(fwVtkAxesActor::New()),
    m_length(1.),
    m_labelOn(true),
    m_transformAxis(vtkSmartPointer<vtkTransform>::New()),
    m_sphereActor(nullptr),
    m_sphereOn(false),
    m_xLabel("x"),
    m_yLabel("y"),
    m_zLabel("z")
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SAxis::updateVisibility, this);
}

//------------------------------------------------------------------------------

SAxis::~SAxis() noexcept
{
    m_axisActor->Delete();
    m_axisActor = nullptr;
    if(m_sphereOn)
    {
        m_sphereActor = nullptr;
    }
}

//------------------------------------------------------------------------------

void SAxis::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>.");

    m_length = config.get<double>("length", 1.0);

    const std::string label = config.get<std::string>("label", "yes");
    SLM_ASSERT("value for 'label' must be 'yes' or 'no'", label == "yes" || label == "no");
    m_labelOn = ( label == "yes" );

    m_xLabel = config.get<std::string>("xLabel", "x");
    m_yLabel = config.get<std::string>("yLabel", "y");
    m_zLabel = config.get<std::string>("zLabel", "z");

    const std::string marker = config.get<std::string>("marker", "no");
    SLM_ASSERT("value for 'marker' must be 'yes' or 'no'", marker == "yes" || marker == "no");
    m_sphereOn = ( marker == "yes" );

    const std::string markerColor = config.get<std::string>("markerColor", "#FFFFFF");
    m_color = ::fwData::Color::New();
    m_color->setRGBA(markerColor);
}

//------------------------------------------------------------------------------

void SAxis::starting()
{
    this->initialize();

    this->buildPipeline();
    this->addToRenderer( m_axisActor );
    if(m_sphereOn)
    {
        this->addToRenderer( m_sphereActor );
    }
    this->requestRender();
}

//------------------------------------------------------------------------------

void SAxis::stopping()
{
    this->removeAllPropFromRenderer();
    this->getRenderer()->RemoveActor(m_axisActor);
    if(m_sphereOn)
    {
        this->getRenderer()->RemoveActor(m_sphereActor);
    }
    this->requestRender();
}

//------------------------------------------------------------------------------

void SAxis::updating()
{
}

//------------------------------------------------------------------------------

void SAxis::buildPipeline()
{
    vtkTransform* transform = m_renderService.lock()->getOrAddVtkTransform(m_transformId);
    m_axisActor->SetTotalLength( m_length, m_length, m_length );
    m_axisActor->SetShaftTypeToCylinder();
    m_axisActor->SetTipTypeToCone();
    m_axisActor->SetXAxisLabelText( m_xLabel.c_str() );
    m_axisActor->SetYAxisLabelText( m_yLabel.c_str() );
    m_axisActor->SetZAxisLabelText( m_zLabel.c_str() );

    if (!m_labelOn)
    {
        m_axisActor->AxisLabelsOff();
    }

    if(m_sphereOn)
    {
        // build a vtkSphereSource
        const double sizeRatio = 4.0;
        auto sphereSource      = vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetRadius(m_length/sizeRatio);
        auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(sphereSource->GetOutputPort());

        m_sphereActor = vtkSmartPointer<vtkActor>::New();
        m_sphereActor->SetMapper(mapper);
        m_sphereActor->GetProperty()->SetColor(m_color->red(), m_color->green(), m_color->blue());
        m_sphereActor->GetProperty()->SetOpacity(m_color->alpha());
        m_sphereActor->SetUserTransform(transform);
    }

    m_axisActor->SetUserTransform(transform);

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SAxis::updateVisibility(bool _isVisible)
{
    m_axisActor->SetVisibility(_isVisible);
    if(m_sphereOn)
    {
        m_sphereActor->SetVisibility(_isVisible);
    }

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

} // namespace visuVTKAdaptor

#endif // ANDROID
