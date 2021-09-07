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

#include "io/patch/structural/creator/data/Patient1.hpp"

#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/String.hpp>

#include <io/atoms/patch/helper/Object.hpp>

namespace sight::io::patch::structural
{

namespace creator
{

namespace data
{

Patient1::Patient1()
{
    m_classname = "::sight::data::Patient";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

Patient1::~Patient1()
{
}

// ----------------------------------------------------------------------------

Patient1::Patient1(const Patient1& cpy) :
    io::atoms::patch::IStructuralCreator(cpy)
{
}

// ----------------------------------------------------------------------------

sight::atoms::Object::sptr Patient1::create()
{
    sight::atoms::Object::sptr patient = this->createObjBase();
    io::atoms::patch::helper::Object helper(patient);

    helper.addAttribute("name", sight::atoms::String::New(""));
    helper.addAttribute("patient_id", sight::atoms::String::New(""));
    helper.addAttribute("birth_date", sight::atoms::String::New(""));
    helper.addAttribute("sex", sight::atoms::String::New(""));

    return patient;
}

} // namespace data

} // namespace creator

} // namespace sight::io::patch::structural
