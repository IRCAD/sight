/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
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

#include <fwData/Composite.hpp>
#include <fwData/camp/mapper.hpp>

#include <fwCamp/UserObject.hpp>
#include <fwCamp/Mapper/ValueMapper.hpp>

fwCampImplementDataMacro((fwData)(Composite))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("values", &::fwData::Composite::m_container)
    ;
}
