/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/dicom/reader/iod/SpatialFiducialsIOD.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"
#include "io/dicom/reader/ie/SpatialFiducials.hpp"

#include <data/fieldHelper/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/PointList.hpp>
#include <data/Vector.hpp>

namespace sight::io::dicom
{

namespace reader
{

namespace iod
{

//------------------------------------------------------------------------------

SpatialFiducialsIOD::SpatialFiducialsIOD(
    const data::DicomSeries::csptr& dicomSeries,
    const io::dicom::container::DicomInstance::sptr& instance,
    const core::log::Logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::reader::iod::InformationObjectDefinition(dicomSeries, instance, logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

SpatialFiducialsIOD::~SpatialFiducialsIOD()
{
}

//------------------------------------------------------------------------------

void SpatialFiducialsIOD::read(data::Series::sptr series)
{
    // Retrieve images
    data::ImageSeries::sptr imageSeries = data::ImageSeries::dynamicCast(series);
    SIGHT_ASSERT("ImageSeries should not be null.", imageSeries);
    data::Image::sptr image = imageSeries->getImage();
    SIGHT_ASSERT("::sight::data::Image not instanced", image);

    // Create GDCM Reader
    SPTR(::gdcm::Reader) reader = std::shared_ptr< ::gdcm::Reader>(new ::gdcm::Reader);

    // Read the first file
    data::DicomSeries::DicomContainerType dicomContainer = m_dicomSeries->getDicomContainer();

    if(dicomContainer.size() > 1)
    {
        m_logger->warning(
            "More than one Spatial Fiducials item have been found in the series. "
            "Only the first one will be read."
        );
    }

    const core::memory::BufferObject::sptr bufferObj         = dicomContainer.begin()->second;
    const core::memory::BufferManager::StreamInfo streamInfo = bufferObj->getStreamInfo();
    SPTR(std::istream) is = streamInfo.stream;
    reader->SetStream(*is);

    const bool success = reader->Read();
    SIGHT_THROW_EXCEPTION_IF(
        io::dicom::exception::Failed(
            "Unable to read the DICOM instance \""
            + bufferObj->getStreamInfo().fsFile.string()
            + "\" using the GDCM Reader."
        ),
        !success
    );

    // Create Information Entity helpers
    io::dicom::reader::ie::SpatialFiducials spatialFiducialsIE(
        m_dicomSeries, reader, m_instance, imageSeries->getImage(),
        m_logger, m_progressCallback, m_cancelRequestedCallback);

    // Retrieve dataset
    const ::gdcm::DataSet& datasetRoot = reader->GetFile().GetDataSet();

    // Retrieve Fiducial Set Sequence
    const ::gdcm::DataElement& fiducialSetSequenceDataElement =
        datasetRoot.GetDataElement(::gdcm::Tag(0x0070, 0x031C));
    const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> fiducialSetSequence =
        fiducialSetSequenceDataElement.GetValueAsSQ();

    for(unsigned int i = 1 ; i <= fiducialSetSequence->GetNumberOfItems() ; ++i)
    {
        ::gdcm::Item sequenceSetItem              = fiducialSetSequence->GetItem(i);
        const ::gdcm::DataSet& sequenceSetDataset = sequenceSetItem.GetNestedDataSet();

        const ::gdcm::DataElement& fiducialSequenceDataElement =
            sequenceSetDataset.GetDataElement(::gdcm::Tag(0x0070, 0x031E));
        const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> fiducialSequence =
            fiducialSequenceDataElement.GetValueAsSQ();

        for(unsigned int j = 1 ; j <= fiducialSequence->GetNumberOfItems() ; ++j)
        {
            ::gdcm::Item fiducialItem              = fiducialSequence->GetItem(j);
            const ::gdcm::DataSet& fiducialDataset = fiducialItem.GetNestedDataSet();
            const std::string shapeType            =
                io::dicom::helper::DicomDataReader::getTagValue<0x0070, 0x0306>(fiducialDataset);

            if(shapeType == "POINT")
            {
                spatialFiducialsIE.readLandmark(fiducialDataset);
            }
            else
            {
                m_logger->warning("Fiducial shape type not supported: \"" + shapeType + "\"");
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace iod

} // namespace reader

} // namespace sight::io::dicom
