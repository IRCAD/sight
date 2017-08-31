/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SPointList3D.hpp"

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>

#include <fwServices/macros.hpp>

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKARAdaptor::SPointList3D, ::fwData::PointList );

namespace visuVTKARAdaptor
{

static const ::fwServices::IService::KeyType s_POINTLIST_IN = "pointList";

//------------------------------------------------------------------------------

SPointList3D::SPointList3D() noexcept :
    m_radius(3.)
{
}

//------------------------------------------------------------------------------

SPointList3D::~SPointList3D() noexcept
{
}

//------------------------------------------------------------------------------

void SPointList3D::configuring()
{
    SLM_TRACE_FUNC();

    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    std::string hexaColor = config.get< std::string >("color");

    m_ptColor = ::fwData::Color::New();
    if (!hexaColor.empty())
    {
        m_ptColor->setRGBA(hexaColor);
    }

    m_radius = config.get<double>("radius", m_radius);
}

//------------------------------------------------------------------------------

void SPointList3D::starting()
{
    this->initialize();

    m_points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(m_points);

    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(m_radius);
    vtkSmartPointer<vtkGlyph3D> glyph3D = vtkSmartPointer<vtkGlyph3D>::New();
    glyph3D->SetSourceConnection(sphereSource->GetOutputPort());
    glyph3D->SetInputData(polydata);
    glyph3D->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(glyph3D->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(static_cast<double>(m_ptColor->red()),
                                   static_cast<double>(m_ptColor->green()),
                                   static_cast<double>(m_ptColor->blue()));

    actor->GetProperty()->SetOpacity(static_cast<double>(m_ptColor->alpha()));

    if (!this->getTransformId().empty())
    {
        actor->SetUserTransform(this->getTransform());
    }

    this->addToRenderer(actor);

    this->updating();
}

//------------------------------------------------------------------------------

void SPointList3D::updating()
{
    m_points->Reset();

    ::fwData::PointList::csptr pl = this->getInput< ::fwData::PointList >(s_POINTLIST_IN);

    {
        ::fwData::mt::ObjectReadLock lock(pl);

        for( ::fwData::Point::sptr pt : pl->getPoints() )
        {
            ::fwData::Point::PointCoordArrayType coord = pt->getCoord();
            m_points->InsertNextPoint(coord[0], coord[1], coord[2]);
        }
    }
    m_points->Modified();

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SPointList3D::swapping()
{
    this->stopping();
    this->starting();
}

//------------------------------------------------------------------------------

void SPointList3D::stopping()
{
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

fwServices::IService::KeyConnectionsMap SPointList3D::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_POINTLIST_IN, ::fwData::PointList::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_POINTLIST_IN, ::fwData::PointList::s_POINT_ADDED_SIG, s_UPDATE_SLOT);
    connections.push(s_POINTLIST_IN, ::fwData::PointList::s_POINT_REMOVED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKARAdaptor
