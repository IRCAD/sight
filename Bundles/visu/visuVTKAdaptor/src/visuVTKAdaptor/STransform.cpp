/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/STransform.hpp"

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

    static TransformCallback* New(::visuVTKAdaptor::STransform* adaptor)
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

    ::visuVTKAdaptor::STransform* m_adaptor;
};

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::STransform);

namespace visuVTKAdaptor
{

const ::fwServices::IService::KeyType STransform::s_TM3D_INOUT = "tm3d";

//------------------------------------------------------------------------------

STransform::STransform() noexcept :
    m_transform(nullptr),
    m_transformCommand(TransformCallback::New(this))
{
}

//------------------------------------------------------------------------------

STransform::~STransform() noexcept
{
    if( m_transformCommand )
    {
        m_transformCommand->Delete();
    }
    m_transformCommand = nullptr;
}

//------------------------------------------------------------------------------

void STransform::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_parentId = config.get< std::string >("parent", "");
}

//------------------------------------------------------------------------------

void STransform::starting()
{
    this->initialize();

    if(!m_transformId.empty())
    {
        m_renderService.lock()->getOrAddVtkTransform(m_transformId);
    }
    if(!m_parentId.empty())
    {
        m_parentTransform = m_renderService.lock()->getOrAddVtkTransform(m_parentId);
    }

    this->updating();
    this->getTransform()->AddObserver( ::vtkCommand::ModifiedEvent, m_transformCommand );
}

//------------------------------------------------------------------------------

void STransform::updateFromVtk()
{
    vtkTransform* vtkTrf = this->getTransform();

    vtkTrf->RemoveObserver( m_transformCommand );
    ::fwData::TransformationMatrix3D::sptr trf = this->getInOut< ::fwData::TransformationMatrix3D >(s_TM3D_INOUT);
    SLM_ASSERT("matrix '" + s_TM3D_INOUT + "' is not defined", trf);

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
                trf->setCoefficient(static_cast<size_t>(lt), static_cast<size_t>(ct), mat->GetElement(lt, ct));
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

void STransform::updating()
{
    vtkTransform* vtkTrf = this->getTransform();

    vtkTrf->RemoveObserver( m_transformCommand );
    ::fwData::TransformationMatrix3D::sptr trf = this->getInOut< ::fwData::TransformationMatrix3D >(s_TM3D_INOUT);
    SLM_ASSERT("matrix '" + s_TM3D_INOUT + "' is not defined", trf);

    vtkMatrix4x4* mat = vtkMatrix4x4::New();

    {
        ::fwData::mt::ObjectReadLock lock(trf);
        for(int lt = 0; lt < 4; lt++)
        {
            for(int ct = 0; ct < 4; ct++)
            {
                mat->SetElement(lt, ct, trf->getCoefficient(static_cast<size_t>(lt), static_cast<size_t>(ct)));
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
    this->requestRender();
}

//------------------------------------------------------------------------------

void STransform::setTransform(vtkTransform* t)
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

vtkTransform* STransform::getTransform()
{
    vtkTransform* t = m_transform;
    if (t == 0)
    {
        t = this->IAdaptor::getTransform();
    }
    return t;
}

//------------------------------------------------------------------------------

void STransform::swapping()
{
    this->updating();
}

//------------------------------------------------------------------------------

void STransform::stopping()
{
    this->getTransform()->RemoveObserver(m_transformCommand);
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap STransform::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_TM3D_INOUT, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

} //namespace visuVTKAdaptor
