/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwData/registry/macros.hpp"

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

void Study::shallowCopy(Study::csptr _src)
{
    this->fieldShallowCopy( _src );
    m_attrInstanceUID = _src->m_attrInstanceUID;
    m_attrDate = _src->m_attrDate;
    m_attrTime = _src->m_attrTime;
    m_attrReferringPhysicianName = _src->m_attrReferringPhysicianName;
    m_attrDescription = _src->m_attrDescription;
    m_attrPatientAge = _src->m_attrPatientAge;
}

//------------------------------------------------------------------------------

void Study::deepCopy(Study::csptr _src)
{
    this->fieldShallowCopy( _src );
    m_attrInstanceUID = _src->m_attrInstanceUID;
    m_attrDate = _src->m_attrDate;
    m_attrTime = _src->m_attrTime;
    m_attrReferringPhysicianName = _src->m_attrReferringPhysicianName;
    m_attrDescription = _src->m_attrDescription;
    m_attrPatientAge = _src->m_attrPatientAge;
}

//------------------------------------------------------------------------------

} // namespace fwMedData

