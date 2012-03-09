/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"

#include "fwData/ResectionDB.hpp"

fwDataRegisterMacro( ::fwData::ResectionDB );

namespace fwData
{

//------------------------------------------------------------------------------

ResectionDB::ResectionDB ()
{}

//------------------------------------------------------------------------------

ResectionDB::~ResectionDB ()
{}

//------------------------------------------------------------------------------

void ResectionDB::shallowCopy( ResectionDB::csptr _source )
{
    this->fieldShallowCopy( _source );
}

//------------------------------------------------------------------------------

void ResectionDB::deepCopy( ResectionDB::csptr _source )
{
    this->fieldDeepCopy( _source );
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
