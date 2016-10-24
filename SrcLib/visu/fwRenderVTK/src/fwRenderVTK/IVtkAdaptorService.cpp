/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderVTK/IVtkAdaptorService.hpp"

#include <fwData/String.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTools/fwID.hpp>

#include <vtkAbstractPropPicker.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>



namespace fwRenderVTK
{

IVtkAdaptorService::IVtkAdaptorService() throw()
    : m_comChannelPriority(0.5),
      m_vtkPipelineModified(true),
      m_rendererId ("default"),
      m_pickerId   (""), // by default no Picker
      m_transformId   (""), // by default no Transform
      m_propCollection ( vtkPropCollection::New() ),
      m_autoRender(true),
      m_autoConnect(true)
{
}

IVtkAdaptorService::~IVtkAdaptorService() throw()
{
    m_propCollection->Delete();
}

void IVtkAdaptorService::info(std::ostream& _sstream )
{
    _sstream << "IVtkAdaptorService : ";
    this->SuperClass::info( _sstream );
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::configuring() throw(fwTools::Failed)
{
    this->setPickerId    ( m_configuration->getAttributeValue ( "picker"    ) );
    this->setRenderId    ( m_configuration->getAttributeValue ( "renderer"  ) );
    this->setTransformId ( m_configuration->getAttributeValue ( "transform" ) );
    doConfigure();
}

//------------------------------------------------------------------------------


void IVtkAdaptorService::starting() throw(fwTools::Failed)
{
    /// Install observation
    if (m_autoConnect)
    {
        m_connections.connect(this->getObject(), this->getSptr(), this->getObjSrvConnections());
    }
    assert( m_renderService.lock() );

    doStart();
    requestRender();
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::stopping() throw(fwTools::Failed)
{
    /// Stop observation
    m_connections.disconnect();
    doStop();
    requestRender();
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::swapping() throw(fwTools::Failed)
{
    m_connections.disconnect();
    if (m_autoConnect)
    {
        m_connections.connect(this->getObject(), this->getSptr(), this->getObjSrvConnections());
    }
    doSwap();
    requestRender();
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::updating() throw(fwTools::Failed)
{
    doUpdate();
    requestRender();
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::setRenderService( SRender::sptr service)
{
    /// Preconditions
    SLM_ASSERT("service not instanced", service);
    assert( this->isStopped() );

    m_renderService = service;
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::setRenderId(SRender::RendererIdType newID)
{
    m_rendererId = newID;
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::setVtkPipelineModified()
{
    m_vtkPipelineModified = true;
}

//------------------------------------------------------------------------------

int IVtkAdaptorService::getStartPriority()
{
    return 0;
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::requestRender()
{
    if ( this->getRenderService()->isStarted() && this->getRenderService()->isShownOnScreen()
         && m_vtkPipelineModified && m_autoRender )
    {
        if ( !this->getRenderService()->getPendingRenderRequest())
        {
            this->getRenderService()->setPendingRenderRequest(true);
            this->getRenderService()->slot(SRender::s_RENDER_SLOT)->asyncRun();
        }
        m_vtkPipelineModified = false;
    }
}

//------------------------------------------------------------------------------

SRender::RendererIdType IVtkAdaptorService::getRenderId() const
{
    return m_rendererId;
}

//------------------------------------------------------------------------------

SRender::sptr IVtkAdaptorService:: getRenderService() const
{
    return m_renderService.lock();
}

//------------------------------------------------------------------------------

vtkRenderer* IVtkAdaptorService::getRenderer()
{
    return m_renderService.lock()->getRenderer(m_rendererId);
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::setPickerId(SRender::PickerIdType newID)
{
    m_pickerId = newID;
}

//------------------------------------------------------------------------------

SRender::PickerIdType IVtkAdaptorService::getPickerId() const
{
    return m_pickerId;
}

//------------------------------------------------------------------------------

vtkAbstractPropPicker* IVtkAdaptorService::getPicker(std::string pickerId)
{
    if (pickerId.empty())
    {
        pickerId = m_pickerId;
    }
    return m_renderService.lock()->getPicker(pickerId);
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::setTransformId(SRender::VtkObjectIdType newID)
{
    m_transformId = newID;
}

//------------------------------------------------------------------------------

SRender::VtkObjectIdType IVtkAdaptorService::getTransformId() const
{
    return m_transformId;
}

//------------------------------------------------------------------------------

vtkTransform* IVtkAdaptorService::getTransform()
{
    return vtkTransform::SafeDownCast(m_renderService.lock()->getVtkObject(m_transformId));
}

//------------------------------------------------------------------------------

vtkObject* IVtkAdaptorService::getVtkObject(const SRender::VtkObjectIdType& objectId) const
{
    if (!objectId.empty())
    {
        return m_renderService.lock()->getVtkObject(objectId);
    }
    return nullptr;
}

//------------------------------------------------------------------------------

vtkRenderWindowInteractor* IVtkAdaptorService::getInteractor()
{
    return this->getRenderer()->GetRenderWindow()->GetInteractor();
}

//------------------------------------------------------------------------------

::fwData::Object::sptr IVtkAdaptorService::getAssociatedObject(vtkProp* prop, int depth)
{
    ::fwData::Object::sptr obj;

    if (prop)
    {
        if ( m_propCollection->IsItemPresent(prop) )
        {
            obj = this->getObject();
        }
        else
        {
            ::fwData::Object::sptr res;
            for( ServiceVector::value_type service :  m_subServices)
            {
                if(!service.expired())
                {
                    res = service.lock()->getAssociatedObject(prop, depth - 1 );
                    if (res)
                    {
                        break;
                    }
                }
            }
            obj = ( res && depth == 0 ) ? this->getObject() : res;
        }
    }
    return obj;
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::registerService( ::fwRenderVTK::IVtkAdaptorService::sptr service)
{
    m_subServices.push_back(service);
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::unregisterServices()
{
    for(ServiceVector::value_type service : m_subServices)
    {
        if(!service.expired())
        {
            service.lock()->stop();
            ::fwServices::OSR::unregisterService(service.lock());
        }
    }
    m_subServices.clear();
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::registerProp(vtkProp* prop)
{
    getProps(m_propCollection, prop);
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::getProps(vtkPropCollection* propc, vtkProp* prop)
{
    int initSize = propc->GetNumberOfItems();

    prop->GetActors(propc);
    prop->GetActors2D(propc);
    prop->GetVolumes(propc);

    if (initSize == propc->GetNumberOfItems())
    {
        propc->AddItem(prop);
    }
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::getAllSubProps(vtkPropCollection* propc, int depth)
{
    vtkProp* prop;

    m_propCollection->InitTraversal();
    while ( (prop = m_propCollection->GetNextProp()) )
    {
        getProps(propc, prop);
    }

    if(depth != 0)
    {
        for( ServiceVector::value_type service :  m_subServices)
        {
            if(!service.expired())
            {
                service.lock()->getAllSubProps( propc, depth - 1 );
            }
        }
    }
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::unregisterProps()
{
    m_propCollection->RemoveAllItems();
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::addToRenderer(vtkProp* prop)
{
    this->registerProp(prop);
    this->getRenderer()->AddViewProp(prop);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::addToPicker(vtkProp* prop, std::string pickerId)
{
    OSLM_ASSERT("Picker '"<< pickerId << "' undefined.", this->getPicker(pickerId));
    this->getPicker(pickerId)->AddPickList(prop);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::removeFromPicker(vtkProp* prop, std::string pickerId)
{
    OSLM_ASSERT("Picker '"<< pickerId << "' undefined.", this->getPicker(pickerId));
    this->getPicker(pickerId)->DeletePickList(prop);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void IVtkAdaptorService::removeAllPropFromRenderer()
{
    vtkPropCollection* propc = m_propCollection;
    vtkProp* prop;

    propc->InitTraversal();
    while ( (prop = propc->GetNextProp()) )
    {
        this->getRenderer()->RemoveViewProp(prop);
    }
    this->unregisterProps();
    this->setVtkPipelineModified();
}


} // namespace fwRenderVTK

