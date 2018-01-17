/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/Tag.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

#include <fwCore/base.hpp>

fwDataRegisterMacro( ::fwData::Tag );

namespace fwData
{
//------------------------------------------------------------------------------

Tag::Tag(::fwData::Object::Key key) :
    m_size(0.5)
{
}

//------------------------------------------------------------------------------

Tag::~Tag ()
{
    if(m_pointList)
    {
        m_pointList->getPoints().clear();
    }
}

//------------------------------------------------------------------------------

void Tag::shallowCopy(const Object::csptr& source )
{
    Tag::csptr other = Tag::dynamicConstCast(source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( source );
    m_sType     = other->m_sType;
    m_size      = other->m_size;
    m_pointList = other->m_pointList;
}

//------------------------------------------------------------------------------

void Tag::cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache)
{
    Tag::csptr other = Tag::dynamicConstCast(source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( source, cache );
    m_sType     = other->m_sType;
    m_size      = other->m_size;
    m_pointList = ::fwData::Object::copy(other->m_pointList, cache);
}

} // namespace fwData
