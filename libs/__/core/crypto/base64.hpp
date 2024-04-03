/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include "core/crypto/secure_string.hpp"

#include <string>

namespace sight::core::crypto
{

/// Decode a base64 message
/// @param _message encoded message
/// @returns decoded message
SIGHT_CORE_API secure_string from_base64(const secure_string& _message);

SIGHT_CORE_API std::string from_base64(const std::string& _message);

/// Encode a message to base64
/// @param _message clear message
/// @returns encoded message
SIGHT_CORE_API secure_string to_base64(const secure_string& _message);

SIGHT_CORE_API std::string to_base64(const std::string& _message);

} // namespace sight::core::crypto
