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


#include "fwMedData/Study.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

fwDataRegisterMacro( ::fwMedData::Study );

namespace fwMedData
{

Study::Study(::fwData::Object::Key key)
{
}

//------------------------------------------------------------------------------

Study::~Study()
{
}

//------------------------------------------------------------------------------

void Study::shallowCopy(const ::fwData::Object::csptr &_source)
{
    Study::csptr other = Study::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( other );
    m_instanceUID            = other->m_instanceUID;
    m_date                   = other->m_date;
    m_time                   = other->m_time;
    m_referringPhysicianName = other->m_referringPhysicianName;
    m_description            = other->m_description;
    m_patientAge             = other->m_patientAge;
}

//------------------------------------------------------------------------------

void Study::cachedDeepCopy(const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache)
{
    Study::csptr other = Study::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( other, cache );
    m_instanceUID            = other->m_instanceUID;
    m_date                   = other->m_date;
    m_time                   = other->m_time;
    m_referringPhysicianName = other->m_referringPhysicianName;
    m_description            = other->m_description;
    m_patientAge             = other->m_patientAge;
}

//------------------------------------------------------------------------------

} // namespace fwMedData

