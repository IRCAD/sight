/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwData/registry/macros.hpp>
#include <fwData/Exception.hpp>

#include "fwMedData/Equipment.hpp"

fwDataRegisterMacro( ::fwMedData::Equipment );

namespace fwMedData
{

Equipment::Equipment(::fwData::Object::Key key)
{
}

//------------------------------------------------------------------------------

Equipment::~Equipment()
{
}

//------------------------------------------------------------------------------

void Equipment::shallowCopy(const ::fwData::Object::csptr &_source)
{
    Equipment::csptr other = Equipment::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( _source );
    m_attrInstitutionName = other->m_attrInstitutionName;
}

//------------------------------------------------------------------------------

void Equipment::cachedDeepCopy(const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache)
{
    Equipment::csptr other = Equipment::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( _source, cache );
    m_attrInstitutionName = other->m_attrInstitutionName;
}

//------------------------------------------------------------------------------

} // namespace fwMedData

