/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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
#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/array.hpp>

namespace sight::io::session::detail::array
{

constexpr static auto s_Size {"Size"};
constexpr static auto s_Sizes {"Sizes"};
constexpr static auto s_Type {"Type"};
constexpr static auto s_IsBufferOwner {"IsBufferOwner"};
constexpr static auto s_uuid {"uuid"};
constexpr static auto s_array {"/array.raw"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& password = ""
)
{
    const auto array = helper::safe_cast<data::array>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::array>(tree, 1);

    // Size
    boost::property_tree::ptree sizesTree;

    for(const auto& size : array->size())
    {
        sizesTree.add(s_Size, size);
    }

    tree.add_child(s_Sizes, sizesTree);

    // type, isBufferOwner
    tree.put(s_Type, array->getType().name());
    tree.put(s_IsBufferOwner, array->getIsBufferOwner());

    // Create the output file inside the archive
    const auto& ostream = archive.openFile(
        std::filesystem::path(array->get_uuid() + s_array),
        password
    );

    // Write back to the archive
    const auto& buffer = array->get_buffer_object();
    ostream->write(static_cast<const char*>(buffer->buffer()), static_cast<std::streamsize>(buffer->size()));
}

//------------------------------------------------------------------------------

inline static data::array::sptr read(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto array = helper::cast_or_create<data::array>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::array>(tree, 0, 1);

    // IsBufferOwner
    array->setIsBufferOwner(tree.get<bool>(s_IsBufferOwner, false));

    // Sizes
    std::vector<std::size_t> sizes;

    for(const auto& sizeTree : tree.get_child(s_Sizes))
    {
        const auto& size = sizeTree.second.get_value<std::size_t>();
        sizes.push_back(size);
    }

    if(!sizes.empty())
    {
        array->resize(sizes, tree.get<std::string>(s_Type), true);
    }

    // Buffer
    const auto& bufferObject = array->get_buffer_object();
    core::memory::buffer_object::lock_t lockerSource(bufferObject);

    // Create the istream from the input file inside the archive
    const auto& uuid    = tree.get<std::string>(s_uuid);
    const auto& istream = archive.openFile(
        std::filesystem::path(uuid + s_array),
        password
    );

    istream->read(static_cast<char*>(bufferObject->buffer()), static_cast<std::streamsize>(bufferObject->size()));

    return array;
}

SIGHT_REGISTER_SERIALIZER(data::array, write, read);

} // namespace sight::io::session::detail::array
