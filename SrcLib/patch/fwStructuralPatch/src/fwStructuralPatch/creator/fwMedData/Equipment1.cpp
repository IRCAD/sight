/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
