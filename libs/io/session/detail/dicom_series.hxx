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
#include "io/session/detail/series.hxx"
#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/dicom_series.hpp>

namespace sight::io::session::detail::dicom_series
{

constexpr static auto s_NumberOfInstances {"NumberOfInstances"};
constexpr static auto s_FirstInstanceNumber {"FirstInstanceNumber"};
constexpr static auto s_sop_classUIDs {"sop_classUIDs"};
constexpr static auto s_sop_classUID {"sop_classUID"};
constexpr static auto s_ComputedTagValues {"ComputedTagValues"};
constexpr static auto s_Instances {"Instances"};
constexpr static auto s_Instance {"Instance"};
constexpr static auto s_Number {"Number"};
constexpr static auto s_Size {"Size"};
constexpr static auto s_uuid {"uuid"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& children,
    const core::crypto::secure_string& password = ""
)
{
    const auto dicomSeries = helper::safe_cast<data::dicom_series>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::dicom_series>(tree, 1);

    // Since dicom_series inherits from Series, we could use SeriesSerializer
    series::write(archive, tree, dicomSeries, children, password);

    // Serialize other attributes
    tree.put(s_NumberOfInstances, dicomSeries->numInstances());
    tree.put(s_FirstInstanceNumber, dicomSeries->getFirstInstanceNumber());

    // sop_classUIDs
    boost::property_tree::ptree sopClassUIDsTree;
    for(const auto& sopClassUID : dicomSeries->getSOPClassUIDs())
    {
        sopClassUIDsTree.add(s_sop_classUID, core::crypto::to_base64(sopClassUID));
    }

    tree.add_child(s_sop_classUIDs, sopClassUIDsTree);

    // ComputedTagValues
    boost::property_tree::ptree computedTagValuesTree;
    for(const auto& [tag, value] : dicomSeries->getComputedTagValues())
    {
        computedTagValuesTree.add(tag, core::crypto::to_base64(value));
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
        instanceTree.put(s_Size, bufferObject->size());

        // Create the output file inside the archive
        const auto& ostream = archive.openFile(
            std::filesystem::path(dicomSeries->get_uuid() + "/" + std::to_string(key) + ".dcm"),
            password
        );

        // Write the data
        ostream->write(
            static_cast<const char*>(bufferObject->buffer()),
            static_cast<std::streamsize>(bufferObject->size())
        );

        instancesTree.add_child(s_Instance, instanceTree);
    }

    tree.add_child(s_Instances, instancesTree);
}

//------------------------------------------------------------------------------

inline static data::dicom_series::sptr read(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto dicomSeries = helper::cast_or_create<data::dicom_series>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::dicom_series>(tree, 0, 1);

    // Since dicom_series inherits from Series, we could use SeriesDeserializer
    series::read(archive, tree, children, dicomSeries, password);

    // Deserialize other attributes
    dicomSeries->setNumberOfInstances(tree.get<std::size_t>(s_NumberOfInstances));
    dicomSeries->setFirstInstanceNumber(tree.get<std::size_t>(s_FirstInstanceNumber));

    // sop_classUIDs
    std::set<std::string> sopClassUIDs;
    for(const auto& [key, value] : tree.get_child(s_sop_classUIDs))
    {
        sopClassUIDs.insert(core::crypto::from_base64(value.get_value<std::string>()));
    }

    dicomSeries->setSOPClassUIDs(sopClassUIDs);

    // ComputedTagValues
    std::map<std::string, std::string> computedTagValues;

    for(const auto& [tag, value] : tree.get_child(s_ComputedTagValues))
    {
        computedTagValues[tag] = core::crypto::from_base64(value.get_value<std::string>());
    }

    dicomSeries->setComputedTagValues(computedTagValues);

    // Dicom Instances
    const auto& uuid = tree.get<std::string>(s_uuid);
    std::map<std::size_t, core::memory::buffer_object::sptr> dicomContainer;

    for(const auto& [key, instance] : tree.get_child(s_Instances))
    {
        // Buffer
        const auto instanceNumber = instance.get<std::size_t>(s_Number);
        const auto size           = instance.get<std::size_t>(s_Size, 0);

        SIGHT_THROW_IF(
            dicomSeries->get_classname()
            << " (UUID="
            << uuid
            << "): Incorrect buffer size stored for Instance '"
            << instanceNumber
            << "'.",
            size == 0
        );

        auto bufferObject = std::make_shared<core::memory::buffer_object>(true);
        core::memory::buffer_object::lock_t lockerSource(bufferObject);
        bufferObject->allocate(size);

        // Create the istream from the input file inside the archive
        const auto& istream = archive.openFile(
            std::filesystem::path(uuid + "/" + std::to_string(instanceNumber) + ".dcm"),
            password
        );

        istream->read(
            static_cast<char*>(bufferObject->buffer()),
            static_cast<std::streamsize>(bufferObject->size())
        );

        dicomContainer[instanceNumber] = bufferObject;
    }

    dicomSeries->setDicomContainer(dicomContainer);

    return dicomSeries;
}

SIGHT_REGISTER_SERIALIZER(data::dicom_series, write, read);

} // namespace sight::io::session::detail::dicom_series
