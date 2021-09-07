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
#include "io/session/detail/Helper.hpp"

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
    const core::crypto::secure_string& password = ""
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
    Helper::writeString(tree, s_Modality, series->getModality(), password);
    Helper::writeString(tree, s_InstanceUID, series->getInstanceUID(), password);
    Helper::writeString(tree, s_Number, series->getNumber(), password);
    Helper::writeString(tree, s_Laterality, series->getLaterality(), password);
    Helper::writeString(tree, s_Date, series->getDate(), password);
    Helper::writeString(tree, s_Time, series->getTime(), password);
    Helper::writeString(tree, s_ProtocolName, series->getProtocolName(), password);
    Helper::writeString(tree, s_Description, series->getDescription(), password);
    Helper::writeString(tree, s_BodyPartExamined, series->getBodyPartExamined(), password);
    Helper::writeString(tree, s_PatientPosition, series->getPatientPosition(), password);
    Helper::writeString(tree, s_AnatomicalOrientationType, series->getAnatomicalOrientationType(), password);
    Helper::writeString(tree, s_PerformedProcedureStepID, series->getPerformedProcedureStepID(), password);
    Helper::writeString(
        tree,
        s_PerformedProcedureStepStartDate,
        series->getPerformedProcedureStepStartDate(),
        password
    );
    Helper::writeString(
        tree,
        s_PerformedProcedureStepStartTime,
        series->getPerformedProcedureStepStartTime(),
        password
    );
    Helper::writeString(tree, s_PerformedProcedureStepEndDate, series->getPerformedProcedureStepEndDate(), password);
    Helper::writeString(tree, s_PerformedProcedureStepEndTime, series->getPerformedProcedureStepEndTime(), password);
    Helper::writeString(
        tree,
        s_PerformedProcedureStepDescription,
        series->getPerformedProcedureStepDescription(),
        password
    );
    Helper::writeString(tree, s_PerformedProcedureComments, series->getPerformedProcedureComments(), password);

    boost::property_tree::ptree namesTree;

    for(const auto& name : series->getPerformingPhysiciansName())
    {
        const auto& raw = password.empty() ? name : core::crypto::encrypt(name, password + s_PerformingPhysiciansName);
        namesTree.add(s_PerformingPhysiciansName, core::crypto::to_base64(raw));
    }

    tree.add_child(s_PerformingPhysiciansNames, namesTree);
}

//------------------------------------------------------------------------------

inline static data::Series::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
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
    series->setModality(Helper::readString(tree, s_Modality, password));
    series->setInstanceUID(Helper::readString(tree, s_InstanceUID, password));
    series->setNumber(Helper::readString(tree, s_Number, password));
    series->setLaterality(Helper::readString(tree, s_Laterality, password));
    series->setDate(Helper::readString(tree, s_Date, password));
    series->setTime(Helper::readString(tree, s_Time, password));
    series->setProtocolName(Helper::readString(tree, s_ProtocolName, password));
    series->setDescription(Helper::readString(tree, s_Description, password));
    series->setBodyPartExamined(Helper::readString(tree, s_BodyPartExamined, password));
    series->setPatientPosition(Helper::readString(tree, s_PatientPosition, password));
    series->setAnatomicalOrientationType(Helper::readString(tree, s_AnatomicalOrientationType, password));
    series->setPerformedProcedureStepID(Helper::readString(tree, s_PerformedProcedureStepID, password));
    series->setPerformedProcedureStepStartDate(Helper::readString(tree, s_PerformedProcedureStepStartDate, password));
    series->setPerformedProcedureStepStartTime(Helper::readString(tree, s_PerformedProcedureStepStartTime, password));
    series->setPerformedProcedureStepEndDate(Helper::readString(tree, s_PerformedProcedureStepEndDate, password));
    series->setPerformedProcedureStepEndTime(Helper::readString(tree, s_PerformedProcedureStepEndTime, password));
    series->setPerformedProcedureStepDescription(
        Helper::readString(
            tree,
            s_PerformedProcedureStepDescription,
            password
        )
    );
    series->setPerformedProcedureComments(Helper::readString(tree, s_PerformedProcedureComments, password));

    // Iterate on performingPhysiciansName
    std::vector<std::string> names;

    for(const auto& nameTree : tree.get_child(s_PerformingPhysiciansNames))
    {
        const auto& raw  = core::crypto::from_base64(nameTree.second.get_value<std::string>());
        const auto& name = password.empty() ? raw : core::crypto::decrypt(raw, password + s_PerformingPhysiciansName);
        names.push_back(name);
    }

    series->setPerformingPhysiciansName(names);

    return series;
}

} // namespace detail::Mesh

} // namespace sight::io
