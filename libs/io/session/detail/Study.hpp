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

#include <data/Study.hpp>

namespace sight::io::session
{

namespace detail::Study
{

constexpr static auto s_InstanceUID {"InstanceUID"};
constexpr static auto s_StudyID {"StudyID"};
constexpr static auto s_Date {"Date"};
constexpr static auto s_Time {"Time"};
constexpr static auto s_ReferringPhysicianName {"ReferringPhysicianName"};
constexpr static auto s_ConsultingPhysicianName {"ConsultingPhysicianName"};
constexpr static auto s_Description {"Description"};
constexpr static auto s_PatientAge {"PatientAge"};
constexpr static auto s_PatientSize {"PatientSize"};
constexpr static auto s_PatientWeight {"PatientWeight"};
constexpr static auto s_PatientBodyMassIndex {"PatientBodyMassIndex"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>&,
    const core::crypto::secure_string& = ""
)
{
    const auto study = Helper::safeCast<data::Study>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Study>(tree, 1);

    Helper::writeString(tree, s_InstanceUID, study->getInstanceUID());
    Helper::writeString(tree, s_StudyID, study->getStudyID());
    Helper::writeString(tree, s_Date, study->getDate());
    Helper::writeString(tree, s_Time, study->getTime());
    Helper::writeString(tree, s_ReferringPhysicianName, study->getReferringPhysicianName());
    Helper::writeString(tree, s_ConsultingPhysicianName, study->getConsultingPhysicianName());
    Helper::writeString(tree, s_Description, study->getDescription());
    Helper::writeString(tree, s_PatientAge, study->getPatientAge());
    Helper::writeString(tree, s_PatientSize, study->getPatientSize());
    Helper::writeString(tree, s_PatientWeight, study->getPatientWeight());
    Helper::writeString(tree, s_PatientBodyMassIndex, study->getPatientBodyMassIndex());
}

//------------------------------------------------------------------------------

inline static data::Study::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>&,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto study = Helper::safeCast<data::Study>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Study>(tree, 0, 1);

    study->setInstanceUID(Helper::readString(tree, s_InstanceUID));
    study->setStudyID(Helper::readString(tree, s_StudyID));
    study->setDate(Helper::readString(tree, s_Date));
    study->setTime(Helper::readString(tree, s_Time));
    study->setReferringPhysicianName(Helper::readString(tree, s_ReferringPhysicianName));
    study->setConsultingPhysicianName(Helper::readString(tree, s_ConsultingPhysicianName));
    study->setDescription(Helper::readString(tree, s_Description));
    study->setPatientAge(Helper::readString(tree, s_PatientAge));
    study->setPatientSize(Helper::readString(tree, s_PatientSize));
    study->setPatientWeight(Helper::readString(tree, s_PatientWeight));
    study->setPatientBodyMassIndex(Helper::readString(tree, s_PatientBodyMassIndex));

    return study;
}

} // namespace detail::Study

} // namespace sight::io
