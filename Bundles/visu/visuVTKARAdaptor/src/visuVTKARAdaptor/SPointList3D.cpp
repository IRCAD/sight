/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "visuVTKARAdaptor/SPointList3D.hpp"

#include <fwCom/Slots.hxx>

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

const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT = "updateVisibility";

static const ::fwServices::IService::KeyType s_POINTLIST_IN = "pointList";

//------------------------------------------------------------------------------

SPointList3D::SPointList3D() noexcept :
    m_radius(3.)
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SPointList3D::updateVisibility, this);
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

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

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

    m_actor = vtkSmartPointer<vtkActor>::New();
    m_actor->SetMapper(mapper);
    m_actor->GetProperty()->SetColor(static_cast<double>(m_ptColor->red()),
                                     static_cast<double>(m_ptColor->green()),
                                     static_cast<double>(m_ptColor->blue()));

    m_actor->GetProperty()->SetOpacity(static_cast<double>(m_ptColor->alpha()));

    if (!this->getTransformId().empty())
    {
        m_actor->SetUserTransform(this->getTransform());
    }

    this->addToRenderer(m_actor);

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

void SPointList3D::updateVisibility(bool isVisible)
{
    m_actor->SetVisibility(isVisible);
    this->setVtkPipelineModified();
    this->requestRender();
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
