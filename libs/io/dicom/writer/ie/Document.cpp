/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/dicom/writer/ie/Document.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"
#include "io/dicom/helper/StructuredReport.hpp" // For graphviz dump
#include "io/dicom/writer/tid/MeasurementReport.hpp"

#include <core/tools/date_and_time.hpp>

#include <data/point_list.hpp>
#include <data/series.hpp>
#include <data/string.hpp>
#include <data/types.hpp>
#include <data/vector.hpp>

#include <boost/algorithm/string/split.hpp>

#include <gdcmUIDGenerator.h>

#include <sstream>

namespace sight::io::dicom::writer::ie
{

//------------------------------------------------------------------------------

Document::Document(
    const SPTR(gdcm::Writer)& _writer,
    const SPTR(io::dicom::container::DicomInstance)& _instance,
    const data::image::csptr& _image,
    bool _use3_dsr,
    const core::log::logger::sptr& _logger,
    ProgressCallback _progress,
    CancelRequestedCallback _cancel
) :
    io::dicom::writer::ie::InformationEntity<data::image>(_writer, _instance, _image, _logger, _progress, _cancel),
    m_use3DSR(_use3_dsr)
{
}

//------------------------------------------------------------------------------

Document::~Document()
= default;

//------------------------------------------------------------------------------

void Document::writeSRDocumentGeneralModule()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    boost::posix_time::ptime ptime = boost::posix_time::second_clock::local_time();

    // Instance Number - Type 1
    io::dicom::helper::DicomDataWriter::setTagValue<int, 0x0020, 0x0013>(0, dataset);

    // Content Date - Type 1 - FIXME: Keep series date ?
    const std::string date = core::tools::get_date(ptime);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0023>(date, dataset);

    // Content Time - Type 1 - FIXME: Keep series time ?
    const std::string time = core::tools::get_time(ptime);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0033>(time, dataset);

    // Performed Procedure Code Sequence (0040,A372) // Type 2 (FIXME: CID 7000 ?)
    io::dicom::helper::DicomDataWriter::createAndSetSequenceTagValue<0x0040, 0xa372>(dataset);

    // Completion flag - Type 1 - (FIXME: Set to COMPLETE ?)
    io::dicom::helper::DicomDataWriter::setTagValue<0x0040, 0xa491>("PARTIAL", dataset);

    // Verification Flag - Type 1 - (FIXME: Set to VERIFIED ?)
    io::dicom::helper::DicomDataWriter::setTagValue<0x0040, 0xa493>("UNVERIFIED", dataset);

    // Pertinent Other Evidence Sequence - Type 1C
    this->writePertinentOtherEvidenceSequence();
}

//------------------------------------------------------------------------------

void Document::writePertinentOtherEvidenceSequence()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Pertinent Other Evidence Sequence - Type 1C
    gdcm::SmartPointer<gdcm::SequenceOfItems> sequence = new gdcm::SequenceOfItems();
    sequence->SetLengthToUndefined();
    io::dicom::helper::DicomDataWriter::createAndSetSequenceTagValue<0x0040, 0xa385>(dataset);

    // Add new study to sequence
    gdcm::Item study_item;
    study_item.SetVLToUndefined();
    gdcm::DataSet& study_item_dataset = study_item.GetNestedDataSet();
    io::dicom::helper::DicomDataWriter::setTagValue<0x0020, 0x000D>(
        m_instance->getStudyInstanceUID(),
        study_item_dataset
    );

    // Referenced Series Sequence - Type 1
    gdcm::SmartPointer<gdcm::SequenceOfItems> series_sequence = new gdcm::SequenceOfItems();
    series_sequence->SetLengthToUndefined();
    io::dicom::helper::DicomDataWriter::setSequenceTagValue<0x0008, 0x1115>(series_sequence, study_item_dataset);

    // Add new series to sequence
    gdcm::Item series_item;
    series_item.SetVLToUndefined();
    gdcm::DataSet& series_item_dataset = series_item.GetNestedDataSet();
    io::dicom::helper::DicomDataWriter::setTagValue<0x0020, 0x000E>(
        m_instance->getSeriesInstanceUID(),
        series_item_dataset
    );

    // Referenced SOP Sequence - Type 1
    gdcm::SmartPointer<gdcm::SequenceOfItems> sop_sequence = new gdcm::SequenceOfItems();
    sop_sequence->SetLengthToUndefined();
    io::dicom::helper::DicomDataWriter::setSequenceTagValue<0x0008, 0x1199>(sop_sequence, series_item_dataset);

    for(const std::string& instance_uid : m_instance->getSOPInstanceUIDContainer())
    {
        // Add new instance to sequence
        gdcm::Item instance_item;
        instance_item.SetVLToUndefined();
        gdcm::DataSet& instance_item_dataset = instance_item.GetNestedDataSet();

        // Referenced SOP Class UID - Type 1
        io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x1150>(
            m_instance->getSOPClassUID(),
            instance_item_dataset
        );

        // Referenced SOP Instance UID - Type 1
        io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x1155>(instance_uid, instance_item_dataset);

        sop_sequence->AddItem(instance_item);
    }

    series_sequence->AddItem(series_item);
    sequence->AddItem(study_item);
}

//------------------------------------------------------------------------------

void Document::writeSRDocumentContentModule()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Create SR Root node
    io::dicom::writer::tid::MeasurementReport report(m_writer, m_instance, m_object);
    SPTR(io::dicom::container::sr::DicomSRNode) root = report.createRootNode(m_use3DSR);
    root->write(dataset);

    // Dump SR
    //::io::dicom::helper::StructuredReport::dumpSR(root);
}

//------------------------------------------------------------------------------

void Document::writeSOPCommonModule()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // SOP Class UID
    std::string sop_class_uid;
    if(!m_use3DSR)
    {
        sop_class_uid = gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::ComprehensiveSR);
    }
    else
    {
        // FIXME Remove hard coded string
        // sopClassUID = gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::Comprehensive3DSR);
        sop_class_uid = "1.2.840.10008.5.1.4.1.1.88.34";
    }

    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0016>(sop_class_uid, dataset);

    // SOP Instance UID
    gdcm::UIDGenerator uid_generator;
    std::string sop_instance_uid = uid_generator.Generate();
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0018>(sop_instance_uid, dataset);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::ie
