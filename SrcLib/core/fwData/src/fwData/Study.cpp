/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"

#include "fwData/Acquisition.hpp"


#include "fwData/Study.hpp"

fwDataRegisterMacro( ::fwData::Study );
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

void Study::shallowCopy( Study::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
    this->m_sHospital = _source->m_sHospital;
    this->m_sModality = _source->m_sModality;
    this->m_sAcquisitionZone = _source->m_sAcquisitionZone;
    this->m_sRISId = _source->m_sRISId;
    this->m_sUID = _source->m_sUID;
    this->m_i32DbID = _source->m_i32DbID;
}

//------------------------------------------------------------------------------

void Study::deepCopy( Study::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );
    this->m_sHospital = _source->m_sHospital;
    this->m_sModality = _source->m_sModality;
    this->m_sAcquisitionZone = _source->m_sAcquisitionZone;
    this->m_sRISId = _source->m_sRISId;
    this->m_sUID = _source->m_sUID;
    this->m_i32DbID = _source->m_i32DbID;
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

