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

#include "SessionSerializer.hpp"

#include "ActivitySeries.hpp"
#include "Array.hpp"
#include "CalibrationInfo.hpp"
#include "Camera.hpp"
#include "CameraSeries.hpp"
#include "Color.hpp"
#include "Composite.hpp"
#include "Edge.hpp"
#include "Equipment.hpp"
#include "Graph.hpp"
#include "Histogram.hpp"
#include "Image.hpp"
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

#include <io/zip/ArchiveWriter.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace sight::io::session
{

namespace detail
{

// The serializer function signature
using serializer = std::function<void (
                                     zip::ArchiveWriter&,
                                     boost::property_tree::ptree&,
                                     data::Object::csptr,
                                     std::map<std::string, data::Object::csptr>&,
                                     const core::crypto::secure_string&
                                 )>;

// Serializer registry
// No concurrency protection as the map is statically initialized
static const std::unordered_map<std::string, serializer> s_serializers = {
    {data::ActivitySeries::classname(), &ActivitySeries::serialize},
    {data::Array::classname(), &Array::serialize},
    {data::Boolean::classname(), &Helper::serialize<data::Boolean>},
    {data::Camera::classname(), &Camera::serialize},
    {data::CameraSeries::classname(), &CameraSeries::serialize},
    {data::CalibrationInfo::classname(), &CalibrationInfo::serialize},
    {data::Color::classname(), &Color::serialize},
    {data::Composite::classname(), &Composite::serialize},
    {data::Edge::classname(), &Edge::serialize},
    {data::Equipment::classname(), &Equipment::serialize},
    {data::Float::classname(), &Helper::serialize<data::Float>},
    {data::Graph::classname(), &Graph::serialize},
    {data::Histogram::classname(), &Histogram::serialize},
    {data::Integer::classname(), &Helper::serialize<data::Integer>},
    {data::Image::classname(), &Image::serialize},
    {data::Landmarks::classname(), &Landmarks::serialize},
    {data::Line::classname(), &Line::serialize},
    {data::List::classname(), &List::serialize},
    {data::Material::classname(), &Material::serialize},
    {data::Matrix4::classname(), &Matrix4::serialize},
    {data::Mesh::classname(), &Mesh::serialize},
    {data::Node::classname(), &Node::serialize},
    {data::Patient::classname(), &Patient::serialize},
    {data::Point::classname(), &Point::serialize},
    {data::PointList::classname(), &PointList::serialize},
    {data::Plane::classname(), &Plane::serialize},
    {data::PlaneList::classname(), &PlaneList::serialize},
    {data::Port::classname(), &Port::serialize},
    {data::ProcessObject::classname(), &ProcessObject::serialize},
    {data::Reconstruction::classname(), &Reconstruction::serialize},
    {data::ReconstructionTraits::classname(), &ReconstructionTraits::serialize},
    {data::Resection::classname(), &Resection::serialize},
    {data::ResectionDB::classname(), &ResectionDB::serialize},
    {data::ROITraits::classname(), &ROITraits::serialize},
    {data::Series::classname(), &Series::serialize},
    {data::SeriesDB::classname(), &SeriesDB::serialize},
    {data::String::classname(), &String::serialize},
    {data::StructureTraits::classname(), &StructureTraits::serialize},
    {data::StructureTraitsDictionary::classname(), &StructureTraitsDictionary::serialize},
    {data::Study::classname(), &Study::serialize},
    {data::Tag::classname(), &Tag::serialize},
    {data::TransferFunction::classname(), &TransferFunction::serialize},
    {data::Vector::classname(), &Vector::serialize}
};

// Return a serializer from a data object class name
inline static serializer findSerializer(const std::string& classname)
{
    const auto& it = s_serializers.find(classname);

    if(it != s_serializers.cend())
    {
        // Return the found serializer
        return it->second;
    }

    SIGHT_THROW("There is no serializer registered for class '" << classname << "'.");
}

/// Serializes recursively a data::Object to an opened archive using an initialized property tree
/// @param cache ptree cache
/// @param archive opened archive
/// @param tree property tree used to store object index
/// @param object root object to serialize
/// @param password password to use for optional encryption. Empty password means no encryption
inline static void deepSerialize(
    std::set<std::string>& cache,
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    const core::crypto::secure_string& password,
    const ISession::EncryptionLevel level
)
{
    // Only serialize non-null object
    if(!object)
    {
        return;
    }

    // Lock the object
    data::mt::locked_ptr<const data::Object> lock(object);

    // First check the cache
    const auto& uuid       = object->getUUID();
    const auto& uuid_it    = cache.find(uuid);
    const auto& class_name = object->getClassname();

    if(uuid_it != cache.cend())
    {
        boost::property_tree::ptree cached_tree;
        cached_tree.put(ISession::s_uuid, uuid);
        tree.add_child(class_name, cached_tree);
    }
    else
    {
        // Store uuid in cache immediately to allow circular reference
        cache.insert(uuid);

        // Create the object ptree
        boost::property_tree::ptree object_tree;

        // Put basic meta information
        object_tree.put(ISession::s_uuid, uuid);

        // Find the serializer using the classname
        const auto& serializer = findSerializer(class_name);

        // This map is used by serializer to store child objects which will be recursively serialized here
        std::map<std::string, data::Object::csptr> children;

        // Ask the serializer to serialize
        serializer(
            archive,
            object_tree,
            object,
            children,
            ISession::pickle(password, core::crypto::secure_string(uuid), level)
        );

        // Serialize children, if needed
        if(!children.empty())
        {
            boost::property_tree::ptree children_tree;

            for(const auto& child : children)
            {
                boost::property_tree::ptree child_tree;

                // Recursively serialize child objects
                deepSerialize(cache, archive, child_tree, child.second, password, level);

                // Append to the children tree
                children_tree.add_child(child.first, child_tree);
            }

            // Add children tree
            object_tree.add_child(ISession::s_children, children_tree);
        }

        // Serialize fields, if needed
        const auto& fields = object->getFields();

        if(!fields.empty())
        {
            boost::property_tree::ptree fields_tree;

            for(const auto& field : fields)
            {
                // Only serialize non null child
                boost::property_tree::ptree field_tree;

                // Recursively serialize field object
                deepSerialize(
                    cache,
                    archive,
                    field_tree,
                    field.second,
                    password,
                    level
                );

                // Append to the fields tree
                fields_tree.add_child(field.first, field_tree);
            }

            // Add fields tree
            object_tree.add_child(ISession::s_fields, fields_tree);
        }

        // Add the new tree to the root
        tree.add_child(class_name, object_tree);
    }
}

//------------------------------------------------------------------------------

void SessionSerializer::serialize(
    const std::filesystem::path& archive_path,
    data::Object::csptr object,
    const core::crypto::secure_string& password
) const
{
    // Initialize the ptree cache
    std::set<std::string> cache;

    // Create the archive that will hold the property tree and all binary files
    const auto& archive = zip::ArchiveWriter::shared(archive_path);

    // Create the tree used to store indexes.
    boost::property_tree::ptree tree;

    // Serialize recursively everything into the tree and the archive
    deepSerialize(cache, *archive, tree, object, password, getEncryptionLevel());

    auto ostream = archive->openFile(
        this->getIndexFilePath(),
        password,
        zip::Method::ZSTD,
        zip::Level::BEST
    );

    // Write the final property tree back to the archive
    boost::property_tree::write_json(
        *ostream,
        tree,
        false
    );
}

} // namespace detail

} // namespace sight::io::session
