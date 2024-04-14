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

#include <sight/io/session/config.hpp>

#include <core/crypto/base64.hpp>

#include <io/zip/archive_reader.hpp>
#include <io/zip/archive_writer.hpp>

#include <boost/property_tree/ptree.hpp>

#include <optional>

namespace sight::io::session::helper
{

constexpr static auto VALUE {"Value"};
constexpr static auto VERSION {".version"};

/// Convenience function to read a version from a tree.
/// Optionally checks the version number, when minVersion or maxVersion > 0.
/// @param[in] _tree boost property tree where version is stored
/// @param[in] _min_version the minimum valid version
/// @param[in] _max_version the maximum valid version
template<typename T>
inline static int read_version(
    const boost::property_tree::ptree& _tree,
    const int _min_version = 0,
    const int _max_version = 0
)
{
    // Add a version number. Not mandatory, but could help for future release
    const int version = _tree.get<int>(T::classname() + VERSION, -1);

    SIGHT_THROW_IF(
        T::classname() << " deserialization is not implemented for version '" << version << "'.",
        (_min_version > 0 && _min_version > version) || (_max_version > 0 && _max_version < version)
    );

    return version;
}

/// Convenience function to write a version number in the property tree
/// @param[inout] _tree boost property tree where the version must be stored
/// @param[in] _version the version number to store
template<typename T>
inline static void write_version(boost::property_tree::ptree& _tree, const int _version = 1)
{
    // Add a version number. Not mandatory, but could help for future release
    _tree.put(T::classname() + VERSION, std::to_string(_version));
}

/// Convenience function to safely read strings from a tree
/// @param[in] _tree boost property tree where string data are stored
/// @param[in] _key the string data key
inline static std::string read_string(
    const boost::property_tree::ptree& _tree,
    const std::string& _key,
    const std::optional<std::string>& _default_value = std::nullopt
)
{
    try
    {
        const auto& base64 = _tree.get<std::string>(_key);
        return core::crypto::from_base64(base64);
    }
    catch(...)
    {
        if(_default_value)
        {
            return *_default_value;
        }

        throw;
    }
}

/// Convenience function to safely put strings into a tree
/// @param[inout] _tree boost property tree where string data must be stored
/// @param[in] _key the string data key
/// @param[in] _value the string data
inline static void write_string(
    boost::property_tree::ptree& _tree,
    const std::string& _key,
    const std::string& _value
)
{
    const auto& base64 = core::crypto::to_base64(_value);
    _tree.put(_key, base64);
}

/// Convenience function to cast and check an object
/// Mainly to factorize error management
/// @param[in] _object the object to cast to type T
template<typename T>
inline static typename T::sptr safe_cast(sight::data::object::sptr _object)
{
    const auto& casted = std::dynamic_pointer_cast<T>(_object);

    SIGHT_THROW_IF(
        "Object '"
        << (_object ? _object->get_classname() : sight::data::object::classname())
        << "' is not a '"
        << T::classname()
        << "'",
        casted == nullptr
    );

    return casted;
}

/// Convenience function to cast and check an object
/// Mainly to factorize error management
/// @param[in] _object the object to cast to type T
template<typename T>
inline static typename T::sptr cast_or_create(sight::data::object::sptr _object)
{
    if(_object)
    {
        return safe_cast<T>(_object);
    }

    return std::make_shared<T>();
}

/// Convenience function to cast and check an object
/// Mainly to factorize error management
/// @param[in] _object the object to cast to type T
template<typename T>
inline static typename T::csptr safe_cast(sight::data::object::csptr _object)
{
    const auto& casted = std::dynamic_pointer_cast<const T>(_object);

    SIGHT_THROW_IF(
        "Object '"
        << (_object ? _object->get_classname() : sight::data::object::classname())
        << "' is not a '"
        << T::classname()
        << "'",
        casted == nullptr
    );

    return casted;
}

/// Generic serialization function
/// @param[in,out] _archive output archive where to write binary files to
/// @param[in,out] _tree output tree where to write trivial class members to
/// @param[in] _object the object to serialize
/// @param[out] _children the list of child objects belonging to object
/// @param[in] _password (optional) the password to use if encryption is enabled
template<typename T>
inline static void serialize(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto& casted = safe_cast<T>(_object);

    // Add a version number. Not mandatory, but could help for future release
    write_version<T>(_tree, 1);

    _tree.put(VALUE, casted->get_value());
}

/// Generic deserialization function
/// @param[in] _archive the archive where to read binary files
/// @param[in] _tree the tree where to read trivial object members
/// @param[in] _children a map of data object that belongs to the object
/// @param[in,out] _object the object to deserialize using the archive, the tree and the children map
/// @param[in] _password (optional) the password used for encryption
template<typename T>
inline static typename T::sptr deserialize(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    const auto& casted = cast_or_create<T>(_object);

    // Check version number. Not mandatory, but could help for future release
    read_version<T>(_tree, 0, 1);

    // Assign the value
    casted->set_value(_tree.get<typename T::value_t>(VALUE));

    return casted;
}

} // namespace sight::io::session::helper
