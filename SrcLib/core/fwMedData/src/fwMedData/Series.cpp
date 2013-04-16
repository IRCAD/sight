/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwData/registry/macros.hpp>
#include <fwData/Exception.hpp>

#include "fwMedData/Patient.hpp"
#include "fwMedData/Study.hpp"
#include "fwMedData/Equipment.hpp"
#include "fwMedData/Series.hpp"


namespace fwMedData
{

Series::Series(::fwData::Object::Key key) :
        m_attrPatient(::fwMedData::Patient::New()),
        m_attrStudy(::fwMedData::Study::New()),
        m_attrEquipment(::fwMedData::Equipment::New())
{
}

//------------------------------------------------------------------------------

Series::~Series()
{}

//------------------------------------------------------------------------------

void Series::shallowCopy(const ::fwData::Object::csptr &_source)
{
    Series::csptr other = Series::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( other );

    m_attrPatient = other->m_attrPatient;
    m_attrStudy = other->m_attrStudy;
    m_attrEquipment = other->m_attrEquipment;

    m_attrInstanceUID = other->m_attrInstanceUID;
    m_attrModality = other->m_attrModality;
    m_attrDate = other->m_attrDate;
    m_attrTime = other->m_attrTime;
    m_attrPerformingPhysiciansName = other->m_attrPerformingPhysiciansName;
    m_attrDescription = other->m_attrDescription;
}

//------------------------------------------------------------------------------

void Series::deepCopy(const ::fwData::Object::csptr &_source)
{
    Series::csptr other = Series::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( other );

    m_attrPatient = ::fwData::Object::copy(other->m_attrPatient);
    m_attrStudy = ::fwData::Object::copy(other->m_attrStudy);
    m_attrEquipment = ::fwData::Object::copy(other->m_attrEquipment);

    m_attrInstanceUID = other->m_attrInstanceUID;
    m_attrModality = other->m_attrModality;
    m_attrDate = other->m_attrDate;
    m_attrTime = other->m_attrTime;
    m_attrPerformingPhysiciansName = other->m_attrPerformingPhysiciansName;
    m_attrDescription = other->m_attrDescription;
}

//------------------------------------------------------------------------------

} // namespace fwMedData

