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

// cspell:ignore NOLINT

#include "io/dimse/helper/series.hpp"

#include "io/dimse/exceptions/TagMissing.hpp"

#include <core/spy_log.hpp>

#include <data/dicom_series.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>

namespace sight::io::dimse::helper
{

//------------------------------------------------------------------------------

void series::release_responses(OFList<QRResponse*> _responses)
{
    while(!_responses.empty())
    {
        delete _responses.front();
        _responses.pop_front();
    }
}

// ----------------------------------------------------------------------------

series::DicomSeriesContainer series::to_fw_med_data(OFList<QRResponse*> _responses)
{
    DicomSeriesContainer series_container;

    // Every while loop run will get all image for a specific study
    for(auto* response : _responses)
    {
        // Be sure we are not in the last response which does not have a dataset
        if(response->m_dataset != nullptr)
        {
            OFString data;

            // Create series and get informations.
            sight::data::dicom_series::sptr series = std::make_shared<sight::data::dicom_series>();

            // series
            if(response->m_dataset->findAndGetOFStringArray(DCM_Modality, data).good() && !data.empty())
            {
                series->set_modality(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, data).good() && !data.empty())
            {
                series->set_series_instance_uid(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_SeriesNumber, data).good() && !data.empty())
            {
                series->set_series_number(std::stoi(data.c_str()));
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_Laterality, data).good() && !data.empty())
            {
                series->set_laterality(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_SeriesDate, data).good() && !data.empty())
            {
                series->set_series_date(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_SeriesTime, data).good() && !data.empty())
            {
                series->set_series_time(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_PerformingPhysicianName, data).good() && !data.empty())
            {
                series->set_performing_physician_name(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_ProtocolName, data).good() && !data.empty())
            {
                series->set_protocol_name(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_SeriesDescription, data).good() && !data.empty())
            {
                series->set_series_description(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_BodyPartExamined, data).good() && !data.empty())
            {
                series->set_body_part_examined(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_PatientPosition, data).good() && !data.empty())
            {
                series->set_patient_position(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(
                   DCM_AnatomicalOrientationType,
                   data
            ).good() && !data.empty())
            {
                series->set_anatomical_orientation_type(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(
                   DCM_PerformedProcedureStepID,
                   data
            ).good() && !data.empty())
            {
                series->set_performed_procedure_step_id(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(
                   DCM_PerformedProcedureStepStartDate,
                   data
            ).good() && !data.empty())
            {
                series->set_performed_procedure_step_start_date(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(
                   DCM_PerformedProcedureStepStartTime,
                   data
            ).good() && !data.empty())
            {
                series->set_performed_procedure_step_start_time(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(
                   DCM_PerformedProcedureStepEndDate,
                   data
            ).good() && !data.empty())
            {
                series->set_performed_procedure_step_end_date(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(
                   DCM_PerformedProcedureStepEndTime,
                   data
            ).good() && !data.empty())
            {
                series->set_performed_procedure_step_end_time(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(
                   DCM_PerformedProcedureStepDescription,
                   data
            ).good() && !data.empty())
            {
                series->set_performed_procedure_step_description(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(
                   DCM_CommentsOnThePerformedProcedureStep,
                   data
            ).good() && !data.empty())
            {
                series->set_comments_on_the_performed_procedure_step(data.c_str());
            }

            // Study
            if(response->m_dataset->findAndGetOFStringArray(DCM_StudyInstanceUID, data).good() && !data.empty())
            {
                series->set_study_instance_uid(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_StudyID, data).good() && !data.empty())
            {
                series->set_study_id(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_StudyDate, data).good() && !data.empty())
            {
                series->set_study_date(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_StudyTime, data).good() && !data.empty())
            {
                series->set_study_time(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_ReferringPhysicianName, data).good() && !data.empty())
            {
                series->set_referring_physician_name(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_StudyDescription, data).good() && !data.empty())
            {
                series->set_study_description(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_PatientAge, data).good() && !data.empty())
            {
                series->set_patient_age(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_PatientSize, data).good() && !data.empty())
            {
                series->set_patient_size(std::stod(data.c_str()));
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_PatientWeight, data).good() && !data.empty())
            {
                series->set_patient_weight(std::stod(data.c_str()));
            }

            // Patient
            if(response->m_dataset->findAndGetOFStringArray(DCM_PatientName, data).good() && !data.empty())
            {
                series->set_patient_name(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_PatientID, data).good() && !data.empty())
            {
                series->set_patient_id(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_PatientBirthDate, data).good() && !data.empty())
            {
                series->set_patient_birth_date(data.c_str());
            }

            if(response->m_dataset->findAndGetOFStringArray(DCM_PatientSex, data).good() && !data.empty())
            {
                series->set_patient_sex(data.c_str());
            }

            // Equipment
            if(response->m_dataset->findAndGetOFStringArray(DCM_InstitutionName, data).good() && !data.empty())
            {
                series->set_institution_name(data.c_str());
            }

            // Number of instances
            // NOLINTNEXTLINE(google-runtime-int)
            long nb_instances = 0;
            if(response->m_dataset->findAndGetLongInt(DCM_NumberOfSeriesRelatedInstances, nb_instances).good())
            {
                series->set_number_of_instances(std::size_t(nb_instances));
            }

            // Add series to container
            series_container.push_back(series);
        }
    }

    return series_container;
}

// ----------------------------------------------------------------------------

series::InstanceUIDContainer series::to_series_instance_uid_container(DicomSeriesContainer _series)
{
    InstanceUIDContainer result;

    for(const auto& s : _series)
    {
        result.emplace_back(s->get_series_instance_uid());
    }

    return result;
}

// ----------------------------------------------------------------------------

series::InstanceUIDContainer series::to_series_instance_uid_container(OFList<QRResponse*> _responses)
{
    InstanceUIDContainer instance_uid_container;

    OFListIterator(QRResponse*) it;

    // Every while loop run will get all image for a specific study
    for(auto* response : _responses)
    {
        // Be sure we are not in the last response which does not have a dataset
        if(response->m_dataset != nullptr)
        {
            OFString series_instance_uid;
            // Only try to get study if we actually have study instance uid, otherwise skip it
            if(const auto result =
                   (*it)->m_dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, series_instance_uid);
               result.good())
            {
                instance_uid_container.emplace_back(series_instance_uid.c_str());
            }
            else
            {
                const std::string msg = "There is no \"SeriersInstanceUID\" tag in the selected series :"
                                        + std::string(result.text());
                throw io::dimse::exceptions::tag_missing(msg);
            }
        }
    }

    return instance_uid_container;
}

} // namespace sight::io::dimse::helper
