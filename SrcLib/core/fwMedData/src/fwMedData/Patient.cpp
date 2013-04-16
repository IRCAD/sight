/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwData/registry/macros.hpp>
#include <fwData/Exception.hpp>

#include "fwMedData/Patient.hpp"

fwDataRegisterMacro( ::fwMedData::Patient );

namespace fwMedData
{

Patient::Patient(::fwData::Object::Key key)
{
}

//------------------------------------------------------------------------------

Patient::~Patient()
{}

//------------------------------------------------------------------------------

void Patient::shallowCopy(const ::fwData::Object::csptr &_source)
{
    Patient::csptr other = Patient::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( other );
    m_attrName = other->m_attrName;
    m_attrBirthdate = other->m_attrBirthdate;
    m_attrPatientId = other->m_attrPatientId;
    m_attrSex = other->m_attrSex;
}

//------------------------------------------------------------------------------

void Patient::deepCopy(const ::fwData::Object::csptr &_source)
{
    Patient::csptr other = Patient::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( other );
    m_attrName = other->m_attrName;
    m_attrBirthdate = other->m_attrBirthdate;
    m_attrPatientId = other->m_attrPatientId;
    m_attrSex = other->m_attrSex;
}

//------------------------------------------------------------------------------

} // namespace fwMedData

