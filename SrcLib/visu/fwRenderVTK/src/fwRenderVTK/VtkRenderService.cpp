/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>

#include <vtkActor.h>
#include <vtkCellPicker.h>
#include <vtkFrustumCoverageCuller.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLight.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkInstantiator.h>
#include <vtkTransform.h>
#include <vtkCamera.h>
//Required for proper object factory initialization
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeTypeOpenGL);

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwServices/Base.hpp>
#include <fwServices/helper/Config.hpp>
#include <fwServices/macros.hpp>
#include <fwTools/fwID.hpp>
#include <fwData/Color.hpp>

#include <fwRuntime/ConfigurationElementContainer.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/CameraMsg.hpp>

#include "fwRenderVTK/IVtkAdaptorService.hpp"
#include "fwRenderVTK/VtkRenderService.hpp"
#include "fwRenderVTK/vtk/InteractorStyle3DForNegato.hpp"


fwServicesRegisterMacro( ::fwRender::IRender, ::fwRenderVTK::VtkRenderService, ::fwData::Composite );

using namespace fwServices;


namespace fwRenderVTK
{

const ::fwCom::Slots::SlotKeyType VtkRenderService::s_RENDER_SLOT = "render";

//-----------------------------------------------------------------------------

VtkRenderService::VtkRenderService() throw() :
    m_pendingRenderRequest(false),
    m_renderMode(RenderMode::AUTO)
{
    m_slotRender = ::fwCom::newSlot( &VtkRenderService::render, this);
    m_slotRender->setWorker(m_associatedWorker);

    m_connections = ::fwServices::helper::SigSlotConnection::New();

    ::fwCom::HasSlots::m_slots(s_RENDER_SLOT, m_slotRender);
}

//-----------------------------------------------------------------------------

VtkRenderService::~VtkRenderService() throw()
{
}

//-----------------------------------------------------------------------------

void VtkRenderService::configureRenderer( ConfigurationType conf )
{
    assert(conf->getName() == "renderer");

    std::string id         = conf->getAttributeValue("id");
    std::string background = conf->getAttributeValue("background");

    if(m_renderers.count(id) == 0)
    {
        m_renderers[id] = vtkRenderer::New();

        m_renderers[id]->SetUseDepthPeeling     ( 1  );
        m_renderers[id]->SetMaximumNumberOfPeels( 8  );
        m_renderers[id]->SetOcclusionRatio      ( 0. );

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

void VtkRenderService::configurePicker( ConfigurationType conf )
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
        OSLM_ASSERT("'" << vtkclass.c_str() << "' instantiation failled.",m_pickers[id]);
        m_pickers[id]->InitializePickList();
        m_pickers[id]->PickFromListOn();
        vtkPicker *picker = vtkPicker::SafeDownCast(m_pickers[id]);
        if (picker)
        {
            picker->SetTolerance(0);
        }
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::configureObject( ConfigurationType conf )
{
    assert(conf->getName() == "adaptor");
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    const std::string id            = conf->getAttributeValue("id");
    const std::string objectId      = conf->getAttributeValue("objectId");
    const std::string adaptor       = conf->getAttributeValue("class");
    const std::string uid           = conf->getAttributeValue("uid");
    const std::string compositeName = "self";

    SLM_ASSERT( "'id' required attribute missing or empty", !id.empty() );
    SLM_ASSERT( "'objectId' required attribute missing or empty", !objectId.empty() );
    SLM_ASSERT( "'adaptor' required attribute missing or empty", !adaptor.empty() );

    const unsigned int compositeObjectCount = composite->getContainer().count(objectId);

    OSLM_TRACE_IF(objectId << " not found in composite. If it exist, associated Adaptor will be destroyed",
                  !(compositeObjectCount == 1 || objectId == compositeName) );


    ::fwData::Object::sptr object;
    if (compositeObjectCount)
    {
        object = ::fwData::Object::dynamicCast(composite->getContainer()[objectId]);
    }
    else if (objectId == compositeName)
    {
        object = ::fwData::Object::dynamicCast(composite);
    }

    if ( m_sceneAdaptors.count(id) == 0 && object )
    {
        OSLM_TRACE ("Adding service : IVtkAdaptorService " << adaptor << " on "<< objectId );
        SceneAdaptor adaptee;
        adaptee.m_config = *(conf->begin());
        if (!uid.empty())
        {
            OSLM_TRACE("VtkRenderService::configureObject : uid = " << uid);
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
        adaptee.getService()->configure();
        adaptee.getService()->setRenderService(VtkRenderService::dynamicCast(this->shared_from_this()));
        adaptee.getService()->setName(id);

        if (this->isStarted())
        {
            adaptee.getService()->start();
        }

        m_sceneAdaptors[id] = adaptee;
    }
    else if(m_sceneAdaptors.count(id) == 1)
    {
        SceneAdaptor &adaptee = m_sceneAdaptors[id];
        SLM_ASSERT("Adaptor service expired !", adaptee.getService() );
        OSLM_ASSERT( adaptee.getService()->getID() <<  " is not started ",adaptee.getService()->isStarted());
        if (object)
        {
            OSLM_TRACE ("Swapping IVtkAdaptorService " << adaptor << " on "<< objectId );
            if(adaptee.getService()->getObject() != object)
            {
                adaptee.getService()->swap(object);
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
        OSLM_TRACE ( "'"<< objectId << "' inexistent, passing by '" << adaptor << "'");
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::configureVtkObject( ConfigurationType conf )
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

vtkTransform * VtkRenderService::createVtkTransform( ConfigurationType conf )
{
    SLM_ASSERT("vtkObject must be contain just only one sub xml element called vtkTransform.", conf->size() == 1 &&
               ( *conf->begin() )->getName() == "vtkTransform");

    ConfigurationType vtkTransformXmlElem = *conf->begin();

    vtkTransform * newMat = vtkTransform::New();

    for(    ::fwRuntime::ConfigurationElement::Iterator elem = vtkTransformXmlElem->begin();
            !(elem == vtkTransformXmlElem->end());
            ++elem )
    {
        SLM_ASSERT("Sorry, the name of xml element must be concatenate.", (*elem)->getName() == "concatenate" );

        std::string transformId = (*elem)->getValue();

        vtkTransform * mat = vtkTransform::SafeDownCast( getVtkObject(transformId) );

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

void VtkRenderService::addVtkObject( const VtkObjectIdType& _id, vtkObject * _vtkObj )
{
    SLM_ASSERT( !_id.empty(), "vtkObject id is empty" );
    SLM_ASSERT( _vtkObj, "vtkObject is NULL" );

    if( m_vtkObjects.count(_id) == 0 )
    {
        m_vtkObjects[_id] = _vtkObj;
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    SLM_FATAL_IF( "Depreciated tag \"win\" in configuration", m_configuration->findConfigurationElement("win") );
    this->initialize();

    std::vector < ::fwRuntime::ConfigurationElement::sptr > vectConfig = m_configuration->find("scene");
    SLM_ASSERT("Missing 'scene' configuration.",!vectConfig.empty());
    m_sceneConfiguration = vectConfig.at(0);

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
        m_timer->setFunction( ::boost::bind( &VtkRenderService::updateTimer, this)  );
        m_timer->setDuration(duration);
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::starting() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->create();
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
                ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
                std::string key = (*iter)->getAttributeValue("waitForKey");
                ::fwData::Composite::const_iterator iterComposite = composite->find(key);
                if(iterComposite != composite->end())
                {
                    this->manageConnection(key, iterComposite->second, *iter);
                }
                m_connect.push_back(*iter);
            }
            else
            {
                ::fwServices::helper::Config::createConnections(*iter, m_connections);
            }
        }
        else
        {
            OSLM_ASSERT("Bad scene configurationType, unknown xml node : " << (*iter)->getName(), false);
        }
    }

    m_interactorManager->getInteractor()->GetRenderWindow()->SetNumberOfLayers(m_renderers.size());
    for( RenderersMapType::iterator iter = m_renderers.begin(); iter != m_renderers.end(); ++iter )
    {
        vtkRenderer *renderer = (*iter).second;
        m_interactorManager->getInteractor()->GetRenderWindow()->AddRenderer(renderer);
    }
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    SceneAdaptorsMapType::iterator adaptorIter;
    for ( adaptorIter = m_sceneAdaptors.begin();
          adaptorIter != m_sceneAdaptors.end();
          ++adaptorIter)
    {
        adaptorIter->second.getService()->start();
        assert(adaptorIter->second.getService()->isStarted());
    }

    if(m_timer)
    {
        m_timer->start();
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::stopping() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    m_connections->disconnect();

    if(m_timer)
    {
        m_timer->stop();
    }

    SceneAdaptorsMapType::iterator adaptorIter;

    for ( adaptorIter = m_sceneAdaptors.begin();
          adaptorIter != m_sceneAdaptors.end();
          ++adaptorIter)
    {
        adaptorIter->second.getService()->stop();
        ::fwServices::OSR::unregisterService(adaptorIter->second.getService());
        adaptorIter->second.getService().reset();
    }

    this->stopContext();

    this->getContainer()->clean();
    this->destroy();

    m_sceneAdaptors.clear();
}

//-----------------------------------------------------------------------------

void VtkRenderService::receiving( ::fwServices::ObjectMsg::csptr message ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(message);

    if( compositeMsg )
    {
        ::fwData::Composite::ContainerType objects;

        ::fwData::Composite::sptr modifiedKeys;

        modifiedKeys = compositeMsg->getAddedKeys();
        this->connectAfterWait(modifiedKeys);
        objects.insert(modifiedKeys->begin(), modifiedKeys->end());

        modifiedKeys = compositeMsg->getNewChangedKeys();
        this->disconnect(compositeMsg->getOldChangedKeys());
        this->connectAfterWait(modifiedKeys);
        objects.insert(modifiedKeys->begin(), modifiedKeys->end());

        modifiedKeys = compositeMsg->getRemovedKeys();
        this->disconnect(modifiedKeys);
        objects.insert(modifiedKeys->begin(), modifiedKeys->end());

        assert ( m_sceneConfiguration );

        BOOST_FOREACH( ::fwData::Composite::ContainerType::value_type objectId, objects)
        {
            std::vector< ConfigurationType > confVec = m_sceneConfiguration->find("adaptor","objectId",objectId.first);
            BOOST_FOREACH( ConfigurationType cfg, confVec )
            {
                this->configureObject(cfg);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::updating() throw(fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void VtkRenderService::render()
{
    m_interactorManager->getInteractor()->Render();
    this->setPendingRenderRequest(false);
}

//-----------------------------------------------------------------------------

bool VtkRenderService::isShownOnScreen()
{
    return this->getContainer()->isShownOnScreen();
}

//-----------------------------------------------------------------------------

void VtkRenderService::updateTimer()
{
    if ( !this->getPendingRenderRequest())
    {
        this->setPendingRenderRequest(true);
        this->slot(VtkRenderService::s_RENDER_SLOT)->asyncRun();
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::startContext()
{
    m_interactorManager = ::fwRenderVTK::IVtkRenderWindowInteractorManager::createManager();
    m_interactorManager->setRenderService(this->getSptr());
    m_interactorManager->installInteractor( this->getContainer() );

    // For Depth peeling (translucent rendering)
//    m_interactorManager->getInteractor()->SetRenderWhenDisabled(false);

#ifndef __linux
    m_interactorManager->getInteractor()->GetRenderWindow()->SetAlphaBitPlanes(1);
    m_interactorManager->getInteractor()->GetRenderWindow()->SetMultiSamples(0);
#endif

//    m_interactor->GetRenderWindow()->PointSmoothingOn();
//    m_interactor->GetRenderWindow()->LineSmoothingOn();
//    m_interactor->GetRenderWindow()->PolygonSmoothingOn();
//    m_interactor->Register(NULL);
    InteractorStyle3DForNegato* interactor = InteractorStyle3DForNegato::New();
    SLM_ASSERT("Can't instantiate interactor", interactor);
    interactor->setAutoRender(m_renderMode == RenderMode::AUTO);
    m_interactorManager->getInteractor()->SetInteractorStyle( interactor );

//    m_interactorManager->getInteractor()->SetRenderModeToDirect();
    //m_interactor->SetRenderModeToFrameRated();
//    m_interactor->SetRenderModeToOneShot();
//    m_interactor->SetRenderModeToMeanTime();
}

//-----------------------------------------------------------------------------

void VtkRenderService::stopContext()
{
    SLM_TRACE_FUNC();

    for( RenderersMapType::iterator iter = m_renderers.begin(); iter != m_renderers.end(); ++iter )
    {
        vtkRenderer *renderer = iter->second;
        renderer->InteractiveOff();
        m_interactorManager->getInteractor()->GetRenderWindow()->RemoveRenderer(renderer);
        renderer->Delete();
    }

    m_renderers.clear();

    m_interactorManager->uninstallInteractor();
    m_interactorManager.reset();
}

//-----------------------------------------------------------------------------

vtkRenderer * VtkRenderService::getRenderer(RendererIdType rendererId)
{
    OSLM_ASSERT("Renderer not found : '" << rendererId << "'", m_renderers.count(rendererId) == 1);

    return m_renderers[rendererId];
}

//-----------------------------------------------------------------------------

vtkAbstractPropPicker * VtkRenderService::getPicker(PickerIdType pickerId)
{
    OSLM_ASSERT("Picker '" << pickerId << "' not found",
                pickerId.empty() ||  ( !pickerId.empty() && m_pickers.count(pickerId) == 1));

    if ( pickerId.empty() )
    {
        return NULL;
    }
    return m_pickers[pickerId];
}

//-----------------------------------------------------------------------------

vtkObject * VtkRenderService::getVtkObject(const VtkObjectIdType& objectId)
{
    OSLM_WARN_IF("vtkObject '" << objectId << "' not found",
                 !(objectId.empty() ||  ( !objectId.empty() && m_vtkObjects.count(objectId) == 1)));

    if ( objectId.empty() || m_vtkObjects.count(objectId) == 0)
    {
        return NULL;
    }
    return m_vtkObjects[objectId];
}

//-----------------------------------------------------------------------------

SPTR (IVtkAdaptorService) VtkRenderService::getAdaptor(VtkRenderService::AdaptorIdType adaptorId)
{
    IVtkAdaptorService::sptr adaptor;
    SceneAdaptorsMapType::iterator it = m_sceneAdaptors.find(adaptorId);

    OSLM_WARN_IF("adaptor '" << adaptorId << "' not found", it == m_sceneAdaptors.end());

    if ( it != m_sceneAdaptors.end() )
    {
        adaptor = it->second.getService();
    }

    return adaptor;
}

//-----------------------------------------------------------------------------

vtkTransform * VtkRenderService::getOrAddVtkTransform( const VtkObjectIdType& _id )
{
    vtkTransform *t = vtkTransform::SafeDownCast(getVtkObject(_id));
    if(t == 0)
    {
        t = vtkTransform::New();
        this->addVtkObject(_id, t);
    }
    return t;
}

//-----------------------------------------------------------------------------

void VtkRenderService::connectAfterWait(::fwData::Composite::sptr composite)
{

    BOOST_FOREACH(::fwData::Composite::value_type element, *composite)
    {
        BOOST_FOREACH(::fwRuntime::ConfigurationElement::sptr connect, m_connect)
        {
            this->manageConnection(element.first, element.second, connect);
        }
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::manageConnection(const std::string key, const ::fwTools::Object::sptr &obj,
                                        ConfigurationType config)
{
    if(config->hasAttribute("waitForKey"))
    {
        std::string waitForKey = config->getAttributeValue("waitForKey");
        if(waitForKey == key)
        {
            ::fwServices::helper::SigSlotConnection::sptr connection;

            ObjectConnectionsMapType::iterator iter = m_objectConnections.find(key);
            if (iter != m_objectConnections.end())
            {
                connection = iter->second;
            }
            else
            {
                connection               = ::fwServices::helper::SigSlotConnection::New();
                m_objectConnections[key] = connection;
            }
            ::fwServices::helper::Config::createConnections(config, connection, obj);
        }
    }
}

//-----------------------------------------------------------------------------

void VtkRenderService::disconnect(::fwData::Composite::sptr composite)
{

    BOOST_FOREACH(::fwData::Composite::value_type element, *composite)
    {
        std::string key = element.first;
        if(m_objectConnections.find(key) != m_objectConnections.end())
        {
            m_objectConnections[key]->disconnect();
            m_objectConnections.erase(key);
        }
    }
}

//-----------------------------------------------------------------------------

} //namespace fwRenderVTK
