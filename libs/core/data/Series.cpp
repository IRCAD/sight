/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "data/Series.hpp"

#include "data/Equipment.hpp"
#include "data/Patient.hpp"
#include "data/Study.hpp"

#include <data/Exception.hpp>
#include <data/registry/macros.hpp>

namespace sight::data
{

Series::Series(data::Object::Key) :
    m_patient(data::Patient::New()),
    m_study(data::Study::New()),
    m_equipment(data::Equipment::New())
{
}

//------------------------------------------------------------------------------

Series::~Series()
{
}

//------------------------------------------------------------------------------

void Series::shallowCopy(const data::Object::csptr& _source)
{
    Series::csptr other = Series::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( other );

    m_patient   = other->m_patient;
    m_study     = other->m_study;
    m_equipment = other->m_equipment;

    m_instanceUID              = other->m_instanceUID;
    m_modality                 = other->m_modality;
    m_date                     = other->m_date;
    m_time                     = other->m_time;
    m_performingPhysiciansName = other->m_performingPhysiciansName;
    m_description              = other->m_description;
}

//------------------------------------------------------------------------------

void Series::cachedDeepCopy(const data::Object::csptr& _source, DeepCopyCacheType& cache)
{
    Series::csptr other = Series::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( other, cache );

    m_patient   = data::Object::copy(other->m_patient, cache);
    m_study     = data::Object::copy(other->m_study, cache);
    m_equipment = data::Object::copy(other->m_equipment, cache);

    m_instanceUID              = other->m_instanceUID;
    m_modality                 = other->m_modality;
    m_date                     = other->m_date;
    m_time                     = other->m_time;
    m_performingPhysiciansName = other->m_performingPhysiciansName;
    m_description              = other->m_description;
}

//------------------------------------------------------------------------------

} // namespace sight::data
