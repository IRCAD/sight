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

#include <string>

namespace sight::core::notification
{

/**
 * @brief Error notification.
 */
class SIGHT_CORE_CLASS_API error final : public message
{
public:

    SIGHT_DECLARE_CLASS(error, message);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// @see message::message(std::string, std::string)
    SIGHT_CORE_API explicit error(std::string _title = {}, std::string _text = {});

    /// @see message::message(params)
    SIGHT_CORE_API explicit error(params _params);

    SIGHT_CORE_API ~error() final = default;
};

} //namespace sight::core::notification
