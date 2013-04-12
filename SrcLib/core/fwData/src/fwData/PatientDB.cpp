/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"

#include "fwData/Patient.hpp"


#include "fwData/PatientDB.hpp"

fwDataRegisterMacro( ::fwData::PatientDB );
namespace fwData
{

//------------------------------------------------------------------------------

PatientDB::PatientDB(::fwData::Object::Key key)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

PatientDB::~PatientDB()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void PatientDB::shallowCopy(const Object::csptr &_source )
{
    PatientDB::csptr other = PatientDB::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_attrPatients = other->m_attrPatients;
}

//------------------------------------------------------------------------------

void PatientDB::deepCopy(const Object::csptr &_source )
{
    PatientDB::csptr other = PatientDB::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source );
    m_attrPatients.clear();
    std::transform(
            other->m_attrPatients.begin(), other->m_attrPatients.end(),
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
