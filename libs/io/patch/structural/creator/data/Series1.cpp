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

#include "io/patch/structural/creator/data/Series1.hpp"

#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <io/atoms/patch/helper/Object.hpp>
#include <io/atoms/patch/StructuralCreatorDB.hpp>

namespace sight::io::patch::structural
{
namespace creator
{
namespace data
{

Series1::Series1()
{
    m_classname = "::sight::data::Series";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

Series1::~Series1()
{
}

// ----------------------------------------------------------------------------

Series1::Series1( const Series1& cpy ) :
    io::atoms::patch::IStructuralCreator(cpy)
{
}

// ----------------------------------------------------------------------------

sight::atoms::Object::sptr Series1::create()
{
    sight::atoms::Object::sptr series = this->createObjBase();
    io::atoms::patch::helper::Object helper(series);

    io::atoms::patch::StructuralCreatorDB::sptr creators = io::atoms::patch::StructuralCreatorDB::getDefault();
    helper.addAttribute("patient", creators->create( "::sight::data::Patient", "1") );
    helper.addAttribute("study", creators->create( "::sight::data::Study", "1") );
    helper.addAttribute("equipment", creators->create( "::sight::data::Equipment", "1") );

    helper.addAttribute("instance_uid", sight::atoms::String::New(""));
    helper.addAttribute("modality", sight::atoms::String::New(""));
    helper.addAttribute("date", sight::atoms::String::New(""));
    helper.addAttribute("time", sight::atoms::String::New(""));
    helper.addAttribute("performing_physicians_name", sight::atoms::Sequence::New());
    helper.addAttribute("description", sight::atoms::String::New(""));

    return series;
}

} // namespace data
} // namespace creator
} // namespace sight::io::patch::structural
