/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SPointList.hpp"

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>

#include <fwComEd/PointListMsg.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <iterator>
#include <algorithm>
#include <functional>

#include <boost/function.hpp>

#include <vtkPoints.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkGlyph3D.h>
#include <vtkCubeSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKARAdaptor::SPointList, ::fwData::PointList );

namespace visuVTKARAdaptor
{

//------------------------------------------------------------------------------

SPointList::SPointList() throw()
{
    //addNewHandledEvent( ::fwComEd::PointListMsg::ELEMENT_ADDED );
    //addNewHandledEvent( ::fwComEd::PointListMsg::ELEMENT_MODIFIED );
    //addNewHandledEvent( ::fwComEd::PointListMsg::ELEMENT_REMOVED );
}

//------------------------------------------------------------------------------

SPointList::~SPointList() throw()
{
}

//------------------------------------------------------------------------------

void SPointList::configuring() throw(fwTools::Failed)
{

    SLM_TRACE_FUNC();

    SLM_ASSERT("configuration missing", m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
    m_imageId = m_configuration->getAttributeValue("imageId");
    SLM_ASSERT("missing 'imageId' attibrute in SPointList configuration", m_imageId != "");

    std::string hexaColor = m_configuration->getAttributeValue("color");
    m_ptColor = ::fwData::Color::New();
    if (!hexaColor.empty())
    {
        m_ptColor->setRGBA(hexaColor);
    }
}

//------------------------------------------------------------------------------

void SPointList::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void SPointList::doUpdate() throw(fwTools::Failed)
{
    vtkSmartPointer<vtkPoints> imgPoints  = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(imgPoints);

    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(3.);
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

    this->addToRenderer(actor);

    ::fwTools::Object::sptr pImage = ::fwTools::fwID::getObject(m_imageId);
    ::fwData::Image::sptr image    = ::fwData::Image::dynamicCast(pImage);
    SLM_ASSERT("This object is not an image", image);

    ::fwData::Image::SizeType size = image->getSize();

    ::fwData::PointList::sptr pl = this->getObject< ::fwData::PointList >();
    for( ::fwData::Point::sptr pt : pl->getPoints() )
    {
        //conversion from qt 2D coordinates to vtk 3D coordinates
        ::fwData::Point::PointCoordArrayType vecSrc = pt->getCoord();
        ::fwData::Point::PointCoordArrayType vecDst;
        vecDst[0] = vecSrc[0] -  size[0]/2;
        vecDst[1] = -vecSrc[1] +  size[1]/2;
        vecDst[2] = 0;

        imgPoints->InsertNextPoint(vecDst[0], vecDst[1], vecDst[2]);
    }
    imgPoints->Modified();
}

//------------------------------------------------------------------------------

void SPointList::doReceive( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

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


