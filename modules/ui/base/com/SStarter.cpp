/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SStarter.hpp"

#include <service/op/Get.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

#include <boost/range/adaptor/reversed.hpp>

namespace sight::module::ui::base::com
{

//-----------------------------------------------------------------------------

SStarter::SStarter() noexcept =
    default;

//-----------------------------------------------------------------------------

SStarter::~SStarter() noexcept =
    default;

//-----------------------------------------------------------------------------

void SStarter::starting()
{
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SStarter::stopping()
{
    std::vector<service::IService::SharedFutureType> futures;

    for(const VectPairIDActionType::value_type& serviceUid : boost::adaptors::reverse(m_uuidServices))
    {
        bool srv_exists = core::tools::fwID::exist(serviceUid.first);
        if(srv_exists && (m_idStartedSrvSet.find(serviceUid.first) != m_idStartedSrvSet.end()))
        {
            service::IService::sptr service = service::get(serviceUid.first);
            if(service->isStarted())
            {
                futures.push_back(service->stop());
            }
        }
    }

    std::ranges::for_each(futures, std::mem_fn(&std::shared_future<void>::wait));

    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SStarter::info(std::ostream& _sstream)
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void SStarter::updating()
{
    for(auto& m_uuidService : m_uuidServices)
    {
        ActionType action = m_uuidService.second;
        IDSrvType uid     = m_uuidService.first;
        bool srv_exists   = core::tools::fwID::exist(uid);

        // Manage special action
        if(action == START_IF_EXISTS)
        {
            if(srv_exists)
            {
                action = START;
            }
            else
            {
                action = DO_NOTHING;
            }
        }
        else if(action == STOP_IF_EXISTS)
        {
            if(srv_exists)
            {
                action = STOP;
            }
            else
            {
                action = DO_NOTHING;
            }
        }

        if(action != DO_NOTHING)
        {
            sight::ui::base::LockAction lock(this->getSptr());

            service::IService::sptr service = service::get(uid);
            SIGHT_ASSERT("service not found", service);
            switch(action)
            {
                case START_OR_STOP:
                    if(service->isStopped())
                    {
                        service->start();
                        service->update();
                        m_idStartedSrvSet.insert(uid);
                    }
                    else
                    {
                        service->stop();
                        m_idStartedSrvSet.erase(uid);
                    }

                    break;

                case START_ONLY_OR_STOP:
                    if(service->isStopped())
                    {
                        service->start();
                        m_idStartedSrvSet.insert(uid);
                    }
                    else
                    {
                        service->stop();
                        m_idStartedSrvSet.erase(uid);
                    }

                    break;

                case START:
                    if(service->isStopped())
                    {
                        service->start();
                        m_idStartedSrvSet.insert(uid);
                    }
                    else
                    {
                        SIGHT_WARN("Service " << service->getID() << " is not stopped");
                    }

                    service->update();
                    break;

                case STOP:
                    if(service->isStarted())
                    {
                        service->stop();
                    }
                    else
                    {
                        SIGHT_WARN("Service " << service->getID() << " is not started");
                    }

                    break;

                case START_ONLY:
                    if(service->isStopped())
                    {
                        service->start();
                        m_idStartedSrvSet.insert(uid);
                    }
                    else
                    {
                        SIGHT_WARN("Service " << service->getID() << " is not stopped");
                    }

                    break;

                default:
                    SIGHT_FATAL(
                        "There is no action (" << action
                        << ") type corresponding to the action id requested for " << uid
                        << "."
                    );
                    break;
            }
        }
        else
        {
            sight::ui::base::dialog::MessageDialog::show(
                "Service unavailable",
                "The service is unavailable.",
                sight::ui::base::dialog::IMessageDialog::WARNING
            );

            SIGHT_INFO("Do nothing for Service " << m_uuidService.first);
        }
    }
}

//-----------------------------------------------------------------------------

void SStarter::configuring()
{
    this->initialize();

    for(const auto& actionCfg : this->getConfiguration())
    {
        std::string actionType = actionCfg.first;
        ActionType action {ActionType::DO_NOTHING};
        if(actionType == "start")
        {
            action = START;
        }
        else if(actionType == "stop")
        {
            action = STOP;
        }
        else if(actionType == "start_or_stop")
        {
            action = START_OR_STOP;
        }
        else if(actionType == "start_only_or_stop")
        {
            action = START_ONLY_OR_STOP;
        }
        else if(actionType == "start_if_exists")
        {
            action = START_IF_EXISTS;
        }
        else if(actionType == "stop_if_exists")
        {
            action = STOP_IF_EXISTS;
        }
        else if(actionType == "start_only")
        {
            action = START_ONLY;
        }
        else if(actionType == "state" || actionType == "<xmlattr>")
        {
            // Ignore
            continue;
        }
        else
        {
            SIGHT_WARN("The \"actionType\":" << actionType << " is not managed by SStarter");
            continue;
        }

        SIGHT_INFO("SStarter " << actionCfg.first);
        const auto uuid = actionCfg.second.get<std::string>("<xmlattr>.uid");

        m_uuidServices.push_back(std::make_pair(uuid, action));
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::base::com
