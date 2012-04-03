/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Study.hpp"

fwDataRegisterMacro( ::fwData::Study );

namespace fwData
{

//------------------------------------------------------------------------------

Study::Study() :
    m_sHospital(""),
    m_sModality(""),
    m_sAcquisitionZone(""),
    m_sRISId(""),
    m_sUID(""),
    m_i32DbID(-1)
{}

//------------------------------------------------------------------------------

Study::~Study()
{}

//------------------------------------------------------------------------------

void Study::shallowCopy( Study::csptr _source )
{
    this->fieldShallowCopy( _source );
    m_sHospital = _source->m_sHospital;
    m_sModality = _source->m_sModality;
    m_sAcquisitionZone = _source->m_sAcquisitionZone;
    m_sRISId = _source->m_sRISId;
    m_sUID = _source->m_sUID;
    m_i32DbID = _source->m_i32DbID;
    m_date = _source->m_date;
    m_time = _source->m_time;
    m_description = _source->m_description;
    m_attrAcquisitions = _source->m_attrAcquisitions;
}

//------------------------------------------------------------------------------

void Study::deepCopy( Study::csptr _source )
{
    this->fieldDeepCopy( _source );
    m_sHospital = _source->m_sHospital;
    m_sModality = _source->m_sModality;
    m_sAcquisitionZone = _source->m_sAcquisitionZone;
    m_sRISId = _source->m_sRISId;
    m_sUID = _source->m_sUID;
    m_i32DbID = _source->m_i32DbID;
    m_date = _source->m_date;
    m_time = _source->m_time;
    m_description = _source->m_description;
    m_attrAcquisitions.clear();
    m_attrAcquisitions.resize(_source->m_attrAcquisitions.size());
    std::transform(_source->m_attrAcquisitions.begin(), _source->m_attrAcquisitions.end(),
                   m_attrAcquisitions.begin(),
                   &::fwData::Object::copy< AcquisitionContainerType::value_type::element_type >
                  );
}

//------------------------------------------------------------------------------

Study::AcquisitionContainerType::size_type  Study::getNumberOfAcquisitions() const
{
    return m_attrAcquisitions.size();
}

//------------------------------------------------------------------------------

void Study::addAcquisition( ::fwData::Acquisition::sptr acquisition )
{
    m_attrAcquisitions.push_back( acquisition );
}

//------------------------------------------------------------------------------

} // namespace fwData

