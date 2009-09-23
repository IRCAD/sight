/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Acquisition.hpp"


#include "fwData/Study.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Study, ::fwData::Study);
namespace fwData
{
const Object::FieldID Study::ID_ACQUISITIONS = "ID_ACQUISITIONS";

//------------------------------------------------------------------------------

Study::Study() :
	m_sHospital(""),
	m_sModality(""),
	m_sAcquisitionZone(""),
	m_sRISId(""),
	m_sUID(""),
	m_i32DbID(-1)
{
	SLM_WARN("::fwData::Study() : (ToDo) field default value");
	setField(Study::ID_ACQUISITIONS);
}

//------------------------------------------------------------------------------

Study::~Study()
{
	SLM_WARN("::fwData::~Study() : (ToDo) Destruction of acquisition vector");
}

//------------------------------------------------------------------------------

Study &Study::operator=(const Study & _study)
{
	this->m_sHospital = _study.m_sHospital;
	this->m_sModality = _study.m_sModality;
	this->m_sAcquisitionZone = _study.m_sAcquisitionZone;
	this->m_sRISId = _study.m_sRISId;
	this->m_sUID = _study.m_sUID;
	this->m_i32DbID = _study.m_i32DbID;

	this->getField( Study::ID_ACQUISITIONS )->children() = _study.getField( Study::ID_ACQUISITIONS )->children();

	return (*this);
}

//------------------------------------------------------------------------------

boost::uint32_t  Study::getAcquisitionSize() const
{
	return this->getField( Study::ID_ACQUISITIONS )->children().size();
}

//------------------------------------------------------------------------------

void Study::addAcquisition( ::fwData::Acquisition::sptr _acquisition )
{
	this->addFieldElement( Study::ID_ACQUISITIONS, _acquisition );
}

//------------------------------------------------------------------------------

std::pair< Study::AcquisitionIterator, Study::AcquisitionIterator > Study::getAcquisitions()
{
	AcquisitionIterator begin(  getField( Study::ID_ACQUISITIONS )->children().begin() );
	AcquisitionIterator   end(  getField( Study::ID_ACQUISITIONS )->children().end()   );
	return std::make_pair( begin, end );
}

//------------------------------------------------------------------------------

std::pair< Study::AcquisitionConstIterator, Study::AcquisitionConstIterator > Study::getAcquisitions() const
{
	AcquisitionConstIterator begin(  getField( Study::ID_ACQUISITIONS )->children().begin()   );
	AcquisitionConstIterator   end(  getField( Study::ID_ACQUISITIONS )->children().end()   );
	return std::make_pair( begin, end );
}

//------------------------------------------------------------------------------

} // namespace fwData

