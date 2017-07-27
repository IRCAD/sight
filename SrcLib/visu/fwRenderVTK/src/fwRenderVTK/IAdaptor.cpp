/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderVTK/IAdaptor.hpp"

#include "fwRenderVTK/registry/adaptors.hpp"

#include <fwData/String.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTools/fwID.hpp>

#include <vtkAbstractPropPicker.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>

namespace fwRenderVTK
{

IAdaptor::IAdaptor() noexcept :
    m_vtkPipelineModified(true),
    m_rendererId("default"),
    m_pickerId(""),
    // by default no Picker
    m_transformId(""),
    // by default no Transform
    m_propCollection( vtkPropCollection::New() ),
    m_autoRender(true)
{
}

IAdaptor::~IAdaptor() noexcept
{
    m_propCollection->Delete();
}

//------------------------------------------------------------------------------

void IAdaptor::configureParams()
{
    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");
    this->setPickerId(config.get<std::string>("picker", ""));
    this->setRendererId(config.get<std::string>("renderer", ""));
    this->setTransformId(config.get<std::string>("transform", ""));
}

//------------------------------------------------------------------------------

void IAdaptor::configuring()
{
    SLM_ERROR("configuring() method must be implemented for '" + this->getClassname() + "'");
    this->configureParams();
}

//------------------------------------------------------------------------------

void IAdaptor::starting()
{
    SLM_ERROR("starting() method must be implemented for '" + this->getClassname() + "'");
    this->initialize();
}

//------------------------------------------------------------------------------

void IAdaptor::stopping()
{
    SLM_ERROR("stopping() method must be implemented for '" + this->getClassname() + "'");
}

//------------------------------------------------------------------------------

void IAdaptor::updating()
{
    SLM_ERROR("updating() method must be implemented for '" + this->getClassname() + "'");
}

//------------------------------------------------------------------------------

void IAdaptor::initialize()
{
    if(m_renderService.expired())
    {
        // retrieve the SRender service associated to this adaptor
        const auto servicesVector = ::fwServices::OSR::getServices("::fwRenderVTK::SRender");

        const auto& registry        = ::fwRenderVTK::registry::getAdaptorRegistry();
        const auto& renderServiceId = registry.at(this->getID());

        auto result =
            std::find_if(servicesVector.begin(), servicesVector.end(),
                         [renderServiceId](const ::fwServices::IService::sptr& srv)
            {
                return srv->getID() == renderServiceId;
            });
        SLM_ASSERT("Can't find '" + renderServiceId + "' SRender service.", result != servicesVector.end());

        ::fwRenderVTK::SRender::sptr renderService = ::fwRenderVTK::SRender::dynamicCast(*result);
        m_autoRender                               = (renderService->getRenderMode() == SRender::RenderMode::AUTO);
        m_renderService                            = renderService;
    }
}

//------------------------------------------------------------------------------

void IAdaptor::setRenderService( SRender::sptr service)
{
    /// Preconditions
    SLM_ASSERT("service not instanced", service);

    m_renderService = service;
}

//------------------------------------------------------------------------------

void IAdaptor::setRendererId(SRender::RendererIdType newID)
{
    m_rendererId = newID;
}

//------------------------------------------------------------------------------

void IAdaptor::setVtkPipelineModified()
{
    m_vtkPipelineModified = true;
}

//------------------------------------------------------------------------------

int IAdaptor::getStartPriority()
{
    return 0;
}

//------------------------------------------------------------------------------

void IAdaptor::requestRender()
{
    if ( (this->getRenderService()->getStatus() == ::fwServices::IService::STARTED ||
          this->getRenderService()->getStatus() == ::fwServices::IService::SWAPPING)
         && this->getRenderService()->isShownOnScreen()
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

SRender::RendererIdType IAdaptor::getRendererId() const
{
    return m_rendererId;
}

//------------------------------------------------------------------------------

SRender::sptr IAdaptor:: getRenderService() const
{
    SLM_ASSERT("SRender service is not initialized, initialize() method must be called first",
               !m_renderService.expired());
    return m_renderService.lock();
}

//------------------------------------------------------------------------------

vtkRenderer* IAdaptor::getRenderer()
{
    return m_renderService.lock()->getRenderer(m_rendererId);
}

//------------------------------------------------------------------------------

void IAdaptor::setPickerId(SRender::PickerIdType newID)
{
    m_pickerId = newID;
}

//------------------------------------------------------------------------------

SRender::PickerIdType IAdaptor::getPickerId() const
{
    return m_pickerId;
}

//------------------------------------------------------------------------------

vtkAbstractPropPicker* IAdaptor::getPicker(std::string pickerId)
{
    if (pickerId.empty())
    {
        pickerId = m_pickerId;
    }
    return m_renderService.lock()->getPicker(pickerId);
}

//------------------------------------------------------------------------------

void IAdaptor::setTransformId(SRender::VtkObjectIdType newID)
{
    m_transformId = newID;
}

//------------------------------------------------------------------------------

SRender::VtkObjectIdType IAdaptor::getTransformId() const
{
    return m_transformId;
}

//------------------------------------------------------------------------------

vtkTransform* IAdaptor::getTransform()
{
    return vtkTransform::SafeDownCast(m_renderService.lock()->getVtkObject(m_transformId));
}

//------------------------------------------------------------------------------

vtkObject* IAdaptor::getVtkObject(const SRender::VtkObjectIdType& objectId) const
{
    if (!objectId.empty())
    {
        return m_renderService.lock()->getVtkObject(objectId);
    }
    return nullptr;
}

//------------------------------------------------------------------------------

vtkRenderWindowInteractor* IAdaptor::getInteractor()
{
    return this->getRenderer()->GetRenderWindow()->GetInteractor();
}

//------------------------------------------------------------------------------

::fwData::Object::sptr IAdaptor::getAssociatedObject(vtkProp* prop, int depth)
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

::fwRenderVTK::IAdaptor::sptr IAdaptor::createSubAdaptor(const std::string& type,
                                                         ::fwServices::IService::Config& config)
{
    ::fwServices::registry::ServiceFactory::sptr srvFactory = ::fwServices::registry::ServiceFactory::getDefault();

    ::fwServices::IService::sptr srv = srvFactory->create(type);
    FW_RAISE_IF("Service of type '" + type + "' cannot be instantiated.", !srv);

    ::fwRenderVTK::IAdaptor::sptr adaptor = ::fwRenderVTK::IAdaptor::dynamicCast(srv);
    FW_RAISE_IF("Service of type '"+ type + "' is not an adaptor", !adaptor);

    m_subServices.push_back(adaptor);

    config.m_type              = type;
    config.m_globalAutoConnect = false;
    config.m_config            = ::fwRuntime::EConfigurationElement::New("service");

    return adaptor;
}

//------------------------------------------------------------------------------

void IAdaptor::registerServiceInput(const ::fwData::Object::csptr& obj,
                                    const std::string& key,
                                    const ::fwRenderVTK::IAdaptor::sptr& srv,
                                    const bool autoConnect,
                                    ::fwServices::IService::Config& config)
{
    ::fwServices::OSR::registerServiceInput(obj, key, srv);

    ObjectServiceConfig objConfig;
    objConfig.m_key         = key;
    objConfig.m_access      = AccessType::INPUT;
    objConfig.m_autoConnect = autoConnect;
    objConfig.m_optional    = false;

    config.m_objects.push_back(objConfig);
}

//------------------------------------------------------------------------------

void IAdaptor::registerServiceInOut(const ::fwData::Object::sptr& obj,
                                    const std::string& key,
                                    const ::fwRenderVTK::IAdaptor::sptr& srv,
                                    const bool autoConnect,
                                    ::fwServices::IService::Config& config)
{
    ::fwServices::OSR::registerService(obj, key, AccessType::INOUT, srv);

    ObjectServiceConfig objConfig;
    objConfig.m_key         = key;
    objConfig.m_access      = AccessType::INOUT;
    objConfig.m_autoConnect = autoConnect;
    objConfig.m_optional    = false;

    config.m_objects.push_back(objConfig);
}

//------------------------------------------------------------------------------

void IAdaptor::registerService( ::fwRenderVTK::IAdaptor::sptr service)
{
    m_subServices.push_back(service);
}

//------------------------------------------------------------------------------

void IAdaptor::unregisterService( ::fwRenderVTK::IAdaptor::sptr service)
{
    auto iter = std::find_if(m_subServices.begin(), m_subServices.end(),
                             [ = ](const ::fwRenderVTK::IAdaptor::wptr& adaptor)
        {
            return adaptor.lock() == service;
        });

    SLM_ASSERT("service '" + service->getID() + "' is not registered", iter != m_subServices.end());
    m_subServices.erase(iter);

    service->stop();
    ::fwServices::OSR::unregisterService(service);
}

//------------------------------------------------------------------------------

void IAdaptor::unregisterServices()
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

void IAdaptor::registerProp(vtkProp* prop)
{
    getProps(m_propCollection, prop);
}

//------------------------------------------------------------------------------

void IAdaptor::getProps(vtkPropCollection* propc, vtkProp* prop)
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

void IAdaptor::getAllSubProps(vtkPropCollection* propc, int depth)
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

void IAdaptor::unregisterProps()
{
    m_propCollection->RemoveAllItems();
}

//------------------------------------------------------------------------------

void IAdaptor::addToRenderer(vtkProp* prop)
{
    this->registerProp(prop);
    this->getRenderer()->AddViewProp(prop);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void IAdaptor::addToPicker(vtkProp* prop, std::string pickerId)
{
    OSLM_ASSERT("Picker '"<< pickerId << "' undefined.", this->getPicker(pickerId));
    this->getPicker(pickerId)->AddPickList(prop);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void IAdaptor::removeFromPicker(vtkProp* prop, std::string pickerId)
{
    OSLM_ASSERT("Picker '"<< pickerId << "' undefined.", this->getPicker(pickerId));
    this->getPicker(pickerId)->DeletePickList(prop);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void IAdaptor::removeAllPropFromRenderer()
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

