/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomData.hpp"
#include "fwGdcmIO/writer/ie/Study.hpp"

#include <fwMedData/Study.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

Study::Study(SPTR(::gdcm::Writer)writer,
             SPTR(::fwGdcmIO::container::DicomInstance)instance,
             ::fwMedData::Study::sptr study) :
    ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::Study >(writer, instance, study)
{
}

//------------------------------------------------------------------------------

Study::~Study()
{
}

//------------------------------------------------------------------------------

void Study::writeGeneralStudyModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Study's date - Type 2
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0008, 0x0020 >(m_object->getDate(), dataset);

    // Study's time - Type 2
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0008, 0x0030 >(m_object->getTime(), dataset);

    // Study 's accession number - Type 2
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0008, 0x0050 >("", dataset);

    // Study's description - Type 3
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0008, 0x1030 >(m_object->getDescription(), dataset);

    // Study's UID - Type 1
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0020, 0x000d >(m_object->getInstanceUID(), dataset);

    // Study's ID - Type 2
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0020, 0x0010 >("Unknown", dataset);

    // Study's referring physician name
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0008, 0x0090 >(m_object->getReferringPhysicianName(), dataset);
}

//------------------------------------------------------------------------------

void Study::writePatientStudyModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // PatientAge
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0010, 0x1010 >(m_object->getPatientAge(), dataset);
}

//------------------------------------------------------------------------------

}// namespace ie
} // namespace writer
} // namespace fwGdcmIO
