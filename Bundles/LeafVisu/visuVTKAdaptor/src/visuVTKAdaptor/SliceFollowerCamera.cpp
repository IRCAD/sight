/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Contributors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwData/Camera.hpp>
#include <fwData/Video.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Material.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>

#include "visuVTKAdaptor/SliceFollowerCamera.hpp"



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::SliceFollowerCamera, ::fwData::Image ) ;

namespace visuVTKAdaptor
{


SliceFollowerCamera::SliceFollowerCamera() throw() : IImagesAdaptor()
{
    m_comChannelPriority = 0.49;
    addNewHandledEvent( ::fwComEd::ImageMsg::BUFFER );
    addNewHandledEvent( ::fwComEd::ImageMsg::SLICE_INDEX );
    addNewHandledEvent( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE );
}

//------------------------------------------------------------------------------

SliceFollowerCamera::~SliceFollowerCamera() throw()
{
}

//------------------------------------------------------------------------------

void SliceFollowerCamera::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwRuntime::ConfigurationElement::sptr cfg;
    //To be managed by ::fwRenderVTK::VtkRenderService
    if(m_configuration->getName() == "config")
    {
        cfg = m_configuration;
    }
    //When directly declared as an image service
    else if( m_configuration->findConfigurationElement("config") )
    {
        cfg = m_configuration->findConfigurationElement("config") ;
    }
    else
    {
        assert(false);
    }

    if( cfg->hasAttribute("scene") )
    {
        OSLM_TRACE("m_configuration->hasAttributeValue scene: true");
        this->setSceneId(cfg->getAttributeValue("scene"));
    }
    else
    {
        OSLM_TRACE("m_configuration->hasAttributeValue scene: false");
    }

    this->setRenderId( cfg->getAttributeValue("renderer") );

//    assert(m_configuration->getName() == "config");
//    this->setRenderId( m_configuration->getAttributeValue("renderer") );
    if(cfg->hasAttribute("sliceIndex"))
    {
         std::string  orientation = cfg->getAttributeValue("sliceIndex");
         if(orientation == "axial" )
         {
             m_orientation = Z_AXIS;
         }
         else if(orientation == "frontal" )
         {
             m_orientation = Y_AXIS;
         }
         else if(orientation == "sagittal" )
         {
             m_orientation = X_AXIS;
         }
    }
}

//------------------------------------------------------------------------------

void SliceFollowerCamera::doStart() throw(fwTools::Failed)
{
    m_camera = this->getRenderer()->GetActiveCamera();
    this->initializeCamera();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void SliceFollowerCamera::doUpdate() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SliceFollowerCamera::doSwap() throw(fwTools::Failed)
{
    initializeCamera();
}

//------------------------------------------------------------------------------

void SliceFollowerCamera::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void SliceFollowerCamera::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid)
    {
        if ( msg->hasEvent( ::fwComEd::ImageMsg::BUFFER ) || ( msg->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE )) )
        {
            initializeCamera();
        }
        if ( msg->hasEvent( ::fwComEd::ImageMsg::SLICE_INDEX ) )
        {
            updateCamera();
        }
        if ( msg->hasEvent( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE ))
        {
            ::fwData::Object::csptr cObjInfo = msg->getDataInfo( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE );
            ::fwData::Object::sptr objInfo = ::boost::const_pointer_cast< ::fwData::Object > ( cObjInfo );
            ::fwData::Composite::sptr info = ::fwData::Composite::dynamicCast ( objInfo );

            int fromSliceType = ::fwData::Integer::dynamicCast( info->getRefMap()["fromSliceType"] )->value();
            int toSliceType =   ::fwData::Integer::dynamicCast( info->getRefMap()["toSliceType"] )->value();

            if( toSliceType == static_cast<int>(m_orientation) )
            {
                setOrientation( static_cast< Orientation >( fromSliceType ));
                initializeCamera();
            }
            else if(fromSliceType == static_cast<int>(m_orientation))
            {
                setOrientation( static_cast< Orientation >( toSliceType ));
                initializeCamera();
            }
        }

    }
}

//------------------------------------------------------------------------------

const int orientationToAxe[3] = { 2, 2, 1 };
void SliceFollowerCamera::initializeCamera()
{
    double imageSize[3];
    this->getImageSize(imageSize);
    double size = imageSize[ orientationToAxe [m_orientation] ];

    double distance = (1.1 * size)
            / ( std::tan( m_camera->GetViewAngle() * (vtkMath::DoublePi() / 180.0) ) );

    m_camera->ParallelProjectionOn();
    setVtkPipelineModified();

    this->updateCamera(distance, size);
}

//------------------------------------------------------------------------------

void SliceFollowerCamera::updateCamera(double distance, double size)
{
    double focalPoint[ 3 ];
    double position[ 3 ];

    SLM_ASSERT("No Camera", m_camera );

    if (distance > 0)
    {
        // Update position according to orientation
        double center[ 3 ];
        getCurrentSliceCenter( center );
        std::copy(center, center+3, focalPoint);

        focalPoint[m_orientation] = 0;

        distance = m_camera->GetDistance();

        std::copy(focalPoint, focalPoint+3, position);

        position[ m_orientation ] -= distance;

        m_camera->SetParallelScale(.51*size);
        m_camera->SetFocalPoint( focalPoint );
        m_camera->SetPosition( position );
    }

    // m_orientation = 0 : 0, 0,1
    // m_orientation = 1 : 0, 0,1
    // m_orientation = 2 : 0,-1,0
    m_camera->SetViewUp(
            0,
            (m_orientation == 2 ? -1 : 0) ,
            (m_orientation <= 1 ?  1 : 0)
    );
    m_camera->OrthogonalizeViewUp();

    this->getRenderer()->ResetCameraClippingRange();
    this->setVtkPipelineModified();
}



} //namespace visuVTKAdaptor
