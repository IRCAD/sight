/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwData/registry/macros.hpp"

#include "fwMedData/Patient.hpp"
#include "fwMedData/Study.hpp"
#include "fwMedData/Equipement.hpp"
#include "fwMedData/Series.hpp"

fwDataRegisterMacro( ::fwMedData::Series );

namespace fwMedData
{

Series::Series(::fwData::Object::Key key)
{
}

//------------------------------------------------------------------------------

Series::~Series()
{}

//------------------------------------------------------------------------------

void Series::shallowCopy(Series::csptr _src)
{
    this->fieldShallowCopy( _src );

    m_attrPatient = _src->m_attrPatient;
    m_attrStudy = _src->m_attrStudy;
    m_attrEquipement = _src->m_attrEquipement;

    m_attrInstanceUID = _src->m_attrInstanceUID;
    m_attrModality = _src->m_attrModality;
    m_attrDate = _src->m_attrDate;
    m_attrTime = _src->m_attrTime;
    m_attrPerformingPhysiciansName = _src->m_attrPerformingPhysiciansName;
    m_attrDescription = _src->m_attrDescription;
}

//------------------------------------------------------------------------------

void Series::deepCopy(Series::csptr _src)
{
    this->fieldDeepCopy( _src );

    m_attrPatient = ::fwData::Object::copy(_src->m_attrPatient);
    m_attrStudy = ::fwData::Object::copy(_src->m_attrStudy);
    m_attrEquipement = ::fwData::Object::copy(_src->m_attrEquipement);

    m_attrInstanceUID = _src->m_attrInstanceUID;
    m_attrModality = _src->m_attrModality;
    m_attrDate = _src->m_attrDate;
    m_attrTime = _src->m_attrTime;
    m_attrPerformingPhysiciansName = _src->m_attrPerformingPhysiciansName;
    m_attrDescription = _src->m_attrDescription;
}

//------------------------------------------------------------------------------

} // namespace fwMedData

