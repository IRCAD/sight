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
    zip::ArchiveWriter& _archive,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& _password = ""
)
{
    const auto array = helper::safe_cast<data::array>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::array>(_tree, 1);

    // Size
    boost::property_tree::ptree sizes_tree;

    for(const auto& size : array->size())
    {
        sizes_tree.add(s_Size, size);
    }

    _tree.add_child(s_Sizes, sizes_tree);

    // type, isBufferOwner
    _tree.put(s_Type, array->getType().name());
    _tree.put(s_IsBufferOwner, array->getIsBufferOwner());

    // Create the output file inside the archive
    const auto& ostream = _archive.openFile(
        std::filesystem::path(array->get_uuid() + s_array),
        _password
    );

    // Write back to the archive
    const auto& buffer = array->get_buffer_object();
    ostream->write(static_cast<const char*>(buffer->buffer()), static_cast<std::streamsize>(buffer->size()));
}

//------------------------------------------------------------------------------

inline static data::array::sptr read(
    zip::ArchiveReader& _archive,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& _password = ""
)
{
    // Create or reuse the object
    auto array = helper::cast_or_create<data::array>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::array>(_tree, 0, 1);

    // IsBufferOwner
    array->setIsBufferOwner(_tree.get<bool>(s_IsBufferOwner, false));

    // Sizes
    std::vector<std::size_t> sizes;

    for(const auto& size_tree : _tree.get_child(s_Sizes))
    {
        const auto& size = size_tree.second.get_value<std::size_t>();
        sizes.push_back(size);
    }

    if(!sizes.empty())
    {
        array->resize(sizes, _tree.get<std::string>(s_Type), true);
    }

    // Buffer
    const auto& buffer_object = array->get_buffer_object();
    core::memory::buffer_object::lock_t locker_source(buffer_object);

    // Create the istream from the input file inside the archive
    const auto& uuid    = _tree.get<std::string>(s_uuid);
    const auto& istream = _archive.openFile(
        std::filesystem::path(uuid + s_array),
        _password
    );

    istream->read(static_cast<char*>(buffer_object->buffer()), static_cast<std::streamsize>(buffer_object->size()));

    return array;
}

SIGHT_REGISTER_SERIALIZER(data::array, write, read);

} // namespace sight::io::session::detail::array
