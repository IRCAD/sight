/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwMedData/Series.hpp"

#include "fwMedData/Equipment.hpp"
#include "fwMedData/Patient.hpp"
#include "fwMedData/Study.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

namespace fwMedData
{

Series::Series(::fwData::Object::Key) :
    m_patient(::fwMedData::Patient::New()),
    m_study(::fwMedData::Study::New()),
    m_equipment(::fwMedData::Equipment::New())
{
}

//------------------------------------------------------------------------------

Series::~Series()
{
}

//------------------------------------------------------------------------------

void Series::shallowCopy(const ::fwData::Object::csptr& _source)
{
    Series::csptr other = Series::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
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

void Series::cachedDeepCopy(const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache)
{
    Series::csptr other = Series::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( other, cache );

    m_patient   = ::fwData::Object::copy(other->m_patient, cache);
    m_study     = ::fwData::Object::copy(other->m_study, cache);
    m_equipment = ::fwData::Object::copy(other->m_equipment, cache);

    m_instanceUID              = other->m_instanceUID;
    m_modality                 = other->m_modality;
    m_date                     = other->m_date;
    m_time                     = other->m_time;
    m_performingPhysiciansName = other->m_performingPhysiciansName;
    m_description              = other->m_description;
}

//------------------------------------------------------------------------------

} // namespace fwMedData
