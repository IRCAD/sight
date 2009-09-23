/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Patient.hpp"


#include "fwData/PatientDB.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::PatientDB, ::fwData::PatientDB );
namespace fwData
{

//------------------------------------------------------------------------------

const Object::FieldID PatientDB::ID_PATIENTS = "ID_PATIENTS";

//------------------------------------------------------------------------------

PatientDB::PatientDB ()
{
	SLM_TRACE_FUNC();
	this->setField( PatientDB::ID_PATIENTS );
}

//------------------------------------------------------------------------------

PatientDB::~PatientDB ()
{
	SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

PatientDB & PatientDB::operator=( const PatientDB & _patientDB )
{
	this->getField( PatientDB::ID_PATIENTS )->children() = _patientDB.getField( PatientDB::ID_PATIENTS )->children();

	return *this;
}

//------------------------------------------------------------------------------

boost::uint32_t  PatientDB::getPatientSize() const
{
	return this->getField( PatientDB::ID_PATIENTS )->children().size();
}

//------------------------------------------------------------------------------

void PatientDB::addPatient( ::fwData::Patient::sptr _patient )
{
	this->addFieldElement(PatientDB::ID_PATIENTS, _patient);
}

//------------------------------------------------------------------------------

std::pair< PatientDB::PatientIterator, PatientDB::PatientIterator > PatientDB::getPatients()
{
	PatientIterator begin(  getField( PatientDB::ID_PATIENTS )->children().begin() );
	PatientIterator   end(  getField( PatientDB::ID_PATIENTS )->children().end()   );
	return std::make_pair( begin, end );
}

//------------------------------------------------------------------------------

std::pair< PatientDB::PatientConstIterator, PatientDB::PatientConstIterator > PatientDB::getPatients() const
{
	PatientConstIterator begin(  getField( PatientDB::ID_PATIENTS )->children().begin()   );
	PatientConstIterator   end(  getField( PatientDB::ID_PATIENTS )->children().end()   );
	return std::make_pair( begin, end );
}

} // end namespace fwData
