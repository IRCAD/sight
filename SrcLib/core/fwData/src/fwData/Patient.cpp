/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Study.hpp"
#include "fwData/Patient.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Patient, ::fwData::Patient );
namespace fwData
{
const Object::FieldID Patient::ID_STUDIES = "ID_STUDIES";
//------------------------------------------------------------------------------

Patient::Patient () :
	m_sName (""),
	m_sFirstname (""),
	m_sIDDicom (""),
	m_sBirthdate ( ::boost::date_time::min_date_time ),
	m_bIsMale (true),
	m_i32DbID (-1)
{
	SLM_WARN("::fwData::Patient() : (ToDo) field default value");
	setField( Patient::ID_STUDIES );
}

//------------------------------------------------------------------------------

Patient::~Patient ()
{
	SLM_WARN("::fwData::~Patient() : (ToDo) Destruction of study vector");
}

//------------------------------------------------------------------------------

Patient &Patient::operator=(const Patient &_patient)
{
	this->m_sName 		= _patient.m_sName;
	this->m_sFirstname	= _patient.m_sFirstname;
	this->m_sIDDicom 	= _patient.m_sIDDicom;
	this->m_sBirthdate	= _patient.m_sBirthdate;
	this->m_bIsMale 	= _patient.m_bIsMale;
	this->m_i32DbID 	= _patient.m_i32DbID;

	this->getField( Patient::ID_STUDIES )->children() = _patient.getField( Patient::ID_STUDIES )->children();

	return (*this);
}

//------------------------------------------------------------------------------

boost::uint32_t  Patient::getStudySize() const
{
	return this->getField( Patient::ID_STUDIES )->children().size();
}

//------------------------------------------------------------------------------

void Patient::addStudy( ::fwData::Study::sptr _study )
{
	this->addFieldElement(Patient::ID_STUDIES, _study);
}

//------------------------------------------------------------------------------

std::pair< Patient::StudyIterator, Patient::StudyIterator > Patient::getStudies()
{
	StudyIterator begin(  getField( Patient::ID_STUDIES )->children().begin() );
	StudyIterator   end(  getField( Patient::ID_STUDIES )->children().end()   );
	return std::make_pair( begin, end );
}

//------------------------------------------------------------------------------

std::pair< Patient::StudyConstIterator, Patient::StudyConstIterator > Patient::getStudies() const
{
	StudyConstIterator begin(  getField( Patient::ID_STUDIES )->children().begin()   );
	StudyConstIterator   end(  getField( Patient::ID_STUDIES )->children().end()   );
	return std::make_pair( begin, end );
}

//------------------------------------------------------------------------------

} // end namespace fwData
