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

#include "detail/reflection/DicomSeriesCampTest.hpp"

#include "detail/reflection/DataCampHelper.hpp"

#include <data/Array.hpp>
#include <data/DicomSeries.hpp>
#include <data/Equipment.hpp>
#include <data/Patient.hpp>
#include <data/Study.hpp>

#include <core/tools/System.hpp>
#include <core/tools/Type.hpp>

#include <filesystem>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( sight::data::detail::reflection::ut::DicomSeriesCampTest );

namespace sight::data
{
namespace detail
{
namespace reflection
{
namespace ut
{

//------------------------------------------------------------------------------

void DicomSeriesCampTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DicomSeriesCampTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DicomSeriesCampTest::propertiesTest()
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

    const std::string filename = "dicom.dcm";

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

                                                                  { "number_of_instances" },
                                                                  { "dicom_container" },
                                                                  { "sop_class_uids" },
                                                                  { "computed_tag_values" },
                                                                  { "first_instance_number" }};

    core::memory::BufferObject::sptr bufferObj = core::memory::BufferObject::New();

    const std::filesystem::path path = core::tools::System::getTemporaryFolder() / "DicomSeriesCampTest";
    std::filesystem::create_directories(path);
    const std::filesystem::path dicomFile = path / filename;
    {
        std::ofstream file;
        file.open(dicomFile.string().c_str(), std::ofstream::out);
        file << "42";
        file.close();
    }

    data::DicomValuesType performing_physicians_names;
    performing_physicians_names.push_back(performing_physicians_name);

    data::DicomSeries::sptr obj = data::DicomSeries::New();
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

    obj->setNumberOfInstances(100);
    obj->addDicomPath(42, dicomFile);
    obj->addBinary(1664, bufferObj);
    obj->addSOPClassUID("1.2.840.10008.5.1.4.1.1.2");
    obj->addComputedTagValue("(0020,0100)", "1664");
    obj->setFirstInstanceNumber(1);

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

    // Number of instances
    ::std::stringstream ss;
    ss << obj->getNumberOfInstances();
    ::DataCampHelper::compareSimplePropertyValue(obj, "@number_of_instances", ss.str());

    // Dicom binaries
    // TODO: BufferObject is not supported yet, so we cannot make unit tests on this part...
    //::DataCampHelper::compareObjectPropertyValue(obj, "@dicom_container.1664", obj->getDicomContainer().at(1664));

    // SOP class UID
    ::DataCampHelper::compareSimplePropertyValue(obj, "@sop_class_uids.0", "1.2.840.10008.5.1.4.1.1.2");
    // SOP class UID
    ::DataCampHelper::compareSimplePropertyValue(obj, "@sop_class_uids.0", "1.2.840.10008.5.1.4.1.1.2");

    // Computed tag values
    ::DataCampHelper::compareSimplePropertyValue(obj, "@computed_tag_values.(0020,0100)", "1664");

    // First instance number
    ::DataCampHelper::compareSimplePropertyValue(obj, "@first_instance_number", "1");
    // Computed tag values
    ::DataCampHelper::compareSimplePropertyValue(obj, "@computed_tag_values.(0020,0100)", "1664");

    // First instance number
    ::DataCampHelper::compareSimplePropertyValue(obj, "@first_instance_number", "1");
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace reflection
} //namespace detail
} //namespace sight::data
