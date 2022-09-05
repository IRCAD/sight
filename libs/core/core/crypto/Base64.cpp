/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include "Base64.hpp"

#include <core/exceptionmacros.hpp>

#include <openssl/evp.h>

#include <string>

namespace sight::core::crypto
{

//------------------------------------------------------------------------------
template<typename T>
inline static T fromBase64(const T& message)
{
    const std::size_t message_size = message.size();

    // Do nothing if the message is empty...
    if(message_size <= 0)
    {
        return message;
    }

    // Compute the ouput size
    const std::size_t predicted = 3 * message_size / 4;

    // Create the output (take care of final \0);
    T output;
    output.resize(predicted + 1);

    // Decode
    const int output_size = EVP_DecodeBlock(
        reinterpret_cast<unsigned char*>(output.data()),
        reinterpret_cast<const unsigned char*>(message.data()),
        static_cast<int>(message_size)
    );

    SIGHT_THROW_EXCEPTION_IF(
        core::Exception("Decoding base64 message failed."),
        output_size < 0
    );

    // Compute padding to be able to remove it
    std::size_t padding = 0;

    if(message[message_size - 1] == '=')
    {
        padding++;

        if(message[message_size - 2] == '=')
        {
            padding++;
        }
    }

    output.resize(static_cast<std::size_t>(output_size) - padding);

    return output;
}

//------------------------------------------------------------------------------

secure_string from_base64(const secure_string& message)
{
    return fromBase64<secure_string>(message);
}

//------------------------------------------------------------------------------

std::string from_base64(const std::string& message)
{
    return fromBase64<std::string>(message);
}

//------------------------------------------------------------------------------

template<typename T>
T toBase64(const T& message)
{
    // Compute the output size
    const std::size_t predicted = 4 * ((message.size() + 2) / 3);

    // Create the output (take care of final \0);
    T output;
    output.resize(predicted + 1);

    // Encode
    const int output_size = EVP_EncodeBlock(
        reinterpret_cast<unsigned char*>(output.data()),
        reinterpret_cast<const unsigned char*>(message.data()),
        static_cast<int>(message.size())
    );

    output.resize(static_cast<std::size_t>(output_size));

    return output;
}

//------------------------------------------------------------------------------

secure_string to_base64(const secure_string& message)
{
    return toBase64<secure_string>(message);
}

//------------------------------------------------------------------------------

std::string to_base64(const std::string& message)
{
    return toBase64<std::string>(message);
}

} // namespace sight::core::crypto
