/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gdcmIO/writer/module/DicomStudy.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"

namespace gdcmIO
{
namespace writer
{
namespace module
{

//------------------------------------------------------------------------------

DicomStudy::DicomStudy()
{}

//------------------------------------------------------------------------------

DicomStudy::~DicomStudy()
{}

//------------------------------------------------------------------------------

void DicomStudy::write(::gdcm::DataSet & a_gDs)
{
    ::fwMedData::Study::csptr study = this->getConcreteObject();
    SLM_ASSERT("fwMedData::Study not instanced", study);

    // Study's date
    helper::GdcmData::setTagValue<0x0008,0x0020>(study->getDate(), a_gDs);   // Type 2

    // Study's time
    helper::GdcmData::setTagValue<0x0008,0x0030>(study->getTime(), a_gDs);   // Type 2

    // Study 's accession number
    helper::GdcmData::setTagValue<0x0008,0x0050>("", a_gDs);                // Type 2

    // Study's description
    helper::GdcmData::setTagValue<0x0008,0x1030>(study->getDescription(), a_gDs);   // Type 3

    // Study's UID
    helper::GdcmData::setTagValue<0x0020,0x000d>(study->getInstanceUID(), a_gDs);   // Type 1

    // Study's ID
    helper::GdcmData::setTagValue<0x0020,0x0010>("", a_gDs);   // Type 2

    // Study's referring physician name
    helper::GdcmData::setTagValue<0x0008,0x0090>(study->getReferringPhysicianName(), a_gDs);

    // PatientAge
    helper::GdcmData::setTagValue<0x0010,0x1010>(study->getPatientAge(), a_gDs);
}

//------------------------------------------------------------------------------

} // namespace module
} // namespace writer
} // namespace gdcmIO
