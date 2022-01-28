/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "core/reflection/camp/MapProperty.hpp"

#include "core/reflection/camp/customtype.hpp"
#include "core/reflection/camp/ExtendedClassVisitor.hpp"

//cspell: ignore Wconversion
#ifndef WIN32
# pragma GCC diagnostic warning "-Wconversion"
#endif

namespace camp
{

MapProperty::MapProperty(const std::string& name, Type elementType) :
    Property(name, static_cast< ::camp::Type>(mappingType)),
    m_elementType(elementType)
{
}

//-----------------------------------------------------------------------------

MapProperty::~MapProperty()
{
}

//-----------------------------------------------------------------------------

void MapProperty::accept(ClassVisitor& visitor) const
{
    ExtendedClassVisitor* mapVisitor = dynamic_cast<ExtendedClassVisitor*>(&visitor);
    if(mapVisitor)
    {
        mapVisitor->visit(*this);
    }
    else
    {
        visitor.visit(*this);
    }
}

//-----------------------------------------------------------------------------

Value MapProperty::getValue(const UserObject& object) const
{
    return getElement(object, 0);
}

//-----------------------------------------------------------------------------

void MapProperty::setValue(const UserObject&, const Value&) const
{
    //FIXME XXX TODO DO something
}

//-----------------------------------------------------------------------------

Type MapProperty::elementType() const
{
    return m_elementType;
}

} // namespace camp
