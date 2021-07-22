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

#include "SessionDeserializer.hpp"

#include "ActivitySeries.hpp"
#include "Array.hpp"
#include "CalibrationInfo.hpp"
#include "Camera.hpp"
#include "CameraSeries.hpp"
#include "Color.hpp"
#include "Composite.hpp"
#include "DicomSeries.hpp"
#include "Edge.hpp"
#include "Equipment.hpp"
#include "Graph.hpp"
#include "Histogram.hpp"
#include "Image.hpp"
#include "ImageSeries.hpp"
#include "Landmarks.hpp"
#include "Line.hpp"
#include "List.hpp"
#include "Material.hpp"
#include "Matrix4.hpp"
#include "Mesh.hpp"
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
#include "String.hpp"
#include "StructureTraits.hpp"
#include "StructureTraitsDictionary.hpp"
#include "Study.hpp"
#include "Tag.hpp"
#include "TransferFunction.hpp"
#include "Vector.hpp"

#include <data/Boolean.hpp>
#include <data/Float.hpp>
#include <data/Integer.hpp>
#include <data/mt/locked_ptr.hpp>

#include <io/zip/ArchiveReader.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <memory>

namespace sight::io::session
{

namespace detail
{

// The deserializer function signature
using deserializer = std::function<data::Object::sptr(
                                       zip::ArchiveReader&,
                                       const boost::property_tree::ptree&,
                                       const std::map<std::string, data::Object::sptr>&,
                                       data::Object::sptr,
                                       const core::crypto::secure_string&
                                   )>;

// Serializer registry
// No concurrency protection as the map is statically initialized
static const std::unordered_map<std::string, deserializer> s_deserializers = {
    {data::ActivitySeries::classname(), &ActivitySeries::deserialize},
    {data::Array::classname(), &Array::deserialize},
    {data::Boolean::classname(), &Helper::deserialize<data::Boolean>},
    {data::Camera::classname(), &Camera::deserialize},
    {data::CameraSeries::classname(), &CameraSeries::deserialize},
    {data::CalibrationInfo::classname(), &CalibrationInfo::deserialize},
    {data::Color::classname(), &Color::deserialize},
    {data::Composite::classname(), &Composite::deserialize},
    {data::DicomSeries::classname(), &DicomSeries::deserialize},
    {data::Edge::classname(), &Edge::deserialize},
    {data::Equipment::classname(), &Equipment::deserialize},
    {data::Float::classname(), &Helper::deserialize<data::Float>},
    {data::Graph::classname(), &Graph::deserialize},
    {data::Histogram::classname(), &Histogram::deserialize},
    {data::Integer::classname(), &Helper::deserialize<data::Integer>},
    {data::Image::classname(), &Image::deserialize},
    {data::ImageSeries::classname(), &ImageSeries::deserialize},
    {data::Landmarks::classname(), &Landmarks::deserialize},
    {data::Line::classname(), &Line::deserialize},
    {data::List::classname(), &List::deserialize},
    {data::Material::classname(), &Material::deserialize},
    {data::Matrix4::classname(), &Matrix4::deserialize},
    {data::Mesh::classname(), &Mesh::deserialize},
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
    {data::String::classname(), &String::deserialize},
    {data::StructureTraits::classname(), &StructureTraits::deserialize},
    {data::StructureTraitsDictionary::classname(), &StructureTraitsDictionary::deserialize},
    {data::Study::classname(), &Study::deserialize},
    {data::Tag::classname(), &Tag::deserialize},
    {data::TransferFunction::classname(), &TransferFunction::deserialize},
    {data::Vector::classname(), &Vector::deserialize}
};

// Return a deserializer from a data object class name
inline static deserializer findDeserializer(const std::string& classname)
{
    const auto& it = s_deserializers.find(classname);

    if(it != s_deserializers.cend())
    {
        // Return the found deserializer
        return it->second;
    }

    SIGHT_THROW("There is no deserializer registered for class '" << classname << "'.");
}

/// Deserializes recursively an initialized archive to a data::Object using an opened property tree
/// @param cache object cache
/// @param archive initialized archive
/// @param tree property tree used to retrieve object index
/// @param password password to use for optional encryption. Empty password means no encryption
inline static data::Object::sptr deepDeserialize(
    std::map<std::string, data::Object::sptr>& cache,
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const core::crypto::secure_string& password,
    const ISession::EncryptionLevel level
)
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
                children[childIt.first] = deepDeserialize(
                    cache,
                    archive,
                    childIt.second,
                    password,
                    level
                );
            }
        }

        // Now, we can really deserialize the object
        const auto& newObject = deserializer(
            archive,
            objectTree,
            children,
            object,
            ISession::pickle(password, core::crypto::secure_string(uuid), level)
        );

        if(newObject != object)
        {
            // This should not happen normally, only if the serializer doesn't reuse object
            newObject->setUUID(uuid);
            cache[uuid] = newObject;
        }

        // Construct field map
        data::Object::FieldMapType fields;

        const auto& fields_it = objectTree.find(ISession::s_fields);

        if(fields_it != objectTree.not_found())
        {
            for(const auto& field_it : fields_it->second)
            {
                fields[field_it.first] = deepDeserialize(
                    cache,
                    archive,
                    field_it.second,
                    password,
                    level
                );
            }
        }

        // Assign the deserialized fields
        newObject->setFields(fields);

        return newObject;
    }
}

//------------------------------------------------------------------------------

data::Object::sptr SessionDeserializer::deserialize(
    const std::filesystem::path& archive_path,
    const core::crypto::secure_string& password
) const
{
    // Initialize the object cache
    std::map<std::string, data::Object::sptr> cache;

    // Create the archive that contain everything
    const auto& archive = zip::ArchiveReader::shared(archive_path);

    // Create the tree used to store everything and read the index.json from the archive
    boost::property_tree::ptree tree;
    {
        // istream must be closed after this, since archive could only open files one by one
        const auto istream = archive->openFile(getIndexFilePath(), password);
        boost::property_tree::read_json(*istream, tree);
    }

    SIGHT_THROW_IF(
        "Empty '" << getIndexFilePath() << "' from archive '" << archive_path << "'.",
        tree.empty()
    );

    return deepDeserialize(cache, *archive, tree, password, getEncryptionLevel());
}

} // namespace detail

} // namespace sight::io::session
