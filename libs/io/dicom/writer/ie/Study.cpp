/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "io/dicom/writer/ie/Study.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

#include <data/Study.hpp>

namespace sight::io::dicom
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

Study::Study(const SPTR(::gdcm::Writer)& writer,
             const SPTR(io::dicom::container::DicomInstance)& instance,
             const data::Study::csptr& study,
             const core::log::Logger::sptr& logger,
             ProgressCallback progress,
             CancelRequestedCallback cancel) :
    io::dicom::writer::ie::InformationEntity< data::Study >(writer, instance, study,
                                                            logger, progress, cancel)
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

    io::dicom::helper::DicomDataWriter::setTagValue<0x0020, 0x000d>(m_object->getInstanceUID(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0020, 0x0010>(m_object->getStudyID(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0020>(m_object->getDate(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0030>(m_object->getTime(), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0090>(m_object->getReferringPhysicianName(), dataset);

    //TODO: >getConsultingPhysicianName() contains only 1 value, gdcm wants a list (VM : 1-N).
    ::gdcm::String<> consultingPhysisicianName[] = {m_object->getConsultingPhysicianName()};

    io::dicom::helper::DicomDataWriter::setTagValues< ::gdcm::String<>, 0x0008, 0x009C>(consultingPhysisicianName, 1,
                                                                                        dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x1030>(m_object->getDescription(), dataset);

    // Study 's accession number - Type 2
    io::dicom::helper::DicomDataWriter::setTagValue< 0x0008, 0x0050 >("", dataset);
}

//------------------------------------------------------------------------------

void Study::writePatientStudyModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    io::dicom::helper::DicomDataWriter::setTagValue<0x0010, 0x1010>(m_object->getPatientAge(), dataset);
    // Following tags are represented as Decimal String, in GDCM DS = Double.
    // To avoid exception of stod, we need to test first if string is empty.
    io::dicom::helper::DicomDataWriter::setTagValue<double, 0x0010, 0x1020>(
        m_object->getPatientSize().empty() ? 0. : std::stod(m_object->getPatientSize()), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<double, 0x0010, 0x1030>(
        m_object->getPatientWeight().empty() ? 0. : std::stod(m_object->getPatientWeight()), dataset);
    io::dicom::helper::DicomDataWriter::setTagValue<double, 0x0010, 0x1022>(
        m_object->getPatientBodyMassIndex().empty() ? 0. : std::stod(m_object->getPatientBodyMassIndex()), dataset);
}

//------------------------------------------------------------------------------

}// namespace ie
} // namespace writer
} // namespace sight::io::dicom
