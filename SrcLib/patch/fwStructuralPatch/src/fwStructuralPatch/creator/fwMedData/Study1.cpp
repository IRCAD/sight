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

#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/Object.hpp>

#include "fwStructuralPatch/creator/fwMedData/Study1.hpp"

namespace fwStructuralPatch
{
namespace creator
{
namespace fwMedData
{

Study1::Study1()
{
    m_classname = "::fwMedData::Study";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

Study1::~Study1()
{
}

// ----------------------------------------------------------------------------

Study1::Study1( const Study1 &cpy ) : ::fwAtomsPatch::IStructuralCreator(cpy)
{
}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr Study1::create()
{
    ::fwAtoms::Object::sptr study = this->createObjBase();
    ::fwAtomsPatch::helper::Object helper(study);

    helper.addAttribute("instance_uid", ::fwAtoms::String::New(""));
    helper.addAttribute("date", ::fwAtoms::String::New(""));
    helper.addAttribute("time", ::fwAtoms::String::New(""));
    helper.addAttribute("referring_physician_name", ::fwAtoms::String::New(""));
    helper.addAttribute("description", ::fwAtoms::String::New(""));
    helper.addAttribute("patient_age", ::fwAtoms::String::New(""));

    return study;
}

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
