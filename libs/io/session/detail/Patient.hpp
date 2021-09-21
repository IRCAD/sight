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
#include "io/session/Helper.hpp"

#include <data/Patient.hpp>

namespace sight::io::session
{

namespace detail::Patient
{

constexpr static auto s_Name {"Name"};
constexpr static auto s_PatientId {"PatientId"};
constexpr static auto s_Birthdate {"Birthdate"};
constexpr static auto s_Sex {"Sex"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>&,
    const core::crypto::secure_string& password = ""
)
{
    const auto patient = Helper::safeCast<data::Patient>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Patient>(tree, 1);

    // Serialize patient data
    Helper::writeString(tree, s_Name, patient->getName(), password);
    Helper::writeString(tree, s_PatientId, patient->getPatientId(), password);
    Helper::writeString(tree, s_Birthdate, patient->getBirthdate(), password);
    Helper::writeString(tree, s_Sex, patient->getSex(), password);
}

//------------------------------------------------------------------------------

inline static data::Patient::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>&,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto patient = Helper::safeCast<data::Patient>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Patient>(tree, 0, 1);

    // Deserialize patient data
    patient->setName(Helper::readString(tree, s_Name, password));
    patient->setPatientId(Helper::readString(tree, s_PatientId, password));
    patient->setBirthdate(Helper::readString(tree, s_Birthdate, password));
    patient->setSex(Helper::readString(tree, s_Sex, password));

    return patient;
}

} // namespace detail::Patient

} // namespace sight::io
