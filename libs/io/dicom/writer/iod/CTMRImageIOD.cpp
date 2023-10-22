/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/dicom/writer/iod/CTMRImageIOD.hpp"

#include "io/dicom/helper/FileWriter.hpp"
#include "io/dicom/writer/ie/Equipment.hpp"
#include "io/dicom/writer/ie/FrameOfReference.hpp"
#include "io/dicom/writer/ie/image.hpp"
#include "io/dicom/writer/ie/Patient.hpp"
#include "io/dicom/writer/ie/series.hpp"
#include "io/dicom/writer/ie/Study.hpp"

#include <core/spy_log.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>

#include <gdcmImageWriter.h>

namespace sight::io::dicom::writer::iod
{

//------------------------------------------------------------------------------

CTMRImageIOD::CTMRImageIOD(
    const SPTR(io::dicom::container::DicomInstance)& _instance,
    const std::filesystem::path& _destination_path,
    const core::log::logger::sptr& _logger,
    ProgressCallback _progress,
    CancelRequestedCallback _cancel
) :
    io::dicom::writer::iod::InformationObjectDefinition(_instance, _destination_path, _logger,
                                                        _progress, _cancel)
{
}

//------------------------------------------------------------------------------

CTMRImageIOD::~CTMRImageIOD()
= default;

//------------------------------------------------------------------------------

void CTMRImageIOD::write(const data::series::csptr& _series)
{
    // Retrieve image series
    data::image_series::csptr image_series = std::dynamic_pointer_cast<const data::image_series>(_series);
    SIGHT_ASSERT("Image series should not be null.", image_series);

    // Create writer
    SPTR(gdcm::ImageWriter) writer = std::make_shared<gdcm::ImageWriter>();

    // Create Information Entity helpers
    io::dicom::writer::ie::Patient patient_ie(writer, m_instance, _series);
    io::dicom::writer::ie::Study study_ie(writer, m_instance, _series);
    io::dicom::writer::ie::series series_ie(writer, m_instance, _series);
    io::dicom::writer::ie::FrameOfReference frame_of_reference_ie(writer, m_instance, _series);
    io::dicom::writer::ie::Equipment equipment_ie(writer, m_instance, _series);
    io::dicom::writer::ie::image image_ie(writer, m_instance, image_series);

    // Write Patient Module - PS 3.3 C.7.1.1
    patient_ie.writePatientModule();

    // Write General Study Module - PS 3.3 C.7.2.1
    study_ie.writeGeneralStudyModule();

    // Write Patient Study Module - PS 3.3 C.7.2.2
    study_ie.writePatientStudyModule();

    // Write General Series Module - PS 3.3 C.7.3.1
    series_ie.writeGeneralSeriesModule();

    // Write Frame of Reference Module - PS 3.3 C.7.4.1
    frame_of_reference_ie.writeFrameOfReferenceModule();

    // Write General Equipment Module - PS 3.3 C.7.5.1
    equipment_ie.writeGeneralEquipmentModule();

    // Write General Image Module - PS 3.3 C.7.6.1
    image_ie.writeGeneralImageModule();

    // Write Image Plane Module - PS 3.3 C.7.6.2
    image_ie.writeImagePlaneModule();

    // Write Image Pixel Module - PS 3.3 C.7.6.3
    image_ie.writeImagePixelModule();

    if(m_instance->getSOPClassUID() == gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::CTImageStorage))
    {
        // Write CT Image Module - PS 3.3 C.8.2.1
        image_ie.writeCTImageModule();
    }
    else if(m_instance->getSOPClassUID() == gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::MRImageStorage))
    {
        // Write MR Image Module - PS 3.3 C.8.3.1
        image_ie.writeMRImageModule();
    }

    // Write VOI LUT Module - PS 3.3 C.11.2
    image_ie.writeVOILUTModule();

    // Write SOP Common Module - PS 3.3 C.12.1
    image_ie.writeSOPCommonModule();

    // Copy dataset to avoid writing conflict with GDCM
    const gdcm::DataSet dataset_copy = writer->GetFile().GetDataSet();

    // Compute number of frames
    std::size_t nb_frames = (m_instance->getIsMultiFiles()) ? (image_series->size()[2]) : 1;

    // Write specific tags according to frame number
    for(unsigned int i = 0 ; i < nb_frames ; ++i)
    {
        // Reset dataset
        writer->GetFile().SetDataSet(dataset_copy);

        // Write SOP Common Module specific tags - PS 3.3 C.12.1
        image_ie.writeSOPCommonModuleSpecificTags(i);

        // Write General Image Module specific tags - PS 3.3 C.7.6.1
        image_ie.writeGeneralImageModuleSpecificTags(i);

        // Write Image Plane Module specific tags - PS 3.3 C.7.6.2
        image_ie.writeImagePlaneModuleSpecificTags(i);

        // Write Image Pixel Module specific tags - PS 3.3 C.7.6.3
        image_ie.writeImagePixelModuleSpecificTags(i);

        // Write file
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(5) << i;
        auto frame_path = m_destinationPath;
        frame_path += ss.str();
        io::dicom::helper::FileWriter::write(frame_path, writer);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::iod
