/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderVTK/SRender.hpp"

#include "fwRenderVTK/IAdaptor.hpp"
#include "fwRenderVTK/IVtkRenderWindowInteractorManager.hpp"
#include "fwRenderVTK/OffScreenInteractorManager.hpp"
#include "fwRenderVTK/registry/adaptors.hpp"
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

fwServicesRegisterMacro( ::fwRender::IRender, ::fwRenderVTK::SRender );

using namespace fwServices;

namespace fwRenderVTK
{
const ::fwCom::Signals::SignalKeyType SRender::s_DROPPED_SIG     = "dropped";
const ::fwCom::Slots::SlotKeyType SRender::s_RENDER_SLOT         = "render";
const ::fwCom::Slots::SlotKeyType SRender::s_REQUEST_RENDER_SLOT = "requestRender";

//-----------------------------------------------------------------------------

SRender::SRender() noexcept :
    m_pendingRenderRequest(false),
    m_renderMode(RenderMode::AUTO),
    m_width(1280),
    m_height(720),
    m_offScreen(false)
{
    newSignal<DroppedSignalType>(s_DROPPED_SIG);

    newSlot(s_RENDER_SLOT, &SRender::render, this);
    newSlot(s_REQUEST_RENDER_SLOT, &SRender::requestRender, this);
}

//-----------------------------------------------------------------------------

SRender::~SRender() noexcept
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

void SRender::configuring()
{
    std::vector < ::fwRuntime::ConfigurationElement::sptr > vectConfig = m_configuration->find("scene");
    SLM_FATAL_IF("Missing 'scene' configuration.", vectConfig.empty());
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

    auto adaptorConfigs = m_sceneConfiguration->findAllConfigurationElement("adaptor");
    for (const auto& currentConfig : adaptorConfigs)
    {
        SLM_FATAL_IF("Missing 'uid' attribute in adaptor configuration", !currentConfig->hasAttribute("uid"));

        SceneAdaptor adaptor;
        std::string uid        = currentConfig->getAttributeValue("uid");
        std::string startValue = currentConfig->getAttributeValue("start");

        SLM_FATAL_IF("Wrong value '"+ startValue +"' for 'start' attribute (require yes or no)",
                     !startValue.empty() && startValue != "yes" && startValue != "no");
        adaptor.m_start = (startValue == "yes");

        m_sceneAdaptors[uid] = adaptor;

        // register the assiciation <adaptor, scene>
        auto& registry = ::fwRenderVTK::registry::getAdaptorRegistry();
        registry[uid] = this->getID();
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

void SRender::starting()
{
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
        else if ((*iter)->getName() == "vtkObject")
        {
            this->configureVtkObject(*iter);
        }
    }

    m_interactorManager->getInteractor()->GetRenderWindow()->SetNumberOfLayers(static_cast<int>(m_renderers.size()));
    for( RenderersMapType::iterator iter = m_renderers.begin(); iter != m_renderers.end(); ++iter )
    {
        vtkRenderer* renderer = (*iter).second;
        m_interactorManager->getInteractor()->GetRenderWindow()->AddRenderer(renderer);
    }

    // Start adaptors according to their starting priority
    std::vector< SPTR(IAdaptor) > startAdaptors;

    const auto servicesVector = ::fwServices::OSR::getServices("::fwRenderVTK::IAdaptor");

    for(auto& sceneAdaptor : m_sceneAdaptors)
    {
        if (sceneAdaptor.second.m_start)
        {
            auto result =
                std::find_if(servicesVector.begin(), servicesVector.end(),
                             [sceneAdaptor](const ::fwServices::IService::sptr& srv)
                {
                    return srv->getID() == sceneAdaptor.first;
                });

            SLM_ASSERT("Adaptor '" + sceneAdaptor.first + "' is not found", result != servicesVector.end());

            sceneAdaptor.second.m_service = ::fwRenderVTK::IAdaptor::dynamicCast(*result);
            startAdaptors.emplace_back(sceneAdaptor.second.getService());
        }
    }

    std::sort(startAdaptors.begin(), startAdaptors.end(),
              [](const SPTR(IAdaptor)& a, const SPTR(IAdaptor)& b)
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

void SRender::stopping()
{
    if(m_timer)
    {
        m_timer->stop();
    }

    // Stop adaptors in the reverse order of their starting priority
    std::vector< SPTR(IAdaptor) > stopAdaptors;

    for(auto& sceneAdaptor : m_sceneAdaptors)
    {
        if(sceneAdaptor.second.m_start && sceneAdaptor.second.getService())
        {
            stopAdaptors.emplace_back(sceneAdaptor.second.getService());
        }
    }

    std::sort(stopAdaptors.begin(), stopAdaptors.end(),
              [](const SPTR(IAdaptor)& a, const SPTR(IAdaptor)& b)
        {
            return b->getStartPriority() < a->getStartPriority();
        });

    for(auto& adaptor : stopAdaptors)
    {
        adaptor->stop();
    }
    stopAdaptors.clear();
    m_sceneAdaptors.clear();

    this->stopContext();

    if (!m_offScreen)
    {
        this->destroy();
    }
}

//-----------------------------------------------------------------------------

void SRender::updating()
{
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
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(m_offScreenImageKey);
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

SPTR(IAdaptor) SRender::getAdaptor(const SRender::AdaptorIdType& adaptorId) const
{
    IAdaptor::sptr adaptor;
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

} //namespace fwRenderVTK
