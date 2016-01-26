/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtoms/String.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>

#include <fwAtomsPatch/StructuralCreatorDB.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include "fwStructuralPatch/creator/fwMedData/Series1.hpp"

namespace fwStructuralPatch
{
namespace creator
{
namespace fwMedData
{

Series1::Series1()
{
    m_classname = "::fwMedData::Series";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

Series1::~Series1()
{
}

// ----------------------------------------------------------------------------

Series1::Series1( const Series1 &cpy ) : ::fwAtomsPatch::IStructuralCreator(cpy)
{
}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr Series1::create()
{
    ::fwAtoms::Object::sptr series = this->createObjBase();
    ::fwAtomsPatch::helper::Object helper(series);

    ::fwAtomsPatch::StructuralCreatorDB::sptr creators = ::fwAtomsPatch::StructuralCreatorDB::getDefault();
    helper.addAttribute("patient", creators->create( "::fwMedData::Patient", "1") );
    helper.addAttribute("study", creators->create( "::fwMedData::Study", "1") );
    helper.addAttribute("equipment", creators->create( "::fwMedData::Equipment", "1") );

    helper.addAttribute("instance_uid", ::fwAtoms::String::New(""));
    helper.addAttribute("modality", ::fwAtoms::String::New(""));
    helper.addAttribute("date", ::fwAtoms::String::New(""));
    helper.addAttribute("time", ::fwAtoms::String::New(""));
    helper.addAttribute("performing_physicians_name", ::fwAtoms::Sequence::New());
    helper.addAttribute("description", ::fwAtoms::String::New(""));

    return series;
}

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
