/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "STimestampSignal.hpp"

#include <core/com/signal.hxx>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

#include <boost/date_time.hpp>

namespace sight::module::ui::com
{

//-----------------------------------------------------------------------------

static const core::com::signals::key_t TRIGGERED_SIG           = "triggered";
static const core::com::signals::key_t TRIGGERED_AS_STRING_SIG = "triggeredAsString";

//-----------------------------------------------------------------------------

STimestampSignal::STimestampSignal() noexcept :
    m_sigTriggered(new_signal<TriggeredSignalType>(TRIGGERED_SIG)),
    m_sigTriggeredAsString(new_signal<TriggeredAsStringSignalType>(TRIGGERED_AS_STRING_SIG))
{
}

//-----------------------------------------------------------------------------

STimestampSignal::~STimestampSignal() noexcept =
    default;

//-----------------------------------------------------------------------------

void STimestampSignal::configuring()
{
    this->initialize();

    service::config_t config = this->getConfiguration();
    m_useSystemClock     = config.get<bool>("useSystemClock", false);
    m_formatStringAsDate = config.get<bool>("formatStringAsDate", true);
}

//-----------------------------------------------------------------------------

void STimestampSignal::starting()
{
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void STimestampSignal::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void STimestampSignal::info(std::ostream& _sstream)
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void STimestampSignal::updating()
{
    double ts = 0.0;
    if(m_useSystemClock)
    {
        const auto now = std::chrono::system_clock::now();
        const auto res = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        ts = static_cast<double>(res);
    }
    else
    {
        const auto now = std::chrono::high_resolution_clock::now();
        const auto res = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        ts = static_cast<double>(res);
    }

    m_sigTriggered->async_emit(ts);

    // Should we return the timestamp as a string or format it ?
    const auto tt                            = static_cast<std::int64_t>(ts);
    const boost::posix_time::ptime localTime =
        boost::posix_time::from_time_t(tt);

    std::ostringstream oss("");
    oss << boost::gregorian::to_iso_extended_string(localTime.date()) << "_";
    oss << std::setw(2) << std::setfill('0') << localTime.time_of_day().hours();
    oss << std::setw(2) << std::setfill('0') << localTime.time_of_day().minutes();
    oss << std::setw(2) << std::setfill('0') << localTime.time_of_day().seconds();

    m_sigTriggeredAsString->async_emit(oss.str());
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::com
