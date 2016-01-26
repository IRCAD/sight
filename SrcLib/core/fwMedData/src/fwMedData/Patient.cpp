/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedData/Patient.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

fwDataRegisterMacro( ::fwMedData::Patient );

namespace fwMedData
{

Patient::Patient(::fwData::Object::Key key)
{
}

//------------------------------------------------------------------------------

Patient::~Patient()
{
}

//------------------------------------------------------------------------------

void Patient::shallowCopy(const ::fwData::Object::csptr &_source)
{
    Patient::csptr other = Patient::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( other );
    m_name      = other->m_name;
    m_birthdate = other->m_birthdate;
    m_patientId = other->m_patientId;
    m_sex       = other->m_sex;
}

//------------------------------------------------------------------------------

void Patient::cachedDeepCopy(const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache)
{
    Patient::csptr other = Patient::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( other, cache );
    m_name      = other->m_name;
    m_birthdate = other->m_birthdate;
    m_patientId = other->m_patientId;
    m_sex       = other->m_sex;
}

//------------------------------------------------------------------------------

} // namespace fwMedData

