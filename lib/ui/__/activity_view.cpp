/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
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

#include <boost/range/iterator_range_core.hpp>

#include <algorithm>

namespace sight::ui
{

//-----------------------------------------------------------------------------

activity_view::activity_view()
{
    new_slot(slots::LAUNCH_ACTIVITY, &activity_view::launch_activity, this);
}

//------------------------------------------------------------------------------

void activity_view::configuring()
{
    this->ui::service::initialize();

    const config_t config = this->get_config();

    sight::activity::launcher::in_out_map_t inout_map;
    std::ranges::for_each(
        m_data,

        [&inout_map](const auto& _p)
        {
            const auto obj = _p.second->lock();
            if(obj != nullptr)
            {
                inout_map.push_back(obj->get_id());
            }
        });

    // Objects declared with the hierarchical syntax keep their position, unbound ones leave an empty slot
    sight::activity::launcher::in_out_map_t bound_data_ids;
    std::size_t data_index = 0;
    for([[maybe_unused]] const auto& _ : boost::make_iterator_range(config.equal_range("data")))
    {
        const auto obj = this->inout(m_data_uids.key(), data_index++).lock();
        bound_data_ids.emplace_back(obj != nullptr ? obj->get_id() : std::string());
    }

    this->parse_configuration(config, inout_map, bound_data_ids);
}

//------------------------------------------------------------------------------

std::optional<std::string> activity_view::resolve_object_type(
    std::string_view _key,
    std::optional<std::size_t> _index
) const
{
    if(_index.has_value() && _key == m_data.key())
    {
        return std::nullopt;
    }

    return service::base::resolve_object_type(_key, _index);
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
