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
#include "io/session/detail/Series.hpp"
#include "io/session/Helper.hpp"

#include <data/DicomSeries.hpp>

namespace sight::io::session
{

namespace detail::DicomSeries
{

constexpr static auto s_NumberOfInstances {"NumberOfInstances"};
constexpr static auto s_FirstInstanceNumber {"FirstInstanceNumber"};
constexpr static auto s_SOPClassUIDs {"SOPClassUIDs"};
constexpr static auto s_SOPClassUID {"SOPClassUID"};
constexpr static auto s_ComputedTagValues {"ComputedTagValues"};
constexpr static auto s_Instances {"Instances"};
constexpr static auto s_Instance {"Instance"};
constexpr static auto s_Number {"Number"};
constexpr static auto s_Size {"Size"};
constexpr static auto s_uuid {"uuid"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& password = ""
)
{
    const auto dicomSeries = Helper::safeCast<data::DicomSeries>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::DicomSeries>(tree, 1);

    // Since DicomSeries inherits from Series, we could use SeriesSerializer
    Series::serialize(archive, tree, dicomSeries, children, password);

    // Serialize other attributes
    tree.put(s_NumberOfInstances, dicomSeries->getNumberOfInstances());
    tree.put(s_FirstInstanceNumber, dicomSeries->getFirstInstanceNumber());

    // SOPClassUIDs
    boost::property_tree::ptree sopClassUIDsTree;
    for(const auto& sopClassUID : dicomSeries->getSOPClassUIDs())
    {
        const auto& encrypted = password.empty()
                                ? sopClassUID
                                : core::crypto::encrypt(sopClassUID, password + s_SOPClassUID);

        sopClassUIDsTree.add(s_SOPClassUID, core::crypto::to_base64(encrypted));
    }

    tree.add_child(s_SOPClassUIDs, sopClassUIDsTree);

    // ComputedTagValues
    boost::property_tree::ptree computedTagValuesTree;
    for(const auto& [tag, value] : dicomSeries->getComputedTagValues())
    {
        const auto& encrypted = password.empty() ? value : core::crypto::encrypt(value, password + tag.c_str());

        computedTagValuesTree.add(tag, core::crypto::to_base64(encrypted));
    }

    tree.add_child(s_ComputedTagValues, computedTagValuesTree);

    // Stores DICOM data.
    /// @todo This must be changed to store all of this as real DICOM files
    /// This requires a complete overhaul of DICOM management, especially "filtering" part as this are
    /// destructive operations.
    boost::property_tree::ptree instancesTree;
    for(const auto& [key, bufferObject] : dicomSeries->getDicomContainer())
    {
        boost::property_tree::ptree instanceTree;

        // Store the instance number and the size to the tree
        instanceTree.put(s_Number, key);
        instanceTree.put(s_Size, bufferObject->getSize());

        // Create the output file inside the archive
        const auto& ostream = archive.openFile(
            std::filesystem::path(dicomSeries->getUUID() + "/" + std::to_string(key) + ".dcm"),
            password
        );

        // Write the data
        ostream->write(
            static_cast<const char*>(bufferObject->getBuffer()),
            static_cast<std::streamsize>(bufferObject->getSize())
        );

        instancesTree.add_child(s_Instance, instanceTree);
    }

    tree.add_child(s_Instances, instancesTree);
}

//------------------------------------------------------------------------------

inline static data::DicomSeries::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto dicomSeries = Helper::safeCast<data::DicomSeries>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::DicomSeries>(tree, 0, 1);

    // Since DicomSeries inherits from Series, we could use SeriesDeserializer
    Series::deserialize(archive, tree, children, dicomSeries, password);

    // Deserialize other attributes
    dicomSeries->setNumberOfInstances(tree.get<std::size_t>(s_NumberOfInstances));
    dicomSeries->setFirstInstanceNumber(tree.get<std::size_t>(s_FirstInstanceNumber));

    // SOPClassUIDs
    std::set<std::string> sopClassUIDs;
    for(const auto& [key, value] : tree.get_child(s_SOPClassUIDs))
    {
        const auto& encrypted = core::crypto::from_base64(value.get_value<std::string>());

        sopClassUIDs.insert(
            password.empty() ? encrypted : core::crypto::decrypt(encrypted, password + s_SOPClassUID)
        );
    }

    dicomSeries->setSOPClassUIDs(sopClassUIDs);

    // ComputedTagValues
    std::map<std::string, std::string> computedTagValues;

    for(const auto& [tag, value] : tree.get_child(s_ComputedTagValues))
    {
        const auto& encrypted = core::crypto::from_base64(value.get_value<std::string>());

        computedTagValues[tag] =
            password.empty() ? encrypted : core::crypto::decrypt(encrypted, password + tag.c_str());
    }

    dicomSeries->setComputedTagValues(computedTagValues);

    // Dicom Instances
    const auto& uuid = tree.get<std::string>(s_uuid);
    std::map<std::size_t, core::memory::BufferObject::sptr> dicomContainer;

    for(const auto& [key, instance] : tree.get_child(s_Instances))
    {
        // Buffer
        const auto instanceNumber = instance.get<std::size_t>(s_Number);
        const auto size           = instance.get<std::size_t>(s_Size, 0);

        SIGHT_THROW_IF(
            dicomSeries->getClassname()
            << " (UUID="
            << uuid
            << "): Incorrect buffer size stored for Instance '"
            << instanceNumber
            << "'.",
            size == 0
        );

        auto bufferObject = core::memory::BufferObject::New();
        core::memory::BufferObject::Lock lockerSource(bufferObject);
        bufferObject->allocate(size);

        // Create the istream from the input file inside the archive
        const auto& istream = archive.openFile(
            std::filesystem::path(uuid + "/" + std::to_string(instanceNumber) + ".dcm"),
            password
        );

        istream->read(
            static_cast<char*>(bufferObject->getBuffer()),
            static_cast<std::streamsize>(bufferObject->getSize())
        );

        dicomContainer[instanceNumber] = bufferObject;
    }

    dicomSeries->setDicomContainer(dicomContainer);

    return dicomSeries;
}

} // namespace detail::DicomSeries

} // namespace sight::io
