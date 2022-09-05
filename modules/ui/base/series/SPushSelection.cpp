/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <data/helper/SeriesDB.hpp>
#include <data/Vector.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

namespace sight::module::ui::base::series
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
    auto destinationSeriesDB = m_seriesDB.lock();
    SIGHT_ASSERT("'seriesDB' key is not found.", destinationSeriesDB);

    data::helper::SeriesDB sDBhelper(*destinationSeriesDB);
    data::SeriesDB::ContainerType container = destinationSeriesDB->getContainer();
    const auto selectedSeries               = m_selectedSeries.lock();

    // Save added series in this container in order to display information on the push
    std::vector<data::Series::sptr> addedSeries;

    // Loop through all selected series
    for(const data::Object::sptr& obj : *selectedSeries)
    {
        data::Series::sptr series = data::Series::dynamicCast(obj);

        if(series && std::find(container.begin(), container.end(), series) == container.end())
        {
            sDBhelper.add(series);
            addedSeries.push_back(series);
        }
    }

    sDBhelper.notify();

    // Display the informations
    sight::ui::base::dialog::MessageDialog messageBox;
    messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::INFO);
    messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
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

        for(const data::Series::sptr& series : addedSeries)
        {
            std::string description = series->getDescription();
            description = (description.empty()) ? "[No description]" : description;
            ss << "- " << description << std::endl;
        }

        messageBox.setMessage(ss.str());
    }

    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::series
