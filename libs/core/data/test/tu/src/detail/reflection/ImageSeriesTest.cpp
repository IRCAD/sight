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

#include "detail/reflection/ImageSeriesTest.hpp"

#include "detail/reflection/DataCampHelper.hpp"

#include <data/Equipment.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Patient.hpp>
#include <data/Reconstruction.hpp>
#include <data/Study.hpp>

#include <fwTest/generator/Image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( data::detail::reflection::ut::ImageSeriesTest );

namespace sight::data
{
namespace detail
{
namespace reflection
{
namespace ut
{

//------------------------------------------------------------------------------

void ImageSeriesTest::setUp()
{
}

//------------------------------------------------------------------------------

void ImageSeriesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageSeriesTest::propertiesTest()
{
    const std::string modality                          = "CT";
    const std::string instance_uid                      = "123456789";
    const std::string number                            = "2";
    const std::string laterality                        = "L";
    const std::string date                              = "20130214";
    const std::string time                              = "143328";
    const std::string performing_physicians_name        = "John Doe";
    const std::string protocolName                      = "elios";
    const std::string description                       = "description";
    const std::string bodyPartExamined                  = "ABDOMEN";
    const std::string patientPosition                   = "unknown";
    const std::string anatomicalOrientationType         = "BIPED";
    const std::string performdedProcedureStepId         = "45896";
    const std::string performedProcedureStepStartDate   = "115548";
    const std::string performedProcedureStepStartTime   = "115535";
    const std::string performedProcedureStepEndDate     = "115945";
    const std::string performedProcedureStepEndTime     = "115944";
    const std::string performedProcedureStepDescription = "step description";
    const std::string performedProcedureComments        = "comments";

    const std::string contrastBolusAgent                   = "unidentified";
    const std::string contrastBolusRoute                   = "route";
    const std::string contrastBolusVolume                  = "63";
    const std::string contrastBolusStartTime               = "115520";
    const std::string contrastBolusStopTime                = "115533";
    const std::string contrastBolusTotalDose               = "42";
    const std::string contrastBolusFlowRate                = "5";
    const std::string contrastBolusFlowDuration            = "40";
    const std::string contrastBolusIngredient              = "IODINE";
    const std::string contrastBolusIngredientConcentration = "12";
    const std::string acquisitionDate                      = "20201120";
    const std::string acquisitionTime                      = "115420";

    const ::DataCampHelper::PropertiesNameType dataProperties = { { "fields" },
                                                                  { "patient" },
                                                                  { "study" },
                                                                  { "equipment" },
                                                                  { "modality" },
                                                                  { "instance_uid" },
                                                                  { "number" },
                                                                  { "laterality" },
                                                                  { "date" },
                                                                  { "time" },
                                                                  { "performing_physicians_name" },
                                                                  { "protocolName" },
                                                                  { "description" },
                                                                  { "body_part_examined" },
                                                                  { "patient_position" },
                                                                  { "anatomical_orientation_type" },
                                                                  { "performded_procedure_step_id" },
                                                                  { "performed_procedure_step_start_date" },
                                                                  { "performed_procedure_step_start_time" },
                                                                  { "performed_procedure_step_end_date" },
                                                                  { "performed_procedure_step_end_time" },
                                                                  { "performed_procedure_step_description" },
                                                                  { "performed_procedure_comments" },

                                                                  { "dicom_reference" },
                                                                  { "image" },
                                                                  { "contrast_bolus_agent" },
                                                                  { "contrast_bolus_route" },
                                                                  { "contrast_bolus_volume" },
                                                                  { "contrast_bolus_start_time" },
                                                                  { "contrast_bolus_stop_time" },
                                                                  { "contrast_bolus_total_dose" },
                                                                  { "contrast_bolus_flow_rate" },
                                                                  { "contrast_bolus_flow_duration" },
                                                                  { "contrast_bolus_ingredient" },
                                                                  { "contrast_bolus_ingredient_concentration" },
                                                                  { "acquisition_date" },
                                                                  { "acquisition_time" }, };

    data::Image::sptr img = data::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img, core::tools::Type::create("uint8"));
    data::DicomValuesType performing_physicians_names;
    performing_physicians_names.push_back(performing_physicians_name);

    data::ImageSeries::sptr obj = data::ImageSeries::New();
    obj->setModality(modality);
    obj->setInstanceUID(instance_uid);
    obj->setNumber(number);
    obj->setLaterality(laterality);
    obj->setDate(date);
    obj->setTime(time);
    obj->setPerformingPhysiciansName(performing_physicians_names);
    obj->setProtocolName(protocolName);
    obj->setDescription(description);
    obj->setBodyPartExamined(bodyPartExamined);
    obj->setPatientPosition(patientPosition);
    obj->setAnatomicalOrientationType(anatomicalOrientationType);
    obj->setPerformedProcedureStepID(performdedProcedureStepId);
    obj->setPerformedProcedureStepStartDate(performedProcedureStepStartDate);
    obj->setPerformedProcedureStepStartTime(performedProcedureStepStartTime);
    obj->setPerformedProcedureStepEndDate(performedProcedureStepEndDate);
    obj->setPerformedProcedureStepEndTime(performedProcedureStepEndTime);
    obj->setPerformedProcedureStepDescription(performedProcedureStepDescription);
    obj->setPerformedProcedureComments(performedProcedureComments);

    obj->setImage(img);
    obj->setContrastAgent(contrastBolusAgent);
    obj->setContrastRoute(contrastBolusRoute);
    obj->setContrastVolume(contrastBolusVolume);
    obj->setContrastStartTime(contrastBolusStartTime);
    obj->setContrastStopTime(contrastBolusStopTime);
    obj->setContrastTotalDose(contrastBolusTotalDose);
    obj->setContrastFlowRate(contrastBolusFlowRate);
    obj->setContrastFlowDuration(contrastBolusFlowDuration);
    obj->setContrastIngredient(contrastBolusIngredient);
    obj->setContrastIngredientConcentration(contrastBolusIngredientConcentration);
    obj->setAcquisitionDate(acquisitionDate);
    obj->setAcquisitionTime(acquisitionTime);

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareObjectPropertyValue(obj, "@patient", obj->getPatient());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@study", obj->getStudy());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@equipment", obj->getEquipment());
    ::DataCampHelper::compareSimplePropertyValue(obj, "@modality", modality);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@instance_uid", instance_uid);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@number", number);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@laterality", laterality);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@date", date);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@time", time);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performing_physicians_name.0", performing_physicians_names[0]);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@protocolName", protocolName);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@description", description);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@body_part_examined", bodyPartExamined);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@patient_position", patientPosition);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@anatomical_orientation_type", anatomicalOrientationType);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performded_procedure_step_id", performdedProcedureStepId);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performed_procedure_step_start_date",
                                                 performedProcedureStepStartDate);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performed_procedure_step_start_time",
                                                 performedProcedureStepStartTime);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performed_procedure_step_end_date",
                                                 performedProcedureStepEndDate);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performed_procedure_step_end_time",
                                                 performedProcedureStepEndTime);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performed_procedure_step_description",
                                                 performedProcedureStepDescription);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performed_procedure_comments", performedProcedureComments);

    ::DataCampHelper::compareObjectPropertyValue(obj, "@image", obj->getImage());
    ::DataCampHelper::compareSimplePropertyValue(obj, "@contrast_bolus_agent", contrastBolusAgent);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@contrast_bolus_route", contrastBolusRoute);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@contrast_bolus_volume", contrastBolusVolume);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@contrast_bolus_start_time", contrastBolusStartTime);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@contrast_bolus_stop_time", contrastBolusStopTime);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@contrast_bolus_total_dose", contrastBolusTotalDose);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@contrast_bolus_flow_rate", contrastBolusFlowRate);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@contrast_bolus_flow_duration", contrastBolusFlowDuration);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@contrast_bolus_ingredient", contrastBolusIngredient);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@contrast_bolus_ingredient_concentration",
                                                 contrastBolusIngredientConcentration);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@acquisition_date", acquisitionDate);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@acquisition_time", acquisitionTime);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace reflection
} //namespace detail
} //namespace sight::data
