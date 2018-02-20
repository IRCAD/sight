/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoTools/SGrabberProxy.hpp"

#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>
#include <arData/FrameTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <boost/tokenizer.hpp>

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
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            m_connections.disconnect();
            srv->stopCamera();
            this->unregisterService(srv);
            srv.reset();
        }
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
        m_exclude = ( mode == "exclude" );
        OSLM_DEBUG( "selection mode => " << (m_exclude ? "Exclude" : "Include") );

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
    if(m_services.empty())
    {
        if(m_grabberImpl.empty())
        {
            const auto srvFactory = ::fwServices::registry::ServiceFactory::getDefault();

            // We select all RGBD grabbers. They should be capable to output a single color frame
            auto grabbersImpl = srvFactory->getImplementationIdFromObjectAndType("::arData::FrameTL",
                                                                                 "::arServices::IRGBDGrabber");

            auto rgbGrabbersImpl = srvFactory->getImplementationIdFromObjectAndType("::arData::FrameTL",
                                                                                    "::arServices::IGrabber");
            std::move(rgbGrabbersImpl.begin(), rgbGrabbersImpl.end(), std::back_inserter(grabbersImpl));

            ::arData::Camera::SourceType sourceType = ::arData::Camera::UNKNOWN;

            size_t numCamerasInSeries = 1;

            auto cameraInput = this->getInput< ::fwData::Object >(s_CAMERA_INPUT);
            auto camera      = ::arData::Camera::dynamicConstCast(cameraInput);
            if(camera)
            {
                sourceType = camera->getCameraSource();
            }
            else
            {
                auto cameraSeries = ::arData::CameraSeries::dynamicConstCast(cameraInput);
                if(cameraSeries)
                {
                    numCamerasInSeries = cameraSeries->getNumberOfCameras();
                    SLM_ASSERT("Camera Series is empty", numCamerasInSeries);

                    // Assume same source on all cameras
                    sourceType = cameraSeries->getCamera(0)->getCameraSource();
                }
            }

            std::vector< std::string > availableExtensionsSelector;
            std::map< std::string, size_t> implToNumTL;

            for(const auto& srvImpl : grabbersImpl)
            {
                if(srvImpl != "::videoTools::SGrabberProxy")
                {
                    SLM_DEBUG( "Evaluating if implementation '" + srvImpl + "' is suitable...");
                    auto objectsType  = srvFactory->getServiceObjects(srvImpl);
                    const auto config = this->getConfigTree();

                    // 1. Verify that we have the same number of timelines
                    objectsType.erase(std::remove_if(objectsType.begin(), objectsType.end(),
                                                     [ & ](const std::string& _type)
                        {
                            return _type != "::arData::FrameTL";
                        }), objectsType.end());

                    size_t numTL   = 0;
                    auto inoutsCfg = config.equal_range("inout");
                    for (auto itCfg = inoutsCfg.first; itCfg != inoutsCfg.second; ++itCfg)
                    {
                        ::fwServices::IService::ConfigType parameterCfg;

                        const std::string key = itCfg->second.get<std::string>("<xmlattr>.key");
                        SLM_DEBUG( "Evaluating if key '" + key + "' is suitable...");
                        const auto obj = this->getInOut< ::fwData::Object >(key);
                        SLM_ASSERT("Object key '" + key + "' not found", obj);
                        if(obj->getClassname() == "::arData::FrameTL")
                        {
                            ++numTL;
                        }
                    }
                    if(numTL > objectsType.size())
                    {
                        if(objectsType.size() == 1 && numCamerasInSeries == numTL)
                        {
                            // We have only one TL, we may emulate a grabber with several timelines using multiple
                            // services
                            implToNumTL[srvImpl] = numTL;
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else
                    {
                        implToNumTL[srvImpl] = 1;
                    }

                    // 2. Filter against the source type
                    if(sourceType != ::arData::Camera::UNKNOWN)
                    {
                        const auto tags = srvFactory->getServiceTags(srvImpl);

                        const ::boost::char_separator<char> sep(",");
                        const ::boost::tokenizer< ::boost::char_separator<char> > tokens(tags, sep);
                        bool capsMatch = false;
                        for(const auto& token : tokens)
                        {
                            ::arData::Camera::SourceType handledSourceType = ::arData::Camera::UNKNOWN;
                            if(token == "FILE")
                            {
                                handledSourceType = ::arData::Camera::FILE;
                            }
                            else if(token == "STREAM")
                            {
                                handledSourceType = ::arData::Camera::STREAM;
                            }
                            else if(token == "DEVICE")
                            {
                                handledSourceType = ::arData::Camera::DEVICE;
                            }
                            if(handledSourceType == sourceType)
                            {
                                capsMatch = true;
                                break;
                            }
                        }
                        if(!capsMatch)
                        {
                            continue;
                        }
                    }

                    // 3. Filter against user choices
                    if(m_selectedServices.empty())
                    {
                        availableExtensionsSelector.push_back(srvImpl);
                    }
                    else
                    {
                        if(m_exclude)
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
                std::map<std::string, std::string> descToExtension;
                std::vector<std::string> descriptions;
                for(const auto& extension : availableExtensionsSelector)
                {
                    const auto desc = srvFactory->getServiceDescription(extension);
                    descToExtension[desc] = extension;
                    descriptions.push_back(desc);
                }
                ::fwGui::dialog::SelectorDialog::sptr selector = ::fwGui::dialog::SelectorDialog::New();

                selector->setTitle("Choose a video grabber");
                selector->setSelections(descriptions);

                const auto selectedDesc = selector->show();
                m_grabberImpl = descToExtension[selectedDesc];
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
            m_services.resize(implToNumTL[m_grabberImpl]);

            size_t srvCount = 0;
            for(auto& srv : m_services)
            {
                srv = this->registerService< ::arServices::IGrabber>(m_grabberImpl);

                auto cameraInput = this->getInput< ::fwData::Object >(s_CAMERA_INPUT);
                auto camera      = ::arData::Camera::dynamicConstCast(cameraInput);
                if(camera)
                {
                    srv->registerInput(camera, s_CAMERA_INPUT);
                }
                else
                {
                    auto cameraSeries = ::arData::CameraSeries::dynamicConstCast(cameraInput);
                    if(cameraSeries)
                    {
                        const size_t numCamerasInSeries = cameraSeries->getNumberOfCameras();
                        SLM_ASSERT("Not enough cameras in series to emulate the grabber",
                                   srvCount < numCamerasInSeries);

                        srv->registerInput(cameraSeries->getCamera(srvCount), s_CAMERA_INPUT);
                    }
                }

                size_t inputTLCount    = 0;
                const auto proxyConfig = this->getConfigTree();
                auto inoutsCfg         = proxyConfig.equal_range("inout");
                for (auto itCfg = inoutsCfg.first; itCfg != inoutsCfg.second; ++itCfg)
                {
                    const std::string key = itCfg->second.get<std::string>("<xmlattr>.key");
                    SLM_ASSERT("Missing 'key' tag.", !key.empty());

                    auto frameTL = this->getInOut< ::arData::FrameTL >(key);
                    if(frameTL)
                    {
                        if(m_services.size() > 1)
                        {
                            if(inputTLCount == srvCount)
                            {
                                // We are emulating a grabber with several ones, reuse the first TL slot
                                srv->registerInOut(frameTL, s_FRAMETL_INOUT);
                                break;
                            }
                        }
                        else
                        {
                            srv->registerInOut(frameTL, key);
                        }
                    }
                    ++inputTLCount;
                }

                ::fwRuntime::ConfigurationElement::csptr srvCfg;
                if ( m_serviceToConfig.find( m_grabberImpl ) != m_serviceToConfig.end() )
                {
                    const auto& srvConfigRegistry = ::fwServices::registry::ServiceConfig::getDefault();
                    srvCfg = srvConfigRegistry->getServiceConfig(m_serviceToConfig[m_grabberImpl], m_grabberImpl );
                    SLM_ASSERT("No service configuration called '" + m_serviceToConfig[m_grabberImpl] + "' was found for "
                               "service type '" + m_grabberImpl + "'",
                               srvCfg );
                    srv->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(srvCfg) );
                    srv->configure();
                }
                srv->setWorker(m_associatedWorker);
                srv->start();

                m_connections.connect(srv, ::arServices::IGrabber::s_POSITION_MODIFIED_SIG,
                                      this->getSptr(), s_MODIFY_POSITION);
                m_connections.connect(srv, ::arServices::IGrabber::s_DURATION_MODIFIED_SIG,
                                      this->getSptr(), s_MODIFY_DURATION);

                ++srvCount;
            }
        }
    }

    for(auto& srv : m_services)
    {
        srv->startCamera();
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::pauseCamera()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->pauseCamera();
        }
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::stopCamera()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->stopCamera();
        }
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::toggleLoopMode()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->toggleLoopMode();
        }
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::setPosition(std::int64_t position)
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->setPosition(position);
        }
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::reconfigure()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            m_connections.disconnect();
            srv->stopCamera();
            this->unregisterService(srv);
            srv.reset();
        }
    }

    m_services.clear();

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
