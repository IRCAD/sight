/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

const core::com::slots::key_t has_parameters::SET_PARAMETER_SLOT         = "setParameter";
const core::com::slots::key_t has_parameters::SET_BOOL_PARAMETER_SLOT    = "setBoolParameter";
const core::com::slots::key_t has_parameters::SET_COLOR_PARAMETER_SLOT   = "setColorParameter";
const core::com::slots::key_t has_parameters::SET_DOUBLE_PARAMETER_SLOT  = "setDoubleParameter";
const core::com::slots::key_t has_parameters::SET_DOUBLE2_PARAMETER_SLOT = "setDouble2Parameter";
const core::com::slots::key_t has_parameters::SET_DOUBLE3_PARAMETER_SLOT = "setDouble3Parameter";
const core::com::slots::key_t has_parameters::SET_INT_PARAMETER_SLOT     = "setIntParameter";
const core::com::slots::key_t has_parameters::SET_INT2_PARAMETER_SLOT    = "setInt2Parameter";
const core::com::slots::key_t has_parameters::SET_INT3_PARAMETER_SLOT    = "setInt3Parameter";
const core::com::slots::key_t has_parameters::SET_ENUM_PARAMETER_SLOT    = "setEnumParameter";

//-----------------------------------------------------------------------------

has_parameters::has_parameters(sight::core::com::slots& _slots)
{
    _slots(SET_PARAMETER_SLOT, core::com::new_slot(&has_parameters::setParameter, this));
    _slots(SET_BOOL_PARAMETER_SLOT, core::com::new_slot(&has_parameters::setBoolParameter, this));
    _slots(SET_COLOR_PARAMETER_SLOT, core::com::new_slot(&has_parameters::setColorParameter, this));
    _slots(SET_DOUBLE_PARAMETER_SLOT, core::com::new_slot(&has_parameters::setDoubleParameter, this));
    _slots(SET_DOUBLE2_PARAMETER_SLOT, core::com::new_slot(&has_parameters::setDouble2Parameter, this));
    _slots(SET_DOUBLE3_PARAMETER_SLOT, core::com::new_slot(&has_parameters::setDouble3Parameter, this));
    _slots(SET_INT_PARAMETER_SLOT, core::com::new_slot(&has_parameters::setIntParameter, this));
    _slots(SET_INT2_PARAMETER_SLOT, core::com::new_slot(&has_parameters::setInt2Parameter, this));
    _slots(SET_INT3_PARAMETER_SLOT, core::com::new_slot(&has_parameters::setInt3Parameter, this));
    _slots(SET_ENUM_PARAMETER_SLOT, core::com::new_slot(&has_parameters::setEnumParameter, this));
}

//-----------------------------------------------------------------------------

has_parameters::~has_parameters()
= default;

//------------------------------------------------------------------------------

void has_parameters::setParameter(sight::ui::parameter_t /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::setBoolParameter(bool /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::setColorParameter(std::array<std::uint8_t, 4> /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::setDoubleParameter(double /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::setDouble2Parameter(double /*unused*/, double /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::setDouble3Parameter(
    double /*unused*/,
    double /*unused*/,
    double /*unused*/,
    std::string /*unused*/
)
{
}

//-----------------------------------------------------------------------------

void has_parameters::setIntParameter(int /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::setInt2Parameter(int /*unused*/, int /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::setInt3Parameter(int /*unused*/, int /*unused*/, int /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void has_parameters::setEnumParameter(std::string /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

} // namespace sight::ui
