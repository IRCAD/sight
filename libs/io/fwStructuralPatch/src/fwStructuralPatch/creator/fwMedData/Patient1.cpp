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

#include "fwStructuralPatch/creator/data/Patient1.hpp"

#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/Object.hpp>

namespace fwStructuralPatch
{
namespace creator
{
namespace sight::data
{

Patient1::Patient1()
{
    m_classname = "data::Patient";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

Patient1::~Patient1()
{
}

// ----------------------------------------------------------------------------

Patient1::Patient1( const Patient1& cpy ) :
    ::fwAtomsPatch::IStructuralCreator(cpy)
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

} // namespace sight::data
} // namespace creator
} // namespace fwStructuralPatch
