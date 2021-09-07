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

#include "io/dicom/writer/ie/SpatialFiducials.hpp"

#include "io/dicom/helper/DicomDataTools.hpp"
#include "io/dicom/helper/DicomDataWriter.hxx"

#include <core/tools/dateAndTime.hpp>

#include <data/fieldHelper/Image.hpp>
#include <data/Image.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

#include <gdcmUIDGenerator.h>

namespace sight::io::dicom
{

namespace writer
{

namespace ie
{

//------------------------------------------------------------------------------

SpatialFiducials::SpatialFiducials(
    const SPTR(::gdcm::Writer)& writer,
    const SPTR(io::dicom::container::DicomInstance)& instance,
    const data::Image::csptr& image,
    const core::log::Logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::writer::ie::InformationEntity<data::Image>(writer, instance, image,
                                                          logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

SpatialFiducials::~SpatialFiducials()
{
}

//------------------------------------------------------------------------------

void SpatialFiducials::writeSpatialFiducialsModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    ::boost::posix_time::ptime ptime = boost::posix_time::second_clock::local_time();

    // Content Date - Type 1 - FIXME: Keep series date ?
    std::string date = core::tools::getDate(ptime);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0023>(date, dataset);

    // Content Time - Type 1 - FIXME: Keep series time ?
    std::string time = core::tools::getTime(ptime);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0033>(time, dataset);

    // Instance Number - Type 1
    io::dicom::helper::DicomDataWriter::setTagValue<unsigned int, 0x0020, 0x0013>(0, dataset);

    // Content Label - Type 1
    io::dicom::helper::DicomDataWriter::setTagValue<0x0070, 0x0080>("SF", dataset);

    // Content Description - Type 2
    io::dicom::helper::DicomDataWriter::setTagValue<0x0070, 0x0081>("Spatial Fiducials", dataset);

    // Content Creator's Name - Type 2 - TODO: Set fiducial creator name ?
    io::dicom::helper::DicomDataWriter::setTagValue<0x0070, 0x0084>("Unknown^Unknown", dataset);

    // Fiducial Set Sequence - Type 1
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> fiducialSetSequence = new ::gdcm::SequenceOfItems();
    io::dicom::helper::DicomDataWriter::setSequenceTagValue<0x0070, 0x031C>(fiducialSetSequence, dataset);

    // Create Fiducial Sequence Item
    ::gdcm::Item fiducialSetItem;
    fiducialSetItem.SetVLToUndefined();
    ::gdcm::DataSet& fiducialSetItemDataset = fiducialSetItem.GetNestedDataSet();

    // Referenced Image Sequence - Type 1C
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> referencedImageSequence = new ::gdcm::SequenceOfItems();
    io::dicom::helper::DicomDataWriter::setSequenceTagValue<0x0008, 0x1140>(
        referencedImageSequence,
        fiducialSetItemDataset
    );

    // Add all referenced image
    for(unsigned int index = 0 ; index < m_instance->getSOPInstanceUIDContainer().size() ; ++index)
    {
        ::gdcm::Item referencedImageItem;
        referencedImageItem.SetVLToUndefined();
        ::gdcm::DataSet& referencedImageItemDataset = referencedImageItem.GetNestedDataSet();

        // Referenced Frame Number - Type 1C
        int frameNumber = index + 1;
        io::dicom::helper::DicomDataWriter::setTagValues<int, 0x0008, 0x1160>(
            &frameNumber,
            1,
            referencedImageItemDataset
        );

        // Referenced SOP Class UID - Type 1
        io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x1150>(
            m_instance->getSOPClassUID(),
            referencedImageItemDataset
        );

        // Referenced SOP Instance UID - Type 1
        io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x1155>(
            m_instance->getSOPInstanceUIDContainer()[index],
            referencedImageItemDataset
        );

        // Add referenced image to sequence
        referencedImageSequence->AddItem(referencedImageItem);
    }

    // Fiducial Sequence - Type 1
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> fiducialSequence = new ::gdcm::SequenceOfItems();
    io::dicom::helper::DicomDataWriter::setSequenceTagValue<0x0070, 0x031E>(
        fiducialSequence,
        fiducialSetItemDataset
    );

    // Write landmarks
    this->writeLandmarks(fiducialSequence);

    // Add Fiducial Set to sequence
    fiducialSetSequence->AddItem(fiducialSetItem);
}

//------------------------------------------------------------------------------

void SpatialFiducials::writeLandmarks(::gdcm::SmartPointer< ::gdcm::SequenceOfItems> sequence)
{
    data::PointList::sptr pointList =
        m_object->getField<data::PointList>(data::fieldHelper::Image::m_imageLandmarksId);
    if(pointList)
    {
        unsigned int index = 0;
        for(const data::Point::sptr& point : pointList->getPoints())
        {
            ::gdcm::Item fiducialItem;
            fiducialItem.SetVLToUndefined();
            ::gdcm::DataSet& fiducialItemDataset = fiducialItem.GetNestedDataSet();

            // Fiducial Identifier - Type 1
            std::stringstream ssIdentifier;
            ssIdentifier << "landmark-" << index;
            io::dicom::helper::DicomDataWriter::setTagValue<0x0070, 0x0310>(ssIdentifier.str(), fiducialItemDataset);

            // Fiducial Description - Type 3
            std::string label =
                point->getField<data::String>(data::fieldHelper::Image::m_labelId)->value();
            io::dicom::helper::DicomDataWriter::setTagValue<0x0070, 0x030F>(label, fiducialItemDataset);

            // Shape Type - Type 1
            io::dicom::helper::DicomDataWriter::setTagValue<0x0070, 0x0306>("POINT", fiducialItemDataset);

            // Graphic Coordinates Data Sequence - Type 1C
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> graphicCoodinatesDataSequence =
                new ::gdcm::SequenceOfItems();
            io::dicom::helper::DicomDataWriter::setSequenceTagValue<0x0070, 0x0318>(
                graphicCoodinatesDataSequence,
                fiducialItemDataset
            );

            ::gdcm::Item graphicDataItem;
            graphicDataItem.SetVLToUndefined();
            ::gdcm::DataSet& graphicDataItemDataset = graphicDataItem.GetNestedDataSet();

            // Graphic Data - Type 1
            float coordinates[2];
            coordinates[0] = static_cast<float>(point->getCoord()[0]);
            coordinates[1] = static_cast<float>(point->getCoord()[1]);
            io::dicom::helper::DicomDataWriter::setTagValues<float, 0x0070, 0x0022>(
                coordinates,
                2,
                graphicDataItemDataset
            );

            // Referenced Image Sequence - Type 1
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> referencedImageSequence = new ::gdcm::SequenceOfItems();
            io::dicom::helper::DicomDataWriter::setSequenceTagValue<0x0008, 0x1140>(
                referencedImageSequence,
                graphicDataItemDataset
            );

            int frameNumber =
                static_cast<int>(io::dicom::helper::DicomDataTools::convertPointToFrameNumber(
                                     m_object,
                                     point
                ));
            this->addReferencedImage(frameNumber, referencedImageSequence);

            // Add graphic data to sequence
            graphicCoodinatesDataSequence->AddItem(graphicDataItem);

            // Add Fiducial to sequence
            sequence->AddItem(fiducialItem);

            // Increase index
            ++index;
        }
    }
}

//------------------------------------------------------------------------------

void SpatialFiducials::writeCommonInstanceReferenceModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Referenced Series Sequence - Type 1C
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> referencedSeriesSequence = new ::gdcm::SequenceOfItems();
    io::dicom::helper::DicomDataWriter::setSequenceTagValue<0x0008, 0x1115>(referencedSeriesSequence, dataset);

    // Create Referenced Series
    ::gdcm::Item referencedSeriesItem;
    referencedSeriesItem.SetVLToUndefined();
    ::gdcm::DataSet& referencedSeriesItemDataset = referencedSeriesItem.GetNestedDataSet();

    // Series Instance UID - Type 1
    io::dicom::helper::DicomDataWriter::setTagValue<0x0020, 0x000E>(
        m_instance->getSeriesInstanceUID(),
        referencedSeriesItemDataset
    );

    // Referenced Instance Sequence - Type 1
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> referencedInstanceSequence = new ::gdcm::SequenceOfItems();
    io::dicom::helper::DicomDataWriter::setSequenceTagValue<0x0008, 0x114A>(
        referencedInstanceSequence,
        referencedSeriesItemDataset
    );

    // Add all referenced image
    for(unsigned int index = 0 ; index < m_instance->getSOPInstanceUIDContainer().size() ; ++index)
    {
        ::gdcm::Item referencedInstanceItem;
        referencedInstanceItem.SetVLToUndefined();
        ::gdcm::DataSet& referencedInstanceItemDataset = referencedInstanceItem.GetNestedDataSet();

        // Referenced SOP Class UID - Type 1
        io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x1150>(
            m_instance->getSOPClassUID(),
            referencedInstanceItemDataset
        );

        // Referenced SOP Instance UID - Type 1
        io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x1155>(
            m_instance->getSOPInstanceUIDContainer()[index],
            referencedInstanceItemDataset
        );

        // Add referenced image to sequence
        referencedInstanceSequence->AddItem(referencedInstanceItem);
    }

    referencedSeriesSequence->AddItem(referencedSeriesItem);

    // Studies Containing Other Referenced Instances Sequence - Type 1C
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> studiesSequence = new ::gdcm::SequenceOfItems();
    io::dicom::helper::DicomDataWriter::setSequenceTagValue<0x0008, 0x1200>(studiesSequence, dataset);

    ::gdcm::Item studiesItem;
    studiesItem.SetVLToUndefined();
    ::gdcm::DataSet& studiesItemDataset = studiesItem.GetNestedDataSet();

    // Study Instance UID - Type 1
    io::dicom::helper::DicomDataWriter::setTagValue<0x0020, 0x000D>(
        m_instance->getStudyInstanceUID(),
        studiesItemDataset
    );

    // Referenced Series Sequence - Type 1
    io::dicom::helper::DicomDataWriter::setSequenceTagValue<0x0008, 0x1115>(
        referencedSeriesSequence,
        studiesItemDataset
    );

    studiesSequence->AddItem(studiesItem);
}

//------------------------------------------------------------------------------

void SpatialFiducials::writeSOPCommonModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // SOP Class UID
    std::string sopClassUID = ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::SpacialFiducialsStorage);
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0016>(sopClassUID, dataset);

    // SOP Instance UID
    ::gdcm::UIDGenerator uidGenerator;
    std::string sopInstanceUID = uidGenerator.Generate();
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x0018>(sopInstanceUID, dataset);
}

//------------------------------------------------------------------------------

void SpatialFiducials::addReferencedImage(
    int frameNumber,
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> referencedImageSequence
)
{
    ::gdcm::Item referencedImageItem;
    referencedImageItem.SetVLToUndefined();
    ::gdcm::DataSet& referencedImageItemDataset = referencedImageItem.GetNestedDataSet();

    // Referenced Frame Number - Type 1C
    io::dicom::helper::DicomDataWriter::setTagValues<int, 0x0008, 0x1160>(
        &frameNumber,
        1,
        referencedImageItemDataset
    );

    // Referenced SOP Class UID - Type 1
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x1150>(
        m_instance->getSOPClassUID(),
        referencedImageItemDataset
    );

    // Referenced SOP Instance UID - Type 1
    io::dicom::helper::DicomDataWriter::setTagValue<0x0008, 0x1155>(
        m_instance->getSOPInstanceUIDContainer()[frameNumber - 1],
        referencedImageItemDataset
    );

    // Add referenced image to sequence
    referencedImageSequence->AddItem(referencedImageItem);
}

//------------------------------------------------------------------------------

} // namespace ie

} // namespace writer

} // namespace sight::io::dicom
