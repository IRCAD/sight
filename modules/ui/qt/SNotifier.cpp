/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "modules/ui/qt/SNotifier.hpp"

#include <core/base.hpp>
#include <core/com/Slots.hxx>

#include <service/macros.hpp>

#include <ui/base/GuiRegistry.hpp>

fwServicesRegisterMacro( ::sight::service::IController, ::sight::module::ui::qt::SNotifier )

namespace sight::module::ui::qt
{

static const core::com::Slots::SlotKeyType s_POP_INFO_SLOT    = "popInfo";
static const core::com::Slots::SlotKeyType s_POP_SUCCESS_SLOT = "popSuccess";
static const core::com::Slots::SlotKeyType s_POP_FAILURE_SLOT = "popFailure";

static const core::com::Slots::SlotKeyType s_SET_ENUM_PARAMETER_SLOT = "setEnumParameter";

//-----------------------------------------------------------------------------

SNotifier::SNotifier() noexcept
{
    // Initialize map to do conversions.
    m_positionMap["TOP_RIGHT"]       = sight::ui::base::dialog::NotificationDialog::Position::TOP_RIGHT;
    m_positionMap["TOP_LEFT"]        = sight::ui::base::dialog::NotificationDialog::Position::TOP_LEFT;
    m_positionMap["CENTERED_TOP"]    = sight::ui::base::dialog::NotificationDialog::Position::CENTERED_TOP;
    m_positionMap["CENTERED"]        = sight::ui::base::dialog::NotificationDialog::Position::CENTERED;
    m_positionMap["BOTTOM_RIGHT"]    = sight::ui::base::dialog::NotificationDialog::Position::BOTTOM_RIGHT;
    m_positionMap["BOTTOM_LEFT"]     = sight::ui::base::dialog::NotificationDialog::Position::BOTTOM_LEFT;
    m_positionMap["CENTERED_BOTTOM"] = sight::ui::base::dialog::NotificationDialog::Position::CENTERED_BOTTOM;

    newSlot(s_POP_INFO_SLOT, &SNotifier::popInfo, this);
    newSlot(s_POP_SUCCESS_SLOT, &SNotifier::popSuccess, this);
    newSlot(s_POP_FAILURE_SLOT, &SNotifier::popFailure, this);
    newSlot(s_SET_ENUM_PARAMETER_SLOT, &SNotifier::setEnumParameter, this);
}

//-----------------------------------------------------------------------------

SNotifier::~SNotifier() noexcept
{
    m_positionMap.clear();
}

//-----------------------------------------------------------------------------

void SNotifier::configuring()
{
    const auto configTree = this->getConfigTree();
    m_defaultMessage = configTree.get< std::string >("message", m_defaultMessage);
    const std::string position = configTree.get< std::string >("position", "TOP_RIGHT");

    if(m_positionMap.find(position) != m_positionMap.end())
    {
        m_notifcationsPosition = m_positionMap[position];
    }
    else
    {
        SLM_ERROR("Position '" + position + "' isn't a valid position value, accepted values are:"
                  "TOP_RIGHT, TOP_LEFT, CENTERED_TOP, CENTERED, BOTTOM_RIGHT, BOTTOM_LEFT, CENTERED_BOTTOM.")
    }

    m_durationInMs = configTree.get<int>("duration", m_durationInMs);

    m_maxStackedNotifs = configTree.get<std::uint8_t>("maxNotifications", m_maxStackedNotifs);

    m_parentContainerID = configTree.get< std::string >("parent.<xmlattr>.uid", m_parentContainerID);
}

//-----------------------------------------------------------------------------

void SNotifier::starting()
{
    m_popups.resize(m_maxStackedNotifs);

    if(!m_parentContainerID.empty())
    {
        auto container = ::sight::ui::base::GuiRegistry::getSIDContainer(m_parentContainerID);

        if(!container)
        {
            container = ::sight::ui::base::GuiRegistry::getWIDContainer(m_parentContainerID);
        }

        // If we have an SID/WID set the container.
        if(container)
        {
            m_containerWhereToDisplayNotifs = container;
        }

    }
}

//-----------------------------------------------------------------------------

void SNotifier::stopping()
{
    m_popups.clear();
}

//-----------------------------------------------------------------------------

void SNotifier::updating()
{

}

//-----------------------------------------------------------------------------

void SNotifier::setEnumParameter(std::string _val, std::string _key)
{
    if(_key == "position")
    {
        if(m_positionMap.find(_val) != m_positionMap.end())
        {
            m_notifcationsPosition = m_positionMap[_val];
        }
        else
        {
            SLM_ERROR("Value '" + _val + "' is not handled for key " + _key);
        }
    }
    else
    {
        SLM_ERROR("Value '" + _val + "' is not handled for key " + _key);
    }

}

//-----------------------------------------------------------------------------

void SNotifier::popInfo(std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::INFO);
}

//-----------------------------------------------------------------------------

void SNotifier::popSuccess(std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::SUCCESS);
}

//-----------------------------------------------------------------------------

void SNotifier::popFailure(std::string _message)
{
    this->showNotification(_message, sight::ui::base::dialog::INotificationDialog::Type::FAILURE);
}

//-----------------------------------------------------------------------------

void SNotifier::showNotification(const std::string& _message, sight::ui::base::dialog::INotificationDialog::Type _type)
{
    size_t indexOfCurrentNotif = 0;
    bool foundAPlace           = false;

    // Find the first free place.
    for (size_t i = 0; i < m_popups.size(); ++i)
    {
        // No popup or a recently hidden popup.
        if(m_popups[i] == nullptr || !m_popups[i]->isVisible())
        {
            m_popups[i].reset();
            indexOfCurrentNotif = i;
            foundAPlace         = true;
            break;
        }
    }

    // No place found, find the oldest (use the indexQueue).
    if(!foundAPlace)
    {
        indexOfCurrentNotif = m_indexQueue.front(); // Oldest index.
        m_indexQueue.pop(); // Remove it.

        //Remove the oldest one.
        m_popups[indexOfCurrentNotif]->close();
        m_popups[indexOfCurrentNotif].reset();
    }

    std::string messageToShow;

    if(_message.empty())
    {
        messageToShow = m_defaultMessage;
    }
    else
    {
        messageToShow = _message;
    }

    sight::ui::base::dialog::NotificationDialog::sptr notif =
        sight::ui::base::dialog::NotificationDialog::New();

    notif->setContainer(m_containerWhereToDisplayNotifs);

    notif->setMessage(messageToShow);
    notif->setType(_type);
    notif->setPosition(m_notifcationsPosition);
    notif->setIndex(static_cast<unsigned int>(indexOfCurrentNotif));
    notif->setDuration(m_durationInMs);
    notif->show();

    m_popups[indexOfCurrentNotif] = notif;

    if(m_indexQueue.size() == m_maxStackedNotifs)
    {
        m_indexQueue.pop();
    }

    m_indexQueue.push(indexOfCurrentNotif);

}

//-----------------------------------------------------------------------------

}
