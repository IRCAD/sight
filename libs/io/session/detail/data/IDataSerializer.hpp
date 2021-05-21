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

#include <core/crypto/secure_string.hpp>
#include <core/macros.hpp>

#include <data/Object.hpp>

#include <io/zip/ArchiveWriter.hpp>

#include <boost/property_tree/ptree.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Interface class used to deserialize objects from a session
class IDataSerializer
{
public:

    SIGHT_DECLARE_CLASS(IDataSerializer)

    /// Delete default copy constructors and assignment operators
    IDataSerializer(const IDataSerializer&)            = delete;
    IDataSerializer(IDataSerializer&&)                 = delete;
    IDataSerializer& operator=(const IDataSerializer&) = delete;
    IDataSerializer& operator=(IDataSerializer&&)      = delete;

    /// Default destructor. Public to allow unique_ptr
    virtual ~IDataSerializer() = default;

    /// Serialization function
    /// @param archive output archive where to write binary files to
    /// @param tree output tree where to write trivial class members to
    /// @param object the object to serialize
    /// @param children [OUT] the list of child objects belonging to object
    /// @param password (optional) the password to use if encryption is enabled
    virtual void serialize(
        const zip::ArchiveWriter::sptr& archive,
        boost::property_tree::ptree& tree,
        const sight::data::Object::csptr& object,
        std::map<std::string, sight::data::Object::csptr>& children,
        const core::crypto::secure_string& password = ""
    ) const                                         = 0;

protected:

    /// Default constructor
    IDataSerializer() = default;

    /// Convenience function to safely put strings into a tree
    /// @param tree boost property tree where string data must be stored
    /// @param key the string data key
    /// @param value the string data
    /// @param password (optional) password used for encryption
    static void writeToTree(
        boost::property_tree::ptree& tree,
        const std::string& key,
        const std::string& value,
        const core::crypto::secure_string& password = ""
    );
};

} // namespace detail::data

} // namespace sight::io::session
