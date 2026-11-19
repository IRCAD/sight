/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

// cspell:ignore qgetenv

#pragma once

#include <QByteArray>
#include <QtGlobal>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <stdexcept>

namespace sight::ui::test::detail
{

/// The factor of SIGHT_UI_TEST_TIMEOUT_SCALE, for a machine known to be slow: 1 when unset.
[[nodiscard]] inline double timeout_scale()
{
    const QByteArray text = qgetenv("SIGHT_UI_TEST_TIMEOUT_SCALE");
    if(text.isEmpty())
    {
        return 1.;
    }

    // A wrong value must not pass as "no scale": the deadlines of the whole run would silently stay short.
    bool valid         = false;
    const double scale = text.toDouble(&valid);
    if(!valid || !std::isfinite(scale) || scale <= 0.)
    {
        throw std::invalid_argument(
                  "SIGHT_UI_TEST_TIMEOUT_SCALE must be a positive number, not '" + text.toStdString() + "'"
        );
    }

    return scale;
}

/// A duration of the test engine on this machine. Apply it once per operation, which deadline_after() does.
/// Rounded up to a tick, so a positive duration stays positive, and kept within what the clock counts.
[[nodiscard]] inline std::chrono::steady_clock::duration scaled(std::chrono::steady_clock::duration _duration)
{
    using duration = std::chrono::steady_clock::duration;
    const double ticks = std::ceil(static_cast<double>(_duration.count()) * timeout_scale());
    if(ticks >= static_cast<double>(duration::max().count()))
    {
        return duration::max();
    }

    if(ticks <= static_cast<double>(duration::min().count()))
    {
        return duration::min();
    }

    return duration(static_cast<duration::rep>(ticks));
}

/// The deadline of an operation that starts now: the latest the clock can tell when it is further.
[[nodiscard]] inline std::chrono::steady_clock::time_point deadline_after(std::chrono::steady_clock::duration _timeout)
{
    const auto now     = std::chrono::steady_clock::now();
    const auto timeout = scaled(_timeout);
    if(timeout > std::chrono::steady_clock::time_point::max() - now)
    {
        return std::chrono::steady_clock::time_point::max();
    }

    return now + timeout;
}

/// How long one look waits for the GUI to answer, whatever the timeout of its step: a step fails on what the GUI
/// showed, never on a GUI too busy to answer in the time asked.
inline constexpr std::chrono::seconds LOOK_TIMEOUT {30};

/// The deadline of the first attempt of an operation that ends at _deadline: the GUI answers it, however short the
/// timeout.
[[nodiscard]] inline std::chrono::steady_clock::time_point first_attempt_deadline(
    std::chrono::steady_clock::time_point _deadline
)
{
    return std::max(_deadline, deadline_after(LOOK_TIMEOUT));
}

} // namespace sight::ui::test::detail
