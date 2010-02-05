/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

#include <fwData/String.hpp>
#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include <fwComEd/ImageMsg.hpp>

#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>

#include "visuVTKAdaptor/Medical3DCamera.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Medical3DCamera, ::fwData::Image ) ;

namespace visuVTKAdaptor
{


std::map< std::string, ::visuVTKAdaptor::IImagesAdaptor::Orientation >
Medical3DCamera::m_orientationConversion
        = ::boost::assign::map_list_of(std::string("axial"),Z_AXIS)
                                      (std::string("frontal"),Y_AXIS)
                                      (std::string("sagittal"),X_AXIS);

Medical3DCamera::Medical3DCamera() throw() : IImagesAdaptor()
{
    addNewHandlingEvent( "CAMERA_ORIENTATION" );
}

//------------------------------------------------------------------------------

Medical3DCamera::~Medical3DCamera() throw()
{
}

//------------------------------------------------------------------------------

void Medical3DCamera::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
    if(m_configuration->hasAttribute("sliceIndex"))
    {
        std::string  orientation = m_configuration->getAttributeValue("sliceIndex");
        SLM_ASSERT("Unknown orientation", m_orientationConversion.find(orientation) != m_orientationConversion.end());
        m_orientation = m_orientationConversion[orientation];
    }
}

//------------------------------------------------------------------------------

void Medical3DCamera::doStart() throw(fwTools::Failed)
{
    m_camera = this->getRenderer()->GetActiveCamera();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Medical3DCamera::doUpdate() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->updateView();
}

//------------------------------------------------------------------------------

void Medical3DCamera::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Medical3DCamera::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void Medical3DCamera::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    ::fwComEd::ImageMsg::csptr imageMsg = ::fwComEd::ImageMsg::dynamicConstCast( msg );

    if ( imageMsg && imageMsg->hasEvent( "CAMERA_ORIENTATION") )
    {
        ::fwData::Object::csptr dataInfo = imageMsg->getDataInfo("CAMERA_ORIENTATION");
        SLM_ASSERT("dataInfo is missing", dataInfo);
        ::fwData::String::csptr orientation = ::fwData::String::dynamicConstCast(dataInfo);
        SLM_ASSERT("dataInfo is missing", orientation);
        SLM_ASSERT("Unknown orientation", m_orientationConversion.find(orientation->value()) != m_orientationConversion.end());
        m_orientation = m_orientationConversion[orientation->value()];
        this->doUpdate();
    }

}

//------------------------------------------------------------------------------

void Medical3DCamera::updateView()
{
    if(m_orientation == Z_AXIS )
    {
        this->resetAxialView();
    }
    else if(m_orientation == Y_AXIS )
    {
        this->resetFrontalView();
    }
    else if(m_orientation == X_AXIS )
    {
        this->resetSagittalView();
    }
}

//------------------------------------------------------------------------------

void Medical3DCamera::resetSagittalView()
{
    m_camera->SetPosition(-1,0,0);
    m_camera->SetFocalPoint(0,0,0);
    m_camera->SetViewUp(0,0,1);
    this->getRenderer()->ResetCamera();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Medical3DCamera::resetFrontalView()
{
    m_camera->SetPosition(0,-1,0);
    m_camera->SetFocalPoint(0,0,0);
    m_camera->SetViewUp(0,0,1);
    this->getRenderer()->ResetCamera();
    this->setVtkPipelineModified();

}

//------------------------------------------------------------------------------

void Medical3DCamera::resetAxialView()
{
    m_camera->SetPosition(0,0,-1);
    m_camera->SetFocalPoint(0,0,0);
    m_camera->SetViewUp(0,-1,0);
    this->getRenderer()->ResetCamera();
    this->setVtkPipelineModified();

}

//------------------------------------------------------------------------------


} //namespace visuVTKAdaptor
