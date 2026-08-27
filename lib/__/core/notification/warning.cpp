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

#include "core/notification/warning.hpp"

#include <utility>

namespace sight::core::notification
{

warning::warning(
    std::string _title,
    std::string _text,
    std::filesystem::path _icon,
    std::string _channel,
    bool _cancelable,
    cancel_hook _cancel_hook,
    std::optional<std::chrono::milliseconds> _duration,
    std::optional<bool> _sound
) :
    message(std::move(_title),
            std::move(_text),
            std::move(_icon),
            std::move(_channel),
            _cancelable,
            std::move(_cancel_hook),
            _duration,
            _sound)
{
}

//------------------------------------------------------------------------------

} //namespace sight::core::notification
