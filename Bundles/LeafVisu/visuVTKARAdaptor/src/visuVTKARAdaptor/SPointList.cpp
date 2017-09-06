/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SPointList.hpp"

#include <arData/Camera.hpp>

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

static const ::fwServices::IService::KeyType s_CAMERA_IN    = "camera";
static const ::fwServices::IService::KeyType s_IMAGE_IN     = "image";
static const ::fwServices::IService::KeyType s_POINTLIST_IN = "pointlist";

//------------------------------------------------------------------------------

SPointList::SPointList() noexcept :
    m_pointColor(::fwData::Color::New())
{
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
    this->updating();
}

//------------------------------------------------------------------------------

void SPointList::updating()
{
    std::array<size_t, 2> resolution;
    std::array<double, 2 > opticalCenter;
    bool needToShift = false;

    ::arData::Camera::csptr camera = this->getInput< ::arData::Camera>(s_CAMERA_IN);
    ::fwData::Image::csptr image   = this->getInput< ::fwData::Image >(s_IMAGE_IN);

    if(camera)
    {
        resolution[0] = camera->getWidth();
        resolution[1] = camera->getHeight();

        opticalCenter[0] = camera->getCx();
        opticalCenter[1] = camera->getCy();

        // Points need to be shifted along with the video.
        needToShift = true;

        OSLM_FATAL_IF("This service uses an image or a camera, not both.", image);
    }
    else if(image)
    {
        resolution[0] = image->getSize()[0];
        resolution[1] = image->getSize()[1];
    }
    else
    {
        OSLM_FATAL("An image or a camera is needed by this service.")
    }

    this->removeAllPropFromRenderer();

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

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    actor->GetProperty()->SetColor(m_pointColor->red(), m_pointColor->green(), m_pointColor->blue());
    actor->GetProperty()->SetOpacity(m_pointColor->alpha());

    this->addToRenderer(actor);

    if(resolution[0] > 0 && resolution[1] > 0)
    {
        ::fwData::PointList::csptr pl = this->getInput< ::fwData::PointList >(s_POINTLIST_IN);
        for(const auto& pt : pl->getPoints() )
        {
            // Convert qt 2D coordinates into vtk 3D coordinates.
            ::fwData::Point::PointCoordArrayType vecSrc = pt->getCoord();
            ::fwData::Point::PointCoordArrayType vecDst;
            vecDst[0] = vecSrc[0];
            vecDst[1] = -vecSrc[1];
            vecDst[2] = 0;

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

void SPointList::swapping()
{
    this->stopping();
    this->starting();
}

//------------------------------------------------------------------------------

void SPointList::stopping()
{
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

} //namespace visuVTKARAdaptor

