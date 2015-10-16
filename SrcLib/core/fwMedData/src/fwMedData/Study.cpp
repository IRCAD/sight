/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwMedData/Study.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

fwDataRegisterMacro( ::fwMedData::Study );

namespace fwMedData
{

Study::Study(::fwData::Object::Key key)
{
}

//------------------------------------------------------------------------------

Study::~Study()
{
}

//------------------------------------------------------------------------------

void Study::shallowCopy(const ::fwData::Object::csptr &_source)
{
    Study::csptr other = Study::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( other );
    m_instanceUID            = other->m_instanceUID;
    m_date                   = other->m_date;
    m_time                   = other->m_time;
    m_referringPhysicianName = other->m_referringPhysicianName;
    m_description            = other->m_description;
    m_patientAge             = other->m_patientAge;
}

//------------------------------------------------------------------------------

void Study::cachedDeepCopy(const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache)
{
    Study::csptr other = Study::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( other, cache );
    m_instanceUID            = other->m_instanceUID;
    m_date                   = other->m_date;
    m_time                   = other->m_time;
    m_referringPhysicianName = other->m_referringPhysicianName;
    m_description            = other->m_description;
    m_patientAge             = other->m_patientAge;
}

//------------------------------------------------------------------------------

} // namespace fwMedData

