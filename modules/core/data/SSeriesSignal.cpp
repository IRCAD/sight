/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include "SSeriesSignal.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/Activity.hpp>

#include <service/macros.hpp>

#include <boost/foreach.hpp>

namespace sight::module::data
{

//------------------------------------------------------------------------------

const core::com::Slots::SlotKeyType SSeriesSignal::s_REPORT_SERIES_SLOT = "reportSeries";

const core::com::Signals::SignalKeyType SSeriesSignal::s_SERIES_ADDED_SIG = "seriesAdded";

//------------------------------------------------------------------------------

SSeriesSignal::SSeriesSignal() noexcept :
    m_sigSeriesAdded(newSignal<SeriesAddedSignalType>(s_SERIES_ADDED_SIG))
{
    newSlot(s_REPORT_SERIES_SLOT, &SSeriesSignal::reportSeriesSlot, this);
}

//------------------------------------------------------------------------------

SSeriesSignal::~SSeriesSignal() noexcept =
    default;

//------------------------------------------------------------------------------

void SSeriesSignal::starting()
{
}

//------------------------------------------------------------------------------

void SSeriesSignal::stopping()
{
}

//------------------------------------------------------------------------------

void SSeriesSignal::configuring()
{
    const service::IService::ConfigType srvconfig = this->getConfiguration();

    if(srvconfig.count("filter") == 1)
    {
        const service::IService::ConfigType& configFilter = srvconfig.get_child("filter");
        SIGHT_ASSERT("A maximum of 1 <mode> tag is allowed", configFilter.count("mode") < 2);

        const auto mode = configFilter.get<std::string>("mode");
        SIGHT_ASSERT(
            "'" + mode + "' value for <mode> tag isn't valid. Allowed values are : 'include', 'exclude'.",
            mode == "include" || mode == "exclude"
        );
        m_filterMode = mode;

        // NOLINTNEXTLINE(bugprone-branch-clone)
        BOOST_FOREACH(const ConfigType::value_type& v, configFilter.equal_range("type"))
        {
            m_types.push_back(v.second.get<std::string>(""));
        }
    }

    SIGHT_ASSERT("A maximum of 1 <filter> tag is allowed", srvconfig.count("filter") < 2);
}

//------------------------------------------------------------------------------

template<typename T>
void SSeriesSignal::reportSeries(const T& addedSeries)
{
    const bool isIncludeMode = m_filterMode == "include";

    for(const auto& series : addedSeries)
    {
        const auto keyIt = std::find(m_types.cbegin(), m_types.cend(), series->getClassname());

        if(keyIt != m_types.end() && isIncludeMode)
        {
            m_sigSeriesAdded->asyncEmit(series);
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesSignal::reportSeriesSlot(sight::data::SeriesSet::container_type addedSeries)
{
    reportSeries(addedSeries);
}

//------------------------------------------------------------------------------

void SSeriesSignal::updating()
{
    const auto series_set = m_series_set.lock();
    SIGHT_ASSERT("input '" << s_SERIES_SET_INPUT << "' does not exist.", series_set);

    reportSeries(*series_set);
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SSeriesSignal::getAutoConnections() const
{
    return {
        {s_SERIES_SET_INPUT, sight::data::SeriesSet::s_ADDED_OBJECTS_SIG, s_REPORT_SERIES_SLOT},
        {s_SERIES_SET_INPUT, sight::data::SeriesSet::s_MODIFIED_SIG, s_UPDATE_SLOT}
    };
}

//------------------------------------------------------------------------------

} // namespace sight::module::data
