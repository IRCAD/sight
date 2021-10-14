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

#pragma once

#include "io/session/config.hpp"
#include "io/session/Helper.hpp"

#include <data/Array.hpp>

namespace sight::io::session
{

namespace detail::Array
{

constexpr static auto s_Size {"Size"};
constexpr static auto s_Sizes {"Sizes"};
constexpr static auto s_Type {"Type"};
constexpr static auto s_IsBufferOwner {"IsBufferOwner"};
constexpr static auto s_uuid {"uuid"};
constexpr static auto s_array {"/array.raw"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>&,
    const core::crypto::secure_string& password = ""
)
{
    const auto array = Helper::safeCast<data::Array>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Array>(tree, 1);

    // Size
    boost::property_tree::ptree sizesTree;

    for(const auto& size : array->getSize())
    {
        sizesTree.add(s_Size, size);
    }

    tree.add_child(s_Sizes, sizesTree);

    // type, isBufferOwner
    tree.put(s_Type, array->getType().string());
    tree.put(s_IsBufferOwner, array->getIsBufferOwner());

    // Create the output file inside the archive
    const auto& ostream = archive.openFile(
        std::filesystem::path(array->getUUID() + s_array),
        password
    );

    // Write back to the archive
    const auto& buffer = array->getBufferObject();
    ostream->write(static_cast<const char*>(buffer->getBuffer()), static_cast<std::streamsize>(buffer->getSize()));
}

//------------------------------------------------------------------------------

inline static data::Array::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>&,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto array = Helper::safeCast<data::Array>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Array>(tree, 0, 1);

    // Type
    array->setType(tree.get<std::string>(s_Type));

    // IsBufferOwner
    array->setIsBufferOwner(tree.get<bool>(s_IsBufferOwner, false));

    // Sizes
    std::vector<size_t> sizes;

    for(const auto& sizeTree : tree.get_child(s_Sizes))
    {
        const auto& size = sizeTree.second.get_value<size_t>();
        sizes.push_back(size);
    }

    array->resize(sizes, true);

    // Buffer
    const auto& bufferObject = array->getBufferObject();
    core::memory::BufferObject::Lock lockerSource(bufferObject);

    // Create the istream from the input file inside the archive
    const auto& uuid    = tree.get<std::string>(s_uuid);
    const auto& istream = archive.openFile(
        std::filesystem::path(uuid + s_array),
        password
    );

    istream->read(static_cast<char*>(bufferObject->getBuffer()), static_cast<std::streamsize>(bufferObject->getSize()));

    return array;
}

} // namespace detail::Array

} // namespace sight::io
