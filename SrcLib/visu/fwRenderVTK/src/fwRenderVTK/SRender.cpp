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

#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>

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

static const ::fwServices::IService::KeyType s_OFFSCREEN_INOUT = "offScreen";

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

void SRender::configureRenderer( const ConfigType& rendererConf )
{
    const std::string id         = rendererConf.get<std::string>("<xmlattr>.id");
    const std::string background = rendererConf.get<std::string>("<xmlattr>.background", "");

    if(m_renderers.count(id) == 0)
    {
        m_renderers[id] = vtkRenderer::New();

//vtk depth peeling not available on android (Offscreen rendering issues)
#ifndef ANDROID
        m_renderers[id]->SetUseDepthPeeling( 1  );
        m_renderers[id]->SetMaximumNumberOfPeels( 8  );
        m_renderers[id]->SetOcclusionRatio( 0. );
#endif

        const int layer = rendererConf.get<int>("<xmlattr>.layer", m_renderers[id]->GetLayer());
        m_renderers[id]->SetLayer(layer);
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
            double color = std::stod(background);
            m_renderers[id]->SetBackground(color, color, color);
        }
    }
}

//-----------------------------------------------------------------------------

void SRender::configurePicker( const ConfigType& pickerConf )
{
    const std::string& id       = pickerConf.get<std::string>("<xmlattr>.id");
    const std::string& vtkclass = pickerConf.get<std::string>("<xmlattr>.vtkclass", "vtkCellPicker");

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

void SRender::configureVtkObject( const ConfigType& vtkObjectConf )
{
    const std::string& id       = vtkObjectConf.get<std::string>("<xmlattr>.id");
    const std::string& vtkClass = vtkObjectConf.get<std::string>("<xmlattr>.class");

    SLM_ASSERT("Empty 'id'.", !id.empty() );
    SLM_ASSERT("Empty 'class'.", !vtkClass.empty() );

    if( m_vtkObjects.count(id) == 0 )
    {
        if ( vtkClass == "vtkTransform" )
        {
            m_vtkObjects[id] = createVtkTransform(vtkObjectConf);
        }
        else
        {
            m_vtkObjects[id] = vtkInstantiator::CreateInstance(vtkClass.c_str());
        }
    }
}

//-----------------------------------------------------------------------------

vtkTransform* SRender::createVtkTransform( const ConfigType& vtkObjectConf )
{
    vtkTransform* newMat = vtkTransform::New();

    SLM_ASSERT("VTK transforms can contain at most one 'vtkTransform' sub-element.",
               vtkObjectConf.count("vtkTransform") <= 1 );

    const ::boost::optional<const ConfigType&> vtkTransformConf = vtkObjectConf.get_child_optional("vtkTransform");

    if(vtkTransformConf.is_initialized())
    {
        BOOST_FOREACH(const ::boost::property_tree::ptree::value_type &v, vtkTransformConf.get())
        {
            SLM_ASSERT("Invalid markup '" + v.first + "', 'concatenate' must be used here.", v.first == "concatenate");

            const std::string& transformId = v.second.data();

            vtkTransform* mat = vtkTransform::SafeDownCast( getVtkObject(transformId) );

            SLM_ASSERT("No transform named '" + transformId + "'.", mat != nullptr);

            const std::string& inverse = v.second.get<std::string>("<xmlattr>.inverse", "no");

            SLM_ASSERT("Inverse must be 'yes' or 'no'.", inverse == "yes" || inverse == "no");

            if(inverse == "yes")
            {
                newMat->Concatenate( mat->GetLinearInverse() );
            }
            else
            {
                newMat->Concatenate( mat );
            }
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
    const ConfigType& srvConf = this->getConfigTree();

    const ::boost::optional<const ConfigType&> inouts = srvConf.get_child_optional("service");

    const size_t nbInouts = inouts.is_initialized() ? inouts->count("inout") : 0;

    SLM_ASSERT("This service accepts at most one inout.", nbInouts <= 1);

    if(nbInouts == 1)
    {
        const std::string& key = inouts->get<std::string>("inout.<xmlattr>.key", "");
        m_offScreen = (key == s_OFFSCREEN_INOUT);

        SLM_ASSERT("'" + key + "' is not a valid key. Only '" + s_OFFSCREEN_INOUT +"' is accepted.", m_offScreen);
    }
    else // no offscreen rendering.
    {
        this->initialize();
    }

    m_sceneConf = srvConf.get_child("service.scene");

    const std::string& renderMode = m_sceneConf.get("<xmlattr>.renderMode", "auto");

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
        SLM_WARN_IF("renderMode '" + renderMode + " is unknown, setting renderMode to 'auto'.",
                    !renderMode.empty());
    }

    m_width  = m_sceneConf.get<unsigned int>("<xmlattr>.width", m_width);
    m_height = m_sceneConf.get<unsigned int>("<xmlattr>.height", m_height);

    BOOST_FOREACH(const ::fwServices::IService::ConfigType::value_type &v, m_sceneConf.equal_range("adaptor"))
    {
        const std::string& adaptorUid = v.second.get<std::string>("<xmlattr>.uid", "");

        SLM_FATAL_IF("Missing 'uid' attribute in adaptor configuration", adaptorUid == "");

        // register the <adaptor, scene> association
        auto& registry = ::fwRenderVTK::registry::getAdaptorRegistry();
        registry[adaptorUid] = this->getID();
    }

    /// Target frame rate (default 30Hz)
    const unsigned int targetFrameRate = srvConf.get<unsigned int>("service.fps", 30);

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
    BOOST_FOREACH(const ::fwServices::IService::ConfigType::value_type &v, m_sceneConf)
    {
        const std::string& subEltName = v.first;
        if(subEltName == "renderer")
        {
            this->configureRenderer(v.second);
        }
        else if(subEltName == "picker")
        {
            this->configurePicker(v.second);
        }
        else if(subEltName == "vtkObject")
        {
            this->configureVtkObject(v.second);
        }
        else if(subEltName != "adaptor" && subEltName != "<xmlattr>")
        {
            SLM_FATAL("Unknown sub-element '" + subEltName + "'.");
        }
    }

    m_interactorManager->getInteractor()->GetRenderWindow()->SetNumberOfLayers(static_cast<int>(m_renderers.size()));
    for( RenderersMapType::iterator iter = m_renderers.begin(); iter != m_renderers.end(); ++iter )
    {
        vtkRenderer* renderer = (*iter).second;
        m_interactorManager->getInteractor()->GetRenderWindow()->AddRenderer(renderer);
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
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_OFFSCREEN_INOUT);
        SLM_ASSERT("Offscreen image not found.", image);

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
