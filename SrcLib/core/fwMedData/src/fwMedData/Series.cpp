/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedData/Equipment.hpp"
#include "fwMedData/Patient.hpp"
#include "fwMedData/Series.hpp"
#include "fwMedData/Study.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

namespace fwMedData
{

Series::Series(::fwData::Object::Key key) :
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

void Series::shallowCopy(const ::fwData::Object::csptr &_source)
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

void Series::cachedDeepCopy(const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache)
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

