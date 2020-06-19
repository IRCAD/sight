/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
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

#include "fwServices/IParametersService.hpp"

#include <fwCom/Slots.hxx>

namespace fwServices
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType IParametersService::s_SET_BOOL_PARAMETER_SLOT    = "setBoolParameter";
const ::fwCom::Slots::SlotKeyType IParametersService::s_SET_COLOR_PARAMETER_SLOT   = "setColorParameter";
const ::fwCom::Slots::SlotKeyType IParametersService::s_SET_DOUBLE_PARAMETER_SLOT  = "setDoubleParameter";
const ::fwCom::Slots::SlotKeyType IParametersService::s_SET_DOUBLE2_PARAMETER_SLOT = "setDouble2Parameter";
const ::fwCom::Slots::SlotKeyType IParametersService::s_SET_DOUBLE3_PARAMETER_SLOT = "setDouble3Parameter";
const ::fwCom::Slots::SlotKeyType IParametersService::s_SET_INT_PARAMETER_SLOT     = "setIntParameter";
const ::fwCom::Slots::SlotKeyType IParametersService::s_SET_INT2_PARAMETER_SLOT    = "setInt2Parameter";
const ::fwCom::Slots::SlotKeyType IParametersService::s_SET_INT3_PARAMETER_SLOT    = "setInt3Parameter";
const ::fwCom::Slots::SlotKeyType IParametersService::s_SET_ENUM_PARAMETER_SLOT    = "setEnumParameter";

//-----------------------------------------------------------------------------

IParametersService::IParametersService()
{
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &IParametersService::setBoolParameter, this);
    newSlot(s_SET_COLOR_PARAMETER_SLOT, &IParametersService::setColorParameter, this);
    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &IParametersService::setDoubleParameter, this);
    newSlot(s_SET_DOUBLE2_PARAMETER_SLOT, &IParametersService::setDouble2Parameter, this);
    newSlot(s_SET_DOUBLE3_PARAMETER_SLOT, &IParametersService::setDouble3Parameter, this);
    newSlot(s_SET_INT_PARAMETER_SLOT, &IParametersService::setIntParameter, this);
    newSlot(s_SET_INT2_PARAMETER_SLOT, &IParametersService::setInt2Parameter, this);
    newSlot(s_SET_INT3_PARAMETER_SLOT, &IParametersService::setInt3Parameter, this);
    newSlot(s_SET_ENUM_PARAMETER_SLOT, &IParametersService::setEnumParameter, this);
}

//-----------------------------------------------------------------------------

IParametersService::~IParametersService()
{
}

//-----------------------------------------------------------------------------

void IParametersService::setBoolParameter(bool, std::string)
{
}

//-----------------------------------------------------------------------------

void IParametersService::setColorParameter(std::array<std::uint8_t, 4>, std::string)
{
}

//-----------------------------------------------------------------------------

void IParametersService::setDoubleParameter(double, std::string)
{

}

//-----------------------------------------------------------------------------

void IParametersService::setDouble2Parameter(double, double, std::string)
{

}

//-----------------------------------------------------------------------------

void IParametersService::setDouble3Parameter(double, double, double, std::string)
{

}

//-----------------------------------------------------------------------------

void IParametersService::setIntParameter(int, std::string)
{

}

//-----------------------------------------------------------------------------

void IParametersService::setInt2Parameter(int, int, std::string)
{

}

//-----------------------------------------------------------------------------

void IParametersService::setInt3Parameter(int, int, int, std::string)
{

}

//-----------------------------------------------------------------------------

void IParametersService::setEnumParameter(std::string, std::string)
{

}

//-----------------------------------------------------------------------------

}
