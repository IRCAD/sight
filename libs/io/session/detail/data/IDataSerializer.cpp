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

#include "IDataSerializer.hpp"

#include <core/crypto/AES256.hpp>
#include <core/crypto/Base64.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

void IDataSerializer::writeToTree(
    boost::property_tree::ptree& tree,
    const std::string& key,
    const std::string& value,
    const core::crypto::secure_string& password
)
{
    const auto& raw    = password.empty() ? value : core::crypto::encrypt(value, password + key.c_str());
    const auto& base64 = core::crypto::to_base64(raw);
    tree.put(key, base64);
}

} // namespace detail::data

} // namespace sight::io::session
