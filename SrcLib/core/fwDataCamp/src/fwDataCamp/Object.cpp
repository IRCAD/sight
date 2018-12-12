/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include <fwCamp/UserObject.hpp>

#include <fwData/Object.hpp>

fwCampImplementDataMacro((fwData)(Object))
{
    builder
    .property("fields", &::fwData::Object::m_fields)
    .function("classname", (const std::string& (fwData::Object::*)() const) &::fwData::Object::getClassname)
    .function("is_a", (bool (fwData::Object::*)(const std::string&) const) &::fwData::Object::isA)
    ;
}
