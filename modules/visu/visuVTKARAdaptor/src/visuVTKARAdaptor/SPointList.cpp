/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "visuVTKARAdaptor/SPointList.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>

#include <fwServices/macros.hpp>

#include <vtkActor.h>
#include <vtkGlyph3D.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKARAdaptor::SPointList, ::fwData::PointList );

namespace visuVTKARAdaptor
{

const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT = "updateVisibility";

static const ::fwServices::IService::KeyType s_CAMERA_IN    = "camera";
static const ::fwServices::IService::KeyType s_IMAGE_IN     = "image";
static const ::fwServices::IService::KeyType s_POINTLIST_IN = "pointlist";

//------------------------------------------------------------------------------

SPointList::SPointList() noexcept :
    m_pointColor(::fwData::Color::New())
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SPointList::updateVisibility, this);
}

//------------------------------------------------------------------------------

SPointList::~SPointList() noexcept
{
}

//------------------------------------------------------------------------------

void SPointList::configuring()
{
    SLM_TRACE_FUNC();

    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    const std::string hexColor = config.get<std::string>("color", "");

    m_pointColor->setRGBA(hexColor);

    m_radius = config.get<double>("radius", 3.);
}

//------------------------------------------------------------------------------

void SPointList::starting()
{
    this->initialize();

    m_actor = vtkSmartPointer<vtkActor>::New();
    this->addToRenderer(m_actor);

    this->updating();
}

//------------------------------------------------------------------------------

void SPointList::updating()
{
    vtkSmartPointer<vtkPoints> imgPoints  = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(imgPoints);

    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(m_radius);

    vtkSmartPointer<vtkGlyph3D> glyph3D = vtkSmartPointer<vtkGlyph3D>::New();
    glyph3D->SetSourceConnection(sphereSource->GetOutputPort());
    glyph3D->SetInputData(polydata);
    glyph3D->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(glyph3D->GetOutputPort());

    m_actor->SetMapper(mapper);

    m_actor->GetProperty()->SetColor(static_cast< double >( m_pointColor->red() ),
                                     static_cast< double >( m_pointColor->green() ),
                                     static_cast <double >( m_pointColor->blue() ));

    m_actor->GetProperty()->SetOpacity(static_cast< double >( m_pointColor->alpha() ));

    ::fwData::PointList::csptr pl = this->getInput< ::fwData::PointList >(s_POINTLIST_IN);

    SLM_ASSERT("Cannot find input '" + s_POINTLIST_IN +"'", pl);

    for(const auto& pt : pl->getPoints() )
    {
        // Convert qt 2D coordinates into vtk 3D coordinates.
        const ::fwData::Point::PointCoordArrayType& vecSrc = pt->getCoord();
        imgPoints->InsertNextPoint(vecSrc[0], -vecSrc[1], 0);
    }

    imgPoints->Modified();

    this->setVtkPipelineModified();
    this->requestRender();

}

//------------------------------------------------------------------------------

void SPointList::swapping()
{
    this->stopping();
    this->starting();
}

//------------------------------------------------------------------------------

void SPointList::updateVisibility(bool isVisible)
{
    m_actor->SetVisibility(isVisible);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SPointList::stopping()
{
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

fwServices::IService::KeyConnectionsMap SPointList::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_POINTLIST_IN, ::fwData::PointList::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_POINTLIST_IN, ::fwData::PointList::s_POINT_ADDED_SIG, s_UPDATE_SLOT);
    connections.push(s_POINTLIST_IN, ::fwData::PointList::s_POINT_REMOVED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKARAdaptor
