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

#include "io/dicom/reader/ie/Series.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"

#include <data/DicomSeries.hpp>
#include <data/ImageSeries.hpp>
#include <data/Series.hpp>
#include <data/types.hpp>

#include <sstream>

namespace sight::io::dicom::reader::ie
{

//------------------------------------------------------------------------------

Series::Series(
    const data::DicomSeries::csptr& dicomSeries,
    const SPTR(gdcm::Reader)& reader,
    const io::dicom::container::DicomInstance::sptr& instance,
    const data::Series::sptr& series,
    const core::log::logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::reader::ie::InformationEntity<data::Series>(dicomSeries, reader, instance, series,
                                                           logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

Series::~Series()
= default;

//------------------------------------------------------------------------------

void Series::readGeneralSeriesModule()
{
    // Retrieve dataset
    const gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    const std::string& modality = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0060>(dataset);
    m_object->setModality(modality);

    const std::string& uid = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x000e>(dataset);
    m_object->setSeriesInstanceUID(uid);

    const std::string& number = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x0011>(dataset);
    if(!number.empty())
    {
        m_object->setSeriesNumber(std::stoi(number));
    }

    const std::string& laterality = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x0060>(dataset);
    m_object->setLaterality(laterality);

    const std::string& date = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0021>(dataset);
    m_object->setSeriesDate(date);

    const std::string& time = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0031>(dataset);
    m_object->setSeriesTime(time);

    const std::string& performingPhysicianName =
        io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x1050>(dataset);
    m_object->setPerformingPhysicianName(performingPhysicianName);

    const std::string& protocolName = io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1030>(dataset);
    m_object->setProtocolName(protocolName);

    const std::string& description = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x103e>(dataset);
    m_object->setSeriesDescription(description);

    const std::string& bodyPartExamined = io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x0015>(dataset);
    m_object->setBodyPartExamined(bodyPartExamined);

    const std::string& patientPosition = io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x5100>(dataset);
    m_object->setPatientPosition(patientPosition);

    const std::string& anatomicalOrientationType =
        io::dicom::helper::DicomDataReader::getTagValue<0x0010, 0x2210>(dataset);
    m_object->setAnatomicalOrientationType(anatomicalOrientationType);

    const std::string& performedProcedureStepID =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0253>(dataset);
    m_object->setPerformedProcedureStepID(performedProcedureStepID);

    const std::string& performedProcedureStepStartDate =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0244>(dataset);
    m_object->setPerformedProcedureStepStartDate(performedProcedureStepStartDate);

    const std::string& performedProcedureStepStartTime =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0245>(dataset);
    m_object->setPerformedProcedureStepStartTime(performedProcedureStepStartTime);

    const std::string& performedProcedureStepEndDate =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0250>(dataset);
    m_object->setPerformedProcedureStepEndDate(performedProcedureStepEndDate);

    const std::string& performedProcedureStepEndTime =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0251>(dataset);
    m_object->setPerformedProcedureStepEndTime(performedProcedureStepEndTime);

    const std::string& performedProcedureStepDescription =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0254>(dataset);
    m_object->setPerformedProcedureStepDescription(performedProcedureStepDescription);

    const std::string& performedProcedureComments =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0280>(dataset);
    m_object->setCommentsOnThePerformedProcedureStep(performedProcedureComments);

    const data::ImageSeries::sptr imageSeries = std::dynamic_pointer_cast<data::ImageSeries>(m_object);
    if(imageSeries)
    {
        const std::string& imageContrastBolusAgent =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x0010>(dataset);
        imageSeries->setContrastBolusAgent(imageContrastBolusAgent);

        const std::string& imageContrastBolusRoute =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1040>(dataset);
        imageSeries->setContrastBolusRoute(imageContrastBolusRoute);

        const std::string& imageContrastBolusVolume = io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1041>(
            dataset
        );

        if(!imageContrastBolusVolume.empty())
        {
            imageSeries->setContrastBolusVolume(std::stod(imageContrastBolusVolume));
        }

        const std::string& imageContrastBolusStartTime =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1042>(dataset);
        imageSeries->setContrastBolusStartTime(imageContrastBolusStartTime);

        const std::string& imageContrastBolusStopTime =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1043>(dataset);
        imageSeries->setContrastBolusStopTime(imageContrastBolusStopTime);

        const std::string& imageContrastBolusTotalDose =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1044>(dataset);

        if(!imageContrastBolusTotalDose.empty())
        {
            imageSeries->setContrastBolusTotalDose(std::stod(imageContrastBolusTotalDose));
        }

        const std::string& imageContrastFlowRate =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1046>(dataset);
        imageSeries->setContrastFlowRate(imageContrastFlowRate);

        const std::string& imageContrastFlowDuration =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1047>(dataset);
        imageSeries->setContrastFlowDuration(imageContrastFlowDuration);

        const std::string& imageContrastBolusIngredient =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1048>(dataset);
        imageSeries->setContrastBolusIngredient(imageContrastBolusIngredient);

        const std::string& imageContrastBolusIngredientConcentration =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1049>(dataset);

        if(!imageContrastBolusIngredientConcentration.empty())
        {
            imageSeries->setContrastBolusIngredientConcentration(std::stod(imageContrastBolusIngredientConcentration));
        }

        const std::string& imageAcquisitionDate =
            io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0022>(dataset);
        imageSeries->setAcquisitionDate(imageAcquisitionDate);

        const std::string& imageAcquisitionTime =
            io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0032>(dataset);
        imageSeries->setAcquisitionTime(imageAcquisitionTime);
    }
}

} // namespace sight::io::dicom::reader::ie
