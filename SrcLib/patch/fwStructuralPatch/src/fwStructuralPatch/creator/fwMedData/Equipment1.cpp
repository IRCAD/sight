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

#include "fwStructuralPatch/creator/fwMedData/Equipment1.hpp"

namespace fwStructuralPatch
{
namespace creator
{
namespace fwMedData
{

// ----------------------------------------------------------------------------

Equipment1::Equipment1()
{
    m_classname = "::fwMedData::Equipment";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

Equipment1::~Equipment1()
{
}

// ----------------------------------------------------------------------------

Equipment1::Equipment1( const Equipment1 &cpy ) : ::fwAtomsPatch::IStructuralCreator(cpy)
{
}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr Equipment1::create()
{
    ::fwAtoms::Object::sptr equipment = this->createObjBase();
    ::fwAtomsPatch::helper::Object helper(equipment);

    helper.addAttribute("institution_name", ::fwAtoms::String::New(""));

    return equipment;
}

// ----------------------------------------------------------------------------

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
