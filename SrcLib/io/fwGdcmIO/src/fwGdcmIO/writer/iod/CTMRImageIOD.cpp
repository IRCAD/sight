/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGdcmIO/writer/iod/CTMRImageIOD.hpp"

#include "fwGdcmIO/helper/FileWriter.hpp"
#include "fwGdcmIO/writer/ie/Equipment.hpp"
#include "fwGdcmIO/writer/ie/FrameOfReference.hpp"
#include "fwGdcmIO/writer/ie/Image.hpp"
#include "fwGdcmIO/writer/ie/Patient.hpp"
#include "fwGdcmIO/writer/ie/Series.hpp"
#include "fwGdcmIO/writer/ie/Study.hpp"

#include <fwCore/spyLog.hpp>

#include <fwData/Image.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <boost/make_shared.hpp>

#include <gdcmImageWriter.h>

namespace fwGdcmIO
{
namespace writer
{
namespace iod
{

//------------------------------------------------------------------------------

CTMRImageIOD::CTMRImageIOD(const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                           const std::filesystem::path& destinationPath,
                           const ::fwLog::Logger::sptr& logger,
                           ProgressCallback progress,
                           CancelRequestedCallback cancel) :
    ::fwGdcmIO::writer::iod::InformationObjectDefinition(instance, destinationPath, logger,
                                                         progress, cancel)
{
}

//------------------------------------------------------------------------------

CTMRImageIOD::~CTMRImageIOD()
{
}

//------------------------------------------------------------------------------

void CTMRImageIOD::write(const ::fwMedData::Series::csptr& series)
{
    // Retrieve image series
    ::fwMedData::ImageSeries::csptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("Image series should not be null.", imageSeries);

    // Retrieve image
    ::fwData::Image::sptr image = imageSeries->getImage();

    // Create writer
    SPTR(::gdcm::ImageWriter) writer = std::make_shared< ::gdcm::ImageWriter >();

    // Create Information Entity helpers
    ::fwGdcmIO::writer::ie::Patient patientIE(writer, m_instance, series->getPatient());
    ::fwGdcmIO::writer::ie::Study studyIE(writer, m_instance, series->getStudy());
    ::fwGdcmIO::writer::ie::Series seriesIE(writer, m_instance, series);
    ::fwGdcmIO::writer::ie::FrameOfReference frameOfReferenceIE(writer, m_instance, series);
    ::fwGdcmIO::writer::ie::Equipment equipmentIE(writer, m_instance, series->getEquipment());
    ::fwGdcmIO::writer::ie::Image imageIE(writer, m_instance, imageSeries->getImage());

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

    if(m_instance->getSOPClassUID() == ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::CTImageStorage))
    {
        // Write CT Image Module - PS 3.3 C.8.2.1
        imageIE.writeCTImageModule();
    }
    else if(m_instance->getSOPClassUID() == ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::MRImageStorage))
    {
        // Write MR Image Module - PS 3.3 C.8.3.1
        imageIE.writeMRImageModule();
    }

    // Write VOI LUT Module - PS 3.3 C.11.2
    imageIE.writeVOILUTModule();

    // Write SOP Common Module - PS 3.3 C.12.1
    imageIE.writeSOPCommonModule();

    // Copy dataset to avoid writing conflict with GDCM
    const ::gdcm::DataSet datasetCopy = writer->GetFile().GetDataSet();

    // Compute number of frames
    std::size_t nbFrames = (m_instance->getIsMultiFiles()) ? (image->getSize()[2]) : 1;

    // Write specific tags according to frame number
    for(unsigned int i = 0; i < nbFrames; ++i)
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
        ::fwGdcmIO::helper::FileWriter::write(framePath, writer);
    }
}
//------------------------------------------------------------------------------

} // namespace iod
} // namespace writer
} // namespace fwGdcmIO
