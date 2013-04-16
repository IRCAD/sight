/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwData/registry/macros.hpp>
#include <fwData/Exception.hpp>

#include "fwMedData/Study.hpp"

fwDataRegisterMacro( ::fwMedData::Study );

namespace fwMedData
{

Study::Study(::fwData::Object::Key key)
{
}

//------------------------------------------------------------------------------

Study::~Study()
{}

//------------------------------------------------------------------------------

void Study::shallowCopy(const ::fwData::Object::csptr &_source)
{
    Study::csptr other = Study::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( other );
    m_attrInstanceUID = other->m_attrInstanceUID;
    m_attrDate = other->m_attrDate;
    m_attrTime = other->m_attrTime;
    m_attrReferringPhysicianName = other->m_attrReferringPhysicianName;
    m_attrDescription = other->m_attrDescription;
    m_attrPatientAge = other->m_attrPatientAge;
}

//------------------------------------------------------------------------------

void Study::deepCopy(const ::fwData::Object::csptr &_source)
{
    Study::csptr other = Study::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( other );
    m_attrInstanceUID = other->m_attrInstanceUID;
    m_attrDate = other->m_attrDate;
    m_attrTime = other->m_attrTime;
    m_attrReferringPhysicianName = other->m_attrReferringPhysicianName;
    m_attrDescription = other->m_attrDescription;
    m_attrPatientAge = other->m_attrPatientAge;
}

//------------------------------------------------------------------------------

} // namespace fwMedData

