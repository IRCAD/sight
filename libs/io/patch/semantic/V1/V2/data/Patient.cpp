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

#include "io/patch/semantic/V1/V2/data/Patient.hpp"

#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <core/tools/UUID.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/helper/Object.hpp>

namespace sight::io::patch::semantic
{
namespace V1
{
namespace V2
{
namespace data
{

Patient::Patient() :
    io::atoms::patch::ISemanticPatch()
{
    m_originClassname = "::sight::data::Patient";
    m_originVersion   = "1";
    this->addContext("MedicalData", "V1", "V2");
}

// ----------------------------------------------------------------------------

Patient::~Patient()
{
}

// ----------------------------------------------------------------------------

Patient::Patient( const Patient& cpy ) :
    io::atoms::patch::ISemanticPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void Patient::apply(
    const sight::atoms::Object::sptr& previous,
    const sight::atoms::Object::sptr& current,
    io::atoms::patch::IPatch::NewVersionsType& newVersions)
{
    ISemanticPatch::apply(previous, current, newVersions);
    io::atoms::patch::helper::cleanFields( current );
    io::atoms::patch::helper::Object helper( current );

    sight::atoms::String::sptr uid = current->getAttribute< sight::atoms::String >("patient_id");
    if( uid->getValue().empty() )
    {
        helper.replaceAttribute("patient_uid", sight::atoms::String::New(core::tools::UUID::generateUUID()));
    }
}

// ----------------------------------------------------------------------------

} // namespace data
} // namespace V2
} // namespace V1
} // namespace sight::io::patch::semantic
