/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#include <boost/foreach.hpp>

#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Material.hpp>
#include <fwData/Boolean.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwServices/ObjectServiceRegistry.hpp>

#include <fwComEd/TransformationMatrix3DMsg.hpp>

#include <vtkCubeSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkRenderWindowInteractor.h>

#include "visuVTKAdaptor/Transform.hpp"



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Transform, ::fwData::TransformationMatrix3D ) ;

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

Transform::Transform() throw()
{
	m_transform = 0;
    addNewHandledEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED );
}

//------------------------------------------------------------------------------

Transform::~Transform() throw()
{}

//------------------------------------------------------------------------------

void Transform::configuring() throw(fwTools::Failed)
{

    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setTransformId( m_configuration->getAttributeValue("transform") );
}

//------------------------------------------------------------------------------

void Transform::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Transform::doUpdate() throw(fwTools::Failed)
{
    //doStop();
    ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();
    vtkMatrix4x4* mat = vtkMatrix4x4::New();

    for(int lt=0; lt<4; lt++)
    {
        for(int ct=0; ct<4; ct++)
        {
            mat->SetElement(lt,ct, trf->getCoefficient(lt,ct));
        }
    }
    vtkTransform* vtkTrf = this->getTransform();
    vtkTrf->SetMatrix(mat);
    vtkTrf->Modified();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Transform::setTransform(vtkTransform *t){
    if ( m_transform != t )
    {
        if (m_transform)
        {
            m_transform->Delete();
        }
        if(t)
        {
            t->Register(NULL);
        }
    }
    
    m_transform = t;
};

//------------------------------------------------------------------------------

vtkTransform * Transform::getTransform(){
    vtkTransform *t = m_transform;
    if (t == 0)
    {
        t = this->IVtkAdaptorService::getTransform();
    }
    return t;
}

//------------------------------------------------------------------------------
void Transform::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Transform::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void Transform::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
//  if ( msg->isAllModified() )
    ::fwComEd::TransformationMatrix3DMsg::csptr transfoMsg = ::fwComEd::TransformationMatrix3DMsg::dynamicConstCast(msg);
    if (transfoMsg && transfoMsg->hasEvent(::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED))
    {

        doUpdate();
//        ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();
//        vtkMatrix4x4* mat = vtkMatrix4x4::New();
//
//        for(int lt=0; lt<4; lt++)
//        {
//            for(int ct=0; ct<4; ct++)
//            {
//                mat->SetElement(lt,ct, trf->getCoefficient(lt,ct));
//            }
//        }
//        vtkTransform* vtkTrf = this->getTransform();
//        vtkTrf->SetMatrix(mat);
//        this->getTransform()->Modified();
//        // @TODO : Hack to force render !! (pb with tracking)
//        if( bForceRender )
//        {
//            this->getRenderService()->render();
//        }
    }
}

} //namespace visuVTKAdaptor
