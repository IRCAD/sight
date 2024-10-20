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

// cspell:ignore NOLINT

#pragma once

#include <sight/core/config.hpp>

#include "core/crypto/secure_string.hpp"

// Convenience macro to generate a pseudo random hash in a pseudo predictable way.
#define SIGHT_PSEUDO_RANDOM_HASH(salt) \
        sight::core::crypto::hash( \
            sight::core::crypto::secure_string( \
                __FILE__ \
                + std::to_string(__LINE__) \
                + std::string(salt) \
            ) \
        )

namespace sight::core::crypto
{

static const int HASH_SIZE = 32;

/// Compute a sha256 hash using openSSL
/// @param _message the message to hash
/// @param _output the output hash
SIGHT_CORE_API void hash(const secure_string& _message, std::array<std::uint8_t, HASH_SIZE>& _output);

/// Compute a sha256 hash using openSSL and return the result as a secure_string
/// @param _message the message to hash
SIGHT_CORE_API secure_string hash(const secure_string& _message);

} // namespace sight::core::crypto
