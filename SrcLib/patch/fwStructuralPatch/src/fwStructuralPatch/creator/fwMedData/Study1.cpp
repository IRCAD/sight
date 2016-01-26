/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
