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

#pragma once

#include "io/session/config.hpp"
#include "io/session/Helper.hpp"

#include <data/Equipment.hpp>
#include <data/Patient.hpp>
#include <data/Series.hpp>
#include <data/Study.hpp>

namespace sight::io::session
{

namespace detail::Series
{

constexpr static auto s_Patient {"Patient"};
constexpr static auto s_Study {"Study"};
constexpr static auto s_Equipment {"Equipment"};
constexpr static auto s_Modality {"Modality"};
constexpr static auto s_InstanceUID {"InstanceUID"};
constexpr static auto s_Number {"Number"};
constexpr static auto s_Laterality {"Laterality"};
constexpr static auto s_Date {"Date"};
constexpr static auto s_Time {"Time"};
constexpr static auto s_ProtocolName {"ProtocolName"};
constexpr static auto s_Description {"Description"};
constexpr static auto s_BodyPartExamined {"BodyPartExamined"};
constexpr static auto s_PatientPosition {"PatientPosition"};
constexpr static auto s_AnatomicalOrientationType {"AnatomicalOrientationType"};
constexpr static auto s_PerformedProcedureStepID {"PerformedProcedureStepID"};
constexpr static auto s_PerformedProcedureStepStartDate {"PerformedProcedureStepStartDate"};
constexpr static auto s_PerformedProcedureStepStartTime {"PerformedProcedureStepStartTime"};
constexpr static auto s_PerformedProcedureStepEndDate {"PerformedProcedureStepEndDate"};
constexpr static auto s_PerformedProcedureStepEndTime {"PerformedProcedureStepEndTime"};
constexpr static auto s_PerformedProcedureStepDescription {"PerformedProcedureStepDescription"};
constexpr static auto s_PerformedProcedureComments {"PerformedProcedureComments"};
constexpr static auto s_PerformingPhysiciansName {"PerformingPhysiciansName"};
constexpr static auto s_PerformingPhysiciansNames {"PerformingPhysiciansNames"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& = ""
)
{
    const auto series = Helper::safeCast<data::Series>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Series>(tree, 1);

    // Fill children map
    children[s_Patient]   = series->getPatient();
    children[s_Study]     = series->getStudy();
    children[s_Equipment] = series->getEquipment();

    // Serialize patient data
    Helper::writeString(tree, s_Modality, series->getModality());
    Helper::writeString(tree, s_InstanceUID, series->getInstanceUID());
    Helper::writeString(tree, s_Number, series->getNumber());
    Helper::writeString(tree, s_Laterality, series->getLaterality());
    Helper::writeString(tree, s_Date, series->getDate());
    Helper::writeString(tree, s_Time, series->getTime());
    Helper::writeString(tree, s_ProtocolName, series->getProtocolName());
    Helper::writeString(tree, s_Description, series->getDescription());
    Helper::writeString(tree, s_BodyPartExamined, series->getBodyPartExamined());
    Helper::writeString(tree, s_PatientPosition, series->getPatientPosition());
    Helper::writeString(tree, s_AnatomicalOrientationType, series->getAnatomicalOrientationType());
    Helper::writeString(tree, s_PerformedProcedureStepID, series->getPerformedProcedureStepID());
    Helper::writeString(tree, s_PerformedProcedureStepStartDate, series->getPerformedProcedureStepStartDate());
    Helper::writeString(tree, s_PerformedProcedureStepStartTime, series->getPerformedProcedureStepStartTime());
    Helper::writeString(tree, s_PerformedProcedureStepEndDate, series->getPerformedProcedureStepEndDate());
    Helper::writeString(tree, s_PerformedProcedureStepEndTime, series->getPerformedProcedureStepEndTime());
    Helper::writeString(tree, s_PerformedProcedureStepDescription, series->getPerformedProcedureStepDescription());
    Helper::writeString(tree, s_PerformedProcedureComments, series->getPerformedProcedureComments());

    boost::property_tree::ptree namesTree;

    for(const auto& name : series->getPerformingPhysiciansName())
    {
        namesTree.add(s_PerformingPhysiciansName, core::crypto::to_base64(name));
    }

    tree.add_child(s_PerformingPhysiciansNames, namesTree);
}

//------------------------------------------------------------------------------

inline static data::Series::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto series = Helper::safeCast<data::Series>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Series>(tree, 0, 1);

    // Set children from map
    series->setPatient(std::dynamic_pointer_cast<data::Patient>(children.at(s_Patient)));
    series->setStudy(std::dynamic_pointer_cast<data::Study>(children.at(s_Study)));
    series->setEquipment(std::dynamic_pointer_cast<data::Equipment>(children.at(s_Equipment)));

    // Deserialize patient data
    series->setModality(Helper::readString(tree, s_Modality));
    series->setInstanceUID(Helper::readString(tree, s_InstanceUID));
    series->setNumber(Helper::readString(tree, s_Number));
    series->setLaterality(Helper::readString(tree, s_Laterality));
    series->setDate(Helper::readString(tree, s_Date));
    series->setTime(Helper::readString(tree, s_Time));
    series->setProtocolName(Helper::readString(tree, s_ProtocolName));
    series->setDescription(Helper::readString(tree, s_Description));
    series->setBodyPartExamined(Helper::readString(tree, s_BodyPartExamined));
    series->setPatientPosition(Helper::readString(tree, s_PatientPosition));
    series->setAnatomicalOrientationType(Helper::readString(tree, s_AnatomicalOrientationType));
    series->setPerformedProcedureStepID(Helper::readString(tree, s_PerformedProcedureStepID));
    series->setPerformedProcedureStepStartDate(Helper::readString(tree, s_PerformedProcedureStepStartDate));
    series->setPerformedProcedureStepStartTime(Helper::readString(tree, s_PerformedProcedureStepStartTime));
    series->setPerformedProcedureStepEndDate(Helper::readString(tree, s_PerformedProcedureStepEndDate));
    series->setPerformedProcedureStepEndTime(Helper::readString(tree, s_PerformedProcedureStepEndTime));
    series->setPerformedProcedureStepDescription(Helper::readString(tree, s_PerformedProcedureStepDescription));
    series->setPerformedProcedureComments(Helper::readString(tree, s_PerformedProcedureComments));

    // Iterate on performingPhysiciansName
    std::vector<std::string> names;

    for(const auto& nameTree : tree.get_child(s_PerformingPhysiciansNames))
    {
        const auto& name = core::crypto::from_base64(nameTree.second.get_value<std::string>());
        names.push_back(name);
    }

    series->setPerformingPhysiciansName(names);

    return series;
}

} // namespace detail::Mesh

} // namespace sight::io
