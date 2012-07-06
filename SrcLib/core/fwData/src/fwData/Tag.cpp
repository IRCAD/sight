/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"

#include "fwData/Tag.hpp"

fwDataRegisterMacro( ::fwData::Tag );

namespace fwData
{
//------------------------------------------------------------------------------

Tag::Tag() : m_size(0.5)
{}

//------------------------------------------------------------------------------

Tag::~Tag ()
{
    if(m_pointList)
    {
        m_pointList->getRefPoints().clear();
    }
}

//------------------------------------------------------------------------------

void Tag::shallowCopy( Tag::csptr source )
{
    this->fieldShallowCopy( source );
    m_sType = source->m_sType;
    m_size = source->m_size;
    m_pointList = source->m_pointList;

}

//------------------------------------------------------------------------------

void Tag::deepCopy( Tag::csptr source )
{
    this->fieldDeepCopy( source );
    m_sType = source->m_sType;
    m_size = source->m_size;
    m_pointList = ::fwData::Object::copy(source->m_pointList);
}

} // namespace fwData


