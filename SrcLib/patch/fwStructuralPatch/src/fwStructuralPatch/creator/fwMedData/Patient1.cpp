/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
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

#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/String.hpp>
#include <fwAtoms/Map.hpp>

#include <fwAtomsPatch/helper/Object.hpp>

#include "fwStructuralPatch/creator/fwMedData/Patient1.hpp"

namespace fwStructuralPatch
{
namespace creator
{
namespace fwMedData
{

Patient1::Patient1()
{
    m_classname = "::fwMedData::Patient";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

Patient1::~Patient1()
{
}

// ----------------------------------------------------------------------------

Patient1::Patient1( const Patient1 &cpy ) : ::fwAtomsPatch::IStructuralCreator(cpy)
{
}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr Patient1::create()
{
    ::fwAtoms::Object::sptr patient = this->createObjBase();
    ::fwAtomsPatch::helper::Object helper(patient);

    helper.addAttribute("name", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_id", ::fwAtoms::String::New(""));
    helper.addAttribute("birth_date", ::fwAtoms::String::New(""));
    helper.addAttribute("sex", ::fwAtoms::String::New(""));

    return patient;
}

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
