/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwData/registry/macros.hpp"

#include "fwMedData/Equipement.hpp"

fwDataRegisterMacro( ::fwMedData::Equipement );

namespace fwMedData
{

Equipement::Equipement(::fwData::Object::Key key)
{
}

//------------------------------------------------------------------------------

Equipement::~Equipement()
{}

//------------------------------------------------------------------------------

void Equipement::shallowCopy(Equipement::csptr _src)
{
    this->fieldShallowCopy( _src );
    m_attrInstitutionName = _src->m_attrInstitutionName;
}

//------------------------------------------------------------------------------

void Equipement::deepCopy(Equipement::csptr _src)
{
    this->fieldDeepCopy( _src );
    m_attrInstitutionName = _src->m_attrInstitutionName;
}

//------------------------------------------------------------------------------

} // namespace fwMedData

