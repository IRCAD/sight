/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "ui/__/view/IActivityView.hpp"

#include "ui/__/dialog/message.hpp"

#include <activity/validator/activity.hpp>
#include <activity/validator/base.hpp>

#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/tools/date_and_time.hpp>
#include <core/tools/uuid.hpp>

#include <data/composite.hpp>
#include <data/string.hpp>

#include <service/macros.hpp>

namespace sight::ui::view
{

const core::com::slots::key_t IActivityView::LAUNCH_ACTIVITY_SLOT = "launchActivity";

//-----------------------------------------------------------------------------

IActivityView::IActivityView()
{
    new_slot(LAUNCH_ACTIVITY_SLOT, &IActivityView::launchActivity, this);
}

//------------------------------------------------------------------------------

void IActivityView::configuring()
{
    this->ui::service::initialize();

    const config_t config = this->get_config();

    sight::activity::launcher::InOutMapType inoutMap;
    std::for_each(
        m_data.begin(),
        m_data.end(),
        [&inoutMap](const auto& p)
        {
            const auto obj = p.second->lock();
            if(obj != nullptr)
            {
                inoutMap.push_back(obj->get_id());
            }
        });
    this->parseConfiguration(config, inoutMap);
}

//------------------------------------------------------------------------------

bool IActivityView::validateActivity(data::activity::sptr activity) const
{
    bool isValid = false;
    std::string message;

    std::tie(isValid, message) = activity::launcher::validateActivity(activity);

    if(!isValid)
    {
        ui::dialog::message::show(
            "Activity launch",
            message,
            ui::dialog::message::CRITICAL
        );
    }

    return isValid;
}

//------------------------------------------------------------------------------

data::activity::sptr IActivityView::createMainActivity() const
{
    auto activity = this->activity::launcher::createMainActivity();

    if(!activity)
    {
        ui::dialog::message::show(
            "Main activity",
            "The main activity " + m_mainActivityId + " can not be launched. \n"
                                                      "This activity needs parameters that cannot be defined",
            ui::dialog::message::INFO
        );
    }

    return activity;
}

} // namespace sight::ui::view
