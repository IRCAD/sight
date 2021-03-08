/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "io/patch/structural/creator/data/Landmarks1.hpp"

#include <atoms/Map.hpp>

#include <io/atoms/patch/helper/Object.hpp>

namespace sight::io::patch::structural
{
namespace creator
{
namespace data
{

// ----------------------------------------------------------------------------

Landmarks1::Landmarks1()
{
    m_classname = "::sight::data::Landmarks";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

Landmarks1::~Landmarks1()
{
}

// ----------------------------------------------------------------------------

Landmarks1::Landmarks1( const Landmarks1& cpy ) :
    io::atoms::patch::IStructuralCreator(cpy)
{
}

// ----------------------------------------------------------------------------

sight::atoms::Object::sptr Landmarks1::create()
{
    sight::atoms::Object::sptr landmarks = this->createObjBase();
    io::atoms::patch::helper::Object helper(landmarks);

    helper.addAttribute("landmarks", sight::atoms::Map::New());

    return landmarks;
}

// ----------------------------------------------------------------------------

} // namespace data
} // namespace creator
} // namespace sight::io::patch::structural
