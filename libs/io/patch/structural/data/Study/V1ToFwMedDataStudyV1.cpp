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

#include "io/patch/structural/data/Study/V1ToFwMedDataStudyV1.hpp"

#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/String.hpp>

namespace sight::io::patch::structural
{

namespace data
{

namespace Study
{

V1ToFwMedDataStudyV1::V1ToFwMedDataStudyV1() :
    io::atoms::patch::IStructuralPatch()
{
    m_originClassname = "::sight::data::Study";
    m_targetClassname = "::sight::data::Study";
    m_originVersion   = "1";
    m_targetVersion   = "1";
}

// ----------------------------------------------------------------------------

V1ToFwMedDataStudyV1::~V1ToFwMedDataStudyV1()
{
}

// ----------------------------------------------------------------------------

V1ToFwMedDataStudyV1::V1ToFwMedDataStudyV1(const V1ToFwMedDataStudyV1& cpy) :
    io::atoms::patch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToFwMedDataStudyV1::apply(
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

    helper.removeAttribute("hospital");
    helper.removeAttribute("modality");
    helper.removeAttribute("acquisition_zone");
    helper.removeAttribute("ris_id");
    helper.removeAttribute("db_id");
    helper.removeAttribute("acquisitions");

    helper.renameAttribute("uid", "instance_uid");

    helper.addAttribute("referring_physician_name", sight::atoms::String::New(""));
    helper.addAttribute("patient_age", sight::atoms::String::New(""));
}

} // namespace Study

} // namespace data

} // namespace sight::io::patch::structural
