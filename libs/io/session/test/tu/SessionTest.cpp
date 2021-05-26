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

#include "SessionTest.hpp"

#include <core/crypto/AES256.hpp>
#include <core/data/ActivitySeries.hpp>
#include <core/data/Boolean.hpp>
#include <core/data/Composite.hpp>
#include <core/data/Equipment.hpp>
#include <core/data/Float.hpp>
#include <core/data/Integer.hpp>
#include <core/data/iterator/MeshIterators.hpp>
#include <core/data/iterator/MeshIterators.hxx>
#include <core/data/Patient.hpp>
#include <core/data/Series.hpp>
#include <core/data/String.hpp>
#include <core/data/Study.hpp>
#include <core/tools/System.hpp>
#include <core/tools/UUID.hpp>

#include <geometry/data/Mesh.hpp>

#include <io/session/detail/SessionDeserializer.hpp>
#include <io/session/detail/SessionSerializer.hpp>
#include <io/session/SessionReader.hpp>
#include <io/session/SessionWriter.hpp>
#include <io/zip/exception/Read.hpp>
#include <io/zip/exception/Write.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Mesh.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::io::session::ut::SessionTest);

namespace sight::io::session
{

namespace ut
{

// For UUID::generateUUID();
using core::tools::UUID;

//------------------------------------------------------------------------------

void SessionTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SessionTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SessionTest::booleanTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "booleanTest.zip";

    const bool right = true;
    const bool wrong = false;
    const std::string fieldName(UUID::generateUUID());

    // Test serialization
    {
        // Create the data::Boolean
        auto rightData = data::Boolean::New(right);

        // Add a field
        rightData->setField(fieldName, data::Boolean::New(wrong));

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session
        sessionWriter->setObject(rightData);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& rightData = data::Boolean::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(rightData);
        CPPUNIT_ASSERT_EQUAL(right, rightData->getValue());

        // Test field
        const auto& fieldData = data::Boolean::dynamicCast(rightData->getField(fieldName));
        CPPUNIT_ASSERT(fieldData);
        CPPUNIT_ASSERT_EQUAL(wrong, fieldData->getValue());
    }
}

//------------------------------------------------------------------------------

void SessionTest::integerTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "integerTest.zip";

    const std::int64_t answer = 42;
    const std::int64_t satan  = 666;
    const std::string fieldName(UUID::generateUUID());

    // Test serialization
    {
        // Create the data::Integer
        auto answerData = data::Integer::New(answer);

        // Add a field
        answerData->setField(fieldName, data::Integer::New(satan));

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session
        sessionWriter->setObject(answerData);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& answerData = data::Integer::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(answerData);
        CPPUNIT_ASSERT_EQUAL(answer, answerData->getValue());

        // Test field
        const auto& fieldData = data::Integer::dynamicCast(answerData->getField(fieldName));
        CPPUNIT_ASSERT(fieldData);
        CPPUNIT_ASSERT_EQUAL(satan, fieldData->getValue());
    }
}

//------------------------------------------------------------------------------

void SessionTest::floatTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "floatTest.zip";

    const float pi     = 3.141592653589793F;
    const float planck = 6.62607015E-10F;
    const std::string fieldName(UUID::generateUUID());

    // Test serialization
    {
        // Create the data::Float
        auto piData = data::Float::New(pi);

        // Add a field
        piData->setField(fieldName, data::Float::New(planck));

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session
        sessionWriter->setObject(piData);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& piData = data::Float::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(piData);
        CPPUNIT_ASSERT_EQUAL(pi, piData->getValue());

        // Test field
        const auto& fieldData = data::Float::dynamicCast(piData->getField(fieldName));
        CPPUNIT_ASSERT(fieldData);
        CPPUNIT_ASSERT_EQUAL(planck, fieldData->getValue());
    }
}

//------------------------------------------------------------------------------

void SessionTest::stringTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "stringTest.zip";

    // Create the data::String
    const std::string verlaine(UUID::generateUUID());
    const core::crypto::secure_string verlaine_suite(UUID::generateUUID());
    const core::crypto::secure_string password(UUID::generateUUID());
    const std::string fieldName(UUID::generateUUID());

    // Test serialization
    {
        const auto& verlaineData = data::String::New(verlaine);

        // Add a String field with a pure binary string (encrypted text)
        const auto& secret = core::crypto::encrypt(verlaine_suite, password);
        verlaineData->setField(fieldName, data::String::New(std::string(secret)));

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(verlaineData);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& verlaineData = data::String::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(verlaineData);
        CPPUNIT_ASSERT_EQUAL(verlaine, verlaineData->getValue());

        // Test field
        const auto& fieldData = data::String::dynamicCast(verlaineData->getField(fieldName));
        CPPUNIT_ASSERT(fieldData);
        const auto& decrypted = core::crypto::decrypt(core::crypto::secure_string(fieldData->getValue()), password);
        CPPUNIT_ASSERT_EQUAL(verlaine_suite, decrypted);
    }
}

//------------------------------------------------------------------------------

void SessionTest::circularTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "circularTest.zip";

    const std::string inception(UUID::generateUUID());

    // Test serialization
    {
        // Introduce a circular reference
        auto inceptionData = data::String::New(inception);
        inceptionData->setField(inception, inceptionData);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(inceptionData);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& inceptionData = data::String::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(inceptionData);
        CPPUNIT_ASSERT_EQUAL(inception, inceptionData->getValue());

        // Test field
        const auto& fieldData = data::String::dynamicCast(inceptionData->getField(inception));
        CPPUNIT_ASSERT(fieldData);
        CPPUNIT_ASSERT_EQUAL(inception, fieldData->getValue());
    }
}

//------------------------------------------------------------------------------

void SessionTest::compositeTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "compositeTest.zip";

    const std::string stringValue(UUID::generateUUID());
    const std::int64_t integerValue = 42;
    const bool booleanValue         = true;
    const float floatValue          = 3.141592653589793F;

    // Test serialization
    {
        // Create composite
        auto composite = data::Composite::New();
        (*composite)[data::String::classname()]    = data::String::New(stringValue);
        (*composite)[data::Integer::classname()]   = data::Integer::New(integerValue);
        (*composite)[data::Boolean::classname()]   = data::Boolean::New(booleanValue);
        (*composite)[data::Float::classname()]     = data::Float::New(floatValue);
        (*composite)[data::Composite::classname()] = composite;

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(composite);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& composite = data::Composite::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(composite);

        const auto& stringData = data::String::dynamicCast((*composite)[data::String::classname()]);
        CPPUNIT_ASSERT(stringData);
        CPPUNIT_ASSERT_EQUAL(stringValue, stringData->getValue());

        const auto& integerData = data::Integer::dynamicCast((*composite)[data::Integer::classname()]);
        CPPUNIT_ASSERT(integerData);
        CPPUNIT_ASSERT_EQUAL(integerValue, integerData->getValue());

        const auto& booleanData = data::Boolean::dynamicCast((*composite)[data::Boolean::classname()]);
        CPPUNIT_ASSERT(booleanData);
        CPPUNIT_ASSERT_EQUAL(booleanValue, booleanData->getValue());

        const auto& floatData = data::Float::dynamicCast((*composite)[data::Float::classname()]);
        CPPUNIT_ASSERT(floatData);
        CPPUNIT_ASSERT_EQUAL(floatValue, floatData->getValue());

        const auto& compositeData = data::Composite::dynamicCast((*composite)[data::Composite::classname()]);
        CPPUNIT_ASSERT(compositeData);
        const auto& compositeStringData = data::String::dynamicCast((*compositeData)[data::String::classname()]);
        CPPUNIT_ASSERT(compositeStringData);
        CPPUNIT_ASSERT_EQUAL(stringValue, compositeStringData->getValue());
    }
}

//------------------------------------------------------------------------------

void SessionTest::meshTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "meshTest.zip";

    // Create a test mesh
    const auto& originalMesh = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleQuadMesh(originalMesh);
    geometry::data::Mesh::shakePoint(originalMesh);
    geometry::data::Mesh::colorizeMeshPoints(originalMesh);
    geometry::data::Mesh::colorizeMeshCells(originalMesh);
    geometry::data::Mesh::generatePointNormals(originalMesh);
    geometry::data::Mesh::generateCellNormals(originalMesh);
    originalMesh->adjustAllocatedMemory();

    // Test serialization
    {
        const auto& mesh = data::Mesh::New();
        mesh->deepCopy(originalMesh);
        mesh->adjustAllocatedMemory();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(mesh);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test values
        const auto& mesh = data::Mesh::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(mesh);

        CPPUNIT_ASSERT_EQUAL(originalMesh->getAttributes(), mesh->getAttributes());
        CPPUNIT_ASSERT_EQUAL(originalMesh->getNumberOfCells(), mesh->getNumberOfCells());
        CPPUNIT_ASSERT_EQUAL(originalMesh->getNumberOfPoints(), mesh->getNumberOfPoints());
        CPPUNIT_ASSERT_EQUAL(originalMesh->getCellDataSize(), mesh->getCellDataSize());
        CPPUNIT_ASSERT_EQUAL(originalMesh->getDataSizeInBytes(), mesh->getDataSizeInBytes());

        auto originalLock       = originalMesh->lock();
        auto originalIt         = originalMesh->begin<data::iterator::PointIterator>();
        const auto& originalEnd = originalMesh->end<data::iterator::PointIterator>();

        auto meshLock       = mesh->lock();
        auto meshIt         = mesh->begin<data::iterator::PointIterator>();
        const auto& meshEnd = mesh->end<data::iterator::PointIterator>();

        for( ; originalIt != originalEnd && meshIt != meshEnd ; ++originalIt, ++meshIt)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(originalIt->point->x, meshIt->point->x, std::numeric_limits<float>::epsilon());
            CPPUNIT_ASSERT_DOUBLES_EQUAL(originalIt->point->y, meshIt->point->y, std::numeric_limits<float>::epsilon());
            CPPUNIT_ASSERT_DOUBLES_EQUAL(originalIt->point->z, meshIt->point->z, std::numeric_limits<float>::epsilon());

            CPPUNIT_ASSERT_DOUBLES_EQUAL(
                originalIt->normal->nx,
                meshIt->normal->nx,
                std::numeric_limits<float>::epsilon()
            );
            CPPUNIT_ASSERT_DOUBLES_EQUAL(
                originalIt->normal->ny,
                meshIt->normal->ny,
                std::numeric_limits<float>::epsilon()
            );
            CPPUNIT_ASSERT_DOUBLES_EQUAL(
                originalIt->normal->nz,
                meshIt->normal->nz,
                std::numeric_limits<float>::epsilon()
            );
        }
    }
}

//------------------------------------------------------------------------------

void SessionTest::equipmentTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "equipmentTest.zip";

    const std::string institutionName(UUID::generateUUID());

    // Test serialization
    {
        const auto& equipment = data::Equipment::New();
        equipment->setInstitutionName(institutionName);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(equipment);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test values
        const auto& equipment = data::Equipment::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(equipment);

        CPPUNIT_ASSERT_EQUAL(institutionName, equipment->getInstitutionName());
    }
}

//------------------------------------------------------------------------------

void SessionTest::patientTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "patientTest.zip";

    const std::string name(UUID::generateUUID());
    const std::string patientId(UUID::generateUUID());
    const std::string birthdate(UUID::generateUUID());
    const std::string sex(UUID::generateUUID());

    // Test serialization
    {
        const auto& patient = data::Patient::New();
        patient->setName(name);
        patient->setPatientId(patientId);
        patient->setBirthdate(birthdate);
        patient->setSex(sex);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(patient);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test values
        const auto& patient = data::Patient::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(patient);

        CPPUNIT_ASSERT_EQUAL(name, patient->getName());
        CPPUNIT_ASSERT_EQUAL(patientId, patient->getPatientId());
        CPPUNIT_ASSERT_EQUAL(birthdate, patient->getBirthdate());
        CPPUNIT_ASSERT_EQUAL(sex, patient->getSex());
    }
}

//------------------------------------------------------------------------------

void SessionTest::studyTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "studyTest.zip";

    const std::string instanceUID(UUID::generateUUID());
    const std::string studyID(UUID::generateUUID());
    const std::string date(UUID::generateUUID());
    const std::string time(UUID::generateUUID());
    const std::string referringPhysicianName(UUID::generateUUID());
    const std::string consultingPhysicianName(UUID::generateUUID());
    const std::string description(UUID::generateUUID());
    const std::string patientAge(UUID::generateUUID());
    const std::string patientSize(UUID::generateUUID());
    const std::string patientWeight(UUID::generateUUID());
    const std::string patientBodyMassIndex(UUID::generateUUID());

    // Test serialization
    {
        const auto& study = data::Study::New();

        study->setInstanceUID(instanceUID);
        study->setStudyID(studyID);
        study->setDate(date);
        study->setTime(time);
        study->setReferringPhysicianName(referringPhysicianName);
        study->setConsultingPhysicianName(consultingPhysicianName);
        study->setDescription(description);
        study->setPatientAge(patientAge);
        study->setPatientSize(patientSize);
        study->setPatientWeight(patientWeight);
        study->setPatientBodyMassIndex(patientBodyMassIndex);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(study);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test values
        const auto& study = data::Study::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(study);

        CPPUNIT_ASSERT_EQUAL(instanceUID, study->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(studyID, study->getStudyID());
        CPPUNIT_ASSERT_EQUAL(date, study->getDate());
        CPPUNIT_ASSERT_EQUAL(time, study->getTime());
        CPPUNIT_ASSERT_EQUAL(referringPhysicianName, study->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(consultingPhysicianName, study->getConsultingPhysicianName());
        CPPUNIT_ASSERT_EQUAL(description, study->getDescription());
        CPPUNIT_ASSERT_EQUAL(patientAge, study->getPatientAge());
        CPPUNIT_ASSERT_EQUAL(patientSize, study->getPatientSize());
        CPPUNIT_ASSERT_EQUAL(patientWeight, study->getPatientWeight());
        CPPUNIT_ASSERT_EQUAL(patientBodyMassIndex, study->getPatientBodyMassIndex());
    }
}

//------------------------------------------------------------------------------

void SessionTest::seriesTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "seriesTest.zip";

    /// Test vector
    const std::string name(UUID::generateUUID());
    const std::string patientId(UUID::generateUUID());
    const std::string birthdate(UUID::generateUUID());
    const std::string sex(UUID::generateUUID());

    const auto& patient = data::Patient::New();
    patient->setName(name);
    patient->setPatientId(patientId);
    patient->setBirthdate(birthdate);
    patient->setSex(sex);

    const std::string instanceUID(UUID::generateUUID());
    const std::string studyID(UUID::generateUUID());
    const std::string date(UUID::generateUUID());
    const std::string time(UUID::generateUUID());
    const std::string referringPhysicianName(UUID::generateUUID());
    const std::string consultingPhysicianName(UUID::generateUUID());
    const std::string description(UUID::generateUUID());
    const std::string patientAge(UUID::generateUUID());
    const std::string patientSize(UUID::generateUUID());
    const std::string patientWeight(UUID::generateUUID());
    const std::string patientBodyMassIndex(UUID::generateUUID());

    const auto& study = data::Study::New();
    study->setInstanceUID(instanceUID);
    study->setStudyID(studyID);
    study->setDate(date);
    study->setTime(time);
    study->setReferringPhysicianName(referringPhysicianName);
    study->setConsultingPhysicianName(consultingPhysicianName);
    study->setDescription(description);
    study->setPatientAge(patientAge);
    study->setPatientSize(patientSize);
    study->setPatientWeight(patientWeight);
    study->setPatientBodyMassIndex(patientBodyMassIndex);

    const std::string institutionName(UUID::generateUUID());

    const auto& equipment = data::Equipment::New();
    equipment->setInstitutionName(institutionName);

    const std::string modality(UUID::generateUUID());
    const std::string number(UUID::generateUUID());
    const std::string laterality(UUID::generateUUID());
    const std::vector<std::string> performingPhysiciansName =
    {UUID::generateUUID(), UUID::generateUUID(), UUID::generateUUID()};
    const std::string protocolName(UUID::generateUUID());
    const std::string bodyPartExamined(UUID::generateUUID());
    const std::string patientPosition(UUID::generateUUID());
    const std::string anatomicalOrientationType(UUID::generateUUID());
    const std::string performdedProcedureStepID(UUID::generateUUID());
    const std::string performedProcedureStepStartDate(UUID::generateUUID());
    const std::string performedProcedureStepStartTime(UUID::generateUUID());
    const std::string performedProcedureStepEndDate(UUID::generateUUID());
    const std::string performedProcedureStepEndTime(UUID::generateUUID());
    const std::string performedProcedureStepDescription(UUID::generateUUID());
    const std::string performedProcedureComments(UUID::generateUUID());

    // Test serialization
    {
        // Set "child" attributes
        const auto& seriesPatient = data::Patient::New();
        seriesPatient->deepCopy(patient);

        const auto& seriesStudy = data::Study::New();
        seriesStudy->deepCopy(study);

        const auto& seriesEquipment = data::Equipment::New();
        seriesEquipment->deepCopy(equipment);

        const auto& series = data::Series::New();
        series->setPatient(seriesPatient);
        series->setStudy(seriesStudy);
        series->setEquipment(seriesEquipment);

        // Fill trivial attributes
        series->setModality(modality);
        series->setInstanceUID(instanceUID);
        series->setNumber(number);
        series->setLaterality(laterality);
        series->setDate(date);
        series->setTime(time);
        series->setPerformingPhysiciansName(performingPhysiciansName);
        series->setProtocolName(protocolName);
        series->setDescription(description);
        series->setBodyPartExamined(bodyPartExamined);
        series->setPatientPosition(patientPosition);
        series->setAnatomicalOrientationType(anatomicalOrientationType);
        series->setPerformedProcedureStepID(performdedProcedureStepID);
        series->setPerformedProcedureStepStartDate(performedProcedureStepStartDate);
        series->setPerformedProcedureStepStartTime(performedProcedureStepStartTime);
        series->setPerformedProcedureStepEndDate(performedProcedureStepEndDate);
        series->setPerformedProcedureStepEndTime(performedProcedureStepEndTime);
        series->setPerformedProcedureStepDescription(performedProcedureStepDescription);
        series->setPerformedProcedureComments(performedProcedureComments);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(series);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test values
        const auto& series = data::Series::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(series);

        // Equipment
        CPPUNIT_ASSERT_EQUAL(institutionName, series->getEquipment()->getInstitutionName());

        // Study
        const auto& seriesStudy = series->getStudy();
        CPPUNIT_ASSERT_EQUAL(instanceUID, seriesStudy->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(studyID, seriesStudy->getStudyID());
        CPPUNIT_ASSERT_EQUAL(date, seriesStudy->getDate());
        CPPUNIT_ASSERT_EQUAL(time, seriesStudy->getTime());
        CPPUNIT_ASSERT_EQUAL(referringPhysicianName, seriesStudy->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(consultingPhysicianName, seriesStudy->getConsultingPhysicianName());
        CPPUNIT_ASSERT_EQUAL(description, seriesStudy->getDescription());
        CPPUNIT_ASSERT_EQUAL(patientAge, seriesStudy->getPatientAge());
        CPPUNIT_ASSERT_EQUAL(patientSize, seriesStudy->getPatientSize());
        CPPUNIT_ASSERT_EQUAL(patientWeight, seriesStudy->getPatientWeight());
        CPPUNIT_ASSERT_EQUAL(patientBodyMassIndex, seriesStudy->getPatientBodyMassIndex());

        // Patient
        const auto& seriesPatient = series->getPatient();
        CPPUNIT_ASSERT_EQUAL(name, seriesPatient->getName());
        CPPUNIT_ASSERT_EQUAL(patientId, seriesPatient->getPatientId());
        CPPUNIT_ASSERT_EQUAL(birthdate, seriesPatient->getBirthdate());
        CPPUNIT_ASSERT_EQUAL(sex, seriesPatient->getSex());

        // Trivial attributes
        CPPUNIT_ASSERT_EQUAL(modality, series->getModality());
        CPPUNIT_ASSERT_EQUAL(instanceUID, series->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(number, series->getNumber());
        CPPUNIT_ASSERT_EQUAL(laterality, series->getLaterality());
        CPPUNIT_ASSERT_EQUAL(date, series->getDate());
        CPPUNIT_ASSERT_EQUAL(time, series->getTime());

        const auto& seriesName = series->getPerformingPhysiciansName();

        for(std::size_t i = 0 ; i < performingPhysiciansName.size() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(performingPhysiciansName[i], seriesName[i]);
        }

        CPPUNIT_ASSERT_EQUAL(protocolName, series->getProtocolName());
        CPPUNIT_ASSERT_EQUAL(description, series->getDescription());
        CPPUNIT_ASSERT_EQUAL(bodyPartExamined, series->getBodyPartExamined());
        CPPUNIT_ASSERT_EQUAL(patientPosition, series->getPatientPosition());
        CPPUNIT_ASSERT_EQUAL(anatomicalOrientationType, series->getAnatomicalOrientationType());
        CPPUNIT_ASSERT_EQUAL(performdedProcedureStepID, series->getPerformedProcedureStepID());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartDate, series->getPerformedProcedureStepStartDate());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartTime, series->getPerformedProcedureStepStartTime());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndDate, series->getPerformedProcedureStepEndDate());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndTime, series->getPerformedProcedureStepEndTime());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepDescription, series->getPerformedProcedureStepDescription());
        CPPUNIT_ASSERT_EQUAL(performedProcedureComments, series->getPerformedProcedureComments());
    }
}

//------------------------------------------------------------------------------

void SessionTest::activitySeriesTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "activitySeriesTest.zip";

    /// Test vector
    const std::string stringValue(UUID::generateUUID());
    const std::int64_t integerValue = 42;
    const bool booleanValue         = true;
    const float floatValue          = 3.141592653589793F;

    // Create composite
    const auto& composite = data::Composite::New();
    (*composite)[data::String::classname()]    = data::String::New(stringValue);
    (*composite)[data::Integer::classname()]   = data::Integer::New(integerValue);
    (*composite)[data::Boolean::classname()]   = data::Boolean::New(booleanValue);
    (*composite)[data::Float::classname()]     = data::Float::New(floatValue);
    (*composite)[data::Composite::classname()] = composite;

    const std::string activityConfigId(UUID::generateUUID());

    const std::string name(UUID::generateUUID());
    const std::string patientId(UUID::generateUUID());
    const std::string birthdate(UUID::generateUUID());
    const std::string sex(UUID::generateUUID());

    const auto& patient = data::Patient::New();
    patient->setName(name);
    patient->setPatientId(patientId);
    patient->setBirthdate(birthdate);
    patient->setSex(sex);

    const std::string instanceUID(UUID::generateUUID());
    const std::string studyID(UUID::generateUUID());
    const std::string date(UUID::generateUUID());
    const std::string time(UUID::generateUUID());
    const std::string referringPhysicianName(UUID::generateUUID());
    const std::string consultingPhysicianName(UUID::generateUUID());
    const std::string description(UUID::generateUUID());
    const std::string patientAge(UUID::generateUUID());
    const std::string patientSize(UUID::generateUUID());
    const std::string patientWeight(UUID::generateUUID());
    const std::string patientBodyMassIndex(UUID::generateUUID());

    const auto& study = data::Study::New();
    study->setInstanceUID(instanceUID);
    study->setStudyID(studyID);
    study->setDate(date);
    study->setTime(time);
    study->setReferringPhysicianName(referringPhysicianName);
    study->setConsultingPhysicianName(consultingPhysicianName);
    study->setDescription(description);
    study->setPatientAge(patientAge);
    study->setPatientSize(patientSize);
    study->setPatientWeight(patientWeight);
    study->setPatientBodyMassIndex(patientBodyMassIndex);

    const std::string institutionName(UUID::generateUUID());

    const auto& equipment = data::Equipment::New();
    equipment->setInstitutionName(institutionName);

    const std::string modality(UUID::generateUUID());
    const std::string number(UUID::generateUUID());
    const std::string laterality(UUID::generateUUID());
    const std::vector<std::string> performingPhysiciansName =
    {UUID::generateUUID(), UUID::generateUUID(), UUID::generateUUID()};
    const std::string protocolName(UUID::generateUUID());
    const std::string bodyPartExamined(UUID::generateUUID());
    const std::string patientPosition(UUID::generateUUID());
    const std::string anatomicalOrientationType(UUID::generateUUID());
    const std::string performdedProcedureStepID(UUID::generateUUID());
    const std::string performedProcedureStepStartDate(UUID::generateUUID());
    const std::string performedProcedureStepStartTime(UUID::generateUUID());
    const std::string performedProcedureStepEndDate(UUID::generateUUID());
    const std::string performedProcedureStepEndTime(UUID::generateUUID());
    const std::string performedProcedureStepDescription(UUID::generateUUID());
    const std::string performedProcedureComments(UUID::generateUUID());

    // Test serialization
    {
        const auto& series = data::ActivitySeries::New();

        // Set "child" attributes
        const auto& seriesComposite = data::Composite::New();
        seriesComposite->deepCopy(composite);

        // For fun...
        (*seriesComposite)[data::ActivitySeries::classname()] = series;

        const auto& seriesPatient = data::Patient::New();
        seriesPatient->deepCopy(patient);

        const auto& seriesStudy = data::Study::New();
        seriesStudy->deepCopy(study);

        const auto& seriesEquipment = data::Equipment::New();
        seriesEquipment->deepCopy(equipment);

        series->setPatient(seriesPatient);
        series->setStudy(seriesStudy);
        series->setEquipment(seriesEquipment);
        series->setData(seriesComposite);

        // Fill trivial attributes
        series->setActivityConfigId(activityConfigId);
        series->setModality(modality);
        series->setInstanceUID(instanceUID);
        series->setNumber(number);
        series->setLaterality(laterality);
        series->setDate(date);
        series->setTime(time);
        series->setPerformingPhysiciansName(performingPhysiciansName);
        series->setProtocolName(protocolName);
        series->setDescription(description);
        series->setBodyPartExamined(bodyPartExamined);
        series->setPatientPosition(patientPosition);
        series->setAnatomicalOrientationType(anatomicalOrientationType);
        series->setPerformedProcedureStepID(performdedProcedureStepID);
        series->setPerformedProcedureStepStartDate(performedProcedureStepStartDate);
        series->setPerformedProcedureStepStartTime(performedProcedureStepStartTime);
        series->setPerformedProcedureStepEndDate(performedProcedureStepEndDate);
        series->setPerformedProcedureStepEndTime(performedProcedureStepEndTime);
        series->setPerformedProcedureStepDescription(performedProcedureStepDescription);
        series->setPerformedProcedureComments(performedProcedureComments);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(series);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test values
        const auto& series = data::ActivitySeries::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(series);

        // Composite
        const auto& seriesComposite = series->getData();
        CPPUNIT_ASSERT(seriesComposite);

        const auto& stringData = data::String::dynamicCast((*seriesComposite)[data::String::classname()]);
        CPPUNIT_ASSERT(stringData);
        CPPUNIT_ASSERT_EQUAL(stringValue, stringData->getValue());

        const auto integerData = data::Integer::dynamicCast((*seriesComposite)[data::Integer::classname()]);
        CPPUNIT_ASSERT(integerData);
        CPPUNIT_ASSERT_EQUAL(integerValue, integerData->getValue());

        const auto& booleanData = data::Boolean::dynamicCast((*seriesComposite)[data::Boolean::classname()]);
        CPPUNIT_ASSERT(booleanData);
        CPPUNIT_ASSERT_EQUAL(booleanValue, booleanData->getValue());

        const auto& floatData = data::Float::dynamicCast((*seriesComposite)[data::Float::classname()]);
        CPPUNIT_ASSERT(floatData);
        CPPUNIT_ASSERT_EQUAL(floatValue, floatData->getValue());

        const auto& compositeData = data::Composite::dynamicCast((*seriesComposite)[data::Composite::classname()]);
        CPPUNIT_ASSERT(compositeData);

        const auto& compositeStringData = data::String::dynamicCast((*compositeData)[data::String::classname()]);
        CPPUNIT_ASSERT(compositeStringData);
        CPPUNIT_ASSERT_EQUAL(stringValue, compositeStringData->getValue());

        // Special circular inclusion test
        const auto& activitySeriesData =
            data::ActivitySeries::dynamicCast((*seriesComposite)[data::ActivitySeries::classname()]);
        CPPUNIT_ASSERT(activitySeriesData);
        CPPUNIT_ASSERT_EQUAL(activityConfigId, activitySeriesData->getActivityConfigId());

        // Equipment
        CPPUNIT_ASSERT_EQUAL(institutionName, series->getEquipment()->getInstitutionName());

        // Study
        const auto& seriesStudy = series->getStudy();
        CPPUNIT_ASSERT_EQUAL(instanceUID, seriesStudy->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(studyID, seriesStudy->getStudyID());
        CPPUNIT_ASSERT_EQUAL(date, seriesStudy->getDate());
        CPPUNIT_ASSERT_EQUAL(time, seriesStudy->getTime());
        CPPUNIT_ASSERT_EQUAL(referringPhysicianName, seriesStudy->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(consultingPhysicianName, seriesStudy->getConsultingPhysicianName());
        CPPUNIT_ASSERT_EQUAL(description, seriesStudy->getDescription());
        CPPUNIT_ASSERT_EQUAL(patientAge, seriesStudy->getPatientAge());
        CPPUNIT_ASSERT_EQUAL(patientSize, seriesStudy->getPatientSize());
        CPPUNIT_ASSERT_EQUAL(patientWeight, seriesStudy->getPatientWeight());
        CPPUNIT_ASSERT_EQUAL(patientBodyMassIndex, seriesStudy->getPatientBodyMassIndex());

        // Patient
        const auto& seriesPatient = series->getPatient();
        CPPUNIT_ASSERT_EQUAL(name, seriesPatient->getName());
        CPPUNIT_ASSERT_EQUAL(patientId, seriesPatient->getPatientId());
        CPPUNIT_ASSERT_EQUAL(birthdate, seriesPatient->getBirthdate());
        CPPUNIT_ASSERT_EQUAL(sex, seriesPatient->getSex());

        // Trivial attributes
        CPPUNIT_ASSERT_EQUAL(modality, series->getModality());
        CPPUNIT_ASSERT_EQUAL(instanceUID, series->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(number, series->getNumber());
        CPPUNIT_ASSERT_EQUAL(laterality, series->getLaterality());
        CPPUNIT_ASSERT_EQUAL(date, series->getDate());
        CPPUNIT_ASSERT_EQUAL(time, series->getTime());

        const auto& seriesName = series->getPerformingPhysiciansName();

        for(std::size_t i = 0 ; i < performingPhysiciansName.size() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(performingPhysiciansName[i], seriesName[i]);
        }

        CPPUNIT_ASSERT_EQUAL(protocolName, series->getProtocolName());
        CPPUNIT_ASSERT_EQUAL(description, series->getDescription());
        CPPUNIT_ASSERT_EQUAL(bodyPartExamined, series->getBodyPartExamined());
        CPPUNIT_ASSERT_EQUAL(patientPosition, series->getPatientPosition());
        CPPUNIT_ASSERT_EQUAL(anatomicalOrientationType, series->getAnatomicalOrientationType());
        CPPUNIT_ASSERT_EQUAL(performdedProcedureStepID, series->getPerformedProcedureStepID());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartDate, series->getPerformedProcedureStepStartDate());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartTime, series->getPerformedProcedureStepStartTime());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndDate, series->getPerformedProcedureStepEndDate());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndTime, series->getPerformedProcedureStepEndTime());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepDescription, series->getPerformedProcedureStepDescription());
        CPPUNIT_ASSERT_EQUAL(performedProcedureComments, series->getPerformedProcedureComments());
    }
}

} // namespace ut

} // namespace sight::io::session
