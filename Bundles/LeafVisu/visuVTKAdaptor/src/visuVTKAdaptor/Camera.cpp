/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwData/Camera.hpp>
#include <fwData/Video.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Material.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/CameraMsg.hpp>
#include <fwComEd/VideoMsg.hpp>

#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkCamera.h>

#include "visuVTKAdaptor/Camera.hpp"



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Camera, ::fwData::Video ) ;

namespace visuVTKAdaptor
{


Camera::Camera() throw()
{
    bCam_init = false;
}

Camera::~Camera() throw()
{
}


void Camera::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
}

void Camera::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
}

void Camera::doUpdate() throw(fwTools::Failed)
{
    ::fwData::Video::sptr video = this->getObject< ::fwData::Video >();
    if (!video->dataAvailable())
        return;

    if(!bCam_init)
    {
        bCam_init = this->initCameraParameters();
    }
}

void Camera::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

void Camera::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}


void Camera::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{

    ::fwComEd::CameraMsg::csptr cameraMsg = ::fwComEd::CameraMsg::dynamicConstCast( msg );
    ::fwComEd::VideoMsg::csptr videoMsg = ::fwComEd::VideoMsg::dynamicConstCast( msg );
    if ( ( cameraMsg && cameraMsg->hasEvent( ::fwComEd::CameraMsg::NEW_CAMERA ) )
            || ( videoMsg && videoMsg->hasEvent(::fwComEd::VideoMsg::VIDEO_IS_REFRESHED) ))
    {
        doUpdate();
    }
    else if( msg->hasEvent( fwComEd::Dictionary::position ) )
    {
        vtkCamera* camera = this->getRenderer()->GetActiveCamera();
        ::fwData::TransformationMatrix3D::sptr transMat =
            this->getObject()->getFieldSingleElement< ::fwData::TransformationMatrix3D>(fwComEd::Dictionary::position);

        vtkMatrix4x4* mat = vtkMatrix4x4::New();

        for(int lt=0; lt<4; lt++)
        {
            for(int ct=0; ct<4; ct++)
            {
                mat->SetElement(lt, ct, transMat->getCoefficient(lt,ct));
            }
        }

        double pos[3];
        pos[0] = mat->Element[0][3];
        pos[1] = mat->Element[1][3];
        pos[2] = mat->Element[2][3];

        double p1[]={0.0, 0.0, 100.0, 1.0};
        double p2[4];
        mat->MultiplyPoint( p1, p2);

        mat->SetElement(0,3,0);
        mat->SetElement(1,3,0);
        mat->SetElement(2,3,0);

        double p3[]={0.0, -1.0, 0.0, 1.0};
        double p4[4];
        mat->MultiplyPoint( p3, p4);

        double p5[]={p4[0],p4[1],p4[2]};
        camera->SetPosition(pos);
        camera->SetFocalPoint(p2);
        camera->SetViewUp(p5);

        mat->Delete();
        this->setVtkPipelineModified();
    }
}


bool Camera::initCameraParameters()
{
    SLM_TRACE_FUNC();
    vtkRenderer* renderer = this->getRenderer();

    vtkCamera* camera = renderer->GetActiveCamera();
    ::fwData::Video::sptr video = this->getObject< ::fwData::Video >();
    ::fwData::TransformationMatrix3D::wptr extrinsic = video->getCamera()->getExtrinsicCameraTransformation();
    ::fwData::TransformationMatrix3D::wptr intrinsic = video->getCamera()->getIntrinsicCameraTransformation();

    /*****************************************************************/
    // Intrinsic matrix
//    double cx = intrinsic.lock()->getCoefficient(0,2);
//    double cy = intrinsic.lock()->getCoefficient(1,2);
//    double fx = intrinsic.lock()->getCoefficient(0,0);
    double fy = intrinsic.lock()->getCoefficient(1,1);

    int width, height, lowerLeftX, lowerLeftY;

//    m_interactor->Render();
    renderer->GetTiledSizeAndOrigin(&width, &height, &lowerLeftX, &lowerLeftY);

    if(width <=  0 || height <= 0)
        return false;

    camera->SetViewAngle(2.0 * atan( (this->getObject< ::fwData::Video >()->getYSize()/2.0)/fy ) / 3.1415 * 180.0);
//    camera->SetViewAngle(2.0 * atan( (600.0/2.0)/fy ) / 3.1415 * 180.0);
    camera->GetCompositeProjectionTransformMatrix(width/height, -1, +1);

    /*****************************************************************/
    // Extrinsic matrix
    vtkMatrix4x4 * mat = vtkMatrix4x4::New();
    vtkTransform* trans = vtkTransform::New();

    for(int l=0; l<4; l++)
    {
        for(int c=0; c <4; c++)
        {
            mat->SetElement(l,c, extrinsic.lock()->getCoefficient(l, c));
        }
    }

    mat->Invert();
    trans->Identity();
    trans->SetMatrix(mat);

    camera->SetPosition (0, 0, 0);
    camera->SetFocalPoint(0, 0, 10);
    camera->SetViewUp(0, -1, 0);
    camera->ApplyTransform(trans);
    camera->SetClippingRange(0.1, 10000);

    mat->Delete();
    trans->Delete();
    this->setVtkPipelineModified();

    return true;
}




} //namespace visuVTKAdaptor
