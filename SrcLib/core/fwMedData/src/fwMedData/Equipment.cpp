/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwData/registry/macros.hpp"

#include "fwMedData/Equipment.hpp"

fwDataRegisterMacro( ::fwMedData::Equipment );

namespace fwMedData
{

Equipment::Equipment(::fwData::Object::Key key)
{
}

//------------------------------------------------------------------------------

Equipment::~Equipment()
{}

//------------------------------------------------------------------------------

void Equipment::shallowCopy(Equipment::csptr _src)
{
    this->fieldShallowCopy( _src );
    m_attrInstitutionName = _src->m_attrInstitutionName;
}

//------------------------------------------------------------------------------

void Equipment::deepCopy(Equipment::csptr _src)
{
    this->fieldDeepCopy( _src );
    m_attrInstitutionName = _src->m_attrInstitutionName;
}

//------------------------------------------------------------------------------

} // namespace fwMedData

