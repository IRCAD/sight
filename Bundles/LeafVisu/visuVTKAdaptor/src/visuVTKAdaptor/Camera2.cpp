/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>


#include <fwData/Boolean.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include <fwComEd/Dictionary.hpp>
//#include <fwComEd/Camera2Msg.hpp>
#include <fwComEd/TransformationMatrix3DMsg.hpp>

#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkIdentityTransform.h>
#include <vtkCamera.h>


#include "visuVTKAdaptor/Camera2.hpp"



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Camera2, ::fwData::TransformationMatrix3D ) ;

namespace visuVTKAdaptor
{


Camera2::Camera2() throw()
{
}

Camera2::~Camera2() throw()
{
}


void Camera2::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
    if ( m_configuration->hasAttribute( "transform" ) )
    {
        this->setTransformId ( m_configuration->getAttributeValue ( "transform" ) );
    }
    
    
}

void Camera2::doStart() throw(fwTools::Failed)
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
 
    //vtkIdentityTransform* trans = vtkIdentityTransform::New();
    //trans->Identity();
    
    //camera->SetUserTransform( trans );
    //camera->SetUserViewTransform( trans );
 
    camera->SetPosition (0, 0, 0);
    camera->SetFocalPoint(1, 0, 0);
    camera->SetViewUp(0, 0, 1);
    //camera->SetClippingRange(0.1, 10000);
    
 //   camera->SetUserViewTransform( this->getTransform() );
}

void Camera2::doUpdate() throw(fwTools::Failed)
{
}

void Camera2::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

void Camera2::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}


void Camera2::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    if( msg->hasEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED ) )
    {
        vtkCamera* camera = this->getRenderer()->GetActiveCamera();
        ::fwData::TransformationMatrix3D::sptr transMat =
            this->getObject< ::fwData::TransformationMatrix3D >();

        vtkMatrix4x4* mat = vtkMatrix4x4::New();

        for(int lt=0; lt<4; lt++)
        {
            for(int ct=0; ct<4; ct++)
            {
                mat->SetElement(lt, ct, transMat->getCoefficient(lt,ct));
                OSLM_ERROR("val = " << transMat->getCoefficient(lt,ct));
            }
        }
        OSLM_ERROR(" ")

        double focal[]={1.0, 0.0, 0.0, 1.0};
        double rfocal[4];
        double position[]={0.0, 0.0, 0.0, 1.0};
        double rposition[4];
        double viewUp[]={0.0, 0.0, 1.0, 1.0};
        double rviewUp[4];

        mat->MultiplyPoint( focal, rfocal);
        mat->MultiplyPoint( position, rposition);
        mat->MultiplyPoint( viewUp, rviewUp);
        for ( int i = 0; i < 3; i ++ )
        {
            //rfocal[i] = rfocal[i] - rposition[i];
            rviewUp[i] = rviewUp[i] - rposition[i];
        }

        mat->Delete();

        camera->SetPosition(rposition);
        camera->SetFocalPoint(rfocal);
        camera->SetViewUp(rviewUp);

        this->getRenderer()->ResetCameraClippingRange();
        this->setVtkPipelineModified();
    }
/*
    if( msg->hasEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED ) )
    {
        vtkCamera* camera = this->getRenderer()->GetActiveCamera();

        ::fwData::TransformationMatrix3D::sptr transMat =
            this->getObject< ::fwData::TransformationMatrix3D >();

        //vtkMatrix4x4* mat = vtkMatrix4x4::New();

        for(int lt=0; lt<4; lt++)
        {
            for(int ct=0; ct<4; ct++)
            {
                //mat->SetElement(lt, ct, transMat->getCoefficient(lt,ct));
        camera->GetUserViewTransform()->GetMatrix()->SetElement(lt, ct, transMat->getCoefficient(lt,ct));
            }
        }
    camera->GetUserViewTransform()->Modified();
        this->setVtkPipelineModified();
    }*/
}







} //namespace visuVTKAdaptor
