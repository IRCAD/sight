/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SPointList.hpp"

#include <arData/Camera.hpp>

#include <fwData/Color.hpp>
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

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKARAdaptor::SPointList, ::fwData::PointList );

namespace visuVTKARAdaptor
{

//------------------------------------------------------------------------------

SPointList::SPointList() throw()
{
}

//------------------------------------------------------------------------------

SPointList::~SPointList() throw()
{
}

//------------------------------------------------------------------------------

void SPointList::doConfigure() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("configuration missing", m_configuration->getName() == "config");
    m_imageId   = m_configuration->getAttributeValue("imageId");
    m_cameraUID = m_configuration->getAttributeValue("cameraUID");

    SLM_ASSERT("missing 'imageId' and 'cameraUID' attribute, at least one of them is required",
               m_imageId != "" || m_cameraUID !="");

    m_hexaColor = m_configuration->getAttributeValue("color");

    m_radius = m_configuration->getAttributeValue("radius");

}

//------------------------------------------------------------------------------

void SPointList::doStart() throw(fwTools::Failed)
{

    this->doUpdate();
}

//------------------------------------------------------------------------------

void SPointList::doUpdate() throw(fwTools::Failed)
{
    std::array<size_t, 2> resolution;
    std::array<double,2 > opticalCenter;
    bool needToShift = false;

    if (!m_cameraUID.empty())
    {
        ::arData::Camera::csptr camera = this->getSafeInput< ::arData::Camera>(m_cameraUID);
        SLM_ASSERT("Missing camera", camera);

        resolution[0] = camera->getWidth();
        resolution[1] = camera->getHeight();

        opticalCenter[0] = camera->getCx();
        opticalCenter[1] = camera->getCy();

        //We need to shift position by opticalCenter.
        needToShift = true;
    }

    else if(!m_imageId.empty())
    {
        ::fwData::Image::csptr image = this->getSafeInput< ::fwData::Image >(m_imageId);
        SLM_ASSERT("Missing image", image);

        resolution[0] = image->getSize()[0];
        resolution[1] = image->getSize()[1];
    }


    this->removeAllPropFromRenderer();

    vtkSmartPointer<vtkPoints> imgPoints  = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(imgPoints);

    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();

    //Handle size
    if(!m_radius.empty())
    {
        sphereSource->SetRadius(std::stod(m_radius));
    }
    else
    {
        sphereSource->SetRadius(3.);
    }

    vtkSmartPointer<vtkGlyph3D> glyph3D = vtkSmartPointer<vtkGlyph3D>::New();
    glyph3D->SetSourceConnection(sphereSource->GetOutputPort());
    glyph3D->SetInputData(polydata);
    glyph3D->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(glyph3D->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    //Handle color
    ::fwData::Color::sptr ptColor = ::fwData::Color::New();

    if (!m_hexaColor.empty())
    {
        ptColor->setRGBA(m_hexaColor);
    }

    actor->GetProperty()->SetColor(ptColor->red(), ptColor->green(), ptColor->blue());
    actor->GetProperty()->SetOpacity(ptColor->alpha());

    this->addToRenderer(actor);

    if(resolution[0] > 0 && resolution[1] > 0)
    {
        ::fwData::PointList::csptr pl = this->getObject< ::fwData::PointList >();
        for(const auto& pt : pl->getPoints() )
        {
            //conversion from qt 2D coordinates to vtk 3D coordinates
            ::fwData::Point::PointCoordArrayType vecSrc = pt->getCoord();
            ::fwData::Point::PointCoordArrayType vecDst;
            vecDst[0] = vecSrc[0] -  static_cast<double>(resolution[0]) / 2.;
            vecDst[1] = -vecSrc[1] + static_cast<double>(resolution[1]) / 2.;
            vecDst[2] = 0;

            //shift points if there
            if(needToShift)
            {
                const double shiftX = static_cast<double>(resolution[0]) / 2. - opticalCenter[0];
                const double shiftY = static_cast<double>(resolution[1]) / 2. - opticalCenter[1];

                vecDst[0] += shiftX;
                vecDst[1] -= shiftY;
            }

            imgPoints->InsertNextPoint(vecDst[0], vecDst[1], vecDst[2]);
        }

        imgPoints->Modified();
    }
    else
    {
        SLM_WARN("Image size is null");
    }
}

//------------------------------------------------------------------------------

void SPointList::doSwap() throw(fwTools::Failed)
{
    this->doStop();
    this->doStart();
}

//------------------------------------------------------------------------------

void SPointList::doStop() throw(fwTools::Failed)
{
    this->removeAllPropFromRenderer();
}


//------------------------------------------------------------------------------

} //namespace visuVTKARAdaptor


