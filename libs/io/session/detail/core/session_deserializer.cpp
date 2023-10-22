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

#include "session_deserializer.hpp"

#include <data/mt/locked_ptr.hpp>

#include <io/session/helper.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <memory>
#include <shared_mutex>

namespace sight::io::session::detail
{

using core::crypto::password_keeper;
using core::crypto::secure_string;
using sight::io::zip::Archive;

// To protect deserializers map

//------------------------------------------------------------------------------
struct DeserializerStruct
{
    std::unordered_map<std::string, deserializer_t> deserializers;
    std::shared_mutex deserializers_mutex;
};

//------------------------------------------------------------------------------

static DeserializerStruct& get_deserializer()
{
    static DeserializerStruct deserializer;
    return deserializer;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

deserializer_t session_deserializer::findDeserializer(const std::string& _classname) const
{
    // First try to find in the customized deserializer map
    if(const auto& custom_it = m_customDeserializers.find(_classname); custom_it != m_customDeserializers.cend())
    {
        // Return the found deserializer
        return custom_it->second;
    }

    // Then try to find in the default deserializer map
    if(auto function = deserializer(_classname); function)
    {
        return function;
    }

    SIGHT_THROW("There is no deserializer registered for class '" << _classname << "'.");
}

//------------------------------------------------------------------------------

data::object::sptr session_deserializer::deepDeserialize(
    std::map<std::string, data::object::sptr>& _cache,
    zip::ArchiveReader& _archive,
    const boost::property_tree::ptree& _tree,
    const secure_string& _password,
    const password_keeper::encryption_policy _encryption_policy
) const
{
    const auto& tree_it = _tree.begin();

    // Do not deserialize empty tree
    if(tree_it == _tree.end())
    {
        return {};
    }

    const auto& object_tree = tree_it->second;

    // Do not deserialize null object tree
    if(object_tree.empty())
    {
        return {};
    }

    const auto serialized_uuid = object_tree.get<std::string>(session::s_uuid);
    const auto& object_it      = _cache.find(serialized_uuid);

    // First check the cache
    if(object_it != _cache.cend())
    {
        return object_it->second;
    }

    // Find the deserializer using the classname
    const auto& classname    = tree_it->first;
    const auto& deserializer = findDeserializer(classname);

    // Try to reuse existing rather than create new one
    // Existing object will be overwritten

    auto object = data::factory::make(classname);

    // Lock for writing (it will do nothing if object is null)
    data::mt::locked_ptr<data::object> object_guard(object);

    // Store the object in cache for later use and to allow circular reference
    _cache[serialized_uuid] = object;

    // Construct children map, if needed
    std::map<std::string, data::object::sptr> children;

    const auto& children_it = object_tree.find(session::s_children);

    if(children_it != object_tree.not_found())
    {
        for(const auto& child_it : children_it->second)
        {
            children[child_it.first] =
                deepDeserialize(_cache, _archive, child_it.second, _password, _encryption_policy);
        }
    }

    // Now, we can really deserialize the object
    const auto& new_object = deserializer(
        _archive,
        object_tree,
        children,
        object,
        session::pickle(_password, secure_string(serialized_uuid), _encryption_policy)
    );

    if(new_object != object)
    {
        // This should not happen normally, only if the deserializer doesn't reuse object
        new_object->set_uuid(object->get_uuid(), true);
        _cache[serialized_uuid] = new_object;
        SIGHT_ASSERT(
            "An object has been replaced by a deserializer, but it is still referenced",
            object.use_count() == 1
        );
    }

    // Do not forget the description
    new_object->setDescription(helper::read_string(object_tree, session::s_description, ""));

    // Construct field map
    data::object::field_map_t fields;

    if(const auto& fields_it = object_tree.find(session::s_fields); fields_it != object_tree.not_found())
    {
        for(const auto& field_it : fields_it->second)
        {
            fields[field_it.first] = deepDeserialize(_cache, _archive, field_it.second, _password, _encryption_policy);
        }
    }

    // Assign the deserialized fields
    new_object->set_fields(fields);

    return new_object;
}

//------------------------------------------------------------------------------

void session_deserializer::setCustomDeserializer(const std::string& _class_name, deserializer_t _deserializer)
{
    if(_deserializer)
    {
        // Set the deserializer for this class name
        m_customDeserializers[_class_name] = _deserializer;
    }
    else
    {
        // Reset the deserializer for this class name
        m_customDeserializers.erase(_class_name);
    }
}

//------------------------------------------------------------------------------

void session_deserializer::setDeserializer(const std::string& _class_name, deserializer_t _deserializer)
{
    // Protect serializers map
    auto& deserializer_struct = get_deserializer();
    std::shared_lock guard(deserializer_struct.deserializers_mutex);

    if(_deserializer)
    {
        // Set the deserializer for this class name
        deserializer_struct.deserializers[_class_name] = _deserializer;
    }
    else
    {
        // Reset the deserializer for this class name
        deserializer_struct.deserializers.erase(_class_name);
    }
}

//------------------------------------------------------------------------------

deserializer_t session_deserializer::deserializer(const std::string& _class_name)
{
    // Protect serializers map
    auto& deserializer_struct = get_deserializer();
    std::shared_lock guard(deserializer_struct.deserializers_mutex);

    if(const auto& it = deserializer_struct.deserializers.find(_class_name);
       it != deserializer_struct.deserializers.end())
    {
        // Return the found deserializer
        return it->second;
    }

    return nullptr;
}

//------------------------------------------------------------------------------

data::object::sptr session_deserializer::deserialize(
    const std::filesystem::path& _archive_path,
    const Archive::ArchiveFormat _archive_format,
    const secure_string& _password,
    const password_keeper::encryption_policy _encryption_policy
) const
{
    zip::ArchiveReader::uptr archive;
    boost::property_tree::ptree tree;

    if(_archive_format == Archive::ArchiveFormat::FILESYSTEM)
    {
        // Throw an exception in debug, but just report an error in release when encryption is not supported, but asked
        if(!_password.empty())
        {
            const std::string& message =
                "Archive format '"
                + std::string(Archive::archiveFormatToString(_archive_format))
                + "' doesn't support encryption.";

            SIGHT_ASSERT(message, false);
            SIGHT_ERROR(message);
        }

        // Create the archive that contain everything
        archive = zip::ArchiveReader::get(_archive_path.parent_path(), _archive_format);

        // Create the tree used to store everything and read the json archive.
        boost::property_tree::read_json(_archive_path.string(), tree);
    }
    else
    {
        // Create the archive that contain everything
        archive = zip::ArchiveReader::get(_archive_path, _archive_format);

        // istream must be closed after this, since archive could only open files one by one
        // Create the tree used to store everything and read the index.json from the archive
        boost::property_tree::read_json(*archive->openFile(getIndexFilePath(), _password), tree);
    }

    SIGHT_THROW_IF(
        "Empty tree from archive '" << _archive_path << "'.",
        tree.empty()
    );

    // Initialize the object cache
    std::map<std::string, data::object::sptr> cache;

    return deepDeserialize(cache, *archive, tree, _password, _encryption_policy);
}

} // namespace sight::io::session::detail
