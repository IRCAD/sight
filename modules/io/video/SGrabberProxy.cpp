/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "modules/io/video/SGrabberProxy.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>
#include <data/FrameTL.hpp>

#include <service/extension/Config.hpp>
#include <service/macros.hpp>
#include <service/registry/ObjectService.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/SelectorDialog.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>

namespace sight::module::io::video
{

//-----------------------------------------------------------------------------

const core::com::Slots::SlotKeyType SGrabberProxy::s_RECONFIGURE_SLOT = "reconfigure";

const core::com::Slots::SlotKeyType s_MODIFY_POSITION = "modifyPosition";
const core::com::Slots::SlotKeyType s_MODIFY_DURATION = "modifyDuration";

const core::com::Slots::SlotKeyType s_FWD_START_CAMERA_SLOT = "forwardStartCamera";
const core::com::Slots::SlotKeyType s_FWD_STOP_CAMERA_SLOT  = "forwardStopCamera";

const core::com::Slots::SlotKeyType s_FWD_PRESENT_FRAME_SLOT = "forwardPresentFrame";

//-----------------------------------------------------------------------------

SGrabberProxy::SGrabberProxy() noexcept
{
    newSlot(s_RECONFIGURE_SLOT, &SGrabberProxy::reconfigure, this);

    newSlot(s_MODIFY_POSITION, &SGrabberProxy::modifyPosition, this);
    newSlot(s_MODIFY_DURATION, &SGrabberProxy::modifyDuration, this);
    newSlot(s_FWD_START_CAMERA_SLOT, &SGrabberProxy::fwdStartCamera, this);
    newSlot(s_FWD_STOP_CAMERA_SLOT, &SGrabberProxy::fwdStopCamera, this);
    newSlot(s_FWD_PRESENT_FRAME_SLOT, &SGrabberProxy::fwdPresentFrame, this);
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
    m_connections.disconnect();

    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
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

        m_type = subConfig.get<std::string>("camera.<xmlattr>.type", "RGB") == "RGB"
                 ? CameraType::RGB
                 : CameraType::RGBD;

        const std::string mode = subConfig.get<std::string>("selection.<xmlattr>.mode", "exclude");
        SIGHT_ASSERT(
            "The xml attribute <mode> must be 'include' (to add the selection to selector list ) or "
            "'exclude' (to exclude the selection of the selector list).",
            mode == "exclude" || mode == "include"
        );
        m_exclude = (mode == "exclude");
        SIGHT_DEBUG("selection mode => " << (m_exclude ? "Exclude" : "Include"));

        const auto selectionCfg = subConfig.equal_range("addSelection");
        for(auto itSelection = selectionCfg.first ; itSelection != selectionCfg.second ; ++itSelection)
        {
            const std::string service = itSelection->second.get<std::string>("<xmlattr>.service");
            m_selectedServices.insert(service);
            SIGHT_DEBUG("add selection => " + service);

            const std::string configId = itSelection->second.get<std::string>("<xmlattr>.config", "");
            // Check if service is not empty.
            SIGHT_ASSERT("add selection with config but service is missing", !service.empty());
            m_serviceToConfig[service].push_back(configId);
            SIGHT_DEBUG("add config '" + configId + "' for service '" + service + "'");
        }

        const auto configCfg = subConfig.equal_range("config");
        for(auto itCfg = configCfg.first ; itCfg != configCfg.second ; ++itCfg)
        {
            const std::string service  = itCfg->second.get<std::string>("<xmlattr>.service");
            const std::string configId = itCfg->second.get<std::string>("<xmlattr>.id");

            m_serviceToConfig[service].push_back(configId);
            SIGHT_DEBUG("add config '" + configId + "' for service '" + service + "'");
        }

        m_guiTitle = subConfig.get<std::string>("gui.<xmlattr>.title", m_guiTitle);
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
            const auto srvFactory       = service::extension::Factory::getDefault();
            const auto srvConfigFactory = service::extension::Config::getDefault();

            // We select all RGBD grabbers. They should be capable to output a single color frame
            auto grabbersImpl = srvFactory->getImplementationIdFromObjectAndType(
                "::sight::data::FrameTL",
                "::sight::service::IRGBDGrabber"
            );

            auto rgbGrabbersImpl = srvFactory->getImplementationIdFromObjectAndType(
                "::sight::data::FrameTL",
                "::sight::service::IGrabber"
            );

            std::move(rgbGrabbersImpl.begin(), rgbGrabbersImpl.end(), std::back_inserter(grabbersImpl));

            data::Camera::SourceType sourceType = data::Camera::UNKNOWN;

            size_t numCamerasInSeries = 1;

            {
                auto cameraInput = m_camera.lock();
                auto camera      = data::Camera::dynamicConstCast(cameraInput.get_shared());
                if(camera)
                {
                    sourceType = camera->getCameraSource();
                }
                else
                {
                    auto cameraSeries = data::CameraSeries::dynamicConstCast(cameraInput.get_shared());
                    if(cameraSeries)
                    {
                        numCamerasInSeries = cameraSeries->getNumberOfCameras();
                        SIGHT_ASSERT("Camera Series is empty", numCamerasInSeries);

                        // Assume same source on all cameras
                        sourceType = cameraSeries->getCamera(0)->getCameraSource();
                    }
                }
            }

            std::vector<std::string> availableExtensionsSelector;
            std::map<std::string, size_t> implToNumTL;

            for(const auto& srvImpl : grabbersImpl)
            {
                if(srvImpl != "::sight::module::io::video::SGrabberProxy")
                {
                    SIGHT_DEBUG("Evaluating if implementation '" + srvImpl + "' is suitable...");
                    auto objectsType  = srvFactory->getServiceObjects(srvImpl);
                    const auto config = this->getConfigTree();

                    // 1. Verify that we have the same number of timelines
                    objectsType.erase(
                        std::remove_if(
                            objectsType.begin(),
                            objectsType.end(),
                            [&](const std::string& _type)
                        {
                            return _type != "data::FrameTL";
                        }),
                        objectsType.end()
                    );

                    size_t numTL   = 0;
                    auto inoutsCfg = config.equal_range("inout");
                    for(auto itCfg = inoutsCfg.first ; itCfg != inoutsCfg.second ; ++itCfg)
                    {
                        service::IService::ConfigType parameterCfg;

                        const std::string key = itCfg->second.get<std::string>("<xmlattr>.key");
                        SIGHT_DEBUG("Evaluating if key '" + key + "' is suitable...");
                        const auto obj = this->getLockedInOut<data::Object>(key);
                        SIGHT_ASSERT("Object key '" + key + "' not found", obj);
                        if(obj->getClassname() == "data::FrameTL")
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
                    if(sourceType != data::Camera::UNKNOWN)
                    {
                        const auto tags = srvFactory->getServiceTags(srvImpl);

                        const ::boost::char_separator<char> sep(",");
                        const ::boost::tokenizer< ::boost::char_separator<char> > tokens(tags, sep);
                        bool capsMatch = false;
                        for(const auto& token : tokens)
                        {
                            // Remove trailing and leading spaces.
                            const auto trimedToken = ::boost::algorithm::trim_copy(token);

                            data::Camera::SourceType handledSourceType = data::Camera::UNKNOWN;
                            if(trimedToken == "FILE")
                            {
                                handledSourceType = data::Camera::FILE;
                            }
                            else if(trimedToken == "STREAM")
                            {
                                handledSourceType = data::Camera::STREAM;
                            }
                            else if(trimedToken == "DEVICE")
                            {
                                handledSourceType = data::Camera::DEVICE;
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

                    availableExtensionsSelector.push_back(srvImpl);
                }
            }

            // 3. Check if specific service or configuration should be included/excluded
            {
                std::map<std::string, std::pair<std::string, std::string> > descToExtension;
                std::vector<std::string> descriptions;

                const auto& srvConfigRegistry = service::extension::Config::getDefault();
                for(const auto& extension : availableExtensionsSelector)
                {
                    // We need to test first if extension have specific configurations to include/exclude.
                    const auto configsIt = m_serviceToConfig.find(extension);
                    std::vector<std::string> selectableConfigs;

                    if(!m_exclude) // Include mode
                    {
                        // Available service/configs are the ones the proxy's configuration.
                        if(configsIt != m_serviceToConfig.end())
                        {
                            selectableConfigs = configsIt->second;
                        }
                    }
                    else // Exclude mode
                    {
                        // Find all configurations for the given grabber.
                        selectableConfigs = srvConfigFactory->getAllConfigForService(extension, true);
                        selectableConfigs.push_back(""); // Add the empty config (default grabber).

                        // Remove configs from the grabber's list.
                        if(configsIt != m_serviceToConfig.end())
                        {
                            const auto& excludedConfigs = configsIt->second;
                            const auto isExcludedConfig = [&excludedConfigs](const std::string& _cfgName) -> bool
                                                          {
                                                              return std::find(
                                                                  excludedConfigs.begin(),
                                                                  excludedConfigs.end(),
                                                                  _cfgName
                                                              ) != excludedConfigs.end();
                                                          };

                            // Remove the ones excluded by the grabber proxy.
                            selectableConfigs.erase(
                                std::remove_if(
                                    selectableConfigs.begin(),
                                    selectableConfigs.end(),
                                    isExcludedConfig
                                ),
                                selectableConfigs.end()
                            );
                        }
                    }

                    // Fill the description list with the available config's descriptions.
                    for(const auto& config : selectableConfigs)
                    {
                        const auto desc = config.empty() ? srvFactory->getServiceDescription(extension)
                                                         : srvConfigRegistry->getConfigDesc(config);
                        descToExtension[desc] = std::make_pair(extension, config);
                        descriptions.push_back(desc);
                    }
                }

                std::string selectedDesc;
                if(descriptions.size() == 0)
                {
                    const std::string msg = "No video grabber implementation found.\n";
                    sight::ui::base::dialog::MessageDialog::show(
                        "Error",
                        msg,
                        sight::ui::base::dialog::MessageDialog::Icons::WARNING
                    );
                    return;
                }
                else if(descriptions.size() == 1)
                {
                    /// Select the only remaining description.
                    selectedDesc = descriptions[0];
                }
                else
                {
                    // Sort the description list.
                    std::sort(std::begin(descriptions), std::end(descriptions));

                    sight::ui::base::dialog::SelectorDialog::sptr selector =
                        sight::ui::base::dialog::SelectorDialog::New();
                    selector->setTitle(m_guiTitle);
                    selector->setSelections(descriptions);

                    selectedDesc = selector->show();
                }

                std::tie(m_grabberImpl, m_grabberConfig) = descToExtension[selectedDesc];
            }

            if(m_grabberImpl.empty())
            {
                const std::string msg = "No video grabber chosen, aborting...\n";
                sight::ui::base::dialog::MessageDialog::show("Warning", msg);
                return;
            }

            m_services.resize(implToNumTL[m_grabberImpl]);

            size_t srvCount = 0;
            for(auto& srv : m_services)
            {
                srv = this->registerService<service::IGrabber>(m_grabberImpl);

                auto cameraInput = m_camera.lock();
                auto camera      = data::Camera::dynamicConstCast(cameraInput.get_shared());
                if(camera)
                {
                    srv->setInput(camera, s_CAMERA_INPUT);
                }
                else
                {
                    auto cameraSeries = data::CameraSeries::dynamicConstCast(cameraInput.get_shared());
                    if(cameraSeries)
                    {
                        const size_t numCamerasInSeries = cameraSeries->getNumberOfCameras();
                        SIGHT_ASSERT(
                            "Not enough cameras in series to emulate the grabber",
                            srvCount < numCamerasInSeries
                        );

                        srv->setInput(cameraSeries->getCamera(srvCount), s_CAMERA_INPUT);
                    }
                }

                size_t inputTLCount    = 0;
                const auto proxyConfig = this->getConfigTree();
                auto inoutsCfg         = proxyConfig.equal_range("inout");
                for(auto itCfg = inoutsCfg.first ; itCfg != inoutsCfg.second ; ++itCfg)
                {
                    const std::string key = itCfg->second.get<std::string>("<xmlattr>.key");
                    SIGHT_ASSERT("Missing 'key' tag.", !key.empty());

                    auto frameTL = this->getLockedInOut<data::FrameTL>(key);
                    if(frameTL)
                    {
                        if(m_services.size() > 1)
                        {
                            if(inputTLCount == srvCount)
                            {
                                // We are emulating a grabber with several ones, reuse the first TL slot
                                srv->setInOut(frameTL.get_shared(), s_FRAMETL_INOUT);
                                break;
                            }
                        }
                        else
                        {
                            srv->setInOut(frameTL.get_shared(), key);
                        }
                    }

                    ++inputTLCount;
                }

                if(!m_grabberConfig.empty())
                {
                    const auto& srvConfigRegistry = service::extension::Config::getDefault();

                    core::runtime::ConfigurationElement::csptr srvCfg =
                        srvConfigRegistry->getServiceConfig(m_grabberConfig, m_grabberImpl);
                    srv->setConfiguration(core::runtime::ConfigurationElement::constCast(srvCfg));
                    srv->configure();
                }

                srv->setWorker(m_associatedWorker);
                srv->start();

                m_connections.connect(
                    srv,
                    service::IGrabber::s_POSITION_MODIFIED_SIG,
                    this->getSptr(),
                    s_MODIFY_POSITION
                );
                m_connections.connect(
                    srv,
                    service::IGrabber::s_DURATION_MODIFIED_SIG,
                    this->getSptr(),
                    s_MODIFY_DURATION
                );
                m_connections.connect(
                    srv,
                    service::IGrabber::s_CAMERA_STARTED_SIG,
                    this->getSptr(),
                    s_FWD_START_CAMERA_SLOT
                );
                m_connections.connect(
                    srv,
                    service::IGrabber::s_CAMERA_STOPPED_SIG,
                    this->getSptr(),
                    s_FWD_STOP_CAMERA_SLOT
                );
                m_connections.connect(
                    srv,
                    service::IGrabber::s_FRAME_PRESENTED_SIG,
                    this->getSptr(),
                    s_FWD_PRESENT_FRAME_SLOT
                );

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

void SGrabberProxy::previousImage()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->previousImage();
        }
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::nextImage()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->nextImage();
        }
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::setStep(int step, std::string key)
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->setStep(step, key);
        }
    }
}

//-----------------------------------------------------------------------------

void SGrabberProxy::reconfigure()
{
    m_connections.disconnect();

    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
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
    auto sig = this->signal<PositionModifiedSignalType>(s_POSITION_MODIFIED_SIG);
    sig->asyncEmit(static_cast<std::int64_t>(position));
}

//-----------------------------------------------------------------------------

void SGrabberProxy::modifyDuration(int64_t duration)
{
    auto sig = this->signal<DurationModifiedSignalType>(s_DURATION_MODIFIED_SIG);
    sig->asyncEmit(static_cast<std::int64_t>(duration));
}

//-----------------------------------------------------------------------------

void SGrabberProxy::fwdStartCamera()
{
    auto sig = this->signal<CameraStartedSignalType>(s_CAMERA_STARTED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SGrabberProxy::fwdStopCamera()
{
    auto sig = this->signal<CameraStoppedSignalType>(s_CAMERA_STOPPED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SGrabberProxy::fwdPresentFrame()
{
    auto sig = this->signal<FramePresentedSignalType>(s_FRAME_PRESENTED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::video
