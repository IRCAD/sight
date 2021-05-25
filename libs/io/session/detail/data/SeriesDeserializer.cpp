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

#include "SeriesDeserializer.hpp"

#include <core/crypto/Base64.hpp>

#include <data/Equipment.hpp>
#include <data/Patient.hpp>
#include <data/Series.hpp>
#include <data/Study.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr SeriesDeserializer::deserialize(
    const zip::ArchiveReader::sptr& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string& password
) const
{
    // Create or reuse the object
    const auto& series =
        object ? sight::data::Series::dynamicCast(object) : sight::data::Series::New();

    SIGHT_ASSERT(
        "Object '" << series->getClassname() << "' is not a '" << sight::data::Series::classname() << "'",
        series
    );

    // Set children from map
    series->setPatient(sight::data::Patient::dynamicCast(children.at("Patient")));
    series->setStudy(sight::data::Study::dynamicCast(children.at("Study")));
    series->setEquipment(sight::data::Equipment::dynamicCast(children.at("Equipment")));

    // Deserialize patient data
    series->setModality(readFromTree(tree, "Modality"));
    series->setInstanceUID(readFromTree(tree, "InstanceUID"));
    series->setNumber(readFromTree(tree, "Number"));
    series->setLaterality(readFromTree(tree, "Laterality"));
    series->setDate(readFromTree(tree, "Date"));
    series->setTime(readFromTree(tree, "Time"));
    series->setProtocolName(readFromTree(tree, "ProtocolName"));
    series->setDescription(readFromTree(tree, "Description"));
    series->setBodyPartExamined(readFromTree(tree, "BodyPartExamined"));
    series->setPatientPosition(readFromTree(tree, "PatientPosition"));
    series->setAnatomicalOrientationType(readFromTree(tree, "AnatomicalOrientationType"));
    series->setPerformedProcedureStepID(readFromTree(tree, "PerformedProcedureStepID"));
    series->setPerformedProcedureStepStartDate(readFromTree(tree, "PerformedProcedureStepStartDate"));
    series->setPerformedProcedureStepStartTime(readFromTree(tree, "PerformedProcedureStepStartTime"));
    series->setPerformedProcedureStepEndDate(readFromTree(tree, "PerformedProcedureStepEndDate"));
    series->setPerformedProcedureStepEndTime(readFromTree(tree, "PerformedProcedureStepEndTime"));
    series->setPerformedProcedureStepDescription(readFromTree(tree, "PerformedProcedureStepDescription"));
    series->setPerformedProcedureComments(readFromTree(tree, "PerformedProcedureComments"));

    // Iterate on performingPhysiciansName
    std::vector<std::string> names;

    for(const auto& nameTree : tree.get_child("PerformingPhysiciansNames"))
    {
        const auto& name = core::crypto::from_base64(nameTree.second.get_value<std::string>());
        names.push_back(name);
    }

    series->setPerformingPhysiciansName(names);

    return series;
}

} // detail::data

} // namespace sight::io::session
