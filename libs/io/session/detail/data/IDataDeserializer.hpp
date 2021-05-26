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

#include <io/zip/ArchiveReader.hpp>

#include <boost/property_tree/ptree.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Interface class used to deserialize objects from a session
class IDataDeserializer
{
public:

    SIGHT_DECLARE_CLASS(IDataDeserializer)

    /// Delete default copy constructors and assignment operators
    IDataDeserializer(const IDataDeserializer&)            = delete;
    IDataDeserializer(IDataDeserializer&&)                 = delete;
    IDataDeserializer& operator=(const IDataDeserializer&) = delete;
    IDataDeserializer& operator=(IDataDeserializer&&)      = delete;

    /// Default destructor. Public to allow unique_ptr
    virtual ~IDataDeserializer() = default;

    /// Serialization function
    /// @param archive the archive where to read binary files
    /// @param tree the tree where to read trivial object members
    /// @param children a map of data object that belongs to the object
    /// @param object the object to deserialize using the archive, the tree and the children map
    /// @param password (optional) the password used for encryption
    virtual sight::data::Object::sptr deserialize(
        const zip::ArchiveReader::sptr& archive,
        const boost::property_tree::ptree& tree,
        const std::map<std::string, sight::data::Object::sptr>& children,
        const sight::data::Object::sptr& object,
        const core::crypto::secure_string& password = ""
    ) const                                         = 0;

protected:

    /// Default constructor
    IDataDeserializer() = default;

    /// Convenience function to safely read strings from a tree
    /// @param tree boost property tree where string data are stored
    /// @param key the string data key
    /// @param password (optional) password used for encryption
    static std::string readFromTree(
        const boost::property_tree::ptree& tree,
        const std::string& key,
        const core::crypto::secure_string& password = ""
    );
};

} // namespace detail::data

} // namespace sight::io::session
