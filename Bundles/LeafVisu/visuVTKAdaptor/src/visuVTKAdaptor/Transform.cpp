/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkRenderWindowInteractor.h>

#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Material.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/TransformationMatrix3DMsg.hpp>

#include "visuVTKAdaptor/Transform.hpp"

class TransformClallback : public ::vtkCommand
{
public:

    static TransformClallback* New(::visuVTKAdaptor::Transform* adaptor) {
        TransformClallback *cb = new TransformClallback;
        cb->m_adaptor = adaptor;
        return cb;
    }

     TransformClallback() : m_adaptor(NULL) {}
    ~TransformClallback() {}

    virtual void Execute( ::vtkObject* pCaller, unsigned long, void* )
    {
        m_adaptor->updateFromVtk();
    }

    ::visuVTKAdaptor::Transform *m_adaptor;
};


fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Transform, ::fwData::TransformationMatrix3D ) ;

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

Transform::Transform() throw()
{
    m_transform = 0;
    m_transformCommand = TransformClallback::New(this);
    //addNewHandledEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED );
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

    if ( m_configuration->hasAttribute( "autoRender" ) )
    {
        const std::string autoRender   = m_configuration->getAttributeValue("autoRender");
        const bool autoRenderValue = (autoRender == "true");
        this->setAutoRender(autoRenderValue);
    }
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
    ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();
    vtkMatrix4x4* mat = getTransform()->GetMatrix();

    {
        ::fwData::mt::ObjectWriteLock lock(trf);
        for(int lt=0; lt<4; lt++)
        {
            for(int ct=0; ct<4; ct++)
            {
                trf->setCoefficient(lt,ct, mat->GetElement(lt,ct));
            }
        }
    }
    ::fwComEd::TransformationMatrix3DMsg::sptr msg = ::fwComEd::TransformationMatrix3DMsg::New();
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

    {
        ::fwData::mt::ObjectReadLock lock(trf);
        for(int lt=0; lt<4; lt++)
        {
            for(int ct=0; ct<4; ct++)
            {
                mat->SetElement(lt,ct, trf->getCoefficient(lt,ct));
            }
        }
    }
    vtkTransform* vtkTrf = this->getTransform();
    vtkTrf->SetMatrix(mat);
    vtkTrf->Modified();
    getTransform()->AddObserver( ::vtkCommand::ModifiedEvent, m_transformCommand );
    mat->Delete();
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

void Transform::doReceive( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    ::fwComEd::TransformationMatrix3DMsg::csptr transfoMsg = ::fwComEd::TransformationMatrix3DMsg::dynamicConstCast(msg);
    if (transfoMsg && transfoMsg->hasEvent(::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED))
    {

        doUpdate();
    }
}

} //namespace visuVTKAdaptor
