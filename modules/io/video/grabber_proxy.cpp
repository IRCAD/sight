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

#include "modules/io/video/grabber_proxy.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/camera.hpp>
#include <data/camera_set.hpp>
#include <data/frame_tl.hpp>

#include <service/extension/config.hpp>
#include <service/macros.hpp>
#include <service/registry.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/selector.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>

namespace sight::module::io::video
{

//-----------------------------------------------------------------------------

using sight::io::service::grabber;

//-----------------------------------------------------------------------------

grabber_proxy::grabber_proxy() noexcept
{
    new_slot(slots::RECONFIGURE, &grabber_proxy::reconfigure, this);
    new_slot(slots::START_TARGET_CAMERA, &grabber_proxy::startTargetCamera, this);

    new_slot(slots::MODIFY_POSITION, &grabber_proxy::modifyPosition, this);
    new_slot(slots::MODIFY_DURATION, &grabber_proxy::modifyDuration, this);
    new_slot(slots::FWD_START_CAMERA, &grabber_proxy::fwdStartCamera, this);
    new_slot(slots::FWD_STOP_CAMERA, &grabber_proxy::fwdStopCamera, this);
    new_slot(slots::FWD_PRESENT_FRAME, &grabber_proxy::fwdPresentFrame, this);

    new_slot(slots::FWD_NOTIFY, &grabber_proxy::fwdNotify, this);

    new_slot(slots::FWD_SET_PARAMETER, &grabber_proxy::fwdSetParameter, this);
    new_slot(slots::FWD_CREATE_JOB, &grabber_proxy::fwdCreateJob, this);
}

//-----------------------------------------------------------------------------

grabber_proxy::~grabber_proxy() noexcept =
    default;

//-----------------------------------------------------------------------------

void grabber_proxy::starting()
{
}

//-----------------------------------------------------------------------------

void grabber_proxy::stopping()
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

void grabber_proxy::configuring()
{
    const auto config = this->get_config();

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

void grabber_proxy::updating()
{
}

//-----------------------------------------------------------------------------

void grabber_proxy::startCamera()
{
    this->startTargetCamera("");
}

//-----------------------------------------------------------------------------

void grabber_proxy::startTargetCamera(std::string impl)
{
    if(m_services.empty())
    {
        if(m_grabberImpl.empty())
        {
            const auto srvFactory       = service::extension::factory::get();
            const auto srvConfigFactory = service::extension::config::getDefault();

            // We select all RGBD grabbers. They should be capable to output a single color frame
            auto grabbersImpl = srvFactory->getImplementationIdFromObjectAndType(
                "sight::data::frame_tl",
                "sight::io::service::rgbd_grabber"
            );

            auto rgbGrabbersImpl = srvFactory->getImplementationIdFromObjectAndType(
                "sight::data::frame_tl",
                "sight::io::service::grabber"
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

            data::camera::SourceType sourceType = data::camera::UNKNOWN;

            std::size_t numCamerasInSeries = 1;

            {
                auto cameraInput = m_camera.lock();
                auto camera      = std::dynamic_pointer_cast<const data::camera>(cameraInput.get_shared());
                if(camera)
                {
                    sourceType = camera->getCameraSource();
                }
                else
                {
                    auto camera_set = std::dynamic_pointer_cast<const data::camera_set>(cameraInput.get_shared());
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
                if(srvImpl != "sight::module::io::video::grabber_proxy")
                {
                    SIGHT_DEBUG("Evaluating if implementation '" + srvImpl + "' is suitable...");
                    auto objectsType  = srvFactory->getServiceObjects(srvImpl);
                    const auto config = this->get_config();

                    // 1. Verify that we have the same number of timelines
                    objectsType.erase(
                        std::remove_if(
                            objectsType.begin(),
                            objectsType.end(),
                            [&](const std::string& _type)
                        {
                            return _type != "data::frame_tl";
                        }),
                        objectsType.end()
                    );

                    std::size_t numTL = 0;
                    auto inoutsCfg    = config.equal_range("inout");
                    for(auto itCfg = inoutsCfg.first ; itCfg != inoutsCfg.second ; ++itCfg)
                    {
                        service::config_t parameterCfg;

                        const auto key = itCfg->second.get<std::string>("<xmlattr>.key");
                        SIGHT_DEBUG("Evaluating if key '" + key + "' is suitable...");
                        const auto obj = this->inout(key).lock();
                        SIGHT_ASSERT("Object key '" + key + "' not found", obj);
                        if(obj->get_classname() == "data::frame_tl")
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
                    if(sourceType != data::camera::UNKNOWN)
                    {
                        const auto tags = srvFactory->getServiceTags(srvImpl);

                        const boost::char_separator<char> sep(",");
                        const boost::tokenizer<boost::char_separator<char> > tokens(tags, sep);
                        bool capsMatch = false;
                        for(const auto& token : tokens)
                        {
                            // Remove trailing and leading spaces.
                            const auto trimedToken = boost::algorithm::trim_copy(token);

                            data::camera::SourceType handledSourceType = data::camera::UNKNOWN;
                            if(trimedToken == "FILE")
                            {
                                handledSourceType = data::camera::FILE;
                            }
                            else if(trimedToken == "STREAM")
                            {
                                handledSourceType = data::camera::STREAM;
                            }
                            else if(trimedToken == "DEVICE")
                            {
                                handledSourceType = data::camera::DEVICE;
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

                const auto& srvConfigRegistry = service::extension::config::getDefault();
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
                    sight::ui::dialog::message::show(
                        "Error",
                        msg,
                        sight::ui::dialog::message::Icons::WARNING
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

                    auto selector = std::make_shared<sight::ui::dialog::selector>();
                    selector->setTitle(m_guiTitle);
                    selector->set_choices(descriptions);

                    selectedDesc = selector->show()[0];
                }

                std::tie(m_grabberImpl, m_grabberConfig) = descToExtension[selectedDesc];
            }

            if(m_grabberImpl.empty())
            {
                const std::string msg = "No video grabber chosen, aborting...\n";
                sight::ui::dialog::message::show("Warning", msg);
                return;
            }

            m_services.resize(implToNumTL[m_grabberImpl]);

            std::size_t srvCount = 0;
            for(auto& srv : m_services)
            {
                srv = this->registerService<grabber>(m_grabberImpl);

                auto cameraInput = m_camera.lock();
                auto camera      = std::dynamic_pointer_cast<const data::camera>(cameraInput.get_shared());
                if(camera)
                {
                    srv->set_input(camera, s_CAMERA_INPUT);
                }
                else
                {
                    auto camera_set = std::dynamic_pointer_cast<const data::camera_set>(cameraInput.get_shared());
                    if(camera_set)
                    {
                        #ifdef DEBUG
                        const std::size_t numCameras = camera_set->size();
                        SIGHT_ASSERT(
                            "Not enough cameras in series to emulate the grabber",
                            srvCount < numCameras
                        );
                        #endif

                        srv->set_input(camera_set->get_camera(srvCount), s_CAMERA_INPUT);
                    }
                }

                std::size_t inputTLCount = 0;
                const auto proxyConfig   = this->get_config();
                auto inoutsCfg           = proxyConfig.equal_range("inout");
                for(auto itCfg = inoutsCfg.first ; itCfg != inoutsCfg.second ; ++itCfg)
                {
                    const auto key = itCfg->second.get<std::string>("<xmlattr>.key");
                    SIGHT_ASSERT("Missing 'key' tag.", !key.empty());

                    auto inout = this->inout(key).lock();
                    if(inout)
                    {
                        if(key == grabber::s_FRAMETL_INOUT)
                        {
                            auto frameTL = std::dynamic_pointer_cast<data::frame_tl>(inout.get_shared());
                            if(m_services.size() > 1)
                            {
                                if(inputTLCount == srvCount)
                                {
                                    // We are emulating a grabber with several ones, reuse the first TL slot
                                    srv->set_inout(frameTL, grabber::s_FRAMETL_INOUT);
                                    break;
                                }
                            }
                            else
                            {
                                srv->set_inout(frameTL, grabber::s_FRAMETL_INOUT);
                            }
                        }
                    }

                    ++inputTLCount;
                }

                if(!m_grabberConfig.empty())
                {
                    const auto& srvConfigRegistry = service::extension::config::getDefault();
                    const auto srvCfg             =
                        srvConfigRegistry->get_service_config(m_grabberConfig, m_grabberImpl);
                    srv->set_config(srvCfg);
                    srv->configure();
                }

                srv->set_worker(this->worker());
                srv->start();

                m_connections.connect(srv, grabber::POSITION_MODIFIED_SIG, this->get_sptr(), slots::MODIFY_POSITION);
                m_connections.connect(srv, grabber::DURATION_MODIFIED_SIG, this->get_sptr(), slots::MODIFY_DURATION);
                m_connections.connect(srv, grabber::CAMERA_STARTED_SIG, this->get_sptr(), slots::FWD_START_CAMERA);
                m_connections.connect(srv, grabber::CAMERA_STOPPED_SIG, this->get_sptr(), slots::FWD_STOP_CAMERA);
                m_connections.connect(srv, grabber::FRAME_PRESENTED_SIG, this->get_sptr(), slots::FWD_PRESENT_FRAME);

                m_connections.connect(srv, notifier::signals::NOTIFIED, this->get_sptr(), slots::FWD_NOTIFY);

                m_connections.connect(
                    srv,
                    grabber::PARAMETER_CHANGED_SIG,
                    this->get_sptr(),
                    slots::FWD_SET_PARAMETER
                );

                m_connections.connect(
                    srv,
                    grabber::JOB_CREATED_SIG,
                    this->get_sptr(),
                    slots::FWD_CREATE_JOB
                );

                m_connections.connect(
                    srv,
                    grabber::FPS_CHANGED_SIG,
                    this->get_sptr(),
                    grabber::FORWARD_FPS_CHANGED_SLOT
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

void grabber_proxy::pauseCamera()
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

void grabber_proxy::stopCamera()
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

void grabber_proxy::toggleLoopMode()
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

void grabber_proxy::setPosition(std::int64_t position)
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

void grabber_proxy::previousImage()
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

void grabber_proxy::nextImage()
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

void grabber_proxy::setStep(int step, std::string key)
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

void grabber_proxy::setParameter(ui::parameter_t value, std::string key)
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

void grabber_proxy::requestSettings()
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

void grabber_proxy::optimize()
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

void grabber_proxy::addROICenter(sight::data::point::sptr p)
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

void grabber_proxy::removeROICenter(sight::data::point::sptr p)
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

void grabber_proxy::reconfigure()
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

void grabber_proxy::modifyPosition(int64_t position)
{
    auto sig = this->signal<PositionModifiedSignalType>(POSITION_MODIFIED_SIG);
    sig->async_emit(static_cast<std::int64_t>(position));
}

//-----------------------------------------------------------------------------

void grabber_proxy::modifyDuration(int64_t duration)
{
    auto sig = this->signal<DurationModifiedSignalType>(DURATION_MODIFIED_SIG);
    sig->async_emit(static_cast<std::int64_t>(duration));
}

//-----------------------------------------------------------------------------

void grabber_proxy::fwdStartCamera()
{
    auto sig = this->signal<CameraStartedSignalType>(CAMERA_STARTED_SIG);
    sig->async_emit();
}

//-----------------------------------------------------------------------------

void grabber_proxy::fwdStopCamera()
{
    auto sig = this->signal<CameraStoppedSignalType>(CAMERA_STOPPED_SIG);
    sig->async_emit();
}

//-----------------------------------------------------------------------------

void grabber_proxy::fwdPresentFrame()
{
    auto sig = this->signal<FramePresentedSignalType>(FRAME_PRESENTED_SIG);
    sig->async_emit();
}

//-----------------------------------------------------------------------------

void grabber_proxy::fwdNotify(service::Notification notification)
{
    notifier::m_notified_sig->async_emit(std::move(notification));
}

//------------------------------------------------------------------------------

void grabber_proxy::fwdSetParameter(ui::parameter_t value, std::string key)
{
    auto sig = this->signal<grabber::ParameterChangedSignalType>(grabber::PARAMETER_CHANGED_SIG);
    sig->async_emit(value, key);
}

//------------------------------------------------------------------------------

void grabber_proxy::fwdCreateJob(sight::core::jobs::base::sptr job)
{
    auto sig = this->signal<grabber::JobCreatedSignalType>(grabber::JOB_CREATED_SIG);
    sig->async_emit(job);
}

//------------------------------------------------------------------------------

void grabber_proxy::forwardFPSChanged(double fps)
{
    auto sig = this->signal<grabber::FPSChangedSignalType>(grabber::FPS_CHANGED_SIG);
    sig->async_emit(fps);
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::video
