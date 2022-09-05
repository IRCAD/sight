/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/Study.hpp"

#include <data/Exception.hpp>
#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::data::Study)

namespace sight::data
{

Study::Study(data::Object::Key /*unused*/)
{
}

//------------------------------------------------------------------------------

Study::~Study()
= default;

//------------------------------------------------------------------------------

void Study::shallowCopy(const data::Object::csptr& _source)
{
    Study::csptr other = Study::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->fieldShallowCopy(other);
    m_studyID                 = other->m_studyID;
    m_instanceUID             = other->m_instanceUID;
    m_date                    = other->m_date;
    m_time                    = other->m_time;
    m_referringPhysicianName  = other->m_referringPhysicianName;
    m_consultingPhysicianName = other->m_consultingPhysicianName;
    m_description             = other->m_description;
    m_patientAge              = other->m_patientAge;
    m_patientSize             = other->m_patientSize;
    m_patientWeight           = other->m_patientWeight;
    m_patientBodyMassIndex    = other->m_patientBodyMassIndex;
}

//------------------------------------------------------------------------------

void Study::cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache)
{
    Study::csptr other = Study::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->fieldDeepCopy(other, cache);
    m_studyID                 = other->m_studyID;
    m_instanceUID             = other->m_instanceUID;
    m_date                    = other->m_date;
    m_time                    = other->m_time;
    m_referringPhysicianName  = other->m_referringPhysicianName;
    m_consultingPhysicianName = other->m_consultingPhysicianName;
    m_description             = other->m_description;
    m_patientAge              = other->m_patientAge;
    m_patientSize             = other->m_patientSize;
    m_patientWeight           = other->m_patientWeight;
    m_patientBodyMassIndex    = other->m_patientBodyMassIndex;
}

//------------------------------------------------------------------------------

bool Study::operator==(const Study& other) const noexcept
{
    if(m_instanceUID != other.m_instanceUID
       || m_studyID != other.m_studyID
       || m_date != other.m_date
       || m_time != other.m_time
       || m_referringPhysicianName != other.m_referringPhysicianName
       || m_consultingPhysicianName != other.m_consultingPhysicianName
       || m_description != other.m_description
       || m_patientAge != other.m_patientAge
       || m_patientSize != other.m_patientSize
       || m_patientWeight != other.m_patientWeight
       || m_patientBodyMassIndex != other.m_patientBodyMassIndex)
    {
        return false;
    }

    // Super class last
    return Object::operator==(other);
}

//------------------------------------------------------------------------------

bool Study::operator!=(const Study& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
