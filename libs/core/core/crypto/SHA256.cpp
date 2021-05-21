/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "SHA256.hpp"

#include <openssl/sha.h>

#include <iomanip>
#include <sstream>

namespace sight::core::crypto
{

//------------------------------------------------------------------------------

void hash(const secure_string& message, unsigned char output[HASH_SIZE])
{
    // Compute SHA256 using openssl
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, message.c_str(), message.length());
    SHA256_Final(output, &sha256);
}

//------------------------------------------------------------------------------

secure_string hash(const secure_string& message)
{
    // The hash array
    unsigned char output[HASH_SIZE];

    // Compute SHA256 using openssl
    hash(message, output);

    // Convert the hash to an hexadecimal string
    std::ostringstream stream;

    stream << std::hex << std::setfill('0');
    for(int byte : output)
    {
        stream << std::setw(2) << byte;
    }

    return secure_string(stream.str());
}

} // namespace sight::core::crypto
