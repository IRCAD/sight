/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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
#include <data/CameraSet.hpp>
#include <data/FrameTL.hpp>

#include <service/extension/Config.hpp>
#include <service/macros.hpp>
#include <service/registry.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/SelectorDialog.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>

namespace sight::module::io::video
{

//-----------------------------------------------------------------------------

using sight::io::base::service::IGrabber;

//-----------------------------------------------------------------------------

SGrabberProxy::SGrabberProxy() noexcept
{
    newSlot(slots::RECONFIGURE, &SGrabberProxy::reconfigure, this);
    newSlot(slots::START_TARGET_CAMERA, &SGrabberProxy::startTargetCamera, this);

    newSlot(slots::MODIFY_POSITION, &SGrabberProxy::modifyPosition, this);
    newSlot(slots::MODIFY_DURATION, &SGrabberProxy::modifyDuration, this);
    newSlot(slots::FWD_START_CAMERA, &SGrabberProxy::fwdStartCamera, this);
    newSlot(slots::FWD_STOP_CAMERA, &SGrabberProxy::fwdStopCamera, this);
    newSlot(slots::FWD_PRESENT_FRAME, &SGrabberProxy::fwdPresentFrame, this);

    newSlot(slots::FWD_NOTIFY, &SGrabberProxy::fwdNotify, this);

    newSlot(slots::FWD_SET_PARAMETER, &SGrabberProxy::fwdSetParameter, this);
    newSlot(slots::FWD_CREATE_JOB, &SGrabberProxy::fwdCreateJob, this);
}

//-----------------------------------------------------------------------------

SGrabberProxy::~SGrabberProxy() noexcept =
    default;

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
    const auto config = this->getConfiguration();

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
            const auto service = itSelection->second.get<std::string>("<xmlattr>.service");
            m_selectedServices.insert(service);
            SIGHT_DEBUG("add selection => " + service);

            const std::string configId = itSelection->second.get<std::string>("<xmlattr>.config", "");
            // Check if service is not empty.
            SIGHT_ASSERT("add selection with config but service is missing", !service.empty());
            m_serviceToConfig[service].push_back(configId);
            SIGHT_DEBUG(
                std::string("add config '") + configId + "' for service '" + service + "'"
            );
        }

        const auto configCfg = subConfig.equal_range("config");
        for(auto itCfg = configCfg.first ; itCfg != configCfg.second ; ++itCfg)
        {
            const auto service  = itCfg->second.get<std::string>("<xmlattr>.service");
            const auto configId = itCfg->second.get<std::string>("<xmlattr>.id");

            m_serviceToConfig[service].push_back(configId);
            SIGHT_DEBUG(
                std::string("add config '") + configId + "' for service '" + service + "'"
            );
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
    this->startTargetCamera("");
}

//-----------------------------------------------------------------------------

void SGrabberProxy::startTargetCamera(std::string impl)
{
    if(m_services.empty())
    {
        if(m_grabberImpl.empty())
        {
            const auto srvFactory       = service::extension::Factory::getDefault();
            const auto srvConfigFactory = service::extension::Config::getDefault();

            // We select all RGBD grabbers. They should be capable to output a single color frame
            auto grabbersImpl = srvFactory->getImplementationIdFromObjectAndType(
                "sight::data::FrameTL",
                "sight::io::base::service::IRGBDGrabber"
            );

            auto rgbGrabbersImpl = srvFactory->getImplementationIdFromObjectAndType(
                "sight::data::FrameTL",
                "sight::io::base::service::IGrabber"
            );

            std::move(rgbGrabbersImpl.begin(), rgbGrabbersImpl.end(), std::back_inserter(grabbersImpl));

            // If we asked for a specific implementation
            // Filter the other out
            if(!impl.empty())
            {
                std::erase_if(
                    grabbersImpl,
                    [&](std::string g)
                    {
                        return g != impl;
                    });
            }

            data::Camera::SourceType sourceType = data::Camera::UNKNOWN;

            std::size_t numCamerasInSeries = 1;

            {
                auto cameraInput = m_camera.lock();
                auto camera      = std::dynamic_pointer_cast<const data::Camera>(cameraInput.get_shared());
                if(camera)
                {
                    sourceType = camera->getCameraSource();
                }
                else
                {
                    auto camera_set = std::dynamic_pointer_cast<const data::CameraSet>(cameraInput.get_shared());
                    if(camera_set)
                    {
                        numCamerasInSeries = camera_set->size();
                        SIGHT_ASSERT("Camera Series is empty", numCamerasInSeries);

                        // Assume same source on all cameras
                        sourceType = camera_set->get_camera(0)->getCameraSource();
                    }
                }
            }

            std::vector<std::string> availableExtensionsSelector;
            std::map<std::string, std::size_t> implToNumTL;

            for(const auto& srvImpl : grabbersImpl)
            {
                if(srvImpl != "sight::module::io::video::SGrabberProxy")
                {
                    SIGHT_DEBUG("Evaluating if implementation '" + srvImpl + "' is suitable...");
                    auto objectsType  = srvFactory->getServiceObjects(srvImpl);
                    const auto config = this->getConfiguration();

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

                    std::size_t numTL = 0;
                    auto inoutsCfg    = config.equal_range("inout");
                    for(auto itCfg = inoutsCfg.first ; itCfg != inoutsCfg.second ; ++itCfg)
                    {
                        service::IService::ConfigType parameterCfg;

                        const auto key = itCfg->second.get<std::string>("<xmlattr>.key");
                        SIGHT_DEBUG("Evaluating if key '" + key + "' is suitable...");
                        const auto obj = this->getInOut(key).lock();
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

                        const boost::char_separator<char> sep(",");
                        const boost::tokenizer<boost::char_separator<char> > tokens(tags, sep);
                        bool capsMatch = false;
                        for(const auto& token : tokens)
                        {
                            // Remove trailing and leading spaces.
                            const auto trimedToken = boost::algorithm::trim_copy(token);

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
                        selectableConfigs.emplace_back(""); // Add the empty config (default grabber).

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
                if(descriptions.empty())
                {
                    const std::string msg = "No video grabber implementation found.\n";
                    sight::ui::base::dialog::MessageDialog::show(
                        "Error",
                        msg,
                        sight::ui::base::dialog::MessageDialog::Icons::WARNING
                    );
                    return;
                }

                if(descriptions.size() == 1)
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

            std::size_t srvCount = 0;
            for(auto& srv : m_services)
            {
                srv = this->registerService<IGrabber>(m_grabberImpl);

                auto cameraInput = m_camera.lock();
                auto camera      = data::Camera::dynamicConstCast(cameraInput.get_shared());
                if(camera)
                {
                    srv->setInput(camera, s_CAMERA_INPUT);
                }
                else
                {
                    auto camera_set = data::CameraSet::dynamicConstCast(cameraInput.get_shared());
                    if(camera_set)
                    {
                        #ifdef DEBUG
                        const std::size_t numCameras = camera_set->size();
                        SIGHT_ASSERT(
                            "Not enough cameras in series to emulate the grabber",
                            srvCount < numCameras
                        );
                        #endif

                        srv->setInput(camera_set->get_camera(srvCount), s_CAMERA_INPUT);
                    }
                }

                std::size_t inputTLCount = 0;
                const auto proxyConfig   = this->getConfiguration();
                auto inoutsCfg           = proxyConfig.equal_range("inout");
                for(auto itCfg = inoutsCfg.first ; itCfg != inoutsCfg.second ; ++itCfg)
                {
                    const auto key = itCfg->second.get<std::string>("<xmlattr>.key");
                    SIGHT_ASSERT("Missing 'key' tag.", !key.empty());

                    auto inout = this->getInOut(key).lock();
                    if(inout)
                    {
                        if(key == IGrabber::s_FRAMETL_INOUT)
                        {
                            auto frameTL = data::FrameTL::dynamicCast(inout.get_shared());
                            if(m_services.size() > 1)
                            {
                                if(inputTLCount == srvCount)
                                {
                                    // We are emulating a grabber with several ones, reuse the first TL slot
                                    srv->setInOut(frameTL, IGrabber::s_FRAMETL_INOUT);
                                    break;
                                }
                            }
                            else
                            {
                                srv->setInOut(frameTL, IGrabber::s_FRAMETL_INOUT);
                            }
                        }
                    }

                    ++inputTLCount;
                }

                if(!m_grabberConfig.empty())
                {
                    const auto& srvConfigRegistry = service::extension::Config::getDefault();
                    const auto srvCfg             = srvConfigRegistry->getServiceConfig(m_grabberConfig, m_grabberImpl);
                    srv->setConfiguration(srvCfg);
                    srv->configure();
                }

                srv->setWorker(this->worker());
                srv->start();

                m_connections.connect(srv, IGrabber::s_POSITION_MODIFIED_SIG, this->getSptr(), slots::MODIFY_POSITION);
                m_connections.connect(srv, IGrabber::s_DURATION_MODIFIED_SIG, this->getSptr(), slots::MODIFY_DURATION);
                m_connections.connect(srv, IGrabber::s_CAMERA_STARTED_SIG, this->getSptr(), slots::FWD_START_CAMERA);
                m_connections.connect(srv, IGrabber::s_CAMERA_STOPPED_SIG, this->getSptr(), slots::FWD_STOP_CAMERA);
                m_connections.connect(srv, IGrabber::s_FRAME_PRESENTED_SIG, this->getSptr(), slots::FWD_PRESENT_FRAME);

                m_connections.connect(srv, IService::signals::s_NOTIFIED, this->getSptr(), slots::FWD_NOTIFY);

                m_connections.connect(
                    srv,
                    IGrabber::s_PARAMETER_CHANGED_SIG,
                    this->getSptr(),
                    slots::FWD_SET_PARAMETER
                );

                m_connections.connect(
                    srv,
                    IGrabber::s_JOB_CREATED_SIG,
                    this->getSptr(),
                    slots::FWD_CREATE_JOB
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
            srv->setParameter(step, key);
        }
    }
}

//------------------------------------------------------------------------------

void SGrabberProxy::setParameter(ui::base::parameter_t value, std::string key)
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->setParameter(value, key);
        }
    }
}

//------------------------------------------------------------------------------

void SGrabberProxy::requestSettings()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->requestSettings();
        }
    }
}

//------------------------------------------------------------------------------

void SGrabberProxy::optimize()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->optimize();
        }
    }
}

//------------------------------------------------------------------------------

void SGrabberProxy::addROICenter(sight::data::Point::sptr p)
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->addROICenter(p);
        }
    }
}

//------------------------------------------------------------------------------

void SGrabberProxy::removeROICenter(sight::data::Point::sptr p)
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->removeROICenter(p);
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

void SGrabberProxy::fwdNotify(IService::NotificationType type, const std::string message)
{
    auto sig = this->signal<IService::signals::notification_t>(IService::signals::s_NOTIFIED);
    sig->asyncEmit(type, message);
}

//------------------------------------------------------------------------------

void SGrabberProxy::fwdSetParameter(ui::base::parameter_t value, std::string key)
{
    auto sig = this->signal<IGrabber::ParameterChangedSignalType>(IGrabber::s_PARAMETER_CHANGED_SIG);
    sig->asyncEmit(value, key);
}

//------------------------------------------------------------------------------

void SGrabberProxy::fwdCreateJob(sight::core::jobs::IJob::sptr job)
{
    auto sig = this->signal<IGrabber::JobCreatedSignalType>(IGrabber::s_JOB_CREATED_SIG);
    sig->asyncEmit(job);
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::video
