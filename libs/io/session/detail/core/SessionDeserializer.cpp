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

#include "SessionDeserializer.hpp"

#include <data/mt/locked_ptr.hpp>

#include <io/session/Helper.hpp>

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

static DeserializerStruct& getDeserializer()
{
    static DeserializerStruct deserializer;
    return deserializer;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

deserializer_t SessionDeserializer::findDeserializer(const std::string& classname) const
{
    // First try to find in the customized deserializer map
    if(const auto& customIt = m_customDeserializers.find(classname); customIt != m_customDeserializers.cend())
    {
        // Return the found deserializer
        return customIt->second;
    }

    // Then try to find in the default deserializer map
    if(auto function = deserializer(classname); function)
    {
        return function;
    }

    SIGHT_THROW("There is no deserializer registered for class '" << classname << "'.");
}

//------------------------------------------------------------------------------

data::Object::sptr SessionDeserializer::deepDeserialize(
    std::map<std::string, data::Object::sptr>& cache,
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const secure_string& password,
    const password_keeper::encryption_policy encryptionPolicy
) const
{
    const auto& treeIt = tree.begin();

    // Do not deserialize empty tree
    if(treeIt == tree.end())
    {
        return {};
    }

    const auto& objectTree = treeIt->second;

    // Do not deserialize null object tree
    if(objectTree.empty())
    {
        return {};
    }

    const auto serialized_uuid = objectTree.get<std::string>(session::s_uuid);
    const auto& objectIt       = cache.find(serialized_uuid);

    // First check the cache
    if(objectIt != cache.cend())
    {
        return objectIt->second;
    }

    // Find the deserializer using the classname
    const auto& classname    = treeIt->first;
    const auto& deserializer = findDeserializer(classname);

    // Try to reuse existing rather than create new one
    // Existing object will be overwritten

    auto object = data::factory::make(classname);

    // Lock for writing (it will do nothing if object is null)
    data::mt::locked_ptr<data::Object> object_guard(object);

    // Store the object in cache for later use and to allow circular reference
    cache[serialized_uuid] = object;

    // Construct children map, if needed
    std::map<std::string, data::Object::sptr> children;

    const auto& childrenIt = objectTree.find(session::s_children);

    if(childrenIt != objectTree.not_found())
    {
        for(const auto& childIt : childrenIt->second)
        {
            children[childIt.first] = deepDeserialize(cache, archive, childIt.second, password, encryptionPolicy);
        }
    }

    // Now, we can really deserialize the object
    const auto& newObject = deserializer(
        archive,
        objectTree,
        children,
        object,
        session::pickle(password, secure_string(serialized_uuid), encryptionPolicy)
    );

    if(newObject != object)
    {
        // This should not happen normally, only if the deserializer doesn't reuse object
        newObject->set_uuid(object->get_uuid(), true);
        cache[serialized_uuid] = newObject;
        SIGHT_ASSERT(
            "An object has been replaced by a deserializer, but it is still referenced",
            object.use_count() == 1
        );
    }

    // Do not forget the description
    newObject->setDescription(Helper::readString(objectTree, session::s_description, ""));

    // Construct field map
    data::Object::FieldMapType fields;

    if(const auto& fields_it = objectTree.find(session::s_fields); fields_it != objectTree.not_found())
    {
        for(const auto& field_it : fields_it->second)
        {
            fields[field_it.first] = deepDeserialize(cache, archive, field_it.second, password, encryptionPolicy);
        }
    }

    // Assign the deserialized fields
    newObject->setFields(fields);

    return newObject;
}

//------------------------------------------------------------------------------

void SessionDeserializer::setCustomDeserializer(const std::string& className, deserializer_t deserializer)
{
    if(deserializer)
    {
        // Set the deserializer for this class name
        m_customDeserializers[className] = deserializer;
    }
    else
    {
        // Reset the deserializer for this class name
        m_customDeserializers.erase(className);
    }
}

//------------------------------------------------------------------------------

void SessionDeserializer::setDeserializer(const std::string& className, deserializer_t deserializer)
{
    // Protect serializers map
    auto& deserializerStruct = getDeserializer();
    std::shared_lock guard(deserializerStruct.deserializers_mutex);

    if(deserializer)
    {
        // Set the deserializer for this class name
        deserializerStruct.deserializers[className] = deserializer;
    }
    else
    {
        // Reset the deserializer for this class name
        deserializerStruct.deserializers.erase(className);
    }
}

//------------------------------------------------------------------------------

deserializer_t SessionDeserializer::deserializer(const std::string& className)
{
    // Protect serializers map
    auto& deserializerStruct = getDeserializer();
    std::shared_lock guard(deserializerStruct.deserializers_mutex);

    if(const auto& it = deserializerStruct.deserializers.find(className);
       it != deserializerStruct.deserializers.end())
    {
        // Return the found deserializer
        return it->second;
    }

    return nullptr;
}

//------------------------------------------------------------------------------

data::Object::sptr SessionDeserializer::deserialize(
    const std::filesystem::path& archive_path,
    const Archive::ArchiveFormat archiveFormat,
    const secure_string& password,
    const password_keeper::encryption_policy encryptionPolicy
) const
{
    zip::ArchiveReader::uptr archive;
    boost::property_tree::ptree tree;

    if(archiveFormat == Archive::ArchiveFormat::FILESYSTEM)
    {
        // Throw an exception in debug, but just report an error in release when encryption is not supported, but asked
        if(!password.empty())
        {
            const std::string& message =
                "Archive format '"
                + std::string(Archive::archiveFormatToString(archiveFormat))
                + "' doesn't support encryption.";

            SIGHT_ASSERT(message, false);
            SIGHT_ERROR(message);
        }

        // Create the archive that contain everything
        archive = zip::ArchiveReader::get(archive_path.parent_path(), archiveFormat);

        // Create the tree used to store everything and read the json archive.
        boost::property_tree::read_json(archive_path.string(), tree);
    }
    else
    {
        // Create the archive that contain everything
        archive = zip::ArchiveReader::get(archive_path, archiveFormat);

        // istream must be closed after this, since archive could only open files one by one
        // Create the tree used to store everything and read the index.json from the archive
        boost::property_tree::read_json(*archive->openFile(getIndexFilePath(), password), tree);
    }

    SIGHT_THROW_IF(
        "Empty tree from archive '" << archive_path << "'.",
        tree.empty()
    );

    // Initialize the object cache
    std::map<std::string, data::Object::sptr> cache;

    return deepDeserialize(cache, *archive, tree, password, encryptionPolicy);
}

} // namespace sight::io::session::detail
