/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwData/registry/macros.hpp"

#include "fwMedData/Patient.hpp"

fwDataRegisterMacro( ::fwMedData::Patient );

namespace fwMedData
{

Patient::Patient(::fwData::Object::Key key)
{
}

//------------------------------------------------------------------------------

Patient::~Patient()
{}

//------------------------------------------------------------------------------

void Patient::shallowCopy(Patient::csptr _src)
{
    this->fieldShallowCopy( _src );
    m_attrName = _src->m_attrName;
    m_attrBirthdate = _src->m_attrBirthdate;
    m_attrId = _src->m_attrId;
    m_attrSex = _src->m_attrSex;
}

//------------------------------------------------------------------------------

void Patient::deepCopy(Patient::csptr _src)
{
    this->fieldDeepCopy( _src );
    m_attrName = _src->m_attrName;
    m_attrBirthdate = _src->m_attrBirthdate;
    m_attrId = _src->m_attrId;
    m_attrSex = _src->m_attrSex;
}

//------------------------------------------------------------------------------

} // namespace fwMedData

