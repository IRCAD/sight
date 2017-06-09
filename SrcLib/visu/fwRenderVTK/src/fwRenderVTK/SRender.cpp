/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderVTK/SRender.hpp"

#include "fwRenderVTK/IVtkAdaptorService.hpp"
#include "fwRenderVTK/IVtkRenderWindowInteractorManager.hpp"
#include "fwRenderVTK/OffScreenInteractorManager.hpp"
#include "fwRenderVTK/vtk/InteractorStyle3DForNegato.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Color.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwRuntime/ConfigurationElementContainer.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/helper/Config.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <fwThread/Timer.hpp>

#include <fwTools/fwID.hpp>

#include <fwVtkIO/vtk.hpp>

#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>

#include <vtkCellPicker.h>
#include <vtkInstantiator.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkWindowToImageFilter.h>

#include <functional>

fwServicesRegisterMacro( ::fwRender::IRender, ::fwRenderVTK::SRender, ::fwData::Composite );

using namespace fwServices;

namespace fwRenderVTK
{
const ::fwCom::Signals::SignalKeyType SRender::s_DROPPED_SIG     = "dropped";
const ::fwCom::Slots::SlotKeyType SRender::s_RENDER_SLOT         = "render";
const ::fwCom::Slots::SlotKeyType SRender::s_REQUEST_RENDER_SLOT = "requestRender";

static const ::fwCom::Slots::SlotKeyType s_ADD_OBJECTS_SLOT    = "addObject";
static const ::fwCom::Slots::SlotKeyType s_CHANGE_OBJECTS_SLOT = "changeObject";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_OBJECTS_SLOT = "removeObjects";

//-----------------------------------------------------------------------------

SRender::SRender() throw() :
    m_pendingRenderRequest(false),
    m_renderMode(RenderMode::AUTO),
    m_width(1280),
    m_height(720),
    m_offScreen(false)
{
    newSignal<DroppedSignalType>(s_DROPPED_SIG);

    newSlot(s_RENDER_SLOT, &SRender::render, this);
    newSlot(s_REQUEST_RENDER_SLOT, &SRender::requestRender, this);
    newSlot(s_ADD_OBJECTS_SLOT, &SRender::addObjects, this);
    newSlot(s_CHANGE_OBJECTS_SLOT, &SRender::changeObjects, this);
    newSlot(s_REMOVE_OBJECTS_SLOT, &SRender::removeObjects, this);
}

//-----------------------------------------------------------------------------

SRender::~SRender() throw()
{
}

//-----------------------------------------------------------------------------

void SRender::configureRenderer( ConfigurationType conf )
{
    assert(conf->getName() == "renderer");

    std::string id         = conf->getAttributeValue("id");
    std::string background = conf->getAttributeValue("background");

    if(m_renderers.count(id) == 0)
    {
        m_renderers[id] = vtkRenderer::New();

//vtk depth peeling not available on android (Offscreen rendering issues)
#ifndef ANDROID
        m_renderers[id]->SetUseDepthPeeling( 1  );
        m_renderers[id]->SetMaximumNumberOfPeels( 8  );
        m_renderers[id]->SetOcclusionRatio( 0. );
#endif

        if(conf->hasAttribute("layer") )
        {
            int layer = ::boost::lexical_cast< int >(conf->getAttributeValue("layer"));
            m_renderers[id]->SetLayer(layer);
        }
    }

    if ( !background.empty() )
    {
        if(background[0] == '#')
        {
            ::fwData::Color::sptr color = ::fwData::Color::New();
            color->setRGBA(background);
            m_renderers[id]->SetBackground(color->getRefRGBA()[0], color->getRefRGBA()[1], color->getRefRGBA()[2]);
        }
        else
        {
            // compatibility with "old" color
            double color = ::boost::lexical_cast<double> (background);
            m_renderers[id]->SetBackground(color, color, color);
        }
    }
}

//-----------------------------------------------------------------------------

void SRender::configurePicker( ConfigurationType conf )
{
    assert(conf->getName() == "picker");

    std::string id       = conf->getAttributeValue("id");
    std::string vtkclass = conf->getAttributeValue("vtkclass");

    if (vtkclass.empty())
    {
        vtkclass = "vtkCellPicker";
    }

    if(m_pickers.count(id) == 0)
    {
        m_pickers[id] = vtkAbstractPropPicker::SafeDownCast(vtkInstantiator::CreateInstance(vtkclass.c_str()));
        OSLM_ASSERT("'" << vtkclass.c_str() << "' instantiation failled.", m_pickers[id]);
        m_pickers[id]->InitializePickList();
        m_pickers[id]->PickFromListOn();
        vtkPicker* picker = vtkPicker::SafeDownCast(m_pickers[id]);
        if (picker)
        {
            picker->SetTolerance(0);
        }
    }
}

//-----------------------------------------------------------------------------

void SRender::configureObject( ConfigurationType conf )
{
    assert(conf->getName() == "adaptor");
    ::fwData::Composite::sptr composite = this->getComposite();

    const std::string id            = conf->getAttributeValue("id");
    const std::string objectId      = conf->getAttributeValue("objectId");
    const std::string adaptor       = conf->getAttributeValue("class");
    const std::string uid           = conf->getAttributeValue("uid");
    const std::string autoConnect   = conf->getAttributeValue("autoConnect");
    const std::string compositeName = "self";

    SLM_ASSERT( "'id' required attribute missing or empty", !id.empty() );
    SLM_ASSERT( "'objectId' required attribute missing or empty", !objectId.empty() );
    SLM_ASSERT( "'adaptor' required attribute missing or empty", !adaptor.empty() );

    const size_t compositeObjectCount = composite->getContainer().count(objectId);

    OSLM_TRACE_IF(objectId << " not found in composite. If it exist, associated Adaptor will be destroyed",
                  !(compositeObjectCount == 1 || objectId == compositeName) );

    ::fwData::Object::csptr object;
    if (compositeObjectCount)
    {
        object = ::fwData::Object::dynamicCast(composite->getContainer()[objectId]);
    }
    else if (objectId == compositeName)
    {
        object = ::fwData::Object::dynamicCast(composite);
    }
    else if(this->isVersion2())
    {
        // Last chance with V2 behavior
        object = this->getInput< ::fwData::Object >(objectId);
        if(!object)
        {
            object = this->getInOut< ::fwData::Object >(objectId);
        }
    }

    if ( m_sceneAdaptors.count(id) == 0 && object )
    {
        OSLM_TRACE("Adding service : IVtkAdaptorService " << adaptor << " on "<< objectId );
        SceneAdaptor adaptee;
        adaptee.m_config = *(conf->begin());
        if (!uid.empty())
        {
            OSLM_TRACE("SRender::configureObject : uid = " << uid);
            adaptee.m_service = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >( object, adaptor, uid);
        }
        else
        {
            adaptee.m_service = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >( object, adaptor);
        }

        assert(adaptee.m_config->getName() == "config");
        assert(adaptee.getService());

        adaptee.getService()->setConfiguration(adaptee.m_config);
        adaptee.getService()->setAutoRender(m_renderMode == RenderMode::AUTO);
        adaptee.getService()->setRenderService(SRender::dynamicCast(this->shared_from_this()));
        adaptee.getService()->configure();
        adaptee.getService()->setName(id);

        if (!autoConnect.empty())
        {
            SLM_ASSERT("'autoConnect' attribut value must be 'yes' or 'no', actual: " + autoConnect,
                       autoConnect == "yes" || autoConnect == "no" );
            adaptee.getService()->setAutoConnect(autoConnect == "yes");
        }

        if (this->isStarted() || this->getStatus() == SWAPPING)
        {
            adaptee.getService()->start();
        }

        m_sceneAdaptors[id] = adaptee;
    }
    else if(m_sceneAdaptors.count(id) == 1)
    {
        SceneAdaptor& adaptee = m_sceneAdaptors[id];
        SLM_ASSERT("Adaptor service expired !", adaptee.getService() );
        OSLM_ASSERT( adaptee.getService()->getID() <<  " is not started ", adaptee.getService()->isStarted());
        if (object)
        {
            OSLM_TRACE("Swapping IVtkAdaptorService " << adaptor << " on "<< objectId );
            if(adaptee.getService()->getObject() != object)
            {
                adaptee.getService()->swap( ::fwData::Object::constCast(object) );
            }
            else
            {
                OSLM_WARN(adaptor << "'s object already is '"
                                  << adaptee.getService()->getObject()->getID()
                                  << "', no need to swap");
            }
        }
        else
        {
            adaptee.getService()->stop();
            ::fwServices::OSR::unregisterService(adaptee.getService());
            adaptee.m_service.reset();
            m_sceneAdaptors.erase(id);
        }
    }
    else
    {
        OSLM_TRACE( "'"<< objectId << "' inexistent, passing by '" << adaptor << "'");
    }
}

//-----------------------------------------------------------------------------

void SRender::configureVtkObject( ConfigurationType conf )
{
    assert(conf->getName() == "vtkObject");

    std::string id       = conf->getAttributeValue("id");
    std::string vtkClass = conf->getAttributeValue("class");
    assert( !id.empty() );
    assert( !vtkClass.empty() );

    if( m_vtkObjects.count(id) == 0 )
    {

        if ( vtkClass == "vtkTransform" && conf->size() == 1 )
        {
            m_vtkObjects[id] = createVtkTransform( conf );
        }
        else
        {
            m_vtkObjects[id] = vtkInstantiator::CreateInstance(vtkClass.c_str());
        }
    }
}

//-----------------------------------------------------------------------------

vtkTransform* SRender::createVtkTransform( ConfigurationType conf )
{
    SLM_ASSERT("vtkObject must be contain just only one sub xml element called vtkTransform.", conf->size() == 1 &&
               ( *conf->begin() )->getName() == "vtkTransform");

    ConfigurationType vtkTransformXmlElem = *conf->begin();

    vtkTransform* newMat = vtkTransform::New();

    for(    ::fwRuntime::ConfigurationElement::Iterator elem = vtkTransformXmlElem->begin();
            !(elem == vtkTransformXmlElem->end());
            ++elem )
    {
        SLM_ASSERT("The name of the xml element must be concatenate.", (*elem)->getName() == "concatenate" );

        std::string transformId = (*elem)->getValue();

        vtkTransform* mat = vtkTransform::SafeDownCast( getVtkObject(transformId) );

        if ( (*elem)->hasAttribute( "inverse" ) && (*elem)->getAttributeValue( "inverse" ) == "yes" )
        {
            newMat->Concatenate( mat->GetLinearInverse() );
        }
        else
        {
            newMat->Concatenate( mat );
        }

    }

    return newMat;
}

//-----------------------------------------------------------------------------

void SRender::addVtkObject( const VtkObjectIdType& _id, vtkObject* _vtkObj )
{
    SLM_ASSERT( "vtkObject id is empty", !_id.empty() );
    SLM_ASSERT( "vtkObject is NULL", _vtkObj );

    if( m_vtkObjects.count(_id) == 0 )
    {
        m_vtkObjects[_id] = _vtkObj;
    }
}

//-----------------------------------------------------------------------------

void SRender::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    SLM_FATAL_IF( "Depreciated tag \"win\" in configuration", m_configuration->findConfigurationElement("win") );

    std::vector < ::fwRuntime::ConfigurationElement::sptr > vectConfig = m_configuration->find("scene");
    SLM_ASSERT("Missing 'scene' configuration.", !vectConfig.empty());
    m_sceneConfiguration = vectConfig.at(0);

    m_offScreenImageKey = m_sceneConfiguration->getAttributeValue("offScreen");
    if (!m_offScreenImageKey.empty())
    {
        m_offScreen = true;
    }

    if (!m_offScreen)
    {
        this->initialize();
    }

    std::string renderMode = m_sceneConfiguration->getAttributeValue("renderMode");
    if (renderMode == "auto")
    {
        m_renderMode = RenderMode::AUTO;
    }
    else if (renderMode == "timer")
    {
        m_renderMode = RenderMode::TIMER;
    }
    else if (renderMode == "none")
    {
        m_renderMode = RenderMode::NONE;
    }
    else
    {
        SLM_WARN_IF("renderMode '" + renderMode + " is unknown, setting renderMode to 'auto'.", !renderMode.empty());
    }

    std::string widthKey = m_sceneConfiguration->getAttributeValue("width");
    if (!widthKey.empty())
    {
        m_width = ::boost::lexical_cast<unsigned int>(widthKey);
    }

    std::string heightKey = m_sceneConfiguration->getAttributeValue("height");
    if (!heightKey.empty())
    {
        m_height = ::boost::lexical_cast<unsigned int>(heightKey);
    }

    /// Target frame rate (default 30Hz)
    unsigned int targetFrameRate = 30;
    ::fwRuntime::ConfigurationElement::sptr fpsConfig = m_configuration->findConfigurationElement("fps");
    if(fpsConfig)
    {
        targetFrameRate = ::boost::lexical_cast< unsigned int >(fpsConfig->getValue());
    }

    if(m_renderMode == RenderMode::TIMER)
    {
        unsigned int timeStep = static_cast<unsigned int>( 1000.f / targetFrameRate );
        m_timer = m_associatedWorker->createTimer();

        ::fwThread::Timer::TimeDurationType duration = ::boost::chrono::milliseconds(timeStep);
        m_timer->setFunction( std::bind( &SRender::requestRender, this)  );
        m_timer->setDuration(duration);
    }
}

//-----------------------------------------------------------------------------

void SRender::starting() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if (!m_offScreen)
    {
        this->create();
    }

    this->startContext();

    // Instantiate vtk object, class...
    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = m_sceneConfiguration->begin(); iter != m_sceneConfiguration->end(); ++iter)
    {
        if ((*iter)->getName() == "renderer")
        {
            this->configureRenderer(*iter);
        }
        else if ((*iter)->getName() == "picker")
        {
            this->configurePicker(*iter);
        }
        else if ((*iter)->getName() == "adaptor")
        {
            this->configureObject(*iter);
        }
        else if ((*iter)->getName() == "vtkObject")
        {
            this->configureVtkObject(*iter);
        }
        else if((*iter)->getName() == "connect")
        {
            if((*iter)->hasAttribute("waitForKey"))
            {
                std::string key = (*iter)->getAttributeValue("waitForKey");

                if(this->isVersion2())
                {
                    auto object = this->getInput< ::fwData::Object >(key);
                    if(object)
                    {
                        this->manageConnection(key, object, *iter);
                    }
                }
                else
                {
                    ::fwData::Composite::sptr composite               = this->getComposite();
                    ::fwData::Composite::const_iterator iterComposite = composite->find(key);
                    if(iterComposite != composite->end())
                    {
                        this->manageConnection(key, iterComposite->second, *iter);

                    }
                }
                m_connect.push_back(*iter);
            }
            else
            {
                ::fwServices::helper::Config::createConnections(*iter, m_connections);
            }
        }
        else if((*iter)->getName() == "proxy")
        {
            if((*iter)->hasAttribute("waitForKey"))
            {
                std::string key = (*iter)->getAttributeValue("waitForKey");

                if(this->isVersion2())
                {
                    auto object = this->getInput< ::fwData::Object >(key);
                    if(object)
                    {
                        this->manageProxy(key, object, *iter);
                    }
                }
                else
                {
                    ::fwData::Composite::sptr composite               = this->getComposite();
                    ::fwData::Composite::const_iterator iterComposite = composite->find(key);
                    if(iterComposite != composite->end())
                    {
                        this->manageProxy(key, iterComposite->second, *iter);
                    }
                }
                m_proxies.push_back(*iter);
            }
            else
            {
                ::fwServices::helper::Config::createProxy("self", *iter, m_proxyMap);
            }
        }
        else
        {
            OSLM_ASSERT("Bad scene configurationType, unknown xml node : " << (*iter)->getName(), false);
        }
    }

    m_interactorManager->getInteractor()->GetRenderWindow()->SetNumberOfLayers(static_cast<int>(m_renderers.size()));
    for( RenderersMapType::iterator iter = m_renderers.begin(); iter != m_renderers.end(); ++iter )
    {
        vtkRenderer* renderer = (*iter).second;
        m_interactorManager->getInteractor()->GetRenderWindow()->AddRenderer(renderer);
    }

    // Start adaptors according to their starting priority
    std::vector< SPTR(IVtkAdaptorService) > startAdaptors;

    for(auto& sceneAdaptor : m_sceneAdaptors)
    {
        startAdaptors.emplace_back(sceneAdaptor.second.getService());
    }

    std::sort(startAdaptors.begin(), startAdaptors.end(),
              [](const SPTR(IVtkAdaptorService)& a, const SPTR(IVtkAdaptorService)& b)
        {
            return b->getStartPriority() > a->getStartPriority();
        });

    for(auto& adaptor : startAdaptors)
    {
        adaptor->start();
        SLM_ASSERT("Adaptor is not started", adaptor->isStarted());
    }

    if(m_timer)
    {
        m_timer->start();
    }
}

//-----------------------------------------------------------------------------

void SRender::stopping() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    m_connections.disconnect();

    if(this->isVersion2())
    {
        ConstObjectMapType container;
        for(auto obj : this->getInputs())
        {
            if(obj.first != s_DEFAULT_OBJECT)
            {
                container[obj.first] = obj.second.lock();
            }
        }
        for(auto obj : this->getInOuts())
        {
            if(obj.first != s_DEFAULT_OBJECT)
            {
                container[obj.first] = obj.second.lock();
            }
        }
        this->disconnect(container);
        ::fwServices::helper::Config::disconnectProxies("self", m_proxyMap);
    }
    else
    {
        this->disconnect(this->getComposite()->getContainer());
    }

    if(m_timer)
    {
        m_timer->stop();
    }

    // Stop adaptors in the reverse order of their starting priority
    std::vector< SPTR(IVtkAdaptorService) > stopAdaptors;

    for(auto& sceneAdaptor : m_sceneAdaptors)
    {
        stopAdaptors.emplace_back(sceneAdaptor.second.getService());
    }

    std::sort(stopAdaptors.begin(), stopAdaptors.end(),
              [](const SPTR(IVtkAdaptorService)& a, const SPTR(IVtkAdaptorService)& b)
        {
            return b->getStartPriority() < a->getStartPriority();
        });

    for(auto& adaptor : stopAdaptors)
    {
        adaptor->stop();
        SLM_ASSERT("Adaptor is not stopped", adaptor->isStopped());
        ::fwServices::OSR::unregisterService(adaptor);
    }
    stopAdaptors.clear();
    m_sceneAdaptors.clear();

    this->stopContext();

    if (!m_offScreen)
    {
        this->destroy();
    }
}

//------------------------------------------------------------------------------

void SRender::configureObjects(::fwData::Composite::ContainerType objects)
{
    SLM_ASSERT("Scene configuration is not defined",  m_sceneConfiguration );
    SLM_ASSERT("This should not be called in a AppXml2 application",  !this->isVersion2() );

    for( ::fwData::Composite::ContainerType::value_type objectId :  objects)
    {
        std::vector< ConfigurationType > confVec = m_sceneConfiguration->find("adaptor", "objectId", objectId.first);
        for( ConfigurationType cfg :  confVec )
        {
            this->configureObject(cfg);
        }
    }
}

//------------------------------------------------------------------------------

void SRender::addObjects(::fwData::Composite::ContainerType objects)
{
    this->configureObjects(objects);
    this->connectAfterWait(objects);
}

//------------------------------------------------------------------------------

void SRender::changeObjects(::fwData::Composite::ContainerType newObjects,
                            ::fwData::Composite::ContainerType oldObjects)
{
    this->disconnect(oldObjects);
    this->configureObjects(newObjects);
    this->connectAfterWait(newObjects);
}

//------------------------------------------------------------------------------

void SRender::removeObjects(::fwData::Composite::ContainerType objects)
{
    this->disconnect(objects);
    this->configureObjects(objects);
}

//-----------------------------------------------------------------------------

void SRender::updating() throw(fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SRender::swapping(const IService::KeyType& key) throw(::fwTools::Failed)
{
    if (this->isVersion2())
    {
        // remove connections
        auto iter = m_objectConnections.find(key);
        if(iter != m_objectConnections.end())
        {
            iter->second.disconnect();
            m_objectConnections.erase(key);
        }
        ::fwServices::helper::Config::disconnectProxies(key, m_proxyMap);
    }

    std::vector< ConfigurationType > confVec = m_sceneConfiguration->find("adaptor", "objectId", key);
    for( ConfigurationType cfg : confVec )
    {
        this->configureObject(cfg);
    }

    // create connections
    this->connectAfterWait(key);
}

//-----------------------------------------------------------------------------

void SRender::render()
{
    OSLM_ASSERT("Scene must be started", this->isStarted());
    if (m_offScreen)
    {
        vtkSmartPointer<vtkRenderWindow> renderWindow = m_interactorManager->getInteractor()->GetRenderWindow();

        renderWindow->Render();

        vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkWindowToImageFilter::New();
        windowToImageFilter->SetInputBufferTypeToRGBA();
        windowToImageFilter->SetInput( renderWindow );
        windowToImageFilter->Update();

        vtkImageData* vtkImage = windowToImageFilter->GetOutput();
        ::fwData::Image::sptr image;
        if (!this->isVersion2())
        {
            ::fwData::Composite::sptr composite = this->getComposite();
            image                               = composite->at< ::fwData::Image >(m_offScreenImageKey);
        }
        else
        {
            image = this->getInOut< ::fwData::Image >(m_offScreenImageKey);
        }
        SLM_ASSERT("Image '" + m_offScreenImageKey + "' not found.", image);

        {
            ::fwData::mt::ObjectWriteLock lock(image);
            ::fwVtkIO::fromVTKImage(vtkImage, image);
        }

        auto sig = image->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }

        // If we don't do explicitly, the filter is not destroyed and this leads to a huge memory leak
        windowToImageFilter->Delete();
    }
    else
    {
        m_interactorManager->getInteractor()->Render();
    }
    this->setPendingRenderRequest(false);
}

//-----------------------------------------------------------------------------

bool SRender::isShownOnScreen()
{
    if (!m_offScreen)
    {
        return this->getContainer()->isShownOnScreen();
    }
    else
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------

void SRender::requestRender()
{
    if ( this->isShownOnScreen() && !this->getPendingRenderRequest())
    {
        this->setPendingRenderRequest(true);
        this->slot(SRender::s_RENDER_SLOT)->asyncRun();
    }
}

//-----------------------------------------------------------------------------

void SRender::startContext()
{
    if (!m_offScreen)
    {
        m_interactorManager = ::fwRenderVTK::IVtkRenderWindowInteractorManager::createManager();
        m_interactorManager->installInteractor( this->getContainer() );
    }
    else
    {
        ::fwRenderVTK::OffScreenInteractorManager::sptr interactorManager =
            ::fwRenderVTK::OffScreenInteractorManager::New();
        interactorManager->installInteractor(m_width, m_height);
        m_interactorManager = interactorManager;
    }

    InteractorStyle3DForNegato* interactor = InteractorStyle3DForNegato::New();
    SLM_ASSERT("Can't instantiate interactor", interactor);
    interactor->setAutoRender(m_renderMode == RenderMode::AUTO);
    m_interactorManager->getInteractor()->SetInteractorStyle( interactor );

    m_interactorManager->setRenderService(this->getSptr());

#ifndef __linux
    m_interactorManager->getInteractor()->GetRenderWindow()->SetAlphaBitPlanes(1);
    m_interactorManager->getInteractor()->GetRenderWindow()->SetMultiSamples(0);
#endif

}

//-----------------------------------------------------------------------------

void SRender::stopContext()
{
    SLM_TRACE_FUNC();

    for( RenderersMapType::iterator iter = m_renderers.begin(); iter != m_renderers.end(); ++iter )
    {
        vtkRenderer* renderer = iter->second;
        renderer->InteractiveOff();
        m_interactorManager->getInteractor()->GetRenderWindow()->RemoveRenderer(renderer);
        renderer->Delete();
    }

    m_renderers.clear();

    m_interactorManager->uninstallInteractor();
    m_interactorManager.reset();
}

//-----------------------------------------------------------------------------

vtkRenderer* SRender::getRenderer(RendererIdType rendererId)
{
    OSLM_ASSERT("Renderer not found : '" << rendererId << "'", m_renderers.count(rendererId) == 1);

    return m_renderers[rendererId];
}

//-----------------------------------------------------------------------------

vtkAbstractPropPicker* SRender::getPicker(PickerIdType pickerId)
{
    PickersMapType::const_iterator iter = m_pickers.find(pickerId);
    if ( iter == m_pickers.end())
    {
        SLM_WARN("Picker '" + pickerId + "' not found");
        return nullptr;
    }
    return iter->second;
}

//-----------------------------------------------------------------------------

vtkObject* SRender::getVtkObject(const VtkObjectIdType& objectId) const
{
    VtkObjectMapType::const_iterator iter = m_vtkObjects.find(objectId);
    if ( iter == m_vtkObjects.end())
    {
        SLM_WARN("vtkObject '" + objectId + "' not found");
        return nullptr;
    }
    return iter->second;
}

//-----------------------------------------------------------------------------

SPTR(IVtkAdaptorService) SRender::getAdaptor(const SRender::AdaptorIdType& adaptorId) const
{
    IVtkAdaptorService::sptr adaptor;
    SceneAdaptorsMapType::const_iterator it = m_sceneAdaptors.find(adaptorId);

    if ( it != m_sceneAdaptors.end() )
    {
        adaptor = it->second.getService();
    }
    else
    {
        SLM_WARN("adaptor '" + adaptorId + "' not found");
    }

    return adaptor;
}

//-----------------------------------------------------------------------------

vtkTransform* SRender::getOrAddVtkTransform( const VtkObjectIdType& _id )
{
    vtkTransform* t = vtkTransform::SafeDownCast(getVtkObject(_id));
    if(t == 0)
    {
        t = vtkTransform::New();
        this->addVtkObject(_id, t);
    }
    return t;
}

//-----------------------------------------------------------------------------

void SRender::connectAfterWait(::fwData::Composite::ContainerType objects)
{
    for(::fwData::Composite::value_type element :  objects)
    {
        for(const ::fwRuntime::ConfigurationElement::sptr& connect :  m_connect)
        {
            this->manageConnection(element.first, element.second, connect);
        }
        for(const ::fwRuntime::ConfigurationElement::sptr& proxy :  m_proxies)
        {
            this->manageProxy(element.first, element.second, proxy);
        }
    }
}

//-----------------------------------------------------------------------------

void SRender::connectAfterWait(const std::string& key)
{
    if (this->isVersion2())
    {
        ::fwData::Object::csptr obj;
        obj = this->getInput< ::fwData::Object >(key);
        if(!obj)
        {
            obj = this->getInOut< ::fwData::Object >(key);
        }
        if (obj)
        {
            for(const ::fwRuntime::ConfigurationElement::sptr& connect :  m_connect)
            {
                this->manageConnection(key, obj, connect);
            }
            for(const ::fwRuntime::ConfigurationElement::sptr& proxy :  m_proxies)
            {
                this->manageProxy(key, obj, proxy);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SRender::manageConnection(const std::string& key, const ::fwData::Object::csptr& obj,
                               const ConfigurationType& config)
{
    if(config->hasAttribute("waitForKey"))
    {
        std::string waitForKey = config->getAttributeValue("waitForKey");
        if(waitForKey == key)
        {
            ::fwServices::helper::Config::createConnections(config, m_objectConnections[key], obj);
        }
    }
}

//-----------------------------------------------------------------------------

void SRender::manageProxy(const std::string& key, const ::fwData::Object::csptr& obj,
                          const ConfigurationType& config)
{
    if(config->hasAttribute("waitForKey"))
    {
        std::string waitForKey = config->getAttributeValue("waitForKey");
        if(waitForKey == key)
        {
            ::fwServices::helper::Config::createProxy(key, config, m_proxyMap, obj);
        }
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SRender::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_ADD_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_CHANGE_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_REMOVE_OBJECTS_SLOT ) );

    return connections;
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SRender::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_DEFAULT_OBJECT, ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_ADD_OBJECTS_SLOT );
    connections.push( s_DEFAULT_OBJECT, ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_CHANGE_OBJECTS_SLOT );
    connections.push( s_DEFAULT_OBJECT, ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_REMOVE_OBJECTS_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

::fwData::Composite::sptr SRender::getComposite()
{
    if(this->isVersion2())
    {
        return this->getInOut< ::fwData::Composite >(s_DEFAULT_OBJECT);
    }
    else
    {
        return this->getObject< ::fwData::Composite >();
    }
}

//-----------------------------------------------------------------------------

} //namespace fwRenderVTK
