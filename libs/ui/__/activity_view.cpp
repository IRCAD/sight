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

#include "ui/__/activity_view.hpp"

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

namespace sight::ui
{

const core::com::slots::key_t activity_view::LAUNCH_ACTIVITY_SLOT = "launch_activity";

//-----------------------------------------------------------------------------

activity_view::activity_view()
{
    new_slot(LAUNCH_ACTIVITY_SLOT, &activity_view::launch_activity, this);
}

//------------------------------------------------------------------------------

void activity_view::configuring()
{
    this->ui::service::initialize();

    const config_t config = this->get_config();

    sight::activity::launcher::in_out_map_t inout_map;
    std::for_each(
        m_data.begin(),
        m_data.end(),
        [&inout_map](const auto& _p)
        {
            const auto obj = _p.second->lock();
            if(obj != nullptr)
            {
                inout_map.push_back(obj->get_id());
            }
        });
    this->parse_configuration(config, inout_map);
}

//------------------------------------------------------------------------------

bool activity_view::validate_activity(data::activity::sptr _activity) const
{
    bool is_valid = false;
    std::string message;

    std::tie(is_valid, message) = activity::launcher::validate_activity(_activity);

    if(!is_valid)
    {
        ui::dialog::message::show(
            "Activity launch",
            message,
            ui::dialog::message::critical
        );
    }

    return is_valid;
}

//------------------------------------------------------------------------------

data::activity::sptr activity_view::create_main_activity() const
{
    auto activity = this->activity::launcher::create_main_activity();

    if(!activity)
    {
        ui::dialog::message::show(
            "Main activity",
            "The main activity " + m_main_activity_id + " can not be launched. \n"
                                                        "This activity needs parameters that cannot be defined",
            ui::dialog::message::info
        );
    }

    return activity;
}

} // namespace sight::ui
