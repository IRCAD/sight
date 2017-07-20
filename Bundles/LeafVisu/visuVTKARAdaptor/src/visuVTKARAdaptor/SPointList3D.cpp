/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SPointList3D.hpp"

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/PointList.hpp>

#include <fwServices/macros.hpp>

#include <boost/lexical_cast.hpp>

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>

#include <algorithm>
#include <functional>
#include <iterator>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKARAdaptor::SPointList3D, ::fwData::PointList );

namespace visuVTKARAdaptor
{

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

void SPointList3D::doConfigure()
{

    SLM_TRACE_FUNC();

    SLM_ASSERT("configuration missing", m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
    this->setTransformId( m_configuration->getAttributeValue("transform") );

    std::string hexaColor = m_configuration->getAttributeValue("color");
    m_ptColor = ::fwData::Color::New();
    if (!hexaColor.empty())
    {
        m_ptColor->setRGBA(hexaColor);
    }

    std::string radiusStr = m_configuration->getAttributeValue("radius");
    if (!radiusStr.empty())
    {
        m_radius = ::boost::lexical_cast<double>(radiusStr);
    }
}

//------------------------------------------------------------------------------

void SPointList3D::doStart()
{
    ::fwData::PointList::sptr pl = this->getObject< ::fwData::PointList >();

    if (pl)
    {
        m_connections.connect(pl, ::fwData::PointList::s_POINT_ADDED_SIG, this->getSptr(), s_UPDATE_SLOT);
    }

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
    actor->GetProperty()->SetColor(m_ptColor->red(), m_ptColor->green(), m_ptColor->blue());
    actor->GetProperty()->SetOpacity(m_ptColor->alpha());

    if (!this->getTransformId().empty())
    {
        actor->SetUserTransform(this->getTransform());
    }

    this->addToRenderer(actor);

    this->doUpdate();
}

//------------------------------------------------------------------------------

void SPointList3D::doUpdate()
{
    m_points->Reset();

    ::fwData::PointList::sptr pl = this->getObject< ::fwData::PointList >();

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
}

//------------------------------------------------------------------------------

void SPointList3D::doSwap()
{
    this->doStop();
    this->doStart();
}

//------------------------------------------------------------------------------

void SPointList3D::doStop()
{
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

} //namespace visuVTKARAdaptor
