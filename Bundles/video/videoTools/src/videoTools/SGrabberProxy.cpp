/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoTools/SGrabberProxy.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

namespace videoTools
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SGrabberProxy::s_RECONFIGURE_SLOT = "reconfigure";

const ::fwCom::Slots::SlotKeyType s_MODIFY_POSITION = "modifyPosition";
const ::fwCom::Slots::SlotKeyType s_MODIFY_DURATION = "modifyDuration";

fwServicesRegisterMacro( ::arServices::IGrabber, ::videoTools::SGrabberProxy, ::arData::FrameTL);

//-----------------------------------------------------------------------------

SGrabberProxy::SGrabberProxy() noexcept
{
    newSlot( s_RECONFIGURE_SLOT, &SGrabberProxy::reconfigure, this );

    newSlot( s_MODIFY_POSITION, &SGrabberProxy::modifyPosition, this );
    newSlot( s_MODIFY_DURATION, &SGrabberProxy::modifyDuration, this );
}

//-----------------------------------------------------------------------------

SGrabberProxy::~SGrabberProxy() noexcept
{
}

//-----------------------------------------------------------------------------

void SGrabberProxy::starting()
{
}

//-----------------------------------------------------------------------------

void SGrabberProxy::stopping()
{
    if(m_service != nullptr)
    {
        m_connections.disconnect();
        m_service->stopCamera();
        this->unregisterService(m_service);
        m_service.reset();

        auto frameTL = this->getInOut< ::arData::FrameTL >(s_FRAMETL_INOUT);
        frameTL->clearTimeline();
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::configuring()
{
    const auto config = this->getConfigTree();

    auto itSubConfig = config.find("config");

    m_selectedServices.clear();
    m_serviceToConfig.clear();

    if(itSubConfig != config.not_found())
    {
        const auto& subConfig = itSubConfig->second;

        m_type = subConfig.get<std::string>("camera.<xmlattr>.type", "RGB") == "RGB" ?
                 CameraType::RGB :
                 CameraType::RGBD;

        const std::string mode = subConfig.get<std::string>("selection.<xmlattr>.mode", "exclude");
        SLM_ASSERT( "The xml attribute <mode> must be 'include' (to add the selection to selector list ) or "
                    "'exclude' (to exclude the selection of the selector list).",
                    mode == "exclude" || mode == "include" );
        m_excludeOrInclude = ( mode == "exclude" );
        OSLM_DEBUG( "selection mode => " << (m_excludeOrInclude ? "Exclude" : "Include") );

        const auto selectionCfg = subConfig.equal_range("addSelection");
        for (auto itSelection = selectionCfg.first; itSelection != selectionCfg.second; ++itSelection)
        {
            const std::string service = itSelection->second.get<std::string>("<xmlattr>.service");
            m_selectedServices.insert(service);
            SLM_DEBUG( "add selection => " + service );

            const std::string configId = itSelection->second.get<std::string>("<xmlattr>.config", "");
            if(!configId.empty())
            {
                m_serviceToConfig[service] = configId;
                SLM_DEBUG( "add config '" + configId + "' for service '" + service + "'");
            }
        }

        const auto configCfg = subConfig.equal_range("config");
        for (auto itCfg = configCfg.first; itCfg != configCfg.second; ++itCfg)
        {
            const std::string service  = itCfg->second.get<std::string>("<xmlattr>.service");
            const std::string configId = itCfg->second.get<std::string>("<xmlattr>.id");

            m_serviceToConfig[service] = configId;
            SLM_DEBUG( "add config '" + configId + "' for service '" + service + "'");
        }
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::updating()
{
}

//-----------------------------------------------------------------------------

void SGrabberProxy::startCamera()
{
    if(m_service == nullptr)
    {
        if(m_grabberImpl.empty())
        {
            const auto srvFactory = ::fwServices::registry::ServiceFactory::getDefault();

            // We select all simple grabbers. If they can't fill the data requirements, they will be eliminated later
            // by the data test
            auto srvImpl = srvFactory->getImplementationIdFromObjectAndType("::arData::FrameTL",
                                                                            "::arServices::IRGBDGrabber");

            // If we chose RGB cameras, we add
            if(m_type == CameraType::RGB)
            {
                auto rgbSrvImpl = srvFactory->getImplementationIdFromObjectAndType("::arData::FrameTL",
                                                                                   "::arServices::IGrabber");

                std::move(rgbSrvImpl.begin(), rgbSrvImpl.end(), std::inserter(srvImpl, srvImpl.begin()));
            }

            std::vector< std::string > availableExtensionsSelector;

            for(const auto& srvImpl : srvImpl)
            {
                if(srvImpl != "::videoTools::SGrabberProxy")
                {
                    SLM_DEBUG( "Evaluating if implementation '" + srvImpl + "' is suitable...");
                    const auto objectsType = srvFactory->getServiceObjects(srvImpl);
                    const auto config      = this->getConfigTree();

                    size_t numTL   = 0;
                    auto inoutsCfg = config.equal_range("inout");
                    for (auto itCfg = inoutsCfg.first; itCfg != inoutsCfg.second; ++itCfg)
                    {
                        ::fwServices::IService::ConfigType parameterCfg;

                        const std::string key = itCfg->second.get<std::string>("<xmlattr>.key");
                        SLM_DEBUG( "Evaluating if key '" + key + "' is suitable...");
                        const auto obj = this->getInOut< ::fwData::Object >(key);
                        SLM_ASSERT("Object key '" + key + "' not found", obj);
                        if(obj->getClassname() != objectsType[numTL])
                        {
                            // Skip this implementation if we don't have the same object type
                            continue;
                        }
                        ++numTL;
                    }
                    if(numTL != config.count("inout"))
                    {
                        continue;
                    }

                    if(m_selectedServices.empty())
                    {
                        availableExtensionsSelector.push_back(srvImpl);
                    }
                    else
                    {
                        if(m_excludeOrInclude)
                        {
                            if(m_selectedServices.find(srvImpl) == m_selectedServices.end())
                            {
                                availableExtensionsSelector.push_back(srvImpl);
                            }
                        }
                        else
                        {
                            if(m_selectedServices.find(srvImpl) != m_selectedServices.end())
                            {
                                availableExtensionsSelector.push_back(srvImpl);
                            }
                        }
                    }
                }
            }

            if(availableExtensionsSelector.size() > 1)
            {
                ::fwGui::dialog::SelectorDialog::sptr selector = ::fwGui::dialog::SelectorDialog::New();

                selector->setTitle("Choose a video grabber");
                selector->setSelections(availableExtensionsSelector);

                m_grabberImpl = selector->show();
            }
            else if( availableExtensionsSelector.size() == 1)
            {
                m_grabberImpl = availableExtensionsSelector.front();
            }
            else
            {
                const std::string msg = "No video grabber implementation found\n";
                ::fwGui::dialog::MessageDialog::showMessageDialog("Error", msg);
                return;
            }

            if(m_grabberImpl.empty())
            {
                const std::string msg = "No video grabber chosen, aborting...\n";
                ::fwGui::dialog::MessageDialog::showMessageDialog("Warning", msg);
                return;
            }
        }

        m_service = this->registerService< ::arServices::IGrabber>(m_grabberImpl);

        auto camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
        if(camera)
        {
            m_service->registerInput(camera, s_CAMERA_INPUT);
        }

        const auto config = this->getConfigTree();
        auto inoutsCfg    = config.equal_range("inout");
        for (auto itCfg = inoutsCfg.first; itCfg != inoutsCfg.second; ++itCfg)
        {
            const std::string key = itCfg->second.get<std::string>("<xmlattr>.key");
            SLM_ASSERT("Missing 'key' tag.", !key.empty());

            auto frameTL = this->getInOut< ::arData::FrameTL >(key);
            if(frameTL)
            {
                m_service->registerInOut(frameTL, key);
            }
        }

        ::fwRuntime::ConfigurationElement::csptr srvCfg;
        if ( m_serviceToConfig.find( m_grabberImpl ) != m_serviceToConfig.end() )
        {
            const auto& srvConfigRegistry = ::fwServices::registry::ServiceConfig::getDefault();
            srvCfg = srvConfigRegistry->getServiceConfig(m_serviceToConfig[m_grabberImpl], m_grabberImpl );
            SLM_ASSERT("No service configuration called '" + m_serviceToConfig[m_grabberImpl] + "' was found for "
                       "service type '" + m_grabberImpl + "'", srvCfg );
            m_service->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(srvCfg) );
            m_service->configure();
        }
        m_service->setWorker(m_associatedWorker);
        m_service->start();

        m_connections.connect(m_service, ::arServices::IGrabber::s_POSITION_MODIFIED_SIG,
                              this->getSptr(), s_MODIFY_POSITION);
        m_connections.connect(m_service, ::arServices::IGrabber::s_DURATION_MODIFIED_SIG,
                              this->getSptr(), s_MODIFY_DURATION);
    }

    m_service->startCamera();
}

//-----------------------------------------------------------------------------

void SGrabberProxy::pauseCamera()
{
    if(m_service != nullptr)
    {
        m_service->pauseCamera();
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::stopCamera()
{
    if(m_service != nullptr)
    {
        m_service->stopCamera();
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::toggleLoopMode()
{
    if(m_service != nullptr)
    {
        m_service->toggleLoopMode();
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::setPosition(std::int64_t position)
{
    if(m_service != nullptr)
    {
        m_service->setPosition(position);
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::reconfigure()
{
    if(m_service != nullptr)
    {
        m_connections.disconnect();
        m_service->stopCamera();
        this->unregisterService(m_service);
        m_service.reset();

        auto frameTL = this->getInOut< ::arData::FrameTL >(s_FRAMETL_INOUT);
        frameTL->clearTimeline();
    }
    m_grabberImpl = "";
}

//-----------------------------------------------------------------------------

void SGrabberProxy::modifyPosition(int64_t position)
{
    auto sig = this->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
    sig->asyncEmit(static_cast<std::int64_t>(position));
}

//-----------------------------------------------------------------------------

void SGrabberProxy::modifyDuration(int64_t duration)
{
    auto sig = this->signal< DurationModifiedSignalType >( s_DURATION_MODIFIED_SIG );
    sig->asyncEmit(static_cast<std::int64_t>(duration));
}

//-----------------------------------------------------------------------------

} // namespace videoTools

