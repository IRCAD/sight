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
    const SPTR(io::dicom::container::DicomInstance)& instance,
    const std::filesystem::path& destinationPath,
    const core::log::logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::writer::iod::InformationObjectDefinition(instance, destinationPath, logger,
                                                        progress, cancel)
{
}

//------------------------------------------------------------------------------

CTMRImageIOD::~CTMRImageIOD()
= default;

//------------------------------------------------------------------------------

void CTMRImageIOD::write(const data::series::csptr& series)
{
    // Retrieve image series
    data::image_series::csptr imageSeries = std::dynamic_pointer_cast<const data::image_series>(series);
    SIGHT_ASSERT("Image series should not be null.", imageSeries);

    // Create writer
    SPTR(gdcm::ImageWriter) writer = std::make_shared<gdcm::ImageWriter>();

    // Create Information Entity helpers
    io::dicom::writer::ie::Patient patientIE(writer, m_instance, series);
    io::dicom::writer::ie::Study studyIE(writer, m_instance, series);
    io::dicom::writer::ie::series seriesIE(writer, m_instance, series);
    io::dicom::writer::ie::FrameOfReference frameOfReferenceIE(writer, m_instance, series);
    io::dicom::writer::ie::Equipment equipmentIE(writer, m_instance, series);
    io::dicom::writer::ie::image imageIE(writer, m_instance, imageSeries);

    // Write Patient Module - PS 3.3 C.7.1.1
    patientIE.writePatientModule();

    // Write General Study Module - PS 3.3 C.7.2.1
    studyIE.writeGeneralStudyModule();

    // Write Patient Study Module - PS 3.3 C.7.2.2
    studyIE.writePatientStudyModule();

    // Write General Series Module - PS 3.3 C.7.3.1
    seriesIE.writeGeneralSeriesModule();

    // Write Frame of Reference Module - PS 3.3 C.7.4.1
    frameOfReferenceIE.writeFrameOfReferenceModule();

    // Write General Equipment Module - PS 3.3 C.7.5.1
    equipmentIE.writeGeneralEquipmentModule();

    // Write General Image Module - PS 3.3 C.7.6.1
    imageIE.writeGeneralImageModule();

    // Write Image Plane Module - PS 3.3 C.7.6.2
    imageIE.writeImagePlaneModule();

    // Write Image Pixel Module - PS 3.3 C.7.6.3
    imageIE.writeImagePixelModule();

    if(m_instance->getSOPClassUID() == gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::CTImageStorage))
    {
        // Write CT Image Module - PS 3.3 C.8.2.1
        imageIE.writeCTImageModule();
    }
    else if(m_instance->getSOPClassUID() == gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::MRImageStorage))
    {
        // Write MR Image Module - PS 3.3 C.8.3.1
        imageIE.writeMRImageModule();
    }

    // Write VOI LUT Module - PS 3.3 C.11.2
    imageIE.writeVOILUTModule();

    // Write SOP Common Module - PS 3.3 C.12.1
    imageIE.writeSOPCommonModule();

    // Copy dataset to avoid writing conflict with GDCM
    const gdcm::DataSet datasetCopy = writer->GetFile().GetDataSet();

    // Compute number of frames
    std::size_t nbFrames = (m_instance->getIsMultiFiles()) ? (imageSeries->size()[2]) : 1;

    // Write specific tags according to frame number
    for(unsigned int i = 0 ; i < nbFrames ; ++i)
    {
        // Reset dataset
        writer->GetFile().SetDataSet(datasetCopy);

        // Write SOP Common Module specific tags - PS 3.3 C.12.1
        imageIE.writeSOPCommonModuleSpecificTags(i);

        // Write General Image Module specific tags - PS 3.3 C.7.6.1
        imageIE.writeGeneralImageModuleSpecificTags(i);

        // Write Image Plane Module specific tags - PS 3.3 C.7.6.2
        imageIE.writeImagePlaneModuleSpecificTags(i);

        // Write Image Pixel Module specific tags - PS 3.3 C.7.6.3
        imageIE.writeImagePixelModuleSpecificTags(i);

        // Write file
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(5) << i;
        auto framePath = m_destinationPath;
        framePath += ss.str();
        io::dicom::helper::FileWriter::write(framePath, writer);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::iod
