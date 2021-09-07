/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "io/patch/structural/data/Patient/V1ToFwMedDataPatientV1.hpp"

#include <atoms/String.hpp>

#include <core/tools/dateAndTime.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <sstream>
#include <string>

namespace sight::io::patch::structural
{

namespace data
{

namespace Patient
{

V1ToFwMedDataPatientV1::V1ToFwMedDataPatientV1() :
    io::atoms::patch::IStructuralPatch()
{
    m_originClassname = "::sight::data::Patient";
    m_targetClassname = "::sight::data::Patient";
    m_originVersion   = "1";
    m_targetVersion   = "1";
}

// ----------------------------------------------------------------------------

V1ToFwMedDataPatientV1::~V1ToFwMedDataPatientV1()
{
}

// ----------------------------------------------------------------------------

V1ToFwMedDataPatientV1::V1ToFwMedDataPatientV1(
    const V1ToFwMedDataPatientV1& cpy
) :
    io::atoms::patch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToFwMedDataPatientV1::apply(
    const sight::atoms::Object::sptr& previous,
    const sight::atoms::Object::sptr& current,
    io::atoms::patch::IPatch::NewVersionsType& newVersions
)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    io::atoms::patch::helper::Object helper(current);

    helper.removeAttribute("firstname");
    helper.removeAttribute("db_id");
    helper.removeAttribute("studies");

    helper.renameAttribute("id_dicom", "patient_id");
    helper.renameAttribute("birthdate", "birth_date");
    helper.renameAttribute("is_male", "sex");

    // Update Name
    std::string nameSeperation = "^";
    if(previous->getAttribute("name")->getString().empty()
       || previous->getAttribute("firstname")->getString().empty())
    {
        nameSeperation = "";
    }

    std::string newName = previous->getAttribute("name")->getString()
                          + nameSeperation
                          + previous->getAttribute("firstname")->getString();
    helper.replaceAttribute("name", sight::atoms::String::New(newName));

    // Update Birthdate
    ::boost::posix_time::ptime bithdate =
        ::boost::posix_time::time_from_string(previous->getAttribute("birthdate")->getString());
    helper.replaceAttribute("birth_date", sight::atoms::String::New(core::tools::getDate(bithdate)));

    // Update Sex
    helper.replaceAttribute(
        "sex",
        sight::atoms::String::New(
            !(previous->getAttribute("is_male")->getString().compare("true")) ? "M" : "F"
        )
    );
}

} // namespace Patient

} // namespace data

} // namespace sight::io::patch::structural
