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

#include "io/dicom/reader/ie/Series.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"

#include <data/DicomSeries.hpp>
#include <data/ImageSeries.hpp>
#include <data/Series.hpp>
#include <data/types.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <sstream>

namespace sight::io::dicom
{

namespace reader
{

namespace ie
{

//------------------------------------------------------------------------------

Series::Series(
    const data::DicomSeries::csptr& dicomSeries,
    const SPTR(::gdcm::Reader)& reader,
    const io::dicom::container::DicomInstance::sptr& instance,
    const data::Series::sptr& series,
    const core::log::Logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::reader::ie::InformationEntity<data::Series>(dicomSeries, reader, instance, series,
                                                           logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

Series::~Series()
{
}

//------------------------------------------------------------------------------

void Series::readGeneralSeriesModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    const std::string& modality = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0060>(dataset);
    m_object->setModality(modality);

    const std::string& uid = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x000e>(dataset);
    m_object->setInstanceUID(uid);

    const std::string& number = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x0011>(dataset);
    m_object->setNumber(number);

    const std::string& laterality = io::dicom::helper::DicomDataReader::getTagValue<0x0020, 0x0060>(dataset);
    m_object->setLaterality(laterality);

    const std::string& date = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0021>(dataset);
    m_object->setDate(date);

    const std::string& time = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0031>(dataset);
    m_object->setTime(time);

    const std::string& performingPhysicianNamesStr =
        io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x1050>(dataset);
    if(!performingPhysicianNamesStr.empty())
    {
        data::DicomValuesType performingPhysicianNames;
        ::boost::split(performingPhysicianNames, performingPhysicianNamesStr, ::boost::is_any_of("\\"));
        m_object->setPerformingPhysiciansName(performingPhysicianNames);
    }

    const std::string& protocolName = io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1030>(dataset);
    m_object->setProtocolName(protocolName);

    const std::string& description = io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x103e>(dataset);
    m_object->setDescription(description);

    const std::string& bodyPartExamined = io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x0015>(dataset);
    m_object->setBodyPartExamined(bodyPartExamined);

    const std::string& patientPosition = io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x5100>(dataset);
    m_object->setPatientPosition(patientPosition);

    const std::string& anatomicalOrientationType =
        io::dicom::helper::DicomDataReader::getTagValue<0x0010, 0x2210>(dataset);
    m_object->setAnatomicalOrientationType(anatomicalOrientationType);

    const std::string& performdedProcedureStepID =
        io::dicom::helper::DicomDataReader::getTagValue<0x0040, 0x0253>(dataset);
    m_object->setPerformedProcedureStepID(performdedProcedureStepID);

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
    m_object->setPerformedProcedureComments(performedProcedureComments);

    const data::ImageSeries::sptr imageSeries = data::ImageSeries::dynamicCast(m_object);
    if(imageSeries)
    {
        const std::string& imageContrastAgent =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x0010>(dataset);
        imageSeries->setContrastAgent(imageContrastAgent);

        const std::string& imageContrastRoute =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1040>(dataset);
        imageSeries->setContrastRoute(imageContrastRoute);

        const std::string& imageContrastVolume = io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1041>(
            dataset
        );
        imageSeries->setContrastVolume(imageContrastVolume);

        const std::string& imageContrastStartTime =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1042>(dataset);
        imageSeries->setContrastStartTime(imageContrastStartTime);

        const std::string& imageContrastStopTime =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1043>(dataset);
        imageSeries->setContrastStopTime(imageContrastStopTime);

        const std::string& imageContrastTotalDose =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1044>(dataset);
        imageSeries->setContrastTotalDose(imageContrastTotalDose);

        const std::string& imageContrastFlowRate =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1046>(dataset);
        imageSeries->setContrastFlowRate(imageContrastFlowRate);

        const std::string& imageContrastFlowDuration =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1047>(dataset);
        imageSeries->setContrastFlowDuration(imageContrastFlowDuration);

        const std::string& imageContrastIngredient =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1048>(dataset);
        imageSeries->setContrastIngredient(imageContrastIngredient);

        const std::string& imageContrastIngredientConcentration =
            io::dicom::helper::DicomDataReader::getTagValue<0x0018, 0x1049>(dataset);
        imageSeries->setContrastIngredientConcentration(imageContrastIngredientConcentration);

        const std::string& imageAcquisitionDate =
            io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0022>(dataset);
        imageSeries->setAcquisitionDate(imageAcquisitionDate);

        const std::string& imageAcquisitionTime =
            io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x0032>(dataset);
        imageSeries->setAcquisitionTime(imageAcquisitionTime);
    }
}

} // namespace ie

} // namespace reader

} // namespace sight::io::dicom
