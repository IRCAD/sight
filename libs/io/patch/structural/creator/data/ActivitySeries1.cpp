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

#include "io/patch/structural/creator/data/ActivitySeries1.hpp"

#include <atoms/Object.hpp>
#include <atoms/String.hpp>

#include <io/atoms/patch/helper/Object.hpp>

namespace sight::io::patch::structural
{
namespace creator
{
namespace data
{

ActivitySeries1::ActivitySeries1()
{
    m_classname = "::sight::data::ActivitySeries";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

ActivitySeries1::~ActivitySeries1()
{
}

// ----------------------------------------------------------------------------

ActivitySeries1::ActivitySeries1( const ActivitySeries1& cpy ) :
    Series1(cpy)
{
}

// ----------------------------------------------------------------------------

sight::atoms::Object::sptr ActivitySeries1::create()
{
    sight::atoms::Object::sptr series = this->Series1::create();
    io::atoms::patch::helper::Object helper(series);

    helper.addAttribute("activity_config_id", sight::atoms::String::New(""));
    helper.addAttribute("data", sight::atoms::Object::New());

    return series;
}

} // namespace data
} // namespace creator
} // namespace sight::io::patch::structural
