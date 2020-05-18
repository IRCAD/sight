/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwGdcmIO/writer/ie/Document.hpp"

#include "fwGdcmIO/helper/DicomDataWriter.hxx"
#include "fwGdcmIO/helper/StructuredReport.hpp"   // For graphviz dump
#include "fwGdcmIO/writer/tid/MeasurementReport.hpp"

#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/types.hpp>

#include <fwTools/dateAndTime.hpp>

#include <boost/algorithm/string/split.hpp>

#include <gdcmUIDGenerator.h>

#include <sstream>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

Document::Document(const SPTR(::gdcm::Writer)& writer,
                   const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                   const ::fwData::Image::csptr& image,
                   bool use3DSR,
                   const ::fwLog::Logger::sptr& logger,
                   ProgressCallback progress,
                   CancelRequestedCallback cancel) :
    ::fwGdcmIO::writer::ie::InformationEntity< ::fwData::Image >(writer, instance, image, logger, progress, cancel),
    m_use3DSR(use3DSR)
{
}

//------------------------------------------------------------------------------

Document::~Document()
{
}

//------------------------------------------------------------------------------

void Document::writeSRDocumentGeneralModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    ::boost::posix_time::ptime ptime = boost::posix_time::second_clock::local_time();

    // Instance Number - Type 1
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< int, 0x0020, 0x0013 >(0, dataset);

    // Content Date - Type 1 - FIXME: Keep series date ?
    const std::string date = ::fwTools::getDate(ptime);
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0023 >(date, dataset);

    // Content Time - Type 1 - FIXME: Keep series time ?
    const std::string time = ::fwTools::getTime(ptime);
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0033 >(time, dataset);

    // Performed Procedure Code Sequence (0040,A372) // Type 2 (FIXME: CID 7000 ?)
    ::fwGdcmIO::helper::DicomDataWriter::createAndSetSequenceTagValue< 0x0040, 0xa372 >(dataset);

    // Completion flag - Type 1 - (FIXME: Set to COMPLETE ?)
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0040, 0xa491 >("PARTIAL", dataset);

    // Verification Flag - Type 1 - (FIXME: Set to VERIFIED ?)
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0040, 0xa493 >("UNVERIFIED", dataset);

    // Pertinent Other Evidence Sequence - Type 1C
    this->writePertinentOtherEvidenceSequence();

}

//------------------------------------------------------------------------------

void Document::writePertinentOtherEvidenceSequence()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Pertinent Other Evidence Sequence - Type 1C
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence = new ::gdcm::SequenceOfItems();
    sequence->SetLengthToUndefined();
    ::fwGdcmIO::helper::DicomDataWriter::createAndSetSequenceTagValue< 0x0040, 0xa385 >(dataset);

    // Add new study to sequence
    ::gdcm::Item studyItem;
    studyItem.SetVLToUndefined();
    ::gdcm::DataSet& studyItemDataset = studyItem.GetNestedDataSet();
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0020, 0x000D >(
        m_instance->getStudyInstanceUID(), studyItemDataset);

    // Referenced Series Sequence - Type 1
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > seriesSequence = new ::gdcm::SequenceOfItems();
    seriesSequence->SetLengthToUndefined();
    ::fwGdcmIO::helper::DicomDataWriter::setSequenceTagValue< 0x0008, 0x1115 >(seriesSequence, studyItemDataset);

    // Add new series to sequence
    ::gdcm::Item seriesItem;
    seriesItem.SetVLToUndefined();
    ::gdcm::DataSet& seriesItemDataset = seriesItem.GetNestedDataSet();
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0020, 0x000E >(
        m_instance->getSeriesInstanceUID(), seriesItemDataset);

    // Referenced SOP Sequence - Type 1
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sopSequence = new ::gdcm::SequenceOfItems();
    sopSequence->SetLengthToUndefined();
    ::fwGdcmIO::helper::DicomDataWriter::setSequenceTagValue< 0x0008, 0x1199 >(sopSequence, seriesItemDataset);

    for(const std::string& instanceUID : m_instance->getSOPInstanceUIDContainer())
    {
        // Add new instance to sequence
        ::gdcm::Item instanceItem;
        instanceItem.SetVLToUndefined();
        ::gdcm::DataSet& instanceItemDataset = instanceItem.GetNestedDataSet();

        // Referenced SOP Class UID - Type 1
        ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x1150 >(
            m_instance->getSOPClassUID(), instanceItemDataset);

        // Referenced SOP Instance UID - Type 1
        ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x1155 >(instanceUID, instanceItemDataset);

        sopSequence->AddItem(instanceItem);
    }

    seriesSequence->AddItem(seriesItem);
    sequence->AddItem(studyItem);
}

//------------------------------------------------------------------------------

void Document::writeSRDocumentContentModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Create SR Root node
    ::fwGdcmIO::writer::tid::MeasurementReport report(m_writer, m_instance, m_object);
    SPTR(::fwGdcmIO::container::sr::DicomSRNode) root = report.createRootNode(m_use3DSR);
    root->write(dataset);

    // Dump SR
    //::fwGdcmIO::helper::StructuredReport::dumpSR(root);
}

//------------------------------------------------------------------------------

void Document::writeSOPCommonModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // SOP Class UID
    std::string sopClassUID;
    if(!m_use3DSR)
    {
        sopClassUID = ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::ComprehensiveSR);
    }
    else
    {
        // FIXME Remove hard coded string
        // sopClassUID = ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::Comprehensive3DSR);
        sopClassUID = "1.2.840.10008.5.1.4.1.1.88.34";
    }
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0016 >(sopClassUID, dataset);

    // SOP Instance UID
    ::gdcm::UIDGenerator uidGenerator;
    std::string sopInstanceUID = uidGenerator.Generate();
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0008, 0x0018 >(sopInstanceUID, dataset);
}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO
