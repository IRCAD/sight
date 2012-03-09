/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"

#include "fwData/Patient.hpp"


#include "fwData/PatientDB.hpp"

fwDataRegisterMacro( ::fwData::PatientDB );
namespace fwData
{

//------------------------------------------------------------------------------

PatientDB::PatientDB ()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

PatientDB::~PatientDB ()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void PatientDB::shallowCopy( PatientDB::csptr _source )
{
    this->fieldShallowCopy( _source );
    m_attrPatients = _source->m_attrPatients;
}

//------------------------------------------------------------------------------

void PatientDB::deepCopy( PatientDB::csptr _source )
{
    this->fieldDeepCopy( _source );
    m_attrPatients.clear();
    std::transform(
            _source->m_attrPatients.begin(), _source->m_attrPatients.end(),
            std::back_inserter(m_attrPatients),
            & ::fwData::Object::copy< PatientContainerType::value_type::element_type >
            );
}

//------------------------------------------------------------------------------

PatientDB::PatientContainerType::size_type  PatientDB::getNumberOfPatients() const
{
    return this->getPatients().size();
}

//------------------------------------------------------------------------------

void PatientDB::addPatient( ::fwData::Patient::sptr _patient )
{
    m_attrPatients.push_back(_patient);
}

} // end namespace fwData
