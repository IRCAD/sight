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

#include "SPushSelection.hpp"

#include <data/SeriesSet.hpp>
#include <data/Vector.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

namespace sight::module::ui::series
{

//------------------------------------------------------------------------------

SPushSelection::SPushSelection() noexcept =
    default;

//------------------------------------------------------------------------------

SPushSelection::~SPushSelection() noexcept =
    default;

//------------------------------------------------------------------------------

void SPushSelection::info(std::ostream& _sstream)
{
    _sstream << "SPushSelection::info";
}

//------------------------------------------------------------------------------

void SPushSelection::starting()
{
}

//------------------------------------------------------------------------------

void SPushSelection::stopping()
{
}

//------------------------------------------------------------------------------

void SPushSelection::configuring()
{
}

//------------------------------------------------------------------------------

void SPushSelection::updating()
{
    auto dest_series_set = m_series_set.lock();
    SIGHT_ASSERT("\"seriesSet\" key is not found.", dest_series_set);

    const auto selectedSeries = m_selectedSeries.lock();

    // Save added series in this container in order to display information on the push
    std::vector<data::Series::sptr> addedSeries;

    {
        const auto scoped_emitter = dest_series_set->scoped_emit();

        // Loop through all selected series
        for(const auto& object : *selectedSeries)
        {
            const auto& series = std::dynamic_pointer_cast<data::Series>(object);

            if(series && dest_series_set->insert(dest_series_set->cend(), series).second)
            {
                addedSeries.push_back(series);
            }
        }
    }

    // Display the informations
    sight::ui::dialog::message messageBox;
    messageBox.setIcon(sight::ui::dialog::message::INFO);
    messageBox.addButton(sight::ui::dialog::message::OK);
    messageBox.setTitle("Push Series");

    if(selectedSeries->empty())
    {
        messageBox.setMessage("Unable to push series, there is no series selected.");
    }
    else if(addedSeries.empty())
    {
        messageBox.setMessage("Unable to push series, the series are already present in the database.");
    }
    else
    {
        std::stringstream ss;
        if(addedSeries.size() == 1)
        {
            ss << addedSeries.size() << " series has been correctly pushed in the database:\n";
        }
        else
        {
            ss << addedSeries.size() << " series have been correctly pushed in the database:\n";
        }

        for(const auto& series : addedSeries)
        {
            std::string description = series->getSeriesDescription();
            description = (description.empty()) ? "[No description]" : description;
            ss << "- " << description << std::endl;
        }

        messageBox.setMessage(ss.str());
    }

    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::series
