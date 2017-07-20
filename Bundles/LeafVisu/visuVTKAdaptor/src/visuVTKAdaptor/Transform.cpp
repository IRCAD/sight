/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/Transform.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Material.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>

class TransformCallback : public ::vtkCommand
{
public:

    //------------------------------------------------------------------------------

    static TransformCallback* New(::visuVTKAdaptor::Transform* adaptor)
    {
        TransformCallback* cb = new TransformCallback;
        cb->m_adaptor = adaptor;
        return cb;
    }

    TransformCallback() :
        m_adaptor(nullptr)
    {
    }
    ~TransformCallback()
    {
    }

    //------------------------------------------------------------------------------

    virtual void Execute( ::vtkObject* pCaller, unsigned long, void* )
    {
        m_adaptor->updateFromVtk();
    }

    ::visuVTKAdaptor::Transform* m_adaptor;
};

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::Transform,
                         ::fwData::TransformationMatrix3D );

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

Transform::Transform() noexcept :
    m_transform(nullptr),
    m_transformCommand(TransformCallback::New(this))
{
}

//------------------------------------------------------------------------------

Transform::~Transform() noexcept
{
    if( m_transformCommand )
    {
        m_transformCommand->Delete();
    }
    m_transformCommand = nullptr;
}

//------------------------------------------------------------------------------

void Transform::doConfigure()
{
    assert(m_configuration->getName() == "config");

    if ( m_configuration->hasAttribute( "autoRender" ) )
    {
        const std::string autoRender = m_configuration->getAttributeValue("autoRender");
        const bool autoRenderValue   = (autoRender == "true");
        this->setAutoRender(autoRenderValue);
    }

    if ( m_configuration->hasAttribute( "parent" ) )
    {
        m_parentId = m_configuration->getAttributeValue("parent");

        if(m_parentId.empty())
        {
            SLM_ERROR("Can't find vtkTransform '" + m_parentId + "'");
        }
    }
}

//------------------------------------------------------------------------------

void Transform::doStart()
{
    if(!m_transformId.empty())
    {
        m_renderService.lock()->getOrAddVtkTransform(m_transformId);
    }
    if(!m_parentId.empty())
    {
        m_parentTransform = m_renderService.lock()->getOrAddVtkTransform(m_parentId);
    }

    this->doUpdate();
    getTransform()->AddObserver( ::vtkCommand::ModifiedEvent, m_transformCommand );
}

//------------------------------------------------------------------------------

void Transform::updateFromVtk()
{
    vtkTransform* vtkTrf = this->getTransform();

    vtkTrf->RemoveObserver( m_transformCommand );
    ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();

    vtkMatrix4x4* mat = nullptr;

    if(m_parentTransform)
    {
        // Get the matrix before concatenation
        mat = vtkTrf->GetConcatenatedTransform(0)->GetMatrix();
    }
    else
    {
        mat = vtkTrf->GetMatrix();
    }

    {
        ::fwData::mt::ObjectWriteLock lock(trf);
        for(int lt = 0; lt < 4; lt++)
        {
            for(int ct = 0; ct < 4; ct++)
            {
                trf->setCoefficient(lt, ct, mat->GetElement(lt, ct));
            }
        }
    }

    auto sig = trf->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }

    vtkTrf->AddObserver( ::vtkCommand::ModifiedEvent, m_transformCommand );
}

//------------------------------------------------------------------------------

int Transform::getStartPriority()
{
    return -10;
}

//------------------------------------------------------------------------------

void Transform::doUpdate()
{
    vtkTransform* vtkTrf = this->getTransform();

    vtkTrf->RemoveObserver( m_transformCommand );
    ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();
    vtkMatrix4x4* mat = vtkMatrix4x4::New();

    {
        ::fwData::mt::ObjectReadLock lock(trf);
        for(int lt = 0; lt < 4; lt++)
        {
            for(int ct = 0; ct < 4; ct++)
            {
                mat->SetElement(lt, ct, trf->getCoefficient(lt, ct));
            }
        }
    }

    if(m_parentTransform)
    {
        vtkTrf->PostMultiply();
        vtkTrf->SetMatrix( mat );
        vtkTrf->Concatenate( m_parentTransform );
    }
    else
    {
        vtkTrf->SetMatrix(mat);
    }

    vtkTrf->Modified();
    vtkTrf->AddObserver( ::vtkCommand::ModifiedEvent, m_transformCommand );
    mat->Delete();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Transform::setTransform(vtkTransform* t)
{
    if ( m_transform != t )
    {
        if (m_transform)
        {
            m_transform->Delete();
        }
        if(t)
        {
            t->Register(nullptr);
        }
    }
    m_transform = t;
}

//------------------------------------------------------------------------------

vtkTransform* Transform::getTransform()
{
    vtkTransform* t = m_transform;
    if (t == 0)
    {
        t = this->IAdaptor::getTransform();
    }
    return t;
}

//------------------------------------------------------------------------------

void Transform::doSwap()
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Transform::doStop()
{
    this->getTransform()->RemoveObserver(m_transformCommand);
    this->unregisterServices();
}

} //namespace visuVTKAdaptor
