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

constexpr static auto NUMBER_OF_INSTANCES {"NumberOfInstances"};
constexpr static auto FIRST_INSTANCE_NUMBER {"FirstInstanceNumber"};
constexpr static auto SOP_CLASS_UI_DS {"sop_classUIDs"};
constexpr static auto SOP_CLASS_UID {"sop_classUID"};
constexpr static auto COMPUTED_TAG_VALUES {"ComputedTagValues"};
constexpr static auto INSTANCES {"Instances"};
constexpr static auto INSTANCE {"Instance"};
constexpr static auto NUMBER {"Number"};
constexpr static auto SIZE {"Size"};
constexpr static auto UUID {"uuid"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& _archive,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& _children,
    const core::crypto::secure_string& _password = ""
)
{
    const auto dicom_series = helper::safe_cast<data::dicom_series>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::dicom_series>(_tree, 1);

    // Since dicom_series inherits from Series, we could use SeriesSerializer
    series::write(_archive, _tree, dicom_series, _children, _password);

    // Serialize other attributes
    _tree.put(NUMBER_OF_INSTANCES, dicom_series->num_instances());
    _tree.put(FIRST_INSTANCE_NUMBER, dicom_series->get_first_instance_number());

    // sop_classUIDs
    boost::property_tree::ptree sop_class_ui_ds_tree;
    for(const auto& sop_class_uid : dicom_series->get_sop_class_ui_ds())
    {
        sop_class_ui_ds_tree.add(SOP_CLASS_UID, core::crypto::to_base64(sop_class_uid));
    }

    _tree.add_child(SOP_CLASS_UI_DS, sop_class_ui_ds_tree);

    // ComputedTagValues
    boost::property_tree::ptree computed_tag_values_tree;
    for(const auto& [tag, value] : dicom_series->get_computed_tag_values())
    {
        computed_tag_values_tree.add(tag, core::crypto::to_base64(value));
    }

    _tree.add_child(COMPUTED_TAG_VALUES, computed_tag_values_tree);

    // Stores DICOM data.
    /// @todo This must be changed to store all of this as real DICOM files
    /// This requires a complete overhaul of DICOM management, especially "filtering" part as this are
    /// destructive operations.
    boost::property_tree::ptree instances_tree;
    for(const auto& [key, bufferObject] : dicom_series->get_dicom_container())
    {
        boost::property_tree::ptree instance_tree;

        // Store the instance number and the size to the tree
        instance_tree.put(NUMBER, key);
        instance_tree.put(SIZE, bufferObject->size());

        // Create the output file inside the archive
        const auto& ostream = _archive.open_file(
            std::filesystem::path(dicom_series->get_uuid() + "/" + std::to_string(key) + ".dcm"),
            _password
        );

        // Write the data
        ostream->write(
            static_cast<const char*>(bufferObject->buffer()),
            static_cast<std::streamsize>(bufferObject->size())
        );

        instances_tree.add_child(INSTANCE, instance_tree);
    }

    _tree.add_child(INSTANCES, instances_tree);
}

//------------------------------------------------------------------------------

inline static data::dicom_series::sptr read(
    zip::archive_reader& _archive,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& _password = ""
)
{
    // Create or reuse the object
    auto dicom_series = helper::cast_or_create<data::dicom_series>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::dicom_series>(_tree, 0, 1);

    // Since dicom_series inherits from Series, we could use SeriesDeserializer
    series::read(_archive, _tree, _children, dicom_series, _password);

    // Deserialize other attributes
    dicom_series->set_number_of_instances(_tree.get<std::size_t>(NUMBER_OF_INSTANCES));
    dicom_series->set_first_instance_number(_tree.get<std::size_t>(FIRST_INSTANCE_NUMBER));

    // sop_classUIDs
    std::set<std::string> sop_class_ui_ds;
    for(const auto& [key, value] : _tree.get_child(SOP_CLASS_UI_DS))
    {
        sop_class_ui_ds.insert(core::crypto::from_base64(value.get_value<std::string>()));
    }

    dicom_series->set_sop_class_ui_ds(sop_class_ui_ds);

    // ComputedTagValues
    std::map<std::string, std::string> computed_tag_values;

    for(const auto& [tag, value] : _tree.get_child(COMPUTED_TAG_VALUES))
    {
        computed_tag_values[tag] = core::crypto::from_base64(value.get_value<std::string>());
    }

    dicom_series->set_computed_tag_values(computed_tag_values);

    // Dicom Instances
    const auto& uuid = _tree.get<std::string>(UUID);
    std::map<std::size_t, core::memory::buffer_object::sptr> dicom_container;

    for(const auto& [key, instance] : _tree.get_child(INSTANCES))
    {
        // Buffer
        const auto instance_number = instance.get<std::size_t>(NUMBER);
        const auto size            = instance.get<std::size_t>(SIZE, 0);

        SIGHT_THROW_IF(
            dicom_series->get_classname()
            << " (UUID="
            << uuid
            << "): Incorrect buffer size stored for Instance '"
            << instance_number
            << "'.",
            size == 0
        );

        auto buffer_object = std::make_shared<core::memory::buffer_object>(true);
        core::memory::buffer_object::lock_t locker_source(buffer_object);
        buffer_object->allocate(size);

        // Create the istream from the input file inside the archive
        const auto& istream = _archive.open_file(
            std::filesystem::path(uuid + "/" + std::to_string(instance_number) + ".dcm"),
            _password
        );

        istream->read(
            static_cast<char*>(buffer_object->buffer()),
            static_cast<std::streamsize>(buffer_object->size())
        );

        dicom_container[instance_number] = buffer_object;
    }

    dicom_series->set_dicom_container(dicom_container);

    return dicom_series;
}

SIGHT_REGISTER_SERIALIZER(data::dicom_series, write, read);

} // namespace sight::io::session::detail::dicom_series
