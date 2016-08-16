/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomData.hpp"
#include "fwGdcmIO/writer/ie/Patient.hpp"

#include <fwMedData/Patient.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

Patient::Patient(SPTR(::gdcm::Writer)writer,
                 SPTR(::fwGdcmIO::container::DicomInstance)instance,
                 ::fwMedData::Patient::sptr patient) :
    ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::Patient >(writer, instance, patient)
{
}

//------------------------------------------------------------------------------

Patient::~Patient()
{
}

//------------------------------------------------------------------------------

void Patient::writePatientModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Patient's name - Type 2
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0010, 0x0010 >(m_object->getName(), dataset);

    // Patient's ID - Type 2
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0010, 0x0020 >(m_object->getPatientId(), dataset);

    // Patient's birth date - Type 2
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0010, 0x0030 >(m_object->getBirthdate(), dataset);

    // Patient's sex - Type 2
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0010, 0x0040 >(m_object->getSex(), dataset);
}

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO
