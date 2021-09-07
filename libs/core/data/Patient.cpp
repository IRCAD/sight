/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "data/Patient.hpp"

#include <data/Exception.hpp>
#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::data::Patient)

namespace sight::data
{

Patient::Patient(data::Object::Key)
{
}

//------------------------------------------------------------------------------

Patient::~Patient()
{
}

//------------------------------------------------------------------------------

void Patient::shallowCopy(const data::Object::csptr& _source)
{
    Patient::csptr other = Patient::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->fieldShallowCopy(other);
    m_name      = other->m_name;
    m_birthdate = other->m_birthdate;
    m_patientId = other->m_patientId;
    m_sex       = other->m_sex;
}

//------------------------------------------------------------------------------

void Patient::cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache)
{
    Patient::csptr other = Patient::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->fieldDeepCopy(other, cache);
    m_name      = other->m_name;
    m_birthdate = other->m_birthdate;
    m_patientId = other->m_patientId;
    m_sex       = other->m_sex;
}

//------------------------------------------------------------------------------

} // namespace sight::data
