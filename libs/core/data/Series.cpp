/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "data/Exception.hpp"
#include "data/Patient.hpp"
#include "data/registry/macros.hpp"
#include "data/Study.hpp"

SIGHT_REGISTER_DATA(sight::data::Series)

namespace sight::data
{

Series::Series() :
    m_patient(Patient::New()),
    m_study(Study::New()),
    m_equipment(Equipment::New())
{
}

Series::Series(Object::Key) :
    Series()
{
}

//------------------------------------------------------------------------------

Series::~Series()
{
}

//------------------------------------------------------------------------------

void Series::from_series(const Series& source)
{
    DeepCopyCacheType cache;

    // Use aliasing to convert the const reference to a shared_ptr without having it to try to delete the reference.
    // This is a trick, the right solution would be to use const reference for shallowCopy and cachedDeepCopy
    Series::cachedDeepCopy(std::shared_ptr<const Series>(std::shared_ptr<const Series> {}, &source), cache);
}

//------------------------------------------------------------------------------

void Series::shallowCopy(const Object::csptr& _source)
{
    Series::csptr other = Series::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->fieldShallowCopy(other);

    m_patient   = other->m_patient;
    m_study     = other->m_study;
    m_equipment = other->m_equipment;

    m_instanceUID                       = other->m_instanceUID;
    m_modality                          = other->m_modality;
    m_date                              = other->m_date;
    m_time                              = other->m_time;
    m_performingPhysiciansName          = other->m_performingPhysiciansName;
    m_description                       = other->m_description;
    m_number                            = other->m_number;
    m_laterality                        = other->m_laterality;
    m_protocolName                      = other->m_protocolName;
    m_bodyPartExamined                  = other->m_bodyPartExamined;
    m_patientPosition                   = other->m_patientPosition;
    m_anatomicalOrientationType         = other->m_anatomicalOrientationType;
    m_performdedProcedureStepID         = other->m_performdedProcedureStepID;
    m_performedProcedureStepStartDate   = other->m_performedProcedureStepStartDate;
    m_performedProcedureStepStartTime   = other->m_performedProcedureStepStartTime;
    m_performedProcedureStepEndDate     = other->m_performedProcedureStepEndDate;
    m_performedProcedureStepEndTime     = other->m_performedProcedureStepEndTime;
    m_performedProcedureStepDescription = other->m_performedProcedureStepDescription;
    m_performedProcedureComments        = other->m_performedProcedureComments;
}

//------------------------------------------------------------------------------

void Series::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Series::csptr other = Series::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->fieldDeepCopy(other, cache);

    m_patient   = Object::copy(other->m_patient, cache);
    m_study     = Object::copy(other->m_study, cache);
    m_equipment = Object::copy(other->m_equipment, cache);

    m_instanceUID                       = other->m_instanceUID;
    m_modality                          = other->m_modality;
    m_date                              = other->m_date;
    m_time                              = other->m_time;
    m_performingPhysiciansName          = other->m_performingPhysiciansName;
    m_description                       = other->m_description;
    m_number                            = other->m_number;
    m_laterality                        = other->m_laterality;
    m_protocolName                      = other->m_protocolName;
    m_bodyPartExamined                  = other->m_bodyPartExamined;
    m_patientPosition                   = other->m_patientPosition;
    m_anatomicalOrientationType         = other->m_anatomicalOrientationType;
    m_performdedProcedureStepID         = other->m_performdedProcedureStepID;
    m_performedProcedureStepStartDate   = other->m_performedProcedureStepStartDate;
    m_performedProcedureStepStartTime   = other->m_performedProcedureStepStartTime;
    m_performedProcedureStepEndDate     = other->m_performedProcedureStepEndDate;
    m_performedProcedureStepEndTime     = other->m_performedProcedureStepEndTime;
    m_performedProcedureStepDescription = other->m_performedProcedureStepDescription;
    m_performedProcedureComments        = other->m_performedProcedureComments;
}

//------------------------------------------------------------------------------

bool Series::operator==(const Series& other) const noexcept
{
    if(m_modality != other.m_modality
       || m_instanceUID != other.m_instanceUID
       || m_number != other.m_number
       || m_laterality != other.m_laterality
       || m_date != other.m_date
       || m_time != other.m_time
       || m_performingPhysiciansName != other.m_performingPhysiciansName
       || m_protocolName != other.m_protocolName
       || m_description != other.m_description
       || m_bodyPartExamined != other.m_bodyPartExamined
       || m_patientPosition != other.m_patientPosition
       || m_anatomicalOrientationType != other.m_anatomicalOrientationType
       || m_performdedProcedureStepID != other.m_performdedProcedureStepID
       || m_performedProcedureStepStartDate != other.m_performedProcedureStepStartDate
       || m_performedProcedureStepStartTime != other.m_performedProcedureStepStartTime
       || m_performedProcedureStepEndDate != other.m_performedProcedureStepEndDate
       || m_performedProcedureStepEndTime != other.m_performedProcedureStepEndTime
       || m_performedProcedureStepDescription != other.m_performedProcedureStepDescription
       || m_performedProcedureComments != other.m_performedProcedureComments
       || !core::tools::is_equal(m_patient, other.m_patient)
       || !core::tools::is_equal(m_study, other.m_study)
       || !core::tools::is_equal(m_equipment, other.m_equipment))
    {
        return false;
    }

    // Super class last
    return Object::operator==(other);
}

//------------------------------------------------------------------------------

bool Series::operator!=(const Series& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
