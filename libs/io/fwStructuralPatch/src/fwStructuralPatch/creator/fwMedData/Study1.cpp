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

#include "fwStructuralPatch/creator/data/Study1.hpp"

#include <atoms/String.hpp>

#include <fwAtomsPatch/helper/Object.hpp>

namespace fwStructuralPatch
{
namespace creator
{
namespace sight::data
{

Study1::Study1()
{
    m_classname = "::sight::data::Study";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

Study1::~Study1()
{
}

// ----------------------------------------------------------------------------

Study1::Study1( const Study1& cpy ) :
    ::fwAtomsPatch::IStructuralCreator(cpy)
{
}

// ----------------------------------------------------------------------------

atoms::Object::sptr Study1::create()
{
    atoms::Object::sptr study = this->createObjBase();
    ::fwAtomsPatch::helper::Object helper(study);

    helper.addAttribute("instance_uid", atoms::String::New(""));
    helper.addAttribute("date", atoms::String::New(""));
    helper.addAttribute("time", atoms::String::New(""));
    helper.addAttribute("referring_physician_name", atoms::String::New(""));
    helper.addAttribute("description", atoms::String::New(""));
    helper.addAttribute("patient_age", atoms::String::New(""));

    return study;
}

} // namespace sight::data
} // namespace creator
} // namespace fwStructuralPatch
