/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/SDistance.hpp"

#include <fwData/Material.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwServices/macros.hpp>

#include <vtkActor.h>
#include <vtkAxisActor2D.h>
#include <vtkCommand.h>
#include <vtkDistanceRepresentation2D.h>
#include <vtkHandleRepresentation.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SDistance, ::fwData::PointList );

namespace visuVTKAdaptor
{

const ::fwServices::IService::KeyType SDistance::s_POINTLIST_INPUT = "pointList";

SDistance::SDistance() noexcept :
    m_distanceRepresentation( vtkDistanceRepresentation2D::New()),
    m_lineActor(vtkActor::New()),
    m_lineSource(vtkLineSource::New())
{
    m_distanceRepresentation->InstantiateHandleRepresentation();
    m_distanceRepresentation->SetLabelFormat("%-#6.3gmm");

    m_lineSource->SetResolution(1);
    vtkPolyDataMapper* lineMapper = vtkPolyDataMapper::New();
    lineMapper->SetInputConnection(m_lineSource->GetOutputPort());

    m_lineActor->SetMapper(lineMapper);
    m_lineActor->GetProperty()->SetLineWidth(5.);
    m_lineActor->GetProperty()->SetColor(1., 1., 0.);
    m_lineActor->GetProperty()->SetOpacity(0.4);

    lineMapper->Delete();
    m_distanceRepresentation->GetAxis()->GetProperty()->SetColor(1.0, 0.0, 1.);
    m_distanceRepresentation->GetAxis()->SetTitlePosition(0.9);

    //fixed font size for displaying distance
    //m_distanceRepresentation->GetAxis()->SizeFontRelativeToAxisOn();
    //m_distanceRepresentation->GetAxis()->GetMapper();
    //m_distanceRepresentation->GetAxis()->SetFontFactor(0.8);
}

//------------------------------------------------------------------------------

SDistance::~SDistance() noexcept
{
    m_lineSource->Delete();
    m_lineActor->Delete();
    m_distanceRepresentation->Delete();
}

//------------------------------------------------------------------------------

void SDistance::setAxisColor( ::fwData::Color::sptr newColor) noexcept
{
    SLM_ASSERT("newColor not instanced", newColor);
    m_distanceRepresentation->GetAxis()->GetProperty()->SetColor(
        newColor->red(), newColor->green(), newColor->blue() );
    m_distanceRepresentation->GetAxis()->GetProperty()->SetOpacity(newColor->alpha() );
    m_lineActor->GetProperty()->SetColor(newColor->red(), newColor->green(), newColor->blue());
    m_lineActor->GetProperty()->SetOpacity( newColor->alpha() * 0.4);

    m_distanceRepresentation->GetAxis()->GetTitleTextProperty()->SetColor(
        newColor->red(), newColor->green(), newColor->blue() );
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SDistance::configuring()
{
    this->configureParams();
}

//------------------------------------------------------------------------------

void SDistance::starting()
{
    this->initialize();

    ::fwData::PointList::csptr ptList = this->getInput< ::fwData::PointList >(s_POINTLIST_INPUT);

    m_point1 = ptList->getPoints().front();
    m_point2 = ptList->getPoints().back();

    m_point1Connection = m_point1.lock()->signal(::fwData::Object::s_MODIFIED_SIG)->connect(
        this->slot(::fwServices::IService::s_UPDATE_SLOT));
    m_point2Connection = m_point2.lock()->signal(::fwData::Object::s_MODIFIED_SIG)->connect(
        this->slot(::fwServices::IService::s_UPDATE_SLOT));

    // set color to distance if Point List have Color Field
    if ( ptList->getField( ::fwDataTools::fieldHelper::Image::m_colorId ) )
    {
        ::fwData::Color::sptr color;
        color = ptList->getField< ::fwData::Color >( ::fwDataTools::fieldHelper::Image::m_colorId );
        this->setAxisColor( color );
    }

    this->addToRenderer(m_lineActor);
    this->addToRenderer(m_distanceRepresentation);

    this->updating();
}

//------------------------------------------------------------------------------

void SDistance::updating()
{
    ::fwData::Point::csptr p1 = m_point1.lock();
    ::fwData::Point::csptr p2 = m_point2.lock();

    double ps1[3];
    std::copy(p1->getCoord().begin(), (p1)->getCoord().end(), ps1 );

    double ps2[3];
    std::copy(p2->getCoord().begin(), (p2)->getCoord().end(), ps2 );

    m_distanceRepresentation->GetPoint1Representation()->SetWorldPosition(ps1);
    m_distanceRepresentation->GetPoint2Representation()->SetWorldPosition(ps2);

    m_distanceRepresentation->SetPoint1DisplayPosition(ps1);
    m_distanceRepresentation->SetPoint2DisplayPosition(ps2);

    m_distanceRepresentation->BuildRepresentation();

    m_lineSource->SetPoint1(ps1);
    m_lineSource->SetPoint2(ps2);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SDistance::stopping()
{
    m_point1Connection.disconnect();
    m_point2Connection.disconnect();

    m_point1.reset();
    m_point2.reset();

    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SDistance::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_POINTLIST_INPUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

#endif // ANDROID
