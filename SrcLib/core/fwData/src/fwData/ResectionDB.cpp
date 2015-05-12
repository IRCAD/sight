/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"

#include "fwData/ResectionDB.hpp"

fwDataRegisterMacro( ::fwData::ResectionDB );

namespace fwData
{

//------------------------------------------------------------------------------

ResectionDB::ResectionDB(::fwData::Object::Key key)
{
}

//------------------------------------------------------------------------------

ResectionDB::~ResectionDB()
{
}

//------------------------------------------------------------------------------

void ResectionDB::shallowCopy(const Object::csptr &_source )
{
    ResectionDB::csptr other = ResectionDB::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
}

//------------------------------------------------------------------------------

void ResectionDB::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    ResectionDB::csptr other = ResectionDB::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );
}

//------------------------------------------------------------------------------

ResectionDB::ResectionContainerType::size_type ResectionDB::getNumberOfResections() const
{
    return m_attrResections.size();
}

//------------------------------------------------------------------------------

void ResectionDB::addResection( ::fwData::Resection::sptr resection )
{
    m_attrResections.push_back( resection );
}

//------------------------------------------------------------------------------

} // end namespace fwData
