/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// fwTools
#include <fwTools/dateAndTime.hpp>

// gdcmIO
#include "gdcmIO/helper/GdcmHelper.hpp"
#include "gdcmIO/writer/DicomPatientWriter.hpp"

namespace gdcmIO
{

namespace writer
{

//------------------------------------------------------------------------------

DicomPatientWriter::DicomPatientWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomPatientWriter::~DicomPatientWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomPatientWriter::write(::gdcm::DataSet & a_gDs)
{
    SLM_TRACE_FUNC();

    ::fwData::Patient::csptr patient = this->getConcreteObject();
    SLM_ASSERT("fwData::Patient not instanced", patient);

    // Patient's name
    {
        std::string value;
        if (patient->getFirstname().empty())
            value = patient->getName() + "^Unknown";
        else
            value = patient->getName() + "^" + patient->getFirstname();
        helper::GdcmData::setTagValue<0x0010,0x0010>( value, a_gDs);                                    // Type 2
        OSLM_TRACE("Patient's name : "<<value);
    }

    // Patient's ID
    helper::GdcmData::setTagValue<0x0010,0x0020>( patient->getIDDicom(), a_gDs);                        // Type 2
    OSLM_TRACE("Patient's ID : "<<patient->getIDDicom());

    // Patient's birth date
    helper::GdcmData::setTagValue<0x0010,0x0030>( ::fwTools::getDate(patient->getBirthdate()), a_gDs);  // Type 2
    OSLM_TRACE("Patient's birth date : "<< ::fwTools::getDate(patient->getBirthdate()) );

    // Patient's sex
    helper::GdcmData::setTagValue<0x0010,0x0040>( ((patient->getIsMale()==true)?"M":"F"), a_gDs);       // Type 2
    OSLM_TRACE("Patient's sex : "<< ((patient->getIsMale())?"M":"F") );
}

}   // namespace writer

}   // namespace gdcmIO
