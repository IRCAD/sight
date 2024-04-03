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

namespace sight::core::crypto
{

/// Decrypt a message using AES256 algorithm
/// @param _message encrypted message
/// @param _password password to use to decrypt the message
/// @returns decrypted message
SIGHT_CORE_API std::string decrypt(const std::string& _message, const secure_string& _password);
SIGHT_CORE_API secure_string decrypt(const secure_string& _message, const secure_string& _password);
SIGHT_CORE_API secure_string decrypt(const char* _message, const secure_string& _password);

/// Encrypt a message using AES256 algorithm
/// @param _message clear message
/// @param _password password to use to encrypt the message
/// @returns encrypted message
SIGHT_CORE_API std::string encrypt(const std::string& _message, const secure_string& _password);
SIGHT_CORE_API secure_string encrypt(const secure_string& _message, const secure_string& _password);
SIGHT_CORE_API secure_string encrypt(const char* _message, const secure_string& _password);

} // namespace sight::core::crypto
