/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "base.hpp"

namespace sight::io::tracking
{

//------------------------------------------------------------------------------

std::string sensor_t::to_string() const
{
    std::stringstream ss;

    ss << "- name: '" << this->name << "'" << std::endl;
    ss << "- optional: '" << this->optional << "'" << std::endl;
    ss << "- tl_index: '" << this->tl_index << "'" << std::endl;

    return ss.str();
}

//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& _os, const sensor_t& _s)
{
    _os << _s.to_string();
    return _os;
}

} // namespace sight::io::tracking
