/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    m_version = "1";
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

    helper.addAttribute("name" , ::fwAtoms::String::New(""));
    helper.addAttribute("patient_id" , ::fwAtoms::String::New(""));
    helper.addAttribute("birth_date" , ::fwAtoms::String::New(""));
    helper.addAttribute("sex" , ::fwAtoms::String::New(""));

    return patient;
}

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
