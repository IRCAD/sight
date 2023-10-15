/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/image_series.hpp>
#include <data/point_list.hpp>
#include <data/vector.hpp>

#include <memory>
#include <utility>

namespace sight::io::dicom::reader::iod
{

//------------------------------------------------------------------------------

SpatialFiducialsIOD::SpatialFiducialsIOD(
    const data::dicom_series::csptr& dicomSeries,
    const io::dicom::container::DicomInstance::sptr& instance,
    const core::log::logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::reader::iod::InformationObjectDefinition(dicomSeries, instance, logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

SpatialFiducialsIOD::~SpatialFiducialsIOD()
= default;

//------------------------------------------------------------------------------

void SpatialFiducialsIOD::read(data::series::sptr series)
{
    // Retrieve images
    data::image_series::sptr imageSeries = std::dynamic_pointer_cast<data::image_series>(series);
    SIGHT_ASSERT("ImageSeries should not be null.", imageSeries);

    // Create GDCM Reader
    SPTR(gdcm::Reader) reader = std::make_shared<gdcm::Reader>();

    // Read the first file
    data::dicom_series::DicomContainerType dicomContainer = m_dicomSeries->getDicomContainer();

    if(dicomContainer.size() > 1)
    {
        m_logger->warning(
            "More than one Spatial Fiducials item have been found in the series. "
            "Only the first one will be read."
        );
    }

    const core::memory::buffer_object::sptr bufferObj          = dicomContainer.begin()->second;
    const core::memory::buffer_manager::stream_info streamInfo = bufferObj->get_stream_info();
    SPTR(std::istream) is = streamInfo.stream;
    reader->SetStream(*is);

    const bool success = reader->Read();
    SIGHT_THROW_EXCEPTION_IF(
        io::dicom::exception::Failed(
            "Unable to read the DICOM instance \""
            + bufferObj->get_stream_info().fs_file.string()
            + "\" using the GDCM Reader."
        ),
        !success
    );

    // Create Information Entity helpers
    io::dicom::reader::ie::SpatialFiducials spatialFiducialsIE(
        m_dicomSeries, reader, m_instance, imageSeries,
        m_logger, m_progressCallback, m_cancelRequestedCallback);

    // Retrieve dataset
    const gdcm::DataSet& datasetRoot = reader->GetFile().GetDataSet();

    // Retrieve Fiducial Set Sequence
    const gdcm::DataElement& fiducialSetSequenceDataElement =
        datasetRoot.GetDataElement(gdcm::Tag(0x0070, 0x031C));
    const gdcm::SmartPointer<gdcm::SequenceOfItems> fiducialSetSequence =
        fiducialSetSequenceDataElement.GetValueAsSQ();

    for(unsigned int i = 1 ; i <= fiducialSetSequence->GetNumberOfItems() ; ++i)
    {
        gdcm::Item sequenceSetItem              = fiducialSetSequence->GetItem(i);
        const gdcm::DataSet& sequenceSetDataset = sequenceSetItem.GetNestedDataSet();

        const gdcm::DataElement& fiducialSequenceDataElement =
            sequenceSetDataset.GetDataElement(gdcm::Tag(0x0070, 0x031E));
        const gdcm::SmartPointer<gdcm::SequenceOfItems> fiducialSequence =
            fiducialSequenceDataElement.GetValueAsSQ();

        for(unsigned int j = 1 ; j <= fiducialSequence->GetNumberOfItems() ; ++j)
        {
            gdcm::Item fiducialItem              = fiducialSequence->GetItem(j);
            const gdcm::DataSet& fiducialDataset = fiducialItem.GetNestedDataSet();
            const std::string shapeType          =
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

} // namespace sight::io::dicom::reader::iod
