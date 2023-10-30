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

#include "session_serializer.hpp"

#include <data/mt/locked_ptr.hpp>

#include <io/session/helper.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <atomic>
#include <shared_mutex>

namespace sight::io::session::detail
{

using core::crypto::password_keeper;
using core::crypto::secure_string;
using sight::io::zip::archive;

//------------------------------------------------------------------------------
struct serializer_struct
{
    std::unordered_map<std::string, serializer_t> serializer;
    std::shared_mutex serializers_mutex;
};

//------------------------------------------------------------------------------

static serializer_struct& get_serializer()
{
    static serializer_struct serializer;
    return serializer;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

serializer_t session_serializer::find_serializer(const std::string& _classname) const
{
    // First try to find in the customized serializer map
    if(const auto& custom_it = m_custom_serializers.find(_classname); custom_it != m_custom_serializers.cend())
    {
        // Return the found serializer
        return custom_it->second;
    }

    // Then try to find in the default deserializer map
    if(auto function = serializer(_classname); function)
    {
        return function;
    }

    SIGHT_THROW("There is no serializer registered for class '" << _classname << "'.");
}

//------------------------------------------------------------------------------

void session_serializer::deep_serialize(
    std::set<std::string>& _cache,
    zip::archive_writer& _archive,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    const secure_string& _password,
    const password_keeper::encryption_policy _encryption_policy
) const
{
    // Only serialize non-null object
    if(!_object)
    {
        return;
    }

    // Lock the object
    data::mt::locked_ptr<const data::object> lock(_object);

    // First check the cache
    const auto& uuid       = _object->get_uuid();
    const auto& uuid_it    = _cache.find(uuid);
    const auto& class_name = _object->get_classname();

    if(uuid_it != _cache.cend())
    {
        boost::property_tree::ptree cached_tree;
        cached_tree.put(session::UUID, uuid);
        _tree.add_child(class_name, cached_tree);
    }
    else
    {
        // Store uuid in cache immediately to allow circular reference
        _cache.insert(uuid);

        // Create the object ptree
        boost::property_tree::ptree object_tree;

        // Put basic meta information
        object_tree.put(session::UUID, uuid);
        helper::write_string(object_tree, session::DESCRIPTION, _object->get_description());

        // Find the serializer using the classname
        const auto& serializer = find_serializer(class_name);

        // This map is used by serializer to store child objects which will be recursively serialized here
        std::map<std::string, data::object::csptr> children;

        // Ask the serializer to serialize
        serializer(
            _archive,
            object_tree,
            _object,
            children,
            session::pickle(_password, secure_string(uuid), _encryption_policy)
        );

        // Serialize children, if needed
        if(!children.empty())
        {
            boost::property_tree::ptree children_tree;

            for(const auto& child : children)
            {
                boost::property_tree::ptree child_tree;

                // Recursively serialize child objects
                deep_serialize(_cache, _archive, child_tree, child.second, _password, _encryption_policy);

                // Append to the children tree
                children_tree.add_child(child.first, child_tree);
            }

            // Add children tree
            object_tree.add_child(session::CHILDREN, children_tree);
        }

        // Serialize fields, if needed
        const auto& fields = _object->get_fields();

        if(!fields.empty())
        {
            boost::property_tree::ptree fields_tree;

            for(const auto& field : fields)
            {
                // Only serialize non-null child
                boost::property_tree::ptree field_tree;

                // Recursively serialize field object
                deep_serialize(_cache, _archive, field_tree, field.second, _password, _encryption_policy);

                // Append to the fields tree
                fields_tree.add_child(field.first, field_tree);
            }

            // Add fields tree
            object_tree.add_child(session::FIELDS, fields_tree);
        }

        // Add the new tree to the root
        _tree.add_child(class_name, object_tree);
    }
}

//------------------------------------------------------------------------------

void session_serializer::set_custom_serializer(const std::string& _class_name, serializer_t _serializer)
{
    if(_serializer)
    {
        // Set the serializer for this class name
        m_custom_serializers[_class_name] = _serializer;
    }
    else
    {
        // Reset the serializer for this class name
        m_custom_serializers.erase(_class_name);
    }
}

//------------------------------------------------------------------------------

void session_serializer::set_serializer(const std::string& _class_name, serializer_t _serializer)
{
    // Protect serializers map
    auto& serializer_struct = get_serializer();
    std::shared_lock guard(serializer_struct.serializers_mutex);

    if(_serializer)
    {
        // Set the serializer for this class name
        serializer_struct.serializer[_class_name] = _serializer;
    }
    else
    {
        SIGHT_THROW("There is no serializer registered for class '" << _class_name << "'.");
    }
}

//------------------------------------------------------------------------------

serializer_t session_serializer::serializer(const std::string& _class_name)
{
    // Protect serializers map
    auto& serializer_struct = get_serializer();
    std::shared_lock guard(serializer_struct.serializers_mutex);

    if(const auto& it = serializer_struct.serializer.find(_class_name); it != serializer_struct.serializer.end())
    {
        // Return the found serializer
        return it->second;
    }

    return nullptr;
}

//------------------------------------------------------------------------------

void session_serializer::serialize(
    const std::filesystem::path& _archive_path,
    data::object::csptr _object,
    const archive::archive_format _archive_format,
    const secure_string& _password,
    const password_keeper::encryption_policy _encryption_policy
) const
{
    zip::archive_writer::uptr archive;

    if(_archive_format == archive::archive_format::filesystem)
    {
        // Throw an exception in debug, but just report an error in release when encryption is not supported, but asked
        if(!_password.empty())
        {
            const std::string& message =
                "Archive format '"
                + std::string(archive::archive_format_to_string(_archive_format))
                + "' doesn't support encryption.";

            SIGHT_ASSERT(message, false);
            SIGHT_ERROR(message);
        }

        // Create the archive that will hold all binary files
        archive = zip::archive_writer::get(_archive_path.parent_path(), _archive_format);
    }
    else
    {
        // Create the archive that will hold the property tree and all binary files
        archive = zip::archive_writer::get(_archive_path, _archive_format);
    }

    // Initialize the ptree cache
    std::set<std::string> cache;

    // Create the tree used to store indexes.
    boost::property_tree::ptree tree;

    // Serialize recursively everything into the tree and the archive
    deep_serialize(cache, *archive, tree, _object, _password, _encryption_policy);

    if(_archive_format == archive::archive_format::filesystem)
    {
        // Write the final property tree to the filesystem
        boost::property_tree::write_json(_archive_path.string(), tree);
    }
    else
    {
        // Open the ostream from the json stored into the archive
        auto ostream = archive->open_file(get_index_file_path(), _password, zip::method::DEFAULT, zip::level::best);

        // Write the final property tree back to the archive
        boost::property_tree::write_json(*ostream, tree, false);
    }
}

} // namespace sight::io::session::detail
