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
}

void Camera2::doStart() throw(fwTools::Failed)
{
    vtkCamera* camera = this->getRenderer()->GetActiveCamera();
    camera->SetPosition (0, 0, 0);
    camera->SetFocalPoint(0, 0, 10);
    camera->SetViewUp(0, -1, 0);
    vtkTransform* trans = vtkTransform::New();
    trans->Identity();
    camera->ApplyTransform(trans);
    camera->SetClippingRange(0.1, 10000);
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







} //namespace visuVTKAdaptor
