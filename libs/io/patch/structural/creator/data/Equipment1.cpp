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

#include "io/patch/structural/creator/data/Equipment1.hpp"

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

// ----------------------------------------------------------------------------

Equipment1::Equipment1()
{
    m_classname = "sight::data::Equipment";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

Equipment1::~Equipment1()
{
}

// ----------------------------------------------------------------------------

Equipment1::Equipment1(const Equipment1& cpy) :
    io::atoms::patch::IStructuralCreator(cpy)
{
}

// ----------------------------------------------------------------------------

sight::atoms::Object::sptr Equipment1::create()
{
    sight::atoms::Object::sptr equipment = this->createObjBase();
    io::atoms::patch::helper::Object helper(equipment);

    helper.addAttribute("institution_name", sight::atoms::String::New(""));

    return equipment;
}

// ----------------------------------------------------------------------------

} // namespace data

} // namespace creator

} // namespace sight::io::patch::structural
