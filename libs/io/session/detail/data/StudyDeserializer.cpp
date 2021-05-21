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

#include "StudyDeserializer.hpp"

#include <core/crypto/Base64.hpp>

#include <data/Study.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr StudyDeserializer::deserialize(
    const zip::ArchiveReader::sptr& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string& password
) const
{
    // Create or reuse the object
    const auto& study =
        object ? sight::data::Study::dynamicCast(object) : sight::data::Study::New();

    SIGHT_ASSERT(
        "Object '" << study->getClassname() << "' is not a '" << sight::data::Study::classname() << "'",
        study
    );

    study->setInstanceUID(readFromTree(tree, "InstanceUID"));
    study->setStudyID(readFromTree(tree, "StudyID"));
    study->setDate(readFromTree(tree, "Date"));
    study->setTime(readFromTree(tree, "Time"));
    study->setReferringPhysicianName(readFromTree(tree, "ReferringPhysicianName"));
    study->setConsultingPhysicianName(readFromTree(tree, "ConsultingPhysicianName"));
    study->setDescription(readFromTree(tree, "Description"));
    study->setPatientAge(readFromTree(tree, "PatientAge"));
    study->setPatientSize(readFromTree(tree, "PatientSize"));
    study->setPatientWeight(readFromTree(tree, "PatientWeight"));
    study->setPatientBodyMassIndex(readFromTree(tree, "PatientBodyMassIndex"));

    return study;
}

} // detail::data

} // namespace sight::io::session
