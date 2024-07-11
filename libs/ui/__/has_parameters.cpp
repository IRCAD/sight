/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "has_parameters.hpp"

#include <core/com/slots.hxx>

namespace sight::ui
{

//-----------------------------------------------------------------------------

const core::com::slots::key_t has_parameters::SET_PARAMETER_SLOT         = "set_parameter";
const core::com::slots::key_t has_parameters::SET_BOOL_PARAMETER_SLOT    = "set_bool_parameter";
const core::com::slots::key_t has_parameters::SET_COLOR_PARAMETER_SLOT   = "set_color_parameter";
const core::com::slots::key_t has_parameters::SET_DOUBLE_PARAMETER_SLOT  = "set_double_parameter";
const core::com::slots::key_t has_parameters::SET_DOUBLE2_PARAMETER_SLOT = "set_double2_parameter";
const core::com::slots::key_t has_parameters::SET_DOUBLE3_PARAMETER_SLOT = "set_double3_parameter";
const core::com::slots::key_t has_parameters::SET_INT_PARAMETER_SLOT     = "set_int_parameter";
const core::com::slots::key_t has_parameters::SET_INT2_PARAMETER_SLOT    = "set_int2_parameter";
const core::com::slots::key_t has_parameters::SET_INT3_PARAMETER_SLOT    = "set_int3_parameter";
const core::com::slots::key_t has_parameters::SET_ENUM_PARAMETER_SLOT    = "set_enum_parameter";

//-----------------------------------------------------------------------------

has_parameters::has_parameters(sight::core::com::slots& _slots)
{
    _slots(SET_PARAMETER_SLOT, core::com::new_slot(&has_parameters::set_parameter, this));
    _slots(SET_BOOL_PARAMETER_SLOT, core::com::new_slot(&has_parameters::set_bool_parameter, this));
    _slots(SET_COLOR_PARAMETER_SLOT, core::com::new_slot(&has_parameters::set_color_parameter, this));
    _slots(SET_DOUBLE_PARAMETER_SLOT, core::com::new_slot(&has_parameters::set_double_parameter, this));
    _slots(SET_DOUBLE2_PARAMETER_SLOT, core::com::new_slot(&has_parameters::set_double2_parameter, this));
    _slots(SET_DOUBLE3_PARAMETER_SLOT, core::com::new_slot(&has_parameters::set_double3_parameter, this));
    _slots(SET_INT_PARAMETER_SLOT, core::com::new_slot(&has_parameters::set_int_parameter, this));
    _slots(SET_INT2_PARAMETER_SLOT, core::com::new_slot(&has_parameters::set_int2_parameter, this));
    _slots(SET_INT3_PARAMETER_SLOT, core::com::new_slot(&has_parameters::set_int3_parameter, this));
    _slots(SET_ENUM_PARAMETER_SLOT, core::com::new_slot(&has_parameters::set_enum_parameter, this));
}

//-----------------------------------------------------------------------------

has_parameters::~has_parameters()
= default;

//------------------------------------------------------------------------------

void has_parameters::set_parameter(sight::ui::parameter_t /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::set_bool_parameter(bool /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::set_color_parameter(std::array<std::uint8_t, 4>/*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::set_double_parameter(double /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::set_double2_parameter(double /*unused*/, double /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::set_double3_parameter(
    double /*unused*/,
    double /*unused*/,
    double /*unused*/,
    std::string /*unused*/
)
{
}

//-----------------------------------------------------------------------------

void has_parameters::set_int_parameter(int /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::set_int2_parameter(int /*unused*/, int /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::set_int3_parameter(int /*unused*/, int /*unused*/, int /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::set_enum_parameter(std::string /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

} // namespace sight::ui
