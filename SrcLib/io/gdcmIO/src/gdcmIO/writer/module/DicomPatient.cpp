/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gdcmIO/helper/GdcmHelper.hpp"
#include "gdcmIO/writer/module/DicomPatient.hpp"

namespace gdcmIO
{
namespace writer
{
namespace module
{

//------------------------------------------------------------------------------

DicomPatient::DicomPatient()
{}

//------------------------------------------------------------------------------

DicomPatient::~DicomPatient()
{}

//------------------------------------------------------------------------------

void DicomPatient::write(::gdcm::DataSet & a_gDs)
{
    ::fwMedData::Patient::csptr patient = this->getConcreteObject();
    SLM_ASSERT("fwMedData::Patient not instanced", patient);

    // Patient's name
    helper::GdcmData::setTagValue<0x0010,0x0010>( patient->getName(), a_gDs); // Type 2

    // Patient's ID
    helper::GdcmData::setTagValue<0x0010,0x0020>( patient->getPatientId(), a_gDs); // Type 2

    // Patient's birth date
    helper::GdcmData::setTagValue<0x0010,0x0030>( patient->getBirthdate(), a_gDs);  // Type 2

    // Patient's sex
    helper::GdcmData::setTagValue<0x0010,0x0040>( patient->getSex(), a_gDs); // Type 2
}

} // namespace module
} // namespace writer
} // namespace gdcmIO
