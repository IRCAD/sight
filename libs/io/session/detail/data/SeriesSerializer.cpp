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

#include "SeriesSerializer.hpp"

#include <core/crypto/Base64.hpp>

#include <data/Equipment.hpp>
#include <data/Patient.hpp>
#include <data/Series.hpp>
#include <data/Study.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void SeriesSerializer::serialize(
    const zip::ArchiveWriter::sptr& archive,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>& children,
    const core::crypto::secure_string& password
) const
{
    const auto& series = sight::data::Series::dynamicCast(object);
    SIGHT_ASSERT(
        "Object '"
        << (object ? object->getClassname() : sight::data::Object::classname())
        << "' is not a '"
        << sight::data::Series::classname()
        << "'",
        series
    );

    // Fill children map
    children["Patient"]   = series->getPatient();
    children["Study"]     = series->getStudy();
    children["Equipment"] = series->getEquipment();

    // Serialize patient data
    writeToTree(tree, "Modality", series->getModality());
    writeToTree(tree, "InstanceUID", series->getInstanceUID());
    writeToTree(tree, "Number", series->getNumber());
    writeToTree(tree, "Laterality", series->getLaterality());
    writeToTree(tree, "Date", series->getDate());
    writeToTree(tree, "Time", series->getTime());
    writeToTree(tree, "ProtocolName", series->getProtocolName());
    writeToTree(tree, "Description", series->getDescription());
    writeToTree(tree, "BodyPartExamined", series->getBodyPartExamined());
    writeToTree(tree, "PatientPosition", series->getPatientPosition());
    writeToTree(tree, "AnatomicalOrientationType", series->getAnatomicalOrientationType());
    writeToTree(tree, "PerformedProcedureStepID", series->getPerformedProcedureStepID());
    writeToTree(tree, "PerformedProcedureStepStartDate", series->getPerformedProcedureStepStartDate());
    writeToTree(tree, "PerformedProcedureStepStartTime", series->getPerformedProcedureStepStartTime());
    writeToTree(tree, "PerformedProcedureStepEndDate", series->getPerformedProcedureStepEndDate());
    writeToTree(tree, "PerformedProcedureStepEndTime", series->getPerformedProcedureStepEndTime());
    writeToTree(tree, "PerformedProcedureStepDescription", series->getPerformedProcedureStepDescription());
    writeToTree(tree, "PerformedProcedureComments", series->getPerformedProcedureComments());

    boost::property_tree::ptree namesTree;

    for(const auto& name : series->getPerformingPhysiciansName())
    {
        namesTree.add("PerformingPhysiciansName", core::crypto::to_base64(name));
    }

    tree.add_child("PerformingPhysiciansNames", namesTree);
}

} // detail::data

} // namespace sight::io::session
