/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "io/session/config.hpp"
#include "io/session/detail/data/GenericDeserializer.hpp"

#include <data/String.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Class used to deserialize String objects from a session
class StringDeserializer : public GenericDeserializer<sight::data::String>
{
public:

    SIGHT_DECLARE_CLASS(StringDeserializer, GenericDeserializer<sight::data::String>);

    /// Delete default copy constructors and assignment operators
    StringDeserializer(const StringDeserializer&)            = delete;
    StringDeserializer(StringDeserializer&&)                 = delete;
    StringDeserializer& operator=(const StringDeserializer&) = delete;
    StringDeserializer& operator=(StringDeserializer&&)      = delete;

    /// Default constructor
    StringDeserializer() = default;

    /// Default destructor. Public to allow unique_ptr
    ~StringDeserializer() override = default;

    /// Deserialization function
    sight::data::Object::sptr deserialize(
        const zip::ArchiveReader::sptr& archive,
        const boost::property_tree::ptree& tree,
        const std::map<std::string, sight::data::Object::sptr>& children,
        const sight::data::Object::sptr& object,
        const core::crypto::secure_string& password = ""
    ) const override;
};

} // namespace detail::data

} // namespace sight::io::session
