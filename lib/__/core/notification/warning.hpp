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

#pragma once

#include <sight/core/config.hpp>

#include "message.hpp"

#include <chrono>
#include <filesystem>
#include <optional>
#include <string>

namespace sight::core::notification
{

/**
 * @brief Warning notification.
 */
class SIGHT_CORE_CLASS_API warning final : public message
{
public:

    SIGHT_DECLARE_CLASS(warning, message);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    SIGHT_CORE_API explicit warning(
        std::string _title                                 = {},
        std::string _text                                  = {},
        std::filesystem::path _icon                        = {},
        std::string _channel                               = {},
        bool _cancelable                                   = false,
        cancel_hook _cancel_hook                           = nullptr,
        std::optional<std::chrono::milliseconds> _duration = std::nullopt,
        std::optional<bool> _sound                         = std::nullopt
    );

    SIGHT_CORE_API ~warning() override = default;
};

} //namespace sight::core::notification
