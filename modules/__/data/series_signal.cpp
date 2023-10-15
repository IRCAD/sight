/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE

#include "series_signal.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hpp>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/activity.hpp>

#include <service/macros.hpp>

#include <boost/foreach.hpp>

namespace sight::module::data
{

//------------------------------------------------------------------------------

const core::com::slots::key_t series_signal::REPORT_SERIES_SLOT = "reportSeries";

const core::com::signals::key_t series_signal::SERIES_ADDED_SIG = "seriesAdded";

//------------------------------------------------------------------------------

series_signal::series_signal() noexcept :
    m_sigSeriesAdded(new_signal<SeriesAddedSignalType>(SERIES_ADDED_SIG))
{
    new_slot(REPORT_SERIES_SLOT, &series_signal::reportSeriesSlot, this);
}

//------------------------------------------------------------------------------

series_signal::~series_signal() noexcept =
    default;

//------------------------------------------------------------------------------

void series_signal::starting()
{
}

//------------------------------------------------------------------------------

void series_signal::stopping()
{
}

//------------------------------------------------------------------------------

void series_signal::configuring()
{
    const service::config_t srvconfig = this->get_config();

    if(srvconfig.count("filter") == 1)
    {
        const service::config_t& configFilter = srvconfig.get_child("filter");
        SIGHT_ASSERT("A maximum of 1 <mode> tag is allowed", configFilter.count("mode") < 2);

        const auto mode = configFilter.get<std::string>("mode");
        SIGHT_ASSERT(
            "'" + mode + "' value for <mode> tag isn't valid. Allowed values are : 'include', 'exclude'.",
            mode == "include" || mode == "exclude"
        );
        m_filterMode = mode;

        // NOLINTNEXTLINE(bugprone-branch-clone)
        BOOST_FOREACH(const config_t::value_type& v, configFilter.equal_range("type"))
        {
            m_types.push_back(v.second.get<std::string>(""));
        }
    }

    SIGHT_ASSERT("A maximum of 1 <filter> tag is allowed", srvconfig.count("filter") < 2);
}

//------------------------------------------------------------------------------

template<typename T>
void series_signal::reportSeries(const T& addedSeries)
{
    const bool isIncludeMode = m_filterMode == "include";

    for(const auto& series : addedSeries)
    {
        const auto keyIt = std::find(m_types.cbegin(), m_types.cend(), series->get_classname());

        if(keyIt != m_types.end() && isIncludeMode)
        {
            m_sigSeriesAdded->async_emit(series);
        }
    }
}

//------------------------------------------------------------------------------

void series_signal::reportSeriesSlot(sight::data::series_set::container_type addedSeries)
{
    reportSeries(addedSeries);
}

//------------------------------------------------------------------------------

void series_signal::updating()
{
    const auto series_set = m_series_set.lock();
    SIGHT_ASSERT("input '" << s_SERIES_SET_INPUT << "' does not exist.", series_set);

    reportSeries(*series_set);
}

//------------------------------------------------------------------------------

service::connections_t series_signal::auto_connections() const
{
    return {
        {s_SERIES_SET_INPUT, sight::data::series_set::ADDED_OBJECTS_SIG, REPORT_SERIES_SLOT},
        {s_SERIES_SET_INPUT, sight::data::series_set::MODIFIED_SIG, service::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

} // namespace sight::module::data
