/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "push_selection.hpp"

#include <data/series_set.hpp>
#include <data/vector.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

namespace sight::module::ui::series
{

//------------------------------------------------------------------------------

push_selection::push_selection() noexcept =
    default;

//------------------------------------------------------------------------------

push_selection::~push_selection() noexcept =
    default;

//------------------------------------------------------------------------------

void push_selection::info(std::ostream& _sstream)
{
    _sstream << "push_selection::info";
}

//------------------------------------------------------------------------------

void push_selection::starting()
{
}

//------------------------------------------------------------------------------

void push_selection::stopping()
{
}

//------------------------------------------------------------------------------

void push_selection::configuring()
{
}

//------------------------------------------------------------------------------

void push_selection::updating()
{
    auto dest_series_set = m_series_set.lock();
    SIGHT_ASSERT("\"seriesSet\" key is not found.", dest_series_set);

    const auto selected_series = m_selected_series.lock();

    // Save added series in this container in order to display information on the push
    std::vector<data::series::sptr> added_series;

    {
        const auto scoped_emitter = dest_series_set->scoped_emit();

        // Loop through all selected series
        for(const auto& object : *selected_series)
        {
            const auto& series = std::dynamic_pointer_cast<data::series>(object);

            if(series && dest_series_set->insert(dest_series_set->cend(), series).second)
            {
                added_series.push_back(series);
            }
        }
    }

    // Display the informations
    sight::ui::dialog::message message_box;
    message_box.set_icon(sight::ui::dialog::message::info);
    message_box.add_button(sight::ui::dialog::message::ok);
    message_box.set_title("Push Series");

    if(selected_series->empty())
    {
        message_box.set_message("Unable to push series, there is no series selected.");
    }
    else if(added_series.empty())
    {
        message_box.set_message("Unable to push series, the series are already present in the database.");
    }
    else
    {
        std::stringstream ss;
        if(added_series.size() == 1)
        {
            ss << added_series.size() << " series has been correctly pushed in the database:\n";
        }
        else
        {
            ss << added_series.size() << " series have been correctly pushed in the database:\n";
        }

        for(const auto& series : added_series)
        {
            std::string description = series->get_series_description();
            description = (description.empty()) ? "[No description]" : description;
            ss << "- " << description << std::endl;
        }

        message_box.set_message(ss.str());
    }

    message_box.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::series
