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
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkRenderWindowInteractor.h>

#include "visuVTKAdaptor/Transform.hpp"

class TransformClallback : public ::vtkCommand
{
public:

    static TransformClallback* New(::visuVTKAdaptor::Transform* adaptor) { 
        TransformClallback *cb = new TransformClallback; 
        cb->m_adaptor = adaptor;
        return cb;
    }

     TransformClallback() {}
    ~TransformClallback() {}

    virtual void Execute( ::vtkObject* pCaller, unsigned long, void* )
    {
        m_adaptor->updateFromVtk();
    }

    ::visuVTKAdaptor::Transform *m_adaptor;
};


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Transform, ::fwData::TransformationMatrix3D ) ;

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

Transform::Transform() throw()
{
	m_transform = 0;
    m_transformCommand = TransformClallback::New(this);
    addNewHandledEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED );
}

//------------------------------------------------------------------------------

Transform::~Transform() throw()
{
    if( m_transformCommand ) m_transformCommand->Delete();
    m_transformCommand = 0;
}

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
    getTransform()->AddObserver( ::vtkCommand::ModifiedEvent, m_transformCommand );

}



void Transform::updateFromVtk()
{
    getTransform()->RemoveObserver( m_transformCommand );
    OSLM_ERROR("UPDATE FROM VTK");
    ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();
    vtkMatrix4x4* mat = getTransform()->GetMatrix();

    for(int lt=0; lt<4; lt++)
    {
        for(int ct=0; ct<4; ct++)
        {
            trf->setCoefficient(lt,ct, mat->GetElement(lt,ct));
        }
    }
    ::fwComEd::TransformationMatrix3DMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED ) ;
    ::fwServices::IEditionService::notify(this->getSptr(), trf, msg);

    getTransform()->AddObserver( ::vtkCommand::ModifiedEvent, m_transformCommand );
}


//------------------------------------------------------------------------------

void Transform::doUpdate() throw(fwTools::Failed)
{
    getTransform()->RemoveObserver( m_transformCommand );
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
    getTransform()->AddObserver( ::vtkCommand::ModifiedEvent, m_transformCommand );
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
