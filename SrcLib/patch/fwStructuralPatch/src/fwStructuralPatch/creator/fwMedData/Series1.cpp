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
