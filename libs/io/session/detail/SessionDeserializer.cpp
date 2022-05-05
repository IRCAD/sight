/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include "ActivitySeries.hpp"
#include "ActivitySet.hpp"
#include "Array.hpp"
#include "CalibrationInfo.hpp"
#include "Camera.hpp"
#include "CameraSeries.hpp"
#include "CameraSet.hpp"
#include "Color.hpp"
#include "Composite.hpp"
#include "DicomSeries.hpp"
#include "Edge.hpp"
#include "Equipment.hpp"
#include "Graph.hpp"
#include "Image.hpp"
#include "ImageSeries.hpp"
#include "Landmarks.hpp"
#include "Line.hpp"
#include "Material.hpp"
#include "Matrix4.hpp"
#include "Mesh.hpp"
#include "ModelSeries.hpp"
#include "Node.hpp"
#include "Patient.hpp"
#include "Plane.hpp"
#include "PlaneList.hpp"
#include "Point.hpp"
#include "PointList.hpp"
#include "Port.hpp"
#include "ProcessObject.hpp"
#include "Reconstruction.hpp"
#include "ReconstructionTraits.hpp"
#include "Resection.hpp"
#include "ResectionDB.hpp"
#include "ROITraits.hpp"
#include "Series.hpp"
#include "SeriesDB.hpp"
#include "SeriesSet.hpp"
#include "String.hpp"
#include "StructureTraits.hpp"
#include "StructureTraitsDictionary.hpp"
#include "Study.hpp"
#include "Tag.hpp"
#include "TransferFunction.hpp"
#include "Vector.hpp"

#include <data/Boolean.hpp>
#include <data/Float.hpp>
#include <data/IContainer.hxx>
#include <data/Integer.hpp>
#include <data/mt/locked_ptr.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <memory>
#include <shared_mutex>

namespace sight::io::session
{

namespace detail
{

using core::crypto::PasswordKeeper;
using core::crypto::secure_string;
using sight::io::zip::Archive;

// To protect deserializers map
static std::shared_mutex s_deserializers_mutex;

// Default serializer registry
static const std::unordered_map<std::string, deserializer_t> s_defaultDeserializers = {
    {data::ActivitySeries::classname(), &ActivitySeries::deserialize},
    {data::ActivitySet::classname(), &ActivitySet::deserialize},
    {data::Array::classname(), &Array::deserialize},
    {data::Boolean::classname(), &Helper::deserialize<data::Boolean>},
    {data::Camera::classname(), &Camera::deserialize},
    {data::CameraSeries::classname(), &CameraSeries::deserialize},
    {data::CameraSet::classname(), &CameraSet::deserialize},
    {data::CalibrationInfo::classname(), &CalibrationInfo::deserialize},
    {data::Color::classname(), &Color::deserialize},
    {data::Composite::classname(), &Composite::deserialize},
    {data::DicomSeries::classname(), &DicomSeries::deserialize},
    {data::Edge::classname(), &Edge::deserialize},
    {data::Equipment::classname(), &Equipment::deserialize},
    {data::Float::classname(), &Helper::deserialize<data::Float>},
    {data::Graph::classname(), &Graph::deserialize},
    {data::Integer::classname(), &Helper::deserialize<data::Integer>},
    {data::Image::classname(), &Image::deserialize},
    {data::ImageSeries::classname(), &ImageSeries::deserialize},
    {data::Landmarks::classname(), &Landmarks::deserialize},
    {data::Line::classname(), &Line::deserialize},
    {data::Material::classname(), &Material::deserialize},
    {data::Matrix4::classname(), &Matrix4::deserialize},
    {data::Mesh::classname(), &Mesh::deserialize},
    {data::ModelSeries::classname(), &ModelSeries::deserialize},
    {data::Node::classname(), &Node::deserialize},
    {data::Patient::classname(), &Patient::deserialize},
    {data::Point::classname(), &Point::deserialize},
    {data::PointList::classname(), &PointList::deserialize},
    {data::Plane::classname(), &Plane::deserialize},
    {data::PlaneList::classname(), &PlaneList::deserialize},
    {data::Port::classname(), &Port::deserialize},
    {data::ProcessObject::classname(), &ProcessObject::deserialize},
    {data::Reconstruction::classname(), &Reconstruction::deserialize},
    {data::ReconstructionTraits::classname(), &ReconstructionTraits::deserialize},
    {data::Resection::classname(), &Resection::deserialize},
    {data::ResectionDB::classname(), &ResectionDB::deserialize},
    {data::ROITraits::classname(), &ROITraits::deserialize},
    {data::Series::classname(), &Series::deserialize},
    {data::SeriesDB::classname(), &SeriesDB::deserialize},
    {data::SeriesSet::classname(), &SeriesSet::deserialize},
    {data::String::classname(), &String::deserialize},
    {data::StructureTraits::classname(), &StructureTraits::deserialize},
    {data::StructureTraitsDictionary::classname(), &StructureTraitsDictionary::deserialize},
    {data::Study::classname(), &Study::deserialize},
    {data::Tag::classname(), &Tag::deserialize},
    {data::TransferFunction::classname(), &TransferFunction::deserialize},
    {data::Vector::classname(), &Vector::deserialize}
};

// Serializer registry
static std::unordered_map<std::string, deserializer_t> s_deserializers = s_defaultDeserializers;

//------------------------------------------------------------------------------

deserializer_t SessionDeserializer::findDeserializer(const std::string& classname) const
{
    // First try to find in the customized serializer map
    if(const auto& customIt = m_customDeserializers.find(classname); customIt != m_customDeserializers.cend())
    {
        // Return the found serializer
        return customIt->second;
    }
    else
    {
        // Protect deserializers map
        std::shared_lock guard(s_deserializers_mutex);

        if(const auto& it = s_deserializers.find(classname); it != s_deserializers.cend())
        {
            // Return the found deserializer
            return it->second;
        }
    }

    SIGHT_THROW("There is no serializer registered for class '" << classname << "'.");
}

//------------------------------------------------------------------------------

data::Object::sptr SessionDeserializer::deepDeserialize(
    std::map<std::string, data::Object::sptr>& cache,
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const secure_string& password,
    const PasswordKeeper::EncryptionPolicy encryptionPolicy
) const
{
    const auto& treeIt = tree.begin();

    // Do not deserialize empty tree
    if(treeIt == tree.end())
    {
        return data::Object::sptr();
    }

    const auto& objectTree = treeIt->second;

    // Do not deserialize null object tree
    if(objectTree.empty())
    {
        return data::Object::sptr();
    }

    const auto& uuid     = objectTree.get<std::string>(ISession::s_uuid);
    const auto& objectIt = cache.find(uuid);

    // First check the cache
    if(objectIt != cache.cend())
    {
        return objectIt->second;
    }
    else
    {
        // Find the serializer using the classname
        const auto& classname    = treeIt->first;
        const auto& deserializer = findDeserializer(classname);

        // Try to reuse existing rather than create new one
        // Existing object will be overwritten
        auto object = std::dynamic_pointer_cast<data::Object>(data::Object::fromUUID(uuid));

        if(!object)
        {
            // Create the new object so we can safely deserialize child
            object = data::factory::New(classname);
            object->setUUID(uuid);
        }

        // Lock for writing (it will do nothing if object is null)
        data::mt::locked_ptr<data::Object> object_guard(object);

        // Store the object in cache for later use and to allow circular reference
        cache[uuid] = object;

        // Construct children map, if needed
        std::map<std::string, data::Object::sptr> children;

        const auto& childrenIt = objectTree.find(ISession::s_children);

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
            ISession::pickle(password, secure_string(uuid), encryptionPolicy)
        );

        if(newObject != object)
        {
            // This should not happen normally, only if the serializer doesn't reuse object
            newObject->setUUID(uuid, true);
            cache[uuid] = newObject;
        }

        // Construct field map
        data::Object::FieldMapType fields;

        const auto& fields_it = objectTree.find(ISession::s_fields);

        if(fields_it != objectTree.not_found())
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
}

//------------------------------------------------------------------------------

void SessionDeserializer::setDeserializer(const std::string& className, deserializer_t deserializer)
{
    if(deserializer)
    {
        // Set the serializer for this class name
        m_customDeserializers[className] = deserializer;
    }
    else
    {
        // Reset the serializer for this class name
        m_customDeserializers.erase(className);
    }
}

//------------------------------------------------------------------------------

void SessionDeserializer::setDefaultDeserializer(const std::string& className, deserializer_t deserializer)
{
    // Protect serializers map
    std::unique_lock guard(s_deserializers_mutex);

    if(deserializer)
    {
        // Set the serializer for this class name
        s_deserializers[className] = deserializer;
    }
    else if(const auto& it = s_deserializers.find(className); it != s_deserializers.cend())
    {
        if(const auto& defaultIt = s_defaultDeserializers.find(className); defaultIt != s_defaultDeserializers.cend())
        {
            // The serializer was found in the default map, use it
            s_deserializers[className] = defaultIt->second;
        }
        else
        {
            // The deserializer was not found in the default map. Remove it completely
            s_deserializers.erase(it);
        }
    }
    else
    {
        SIGHT_THROW("There is no deserializer registered for class '" << className << "'.");
    }
}

//------------------------------------------------------------------------------

data::Object::sptr SessionDeserializer::deserialize(
    const std::filesystem::path& archive_path,
    const Archive::ArchiveFormat archiveFormat,
    const secure_string& password,
    const PasswordKeeper::EncryptionPolicy encryptionPolicy
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

} // namespace detail

} // namespace sight::io::session
