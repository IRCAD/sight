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
#include <core/tools/System.hpp>
#include <core/tools/UUID.hpp>

#include <data/ActivitySeries.hpp>
#include <data/Array.hpp>
#include <data/Boolean.hpp>
#include <data/CalibrationInfo.hpp>
#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>
#include <data/Color.hpp>
#include <data/Composite.hpp>
#include <data/Edge.hpp>
#include <data/Equipment.hpp>
#include <data/Float.hpp>
#include <data/Graph.hpp>
#include <data/Histogram.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/iterator/ImageIterator.hpp>
#include <data/iterator/ImageIterator.hxx>
#include <data/iterator/MeshIterators.hpp>
#include <data/iterator/MeshIterators.hxx>
#include <data/Landmarks.hpp>
#include <data/Line.hpp>
#include <data/List.hpp>
#include <data/Material.hpp>
#include <data/Matrix4.hpp>
#include <data/mt/locked_ptr.hpp>
#include <data/Node.hpp>
#include <data/Patient.hpp>
#include <data/Plane.hpp>
#include <data/PlaneList.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/Port.hpp>
#include <data/ProcessObject.hpp>
#include <data/Reconstruction.hpp>
#include <data/ReconstructionTraits.hpp>
#include <data/Resection.hpp>
#include <data/ResectionDB.hpp>
#include <data/ROITraits.hpp>
#include <data/Series.hpp>
#include <data/SeriesDB.hpp>
#include <data/String.hpp>
#include <data/StructureTraits.hpp>
#include <data/StructureTraitsDictionary.hpp>
#include <data/Study.hpp>
#include <data/Tag.hpp>
#include <data/TransferFunction.hpp>
#include <data/Vector.hpp>

#include <geometry/data/Mesh.hpp>

#include <io/session/detail/SessionDeserializer.hpp>
#include <io/session/detail/SessionSerializer.hpp>
#include <io/session/SessionReader.hpp>
#include <io/session/SessionWriter.hpp>
#include <io/zip/exception/Read.hpp>
#include <io/zip/exception/Write.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Image.hpp>
#include <utestData/generator/Mesh.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::io::session::ut::SessionTest);

static const double DOUBLE_EPSILON = std::numeric_limits<double>::epsilon();
static const float FLOAT_EPSILON   = std::numeric_limits<float>::epsilon();

namespace sight::io::session
{

namespace ut
{

// For UUID::generateUUID();
using core::tools::UUID;

//------------------------------------------------------------------------------

inline static const data::Boolean::csptr& expectedBoolean(const std::size_t variant = 0)
{
    if(variant % 2 == 0)
    {
        static const data::Boolean::csptr boolean = data::Boolean::New(true);
        return boolean;
    }
    else
    {
        static const data::Boolean::csptr boolean = data::Boolean::New(false);
        return boolean;
    }
}

//------------------------------------------------------------------------------

inline static data::Boolean::sptr newBoolean(const std::size_t variant = 0)
{
    const auto& boolean = data::Boolean::New();
    boolean->deepCopy(expectedBoolean(variant));
    return boolean;
}

//------------------------------------------------------------------------------

inline static void testBoolean(const data::Boolean::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);
    CPPUNIT_ASSERT_EQUAL(expectedBoolean(variant)->getValue(), actual->getValue());
}

//------------------------------------------------------------------------------

inline static const data::Integer::csptr& expectedInteger(const std::size_t variant = 0)
{
    static std::map<std::size_t, data::Integer::csptr> integers;
    const auto& it = integers.find(variant);

    if(it == integers.cend())
    {
        return integers.insert_or_assign(
            variant,
            data::Integer::New(1 + static_cast<std::int64_t>(variant))
        ).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Integer::sptr newInteger(const std::size_t variant = 0)
{
    const auto& integer = data::Integer::New();
    integer->deepCopy(expectedInteger(variant));
    return integer;
}

//------------------------------------------------------------------------------

inline static void testInteger(const data::Integer::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);
    CPPUNIT_ASSERT_EQUAL(expectedInteger(variant)->getValue(), actual->getValue());
}

//------------------------------------------------------------------------------

inline static const data::Float::csptr& expectedFloat(const std::size_t variant = 0)
{
    static std::map<std::size_t, data::Float::csptr> floats;
    const auto& it = floats.find(variant);

    if(it == floats.cend())
    {
        return floats.insert_or_assign(
            variant,
            data::Float::New(1.0F + static_cast<float>(variant))
        ).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Float::sptr newFloat(const std::size_t variant = 0)
{
    const auto& real = data::Float::New();
    real->deepCopy(expectedFloat(variant));
    return real;
}

//------------------------------------------------------------------------------

inline static void testFloat(const data::Float::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedFloat(variant)->getValue(), actual->getValue(), FLOAT_EPSILON);
}

//------------------------------------------------------------------------------

inline static const data::String::csptr& expectedString(const std::size_t variant = 0)
{
    static std::map<std::size_t, data::String::csptr> strings;
    const auto& it = strings.find(variant);

    if(it == strings.cend())
    {
        data::String::csptr string;

        if(variant == 0)
        {
            string = data::String::New(core::crypto::encrypt(UUID::generateUUID(), "password"));
        }
        else
        {
            string = data::String::New(UUID::generateUUID());
        }

        return strings.insert_or_assign(variant, string).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::String::sptr newString(const std::size_t variant = 0)
{
    const auto& string = data::String::New();
    string->deepCopy(expectedString(variant));
    return string;
}

//------------------------------------------------------------------------------

inline static void testString(const data::String::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    if(variant == 0)
    {
        const auto& expectedEncrypted = expectedString(variant)->getValue();
        const auto& actualEncrypted   = actual->getValue();
        CPPUNIT_ASSERT_EQUAL(expectedEncrypted, actualEncrypted);

        const auto& expectedDecrypted = core::crypto::decrypt(expectedEncrypted, "password");
        const auto& actualDecrypted   = core::crypto::decrypt(actualEncrypted, "password");
        CPPUNIT_ASSERT_EQUAL(expectedDecrypted, actualDecrypted);
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(expectedString(variant)->getValue(), actual->getValue());
    }
}

//------------------------------------------------------------------------------

inline static const data::Composite::csptr& expectedComposite(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp        = data::Composite::New();
            auto& container = tmp->getContainer();
            container[data::Boolean::classname()] = newBoolean(variant);
            container[data::Integer::classname()] = newInteger(variant);
            container[data::Float::classname()]   = newFloat(variant);
            container[data::String::classname()]  = newString(variant);

            return tmp;
        };

    static std::map<std::size_t, data::Composite::csptr> composites;
    const auto& it = composites.find(variant);

    if(it == composites.cend())
    {
        return composites.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Composite::sptr newComposite(const std::size_t variant = 0)
{
    const auto& composite = data::Composite::New();
    composite->deepCopy(expectedComposite(variant));
    return composite;
}

//------------------------------------------------------------------------------

inline static void testComposite(const data::Composite::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& container = actual->getContainer();

    testBoolean(std::dynamic_pointer_cast<data::Boolean>(container.at(data::Boolean::classname())), variant);
    testInteger(std::dynamic_pointer_cast<data::Integer>(container.at(data::Integer::classname())), variant);
    testFloat(std::dynamic_pointer_cast<data::Float>(container.at(data::Float::classname())), variant);
    testString(std::dynamic_pointer_cast<data::String>(container.at(data::String::classname())), variant);
}

//------------------------------------------------------------------------------

inline static const data::Mesh::csptr& expectedMesh(const std::size_t variant = 0)
{
    const auto& generator =
        []
        {
            auto tmp = data::Mesh::New();
            utestData::generator::Mesh::generateTriangleQuadMesh(tmp);
            geometry::data::Mesh::shakePoint(tmp);
            geometry::data::Mesh::colorizeMeshPoints(tmp);
            geometry::data::Mesh::colorizeMeshCells(tmp);
            geometry::data::Mesh::generatePointNormals(tmp);
            geometry::data::Mesh::generateCellNormals(tmp);
            tmp->adjustAllocatedMemory();

            return tmp;
        };

    static std::map<std::size_t, data::Mesh::csptr> meshes;
    const auto& it = meshes.find(variant);

    if(it == meshes.cend())
    {
        return meshes.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Mesh::sptr newMesh(const std::size_t variant = 0)
{
    const auto& mesh = data::Mesh::New();
    mesh->deepCopy(expectedMesh(variant));
    mesh->adjustAllocatedMemory();
    return mesh;
}

//------------------------------------------------------------------------------

inline static void testMesh(const data::Mesh::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    // This is need to use iterators
    const auto& expected = expectedMesh(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getAttributes(), actual->getAttributes());
    CPPUNIT_ASSERT_EQUAL(expected->getNumberOfCells(), actual->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(expected->getNumberOfPoints(), actual->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(expected->getCellDataSize(), actual->getCellDataSize());
    CPPUNIT_ASSERT_EQUAL(expected->getDataSizeInBytes(), actual->getDataSizeInBytes());

    data::mt::locked_ptr<const data::Mesh> expectedGuard(expected);
    data::mt::locked_ptr<const data::Mesh> actualGuard(actual);

    for(auto expectedIt = expected->begin<data::iterator::PointIterator>(),
        expectedEnd = expected->end<data::iterator::PointIterator>(),
        actualIt = actual->begin<data::iterator::PointIterator>(),
        actualEnd = actual->end<data::iterator::PointIterator>() ;
        expectedIt != expectedEnd && actualIt != actualEnd ;
        ++expectedIt, ++actualIt)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedIt->point->x, actualIt->point->x, FLOAT_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedIt->point->y, actualIt->point->y, FLOAT_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedIt->point->z, actualIt->point->z, FLOAT_EPSILON);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            expectedIt->normal->nx,
            actualIt->normal->nx,
            FLOAT_EPSILON
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            expectedIt->normal->ny,
            actualIt->normal->ny,
            FLOAT_EPSILON
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            expectedIt->normal->nz,
            actualIt->normal->nz,
            FLOAT_EPSILON
        );
    }
}

//------------------------------------------------------------------------------

inline static const data::Equipment::csptr& expectedEquipment(const std::size_t variant = 0)
{
    const auto& generator =
        []
        {
            auto tmp = data::Equipment::New();
            tmp->setInstitutionName(UUID::generateUUID());

            return tmp;
        };

    static std::map<std::size_t, data::Equipment::csptr> equipments;
    const auto& it = equipments.find(variant);

    if(it == equipments.cend())
    {
        return equipments.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Equipment::sptr newEquipment(const std::size_t variant = 0)
{
    const auto& equipment = data::Equipment::New();
    equipment->deepCopy(expectedEquipment(variant));
    return equipment;
}

//------------------------------------------------------------------------------

inline static void testEquipment(const data::Equipment::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedEquipment(variant);
    CPPUNIT_ASSERT_EQUAL(expected->getInstitutionName(), actual->getInstitutionName());
}

//------------------------------------------------------------------------------

inline static const data::Patient::csptr& expectedPatient(const std::size_t variant = 0)
{
    const auto& generator =
        []
        {
            auto tmp = data::Patient::New();
            tmp->setName(UUID::generateUUID());
            tmp->setPatientId(UUID::generateUUID());
            tmp->setBirthdate(UUID::generateUUID());
            tmp->setSex(UUID::generateUUID());

            return tmp;
        };

    static std::map<std::size_t, data::Patient::csptr> patients;
    const auto& it = patients.find(variant);

    if(it == patients.cend())
    {
        return patients.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Patient::sptr newPatient(const std::size_t variant = 0)
{
    const auto& patient = data::Patient::New();
    patient->deepCopy(expectedPatient(variant));
    return patient;
}

//------------------------------------------------------------------------------

inline static void testPatient(const data::Patient::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedPatient(variant);
    CPPUNIT_ASSERT_EQUAL(expected->getName(), actual->getName());
    CPPUNIT_ASSERT_EQUAL(expected->getPatientId(), actual->getPatientId());
    CPPUNIT_ASSERT_EQUAL(expected->getBirthdate(), actual->getBirthdate());
    CPPUNIT_ASSERT_EQUAL(expected->getSex(), actual->getSex());
}

//------------------------------------------------------------------------------

inline static const data::Study::csptr& expectedStudy(const std::size_t variant = 0)
{
    const auto& generator =
        []
        {
            auto tmp = data::Study::New();
            tmp->setInstanceUID(UUID::generateUUID());
            tmp->setStudyID(UUID::generateUUID());
            tmp->setDate(UUID::generateUUID());
            tmp->setTime(UUID::generateUUID());
            tmp->setReferringPhysicianName(UUID::generateUUID());
            tmp->setConsultingPhysicianName(UUID::generateUUID());
            tmp->setDescription(UUID::generateUUID());
            tmp->setPatientAge(UUID::generateUUID());
            tmp->setPatientSize(UUID::generateUUID());
            tmp->setPatientWeight(UUID::generateUUID());
            tmp->setPatientBodyMassIndex(UUID::generateUUID());

            return tmp;
        };

    static std::map<std::size_t, data::Study::csptr> studies;
    const auto& it = studies.find(variant);

    if(it == studies.cend())
    {
        return studies.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Study::sptr newStudy(const std::size_t variant = 0)
{
    const auto& study = data::Study::New();
    study->deepCopy(expectedStudy(variant));
    return study;
}

//------------------------------------------------------------------------------

inline static void testStudy(const data::Study::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedStudy(variant);
    CPPUNIT_ASSERT_EQUAL(expected->getInstanceUID(), actual->getInstanceUID());
    CPPUNIT_ASSERT_EQUAL(expected->getStudyID(), actual->getStudyID());
    CPPUNIT_ASSERT_EQUAL(expected->getDate(), actual->getDate());
    CPPUNIT_ASSERT_EQUAL(expected->getTime(), actual->getTime());
    CPPUNIT_ASSERT_EQUAL(expected->getReferringPhysicianName(), actual->getReferringPhysicianName());
    CPPUNIT_ASSERT_EQUAL(expected->getConsultingPhysicianName(), actual->getConsultingPhysicianName());
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());
    CPPUNIT_ASSERT_EQUAL(expected->getPatientAge(), actual->getPatientAge());
    CPPUNIT_ASSERT_EQUAL(expected->getPatientSize(), actual->getPatientSize());
    CPPUNIT_ASSERT_EQUAL(expected->getPatientWeight(), actual->getPatientWeight());
    CPPUNIT_ASSERT_EQUAL(expected->getPatientBodyMassIndex(), actual->getPatientBodyMassIndex());
}

//------------------------------------------------------------------------------

inline static const data::Series::csptr& expectedSeries(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::Series::New();
            tmp->setPatient(newPatient(variant));
            tmp->setStudy(newStudy(variant));
            tmp->setEquipment(newEquipment(variant));

            // Fill trivial attributes
            tmp->setModality(UUID::generateUUID());
            tmp->setInstanceUID(UUID::generateUUID());
            tmp->setNumber(UUID::generateUUID());
            tmp->setLaterality(UUID::generateUUID());
            tmp->setDate(UUID::generateUUID());
            tmp->setTime(UUID::generateUUID());
            tmp->setPerformingPhysiciansName(
                {
                    UUID::generateUUID(),
                    UUID::generateUUID(),
                    UUID::generateUUID()
                });
            tmp->setProtocolName(UUID::generateUUID());
            tmp->setDescription(UUID::generateUUID());
            tmp->setBodyPartExamined(UUID::generateUUID());
            tmp->setPatientPosition(UUID::generateUUID());
            tmp->setAnatomicalOrientationType(UUID::generateUUID());
            tmp->setPerformedProcedureStepID(UUID::generateUUID());
            tmp->setPerformedProcedureStepStartDate(UUID::generateUUID());
            tmp->setPerformedProcedureStepStartTime(UUID::generateUUID());
            tmp->setPerformedProcedureStepEndDate(UUID::generateUUID());
            tmp->setPerformedProcedureStepEndTime(UUID::generateUUID());
            tmp->setPerformedProcedureStepDescription(UUID::generateUUID());
            tmp->setPerformedProcedureComments(UUID::generateUUID());

            return tmp;
        };

    static std::map<std::size_t, data::Series::csptr> series;
    const auto& it = series.find(variant);

    if(it == series.cend())
    {
        return series.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Series::sptr newSeries(const std::size_t variant = 0)
{
    const auto& series = data::Series::New();
    series->deepCopy(expectedSeries(variant));
    return series;
}

//------------------------------------------------------------------------------

inline static void testSeries(const data::Series::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedSeries(variant);
    // Equipment
    testEquipment(actual->getEquipment(), variant);

    // Study
    testStudy(actual->getStudy(), variant);

    // Patient
    testPatient(actual->getPatient(), variant);

    // Trivial attributes
    CPPUNIT_ASSERT_EQUAL(expected->getModality(), actual->getModality());
    CPPUNIT_ASSERT_EQUAL(expected->getInstanceUID(), actual->getInstanceUID());
    CPPUNIT_ASSERT_EQUAL(expected->getNumber(), actual->getNumber());
    CPPUNIT_ASSERT_EQUAL(expected->getLaterality(), actual->getLaterality());
    CPPUNIT_ASSERT_EQUAL(expected->getDate(), actual->getDate());
    CPPUNIT_ASSERT_EQUAL(expected->getTime(), actual->getTime());

    const auto& expectedNames = expected->getPerformingPhysiciansName();
    const auto& actualNames   = actual->getPerformingPhysiciansName();
    CPPUNIT_ASSERT_EQUAL(expectedNames.size(), actualNames.size());

    for(std::size_t i = 0 ; i < expectedNames.size() ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(expectedNames[i], actualNames[i]);
    }

    CPPUNIT_ASSERT_EQUAL(expected->getProtocolName(), actual->getProtocolName());
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());
    CPPUNIT_ASSERT_EQUAL(expected->getBodyPartExamined(), actual->getBodyPartExamined());
    CPPUNIT_ASSERT_EQUAL(expected->getPatientPosition(), actual->getPatientPosition());
    CPPUNIT_ASSERT_EQUAL(expected->getAnatomicalOrientationType(), actual->getAnatomicalOrientationType());
    CPPUNIT_ASSERT_EQUAL(expected->getPerformedProcedureStepID(), actual->getPerformedProcedureStepID());
    CPPUNIT_ASSERT_EQUAL(expected->getPerformedProcedureStepStartDate(), actual->getPerformedProcedureStepStartDate());
    CPPUNIT_ASSERT_EQUAL(expected->getPerformedProcedureStepStartTime(), actual->getPerformedProcedureStepStartTime());
    CPPUNIT_ASSERT_EQUAL(expected->getPerformedProcedureStepEndDate(), actual->getPerformedProcedureStepEndDate());
    CPPUNIT_ASSERT_EQUAL(expected->getPerformedProcedureStepEndTime(), actual->getPerformedProcedureStepEndTime());
    CPPUNIT_ASSERT_EQUAL(
        expected->getPerformedProcedureStepDescription(),
        actual->getPerformedProcedureStepDescription()
    );
    CPPUNIT_ASSERT_EQUAL(expected->getPerformedProcedureComments(), actual->getPerformedProcedureComments());
}

//------------------------------------------------------------------------------

inline static const data::ActivitySeries::csptr& expectedActivitySeries(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::ActivitySeries::New();
            tmp->setData(newComposite(variant));
            tmp->setPatient(newPatient(variant));
            tmp->setStudy(newStudy(variant));
            tmp->setEquipment(newEquipment(variant));

            // Fill trivial attributes
            tmp->setActivityConfigId(UUID::generateUUID());
            tmp->setModality(UUID::generateUUID());
            tmp->setInstanceUID(UUID::generateUUID());
            tmp->setNumber(UUID::generateUUID());
            tmp->setLaterality(UUID::generateUUID());
            tmp->setDate(UUID::generateUUID());
            tmp->setTime(UUID::generateUUID());
            tmp->setPerformingPhysiciansName({UUID::generateUUID(), UUID::generateUUID()});
            tmp->setProtocolName(UUID::generateUUID());
            tmp->setDescription(UUID::generateUUID());
            tmp->setBodyPartExamined(UUID::generateUUID());
            tmp->setPatientPosition(UUID::generateUUID());
            tmp->setAnatomicalOrientationType(UUID::generateUUID());
            tmp->setPerformedProcedureStepID(UUID::generateUUID());
            tmp->setPerformedProcedureStepStartDate(UUID::generateUUID());
            tmp->setPerformedProcedureStepStartTime(UUID::generateUUID());
            tmp->setPerformedProcedureStepEndDate(UUID::generateUUID());
            tmp->setPerformedProcedureStepEndTime(UUID::generateUUID());
            tmp->setPerformedProcedureStepDescription(UUID::generateUUID());
            tmp->setPerformedProcedureComments(UUID::generateUUID());

            return tmp;
        };

    static std::map<std::size_t, data::ActivitySeries::csptr> activitySeries;
    const auto& it = activitySeries.find(variant);

    if(it == activitySeries.cend())
    {
        return activitySeries.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::ActivitySeries::sptr newActivitySeries(const std::size_t variant = 0)
{
    const auto& activitySeries = data::ActivitySeries::New();
    activitySeries->deepCopy(expectedActivitySeries(variant));
    return activitySeries;
}

//------------------------------------------------------------------------------

inline static void testActivitySeries(const data::ActivitySeries::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedActivitySeries(variant);

    // Equipment
    testEquipment(actual->getEquipment(), variant);

    // Study
    testStudy(actual->getStudy(), variant);

    // Patient
    testPatient(actual->getPatient(), variant);

    // Data
    testComposite(actual->getData(), variant);

    // Trivial attributes
    CPPUNIT_ASSERT_EQUAL(expected->getActivityConfigId(), actual->getActivityConfigId());
    CPPUNIT_ASSERT_EQUAL(expected->getModality(), actual->getModality());
    CPPUNIT_ASSERT_EQUAL(expected->getInstanceUID(), actual->getInstanceUID());
    CPPUNIT_ASSERT_EQUAL(expected->getNumber(), actual->getNumber());
    CPPUNIT_ASSERT_EQUAL(expected->getLaterality(), actual->getLaterality());
    CPPUNIT_ASSERT_EQUAL(expected->getDate(), actual->getDate());
    CPPUNIT_ASSERT_EQUAL(expected->getTime(), actual->getTime());

    const auto& expectedNames = expected->getPerformingPhysiciansName();
    const auto& actualNames   = actual->getPerformingPhysiciansName();
    CPPUNIT_ASSERT_EQUAL(expectedNames.size(), actualNames.size());

    for(std::size_t i = 0 ; i < expectedNames.size() ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(expectedNames[i], actualNames[i]);
    }

    CPPUNIT_ASSERT_EQUAL(expected->getProtocolName(), actual->getProtocolName());
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());
    CPPUNIT_ASSERT_EQUAL(expected->getBodyPartExamined(), actual->getBodyPartExamined());
    CPPUNIT_ASSERT_EQUAL(expected->getPatientPosition(), actual->getPatientPosition());
    CPPUNIT_ASSERT_EQUAL(expected->getAnatomicalOrientationType(), actual->getAnatomicalOrientationType());
    CPPUNIT_ASSERT_EQUAL(expected->getPerformedProcedureStepID(), actual->getPerformedProcedureStepID());
    CPPUNIT_ASSERT_EQUAL(
        expected->getPerformedProcedureStepStartDate(),
        actual->getPerformedProcedureStepStartDate()
    );
    CPPUNIT_ASSERT_EQUAL(
        expected->getPerformedProcedureStepStartTime(),
        actual->getPerformedProcedureStepStartTime()
    );
    CPPUNIT_ASSERT_EQUAL(
        expected->getPerformedProcedureStepEndDate(),
        actual->getPerformedProcedureStepEndDate()
    );
    CPPUNIT_ASSERT_EQUAL(
        expected->getPerformedProcedureStepEndTime(),
        actual->getPerformedProcedureStepEndTime()
    );
    CPPUNIT_ASSERT_EQUAL(
        expected->getPerformedProcedureStepDescription(),
        actual->getPerformedProcedureStepDescription()
    );
    CPPUNIT_ASSERT_EQUAL(expected->getPerformedProcedureComments(), actual->getPerformedProcedureComments());
}

//------------------------------------------------------------------------------

inline static const data::Array::csptr& expectedArray(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::Array::New();

            if(variant % 2 == 0)
            {
                tmp->resize(
                    {variant + 2, variant + 2},
                    core::tools::Type::s_UINT8,
                    true
                );

                std::uint8_t counter = 0;
                for(auto it = tmp->begin<std::uint8_t>(),
                    end = tmp->end<std::uint8_t>() ;
                    it != end ;
                    ++it)
                {
                    *it = static_cast<std::uint8_t>(variant) + counter++;
                }
            }
            else
            {
                tmp->resize(
                    {variant + 2, variant + 2},
                    core::tools::Type::s_DOUBLE,
                    true
                );

                double counter = 0.0;
                for(auto it = tmp->begin<double>(),
                    end = tmp->end<double>() ;
                    it != end ;
                    ++it)
                {
                    *it = static_cast<double>(variant) + counter++;
                }
            }

            return tmp;
        };

    static std::map<std::size_t, data::Array::csptr> arrays;
    const auto& it = arrays.find(variant);

    if(it == arrays.cend())
    {
        return arrays.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Array::sptr newArray(const std::size_t variant = 0)
{
    const auto& array = data::Array::New();
    array->deepCopy(expectedArray(variant));
    return array;
}

//------------------------------------------------------------------------------

inline static void testArray(const data::Array::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedArray(variant);

    const auto& expectedSize = expected->getSize();
    const auto& actualSize   = actual->getSize();

    for(std::size_t i = 0, end = expectedSize.size() ; i < end ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(expectedSize[i], actualSize[i]);
    }

    if(variant % 2 == 0)
    {
        for(auto expectedIt = expected->begin<std::uint8_t>(),
            expectedEnd = expected->end<std::uint8_t>(),
            actualIt = actual->begin<std::uint8_t>(),
            actualEnd = actual->end<std::uint8_t>() ;
            expectedIt != expectedEnd && actualIt != actualEnd ;
            ++expectedIt, ++actualIt)
        {
            CPPUNIT_ASSERT_EQUAL(*expectedIt, *actualIt);
        }
    }
    else
    {
        for(auto expectedIt = expected->begin<double>(),
            expectedEnd = expected->end<double>(),
            actualIt = actual->begin<double>(),
            actualEnd = actual->end<double>() ;
            expectedIt != expectedEnd && actualIt != actualEnd ;
            ++expectedIt, ++actualIt)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(*expectedIt, *actualIt, DOUBLE_EPSILON);
        }
    }
}

//------------------------------------------------------------------------------

inline static const data::Image::csptr& expectedImage(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp        = data::Image::New();
            const auto lock = tmp->lock();

            const double dvariant = static_cast<double>(variant);

            utestData::generator::Image::generateImage(
                tmp,
                {variant + 3, variant + 3, variant + 3},
                {dvariant + 1.0, dvariant + 1.0, dvariant + 1.0},
                {dvariant + 0.0, dvariant + 0.0, dvariant + 0.0},
                variant % 2 == 0 ? core::tools::Type::s_UINT8 : core::tools::Type::s_FLOAT,
                variant % 2 == 0 ? data::Image::PixelFormat::BGRA : data::Image::PixelFormat::GRAY_SCALE
            );
            utestData::generator::Image::randomizeImage(tmp);

            return tmp;
        };

    static std::map<std::size_t, data::Image::csptr> images;
    const auto& it = images.find(variant);

    if(it == images.cend())
    {
        return images.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Image::sptr newImage(const std::size_t variant = 0)
{
    const auto& image = data::Image::New();
    image->deepCopy(expectedImage(variant));
    return image;
}

//------------------------------------------------------------------------------

inline static void testImage(const data::Image::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedImage(variant);

    const auto& expectedSize = expected->getSize2();
    const auto& actualSize   = actual->getSize2();

    for(std::size_t i = 0, end = expectedSize.size() ; i < end ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(expectedSize[i], actualSize[i]);
    }

    const auto& expectedSpacing = expected->getSpacing2();
    const auto& actualSpacing   = actual->getSpacing2();

    for(std::size_t i = 0, end = expectedSpacing.size() ; i < end ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedSpacing[i], actualSpacing[i], DOUBLE_EPSILON);
    }

    const auto& expectedOrigin = expected->getOrigin2();
    const auto& actualOrigin   = actual->getOrigin2();

    for(std::size_t i = 0, end = expectedOrigin.size() ; i < end ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedOrigin[i], actualOrigin[i], DOUBLE_EPSILON);
    }

    CPPUNIT_ASSERT_EQUAL(expected->getType(), actual->getType());

    if(variant % 2 == 0)
    {
        for(auto expectedIt = expected->begin<std::uint8_t>(),
            expectedEnd = expected->end<std::uint8_t>(),
            actualIt = actual->begin<std::uint8_t>(),
            actualEnd = actual->end<std::uint8_t>() ;
            expectedIt != expectedEnd && actualIt != actualEnd ;
            ++expectedIt, ++actualIt)
        {
            CPPUNIT_ASSERT_EQUAL(*expectedIt, *actualIt);
        }
    }
    else
    {
        for(auto expectedIt = expected->begin<float>(),
            expectedEnd = expected->end<float>(),
            actualIt = actual->begin<float>(),
            actualEnd = actual->end<float>() ;
            expectedIt != expectedEnd && actualIt != actualEnd ;
            ++expectedIt, ++actualIt)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(*expectedIt, *actualIt, FLOAT_EPSILON);
        }
    }
}

//------------------------------------------------------------------------------

inline static const data::Vector::csptr& expectedVector(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp        = data::Vector::New();
            auto& container = tmp->getContainer();
            container.push_back(newBoolean(variant));
            container.push_back(newInteger(variant));
            container.push_back(newFloat(variant));
            container.push_back(newString(variant));
            container.push_back(newActivitySeries(variant));

            return tmp;
        };

    static std::map<std::size_t, data::Vector::csptr> vectors;
    const auto& it = vectors.find(variant);

    if(it == vectors.cend())
    {
        return vectors.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Vector::sptr newVector(const std::size_t variant = 0)
{
    const auto& vector = data::Vector::New();
    vector->deepCopy(expectedVector(variant));
    return vector;
}

//------------------------------------------------------------------------------

inline static void testVector(const data::Vector::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    auto it = actual->getContainer().cbegin();
    testBoolean(std::dynamic_pointer_cast<data::Boolean>(*it++), variant);
    testInteger(std::dynamic_pointer_cast<data::Integer>(*it++), variant);
    testFloat(std::dynamic_pointer_cast<data::Float>(*it++), variant);
    testString(std::dynamic_pointer_cast<data::String>(*it++), variant);
    testActivitySeries(std::dynamic_pointer_cast<data::ActivitySeries>(*it++), variant);
}

//------------------------------------------------------------------------------

inline static const data::Point::csptr& expectedPoint(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::Point::New();

            const double dvariant = static_cast<double>(variant);
            tmp->setCoord({1.0 + dvariant, 2.0 + dvariant, 3.0 + dvariant});

            return tmp;
        };

    static std::map<std::size_t, data::Point::csptr> points;
    const auto& it = points.find(variant);

    if(it == points.cend())
    {
        return points.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Point::sptr newPoint(const std::size_t variant = 0)
{
    const auto& point = data::Point::New();
    point->deepCopy(expectedPoint(variant));
    return point;
}

//------------------------------------------------------------------------------

inline static void testPoint(const data::Point::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expectedCoord = expectedPoint(variant)->getCoord();
    const auto& actualCoord   = actual->getCoord();
    CPPUNIT_ASSERT_EQUAL(expectedCoord.size(), actualCoord.size());

    for(auto expectedIt = expectedCoord.cbegin(),
        expectedEnd = expectedCoord.cend(),
        actualIt = actualCoord.cbegin(),
        actualEnd = actualCoord.cend() ;
        expectedIt != expectedEnd && actualIt != actualEnd ;
        ++expectedIt, ++actualIt)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(*expectedIt, *actualIt, DOUBLE_EPSILON);
    }
}

//------------------------------------------------------------------------------

inline static const data::PointList::csptr& expectedPointList(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp     = data::PointList::New();
            auto& points = tmp->getPoints();
            for(std::size_t i = 0, end = variant + 3, pointVariant = variant * end ; i < end ; ++i)
            {
                points.push_back(newPoint(pointVariant + i));
            }

            return tmp;
        };

    static std::map<std::size_t, data::PointList::csptr> pointLists;
    const auto& it = pointLists.find(variant);

    if(it == pointLists.cend())
    {
        return pointLists.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::PointList::sptr newPointList(const std::size_t variant = 0)
{
    const auto& pointList = data::PointList::New();
    pointList->deepCopy(expectedPointList(variant));
    return pointList;
}

//------------------------------------------------------------------------------

inline static void testPointList(const data::PointList::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expectedPoints = expectedPointList(variant)->getPoints();
    const auto& actualPoints   = actual->getPoints();
    CPPUNIT_ASSERT_EQUAL(expectedPoints.size(), actualPoints.size());

    for(std::size_t i = 0, end = expectedPoints.size(), pointVariant = variant * end ; i < end ; ++i)
    {
        testPoint(actualPoints.at(i), pointVariant + i);
    }
}

//------------------------------------------------------------------------------

inline static const data::CalibrationInfo::csptr& expectedCalibrationInfo(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::CalibrationInfo::New();

            for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
            {
                // Create the Image
                auto image = newImage(variant + i);

                // Create the PointList
                auto pointList = newPointList(variant + i);

                tmp->addRecord(image, pointList);
            }

            return tmp;
        };

    static std::map<std::size_t, data::CalibrationInfo::csptr> calibrationInfos;
    const auto& it = calibrationInfos.find(variant);

    if(it == calibrationInfos.cend())
    {
        return calibrationInfos.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::CalibrationInfo::sptr newCalibrationInfo(const std::size_t variant = 0)
{
    const auto& calibrationInfo = data::CalibrationInfo::New();
    calibrationInfo->deepCopy(expectedCalibrationInfo(variant));
    return calibrationInfo;
}

//------------------------------------------------------------------------------

inline static void testCalibrationInfo(const data::CalibrationInfo::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        const auto& image = actual->getImage(i);
        testImage(image, variant + i);

        const auto& pointList = actual->getPointList(image);
        testPointList(pointList, variant + i);
    }
}

//------------------------------------------------------------------------------

inline static const data::Camera::csptr& expectedCamera(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::Camera::New();

            const double dvariant = static_cast<double>(variant);
            const float fvariant  = static_cast<float>(variant);

            tmp->setWidth(100 + variant);
            tmp->setHeight(100 + variant);
            tmp->setFx(0.1 + dvariant);
            tmp->setFy(0.2 + dvariant);
            tmp->setCx(0.3 + dvariant);
            tmp->setCy(0.4 + dvariant);
            tmp->setDistortionCoefficient(
                0.5 + dvariant,
                0.6 + dvariant,
                0.7 + dvariant,
                0.8 + dvariant,
                0.9 + dvariant
            );
            tmp->setSkew(0.11 + dvariant);
            tmp->setIsCalibrated(variant % 2 == 0);
            tmp->setDescription(UUID::generateUUID());
            tmp->setCameraID(UUID::generateUUID());
            tmp->setMaximumFrameRate(66.6F + fvariant);
            tmp->setPixelFormat(
                variant % 2 == 0
                ? data::Camera::PixelFormat::AYUV444
                : data::Camera::PixelFormat::BGR24
            );
            tmp->setVideoFile("/" + UUID::generateUUID());
            tmp->setStreamUrl(UUID::generateUUID());
            tmp->setCameraSource(
                variant % 2 == 0
                ? data::Camera::SourceType::DEVICE
                : data::Camera::SourceType::UNKNOWN
            );
            tmp->setScale(0.789 + dvariant);

            return tmp;
        };

    static std::map<std::size_t, data::Camera::csptr> cameras;
    const auto& it = cameras.find(variant);

    if(it == cameras.cend())
    {
        return cameras.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Camera::sptr newCamera(const std::size_t variant = 0)
{
    const auto& camera = data::Camera::New();
    camera->deepCopy(expectedCamera(variant));
    return camera;
}

//------------------------------------------------------------------------------

inline static void testCamera(const data::Camera::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedCamera(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getWidth(), actual->getWidth());
    CPPUNIT_ASSERT_EQUAL(expected->getHeight(), actual->getHeight());

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->getFx(), actual->getFx(), DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->getFy(), actual->getFy(), DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->getCx(), actual->getCx(), DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->getCy(), actual->getCy(), DOUBLE_EPSILON);

    const auto& expectedCoefficient = expected->getDistortionCoefficient();
    const auto& actualCoefficient   = actual->getDistortionCoefficient();
    CPPUNIT_ASSERT_EQUAL(expectedCoefficient.size(), actualCoefficient.size());
    for(std::size_t i = 0, end = expectedCoefficient.size() ; i < end ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedCoefficient[i], actualCoefficient[i], DOUBLE_EPSILON);
    }

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->getSkew(), actual->getSkew(), DOUBLE_EPSILON);
    CPPUNIT_ASSERT_EQUAL(expected->getIsCalibrated(), actual->getIsCalibrated());
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());
    CPPUNIT_ASSERT_EQUAL(expected->getCameraID(), actual->getCameraID());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->getMaximumFrameRate(), actual->getMaximumFrameRate(), FLOAT_EPSILON);
    CPPUNIT_ASSERT_EQUAL(expected->getPixelFormat(), actual->getPixelFormat());
    CPPUNIT_ASSERT_EQUAL(expected->getVideoFile(), actual->getVideoFile());
    CPPUNIT_ASSERT_EQUAL(expected->getStreamUrl(), actual->getStreamUrl());
    CPPUNIT_ASSERT_EQUAL(expected->getCameraSource(), actual->getCameraSource());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->getScale(), actual->getScale(), DOUBLE_EPSILON);
}

//------------------------------------------------------------------------------

inline static const data::CameraSeries::csptr& expectedCameraSeries(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::CameraSeries::New();

            for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
            {
                tmp->addCamera(newCamera(variant + i));
            }

            return tmp;
        };

    static std::map<std::size_t, data::CameraSeries::csptr> cameraSeries;
    const auto& it = cameraSeries.find(variant);

    if(it == cameraSeries.cend())
    {
        return cameraSeries.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::CameraSeries::sptr newCameraSeries(const std::size_t variant = 0)
{
    const auto& cameraSeries = data::CameraSeries::New();
    cameraSeries->deepCopy(expectedCameraSeries(variant));
    return cameraSeries;
}

//------------------------------------------------------------------------------

inline static void testCameraSeries(const data::CameraSeries::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    for(std::size_t i = 0, end = actual->getNumberOfCameras() ; i < end ; ++i)
    {
        testCamera(actual->getCamera(i), variant + i);
    }
}

//------------------------------------------------------------------------------

inline static const data::Color::csptr& expectedColor(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp             = data::Color::New();
            const float fvariant = static_cast<float>(variant);
            tmp->setRGBA(0.1F + fvariant, 0.2F + fvariant, 0.3F + fvariant, 0.4F + fvariant);

            return tmp;
        };

    static std::map<std::size_t, data::Color::csptr> colors;
    const auto& it = colors.find(variant);

    if(it == colors.cend())
    {
        return colors.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Color::sptr newColor(const std::size_t variant = 0)
{
    const auto& color = data::Color::New();
    color->deepCopy(expectedColor(variant));
    return color;
}

//------------------------------------------------------------------------------

inline static void testColor(const data::Color::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedColor(variant);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->red(), actual->red(), FLOAT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->green(), actual->green(), FLOAT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->blue(), actual->blue(), FLOAT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->alpha(), actual->alpha(), FLOAT_EPSILON);
}

//------------------------------------------------------------------------------

inline static const data::Edge::csptr& expectedEdge(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::Edge::New();
            tmp->setNature(UUID::generateUUID());
            tmp->setIdentifiers(UUID::generateUUID(), UUID::generateUUID());

            return tmp;
        };

    static std::map<std::size_t, data::Edge::csptr> edges;
    const auto& it = edges.find(variant);

    if(it == edges.cend())
    {
        return edges.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Edge::sptr newEdge(const std::size_t variant = 0)
{
    const auto& edge = data::Edge::New();
    edge->deepCopy(expectedEdge(variant));
    return edge;
}

//------------------------------------------------------------------------------

inline static void testEdge(const data::Edge::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedEdge(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getFromPortID(), actual->getFromPortID());
    CPPUNIT_ASSERT_EQUAL(expected->getToPortID(), actual->getToPortID());
    CPPUNIT_ASSERT_EQUAL(expected->getNature(), actual->getNature());
}

//------------------------------------------------------------------------------

inline static const data::Port::csptr& expectedPort(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::Port::New();
            tmp->setIdentifier(UUID::generateUUID());
            tmp->setType(UUID::generateUUID());

            return tmp;
        };

    static std::map<std::size_t, data::Port::csptr> ports;
    const auto& it = ports.find(variant);

    if(it == ports.cend())
    {
        return ports.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Port::sptr newPort(const std::size_t variant = 0)
{
    const auto& port = data::Port::New();
    port->deepCopy(expectedPort(variant));
    return port;
}

//------------------------------------------------------------------------------

inline static void testPort(const data::Port::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedPort(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getIdentifier(), actual->getIdentifier());
    CPPUNIT_ASSERT_EQUAL(expected->getType(), actual->getType());
}

//------------------------------------------------------------------------------

inline static const data::Node::csptr& expectedNode(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::Node::New();
            tmp->setObject(newString(variant));

            const std::size_t portCount = variant + 2;
            for(std::size_t i = 0, end = portCount ; i < end ; ++i)
            {
                tmp->addInputPort(newPort(variant + i));
            }

            for(std::size_t i = portCount, end = 2 * (portCount) ; i < end ; ++i)
            {
                tmp->addOutputPort(newPort(variant + i));
            }

            return tmp;
        };

    static std::map<std::size_t, data::Node::csptr> nodes;
    const auto& it = nodes.find(variant);

    if(it == nodes.cend())
    {
        return nodes.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Node::sptr newNode(const std::size_t variant = 0)
{
    const auto& node = data::Node::New();
    node->deepCopy(expectedNode(variant));
    return node;
}

//------------------------------------------------------------------------------

inline static void testNode(const data::Node::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    testString(std::dynamic_pointer_cast<data::String>(actual->getObject()), variant);

    const auto& inputs = actual->getInputPorts();
    for(std::size_t i = 0, end = inputs.size() ; i < end ; ++i)
    {
        testPort(inputs.at(i), variant + i);
    }

    const auto& outputs = actual->getOutputPorts();
    for(std::size_t i = 0, end = outputs.size() ; i < end ; ++i)
    {
        testPort(outputs.at(i), inputs.size() + variant + i);
    }
}

//------------------------------------------------------------------------------

inline static const data::Graph::csptr& expectedGraph(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::Graph::New();

            for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
            {
                auto upNode   = newNode(variant + i);
                auto downNode = newNode(variant + i + 1);

                tmp->addNode(upNode);
                tmp->addNode(downNode);
                tmp->addEdge(newEdge(variant + i), upNode, downNode);
            }

            return tmp;
        };

    static std::map<std::size_t, data::Graph::csptr> graphs;
    const auto& it = graphs.find(variant);

    if(it == graphs.cend())
    {
        return graphs.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Graph::sptr newGraph(const std::size_t variant = 0)
{
    const auto& graph = data::Graph::New();
    graph->deepCopy(expectedGraph(variant));
    return graph;
}

//------------------------------------------------------------------------------

inline static void testGraph(const data::Graph::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    std::size_t i = 0;
    for(const auto& connection : actual->getConnections())
    {
        testEdge(connection.first, i + variant);
        testNode(connection.second.first, i + variant);
        testNode(connection.second.second, ++i + variant);
    }
}

//------------------------------------------------------------------------------

inline static const data::Histogram::csptr& expectedHistogram(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::Histogram::New();

            std::vector<long> values;
            for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
            {
                values.push_back(static_cast<long>(i + variant));
            }

            tmp->setValues(values);

            const float fvariant = static_cast<float>(variant);
            tmp->setBinsWidth(0.1F + fvariant);
            tmp->setMaxValue(0.2F + fvariant);
            tmp->setMinValue(0.3F + fvariant);

            return tmp;
        };

    static std::map<std::size_t, data::Histogram::csptr> histograms;
    const auto& it = histograms.find(variant);

    if(it == histograms.cend())
    {
        return histograms.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Histogram::sptr newHistogram(const std::size_t variant = 0)
{
    const auto& histogram = data::Histogram::New();
    histogram->deepCopy(expectedHistogram(variant));
    return histogram;
}

//------------------------------------------------------------------------------

inline static void testHistogram(const data::Histogram::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedHistogram(variant);

    const auto& expectedValues = expected->getValues();
    const auto& actualValues   = actual->getValues();
    CPPUNIT_ASSERT_EQUAL(expectedValues.size(), actualValues.size());

    for(size_t i = 0, end = expectedValues.size() ; i < end ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(expectedValues.at(i), actualValues.at(i));
    }

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->getBinsWidth(), actual->getBinsWidth(), FLOAT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->getMaxValue(), actual->getMaxValue(), FLOAT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->getMinValue(), actual->getMinValue(), FLOAT_EPSILON);
}

//------------------------------------------------------------------------------

inline static const data::Landmarks::csptr& expectedLandmarks(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::Landmarks::New();

            const float fvariant  = static_cast<float>(variant);
            const double dvariant = static_cast<double>(variant);

            for(std::size_t i = 0, endi = variant + 2 ; i < endi ; ++i)
            {
                const std::string name = UUID::generateUUID();

                tmp->addGroup(
                    name,
                    {1.0F + fvariant, 2.0F + fvariant, 3.0F + fvariant, 4.0F + fvariant},
                    fvariant,
                    variant % 2 == 0 ? data::Landmarks::Shape::CUBE : data::Landmarks::Shape::SPHERE,
                    variant % 2 == 0
                );

                for(std::size_t j = 0, endj = variant + 2 ; j < endj ; ++j)
                {
                    const double dj = static_cast<double>(j);
                    tmp->addPoint(name, {1.0 + dvariant + dj, 2.0 + dvariant + dj, 3.0 + dvariant + dj});
                }
            }

            return tmp;
        };

    static std::map<std::size_t, data::Landmarks::csptr> landmarks;
    const auto& it = landmarks.find(variant);

    if(it == landmarks.cend())
    {
        return landmarks.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Landmarks::sptr newLandmarks(const std::size_t variant = 0)
{
    const auto& landmarks = data::Landmarks::New();
    landmarks->deepCopy(expectedLandmarks(variant));
    return landmarks;
}

//------------------------------------------------------------------------------

inline static void testLandmarks(const data::Landmarks::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedLandmarks(variant);

    const auto& expectedGroupNames = expected->getGroupNames();
    const auto& actualGroupNames   = actual->getGroupNames();

    CPPUNIT_ASSERT_EQUAL(expectedGroupNames.size(), actualGroupNames.size());

    for(const auto& name : expectedGroupNames)
    {
        // Test name
        CPPUNIT_ASSERT_NO_THROW(expected->getGroup(name));
        const auto& expectedGroup = expected->getGroup(name);

        CPPUNIT_ASSERT_NO_THROW(actual->getGroup(name));
        const auto& actualGroup = actual->getGroup(name);

        // Test color
        for(size_t i = 0, end = expectedGroup.m_color.size() ; i < end ; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedGroup.m_color[i], actualGroup.m_color[i], FLOAT_EPSILON);
        }

        // Test size
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedGroup.m_size, actualGroup.m_size, FLOAT_EPSILON);

        // Test shape
        CPPUNIT_ASSERT_EQUAL(expectedGroup.m_shape, actualGroup.m_shape);

        // Test visibility
        CPPUNIT_ASSERT_EQUAL(expectedGroup.m_visibility, actualGroup.m_visibility);

        // Test points
        for(std::size_t i = 0, endi = expectedGroup.m_points.size() ; i < endi ; ++i)
        {
            for(std::size_t j = 0, endj = expectedGroup.m_points[i].size() ; j < endj ; ++j)
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL(
                    expectedGroup.m_points[i][j],
                    actualGroup.m_points[i][j],
                    DOUBLE_EPSILON
                );
            }
        }
    }
}

//------------------------------------------------------------------------------

inline static const data::Line::csptr& expectedLine(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::Line::New();

            tmp->setPosition(newPoint(variant));
            tmp->setDirection(newPoint(variant + 1));

            return tmp;
        };

    static std::map<std::size_t, data::Line::csptr> lines;
    const auto& it = lines.find(variant);

    if(it == lines.cend())
    {
        return lines.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Line::sptr newLine(const std::size_t variant = 0)
{
    const auto& line = data::Line::New();
    line->deepCopy(expectedLine(variant));
    return line;
}

//------------------------------------------------------------------------------

inline static void testLine(const data::Line::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    testPoint(actual->getPosition(), variant);
    testPoint(actual->getDirection(), variant + 1);
}

//------------------------------------------------------------------------------

inline static const data::List::csptr& expectedList(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp        = data::List::New();
            auto& container = tmp->getContainer();
            container.push_back(newBoolean(variant));
            container.push_back(newInteger(variant));
            container.push_back(newFloat(variant));
            container.push_back(newString(variant));
            container.push_back(newActivitySeries(variant));

            return tmp;
        };

    static std::map<std::size_t, data::List::csptr> lists;
    const auto& it = lists.find(variant);

    if(it == lists.cend())
    {
        return lists.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::List::sptr newList(const std::size_t variant = 0)
{
    const auto& list = data::List::New();
    list->deepCopy(expectedList(variant));
    return list;
}

//------------------------------------------------------------------------------

inline static void testList(const data::List::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    auto it = actual->getContainer().cbegin();
    testBoolean(std::dynamic_pointer_cast<data::Boolean>(*it++), variant);
    testInteger(std::dynamic_pointer_cast<data::Integer>(*it++), variant);
    testFloat(std::dynamic_pointer_cast<data::Float>(*it++), variant);
    testString(std::dynamic_pointer_cast<data::String>(*it++), variant);
    testActivitySeries(std::dynamic_pointer_cast<data::ActivitySeries>(*it++), variant);
}

//------------------------------------------------------------------------------

inline static const data::Material::csptr& expectedMaterial(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            data::Material::ShadingType shading[] = {
                data::Material::ShadingType::AMBIENT,
                data::Material::ShadingType::FLAT,
                data::Material::ShadingType::GOURAUD,
                data::Material::ShadingType::PHONG
            };

            data::Material::RepresentationType representation[] = {
                data::Material::RepresentationType::EDGE,
                data::Material::RepresentationType::POINT,
                data::Material::RepresentationType::SURFACE,
                data::Material::RepresentationType::WIREFRAME
            };

            data::Material::OptionsType options[] = {
                data::Material::OptionsType::CELLS_NORMALS,
                data::Material::OptionsType::NORMALS,
                data::Material::OptionsType::STANDARD
            };

            auto tmp = data::Material::New();

            // Set ambient color
            tmp->setAmbient(newColor(variant));

            // Set diffuse color
            tmp->setDiffuse(newColor(variant + 1));

            // Set diffuse texture
            tmp->setDiffuseTexture(newImage(variant));

            // Others
            tmp->setShadingMode(shading[variant % std::size(shading)]);
            tmp->setRepresentationMode(representation[variant % std::size(representation)]);
            tmp->setOptionsMode(options[variant % std::size(options)]);
            tmp->setDiffuseTextureFiltering(
                variant % 2 == 0
                ? data::Material::FilteringType::LINEAR
                : data::Material::FilteringType::NEAREST
            );
            tmp->setDiffuseTextureWrapping(
                variant % 2 == 0
                ? data::Material::WrappingType::CLAMP
                : data::Material::WrappingType::REPEAT
            );
            return tmp;
        };

    static std::map<std::size_t, data::Material::csptr> materials;
    const auto& it = materials.find(variant);

    if(it == materials.cend())
    {
        return materials.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Material::sptr newMaterial(const std::size_t variant = 0)
{
    const auto& material = data::Material::New();
    material->deepCopy(expectedMaterial(variant));
    return material;
}

//------------------------------------------------------------------------------

inline static void testMaterial(const data::Material::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedMaterial(variant);

    // Test ambient
    testColor(actual->ambient(), variant);

    // Test diffuse
    testColor(actual->diffuse(), variant + 1);

    // Test diffuse texture
    testImage(actual->getDiffuseTexture(), variant);

    // Test other attributes
    CPPUNIT_ASSERT_EQUAL(expected->getShadingMode(), actual->getShadingMode());
    CPPUNIT_ASSERT_EQUAL(expected->getRepresentationMode(), actual->getRepresentationMode());
    CPPUNIT_ASSERT_EQUAL(expected->getOptionsMode(), actual->getOptionsMode());
    CPPUNIT_ASSERT_EQUAL(expected->getDiffuseTextureFiltering(), actual->getDiffuseTextureFiltering());
    CPPUNIT_ASSERT_EQUAL(expected->getDiffuseTextureWrapping(), actual->getDiffuseTextureWrapping());
}

//------------------------------------------------------------------------------

inline static const data::Matrix4::csptr& expectedMatrix4(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp           = data::Matrix4::New();
            auto& coefficients = tmp->getCoefficients();

            double counter = 0.0001;
            for(auto it = coefficients.begin(), end = coefficients.end() ; it != end ; ++it)
            {
                *it = static_cast<double>(variant) + counter++;
            }

            return tmp;
        };

    static std::map<std::size_t, data::Matrix4::csptr> matrix4s;
    const auto& it = matrix4s.find(variant);

    if(it == matrix4s.cend())
    {
        return matrix4s.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Matrix4::sptr newMatrix4(const std::size_t variant = 0)
{
    const auto& matrix4 = data::Matrix4::New();
    matrix4->deepCopy(expectedMatrix4(variant));
    return matrix4;
}

//------------------------------------------------------------------------------

inline static void testMatrix4(const data::Matrix4::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected             = expectedMatrix4(variant);
    const auto& expectedCoefficients = expected->getCoefficients();
    const auto& actualCoefficients   = actual->getCoefficients();

    CPPUNIT_ASSERT_EQUAL(expectedCoefficients.size(), actualCoefficients.size());

    for(std::size_t i = 0, end = expectedCoefficients.size() ; i != end ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedCoefficients[i], actualCoefficients[i], DOUBLE_EPSILON);
    }
}

//------------------------------------------------------------------------------

inline static const data::Plane::csptr& expectedPlane(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp     = data::Plane::New();
            auto& points = tmp->getPoints();

            for(std::size_t i = 0, end = points.size() ; i < end ; ++i)
            {
                points[i] = newPoint(i + variant);
            }

            return tmp;
        };

    static std::map<std::size_t, data::Plane::csptr> planes;
    const auto& it = planes.find(variant);

    if(it == planes.cend())
    {
        return planes.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Plane::sptr newPlane(const std::size_t variant = 0)
{
    const auto& plane = data::Plane::New();
    plane->deepCopy(expectedPlane(variant));
    return plane;
}

//------------------------------------------------------------------------------

inline static void testPlane(const data::Plane::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& points = actual->getPoints();

    for(std::size_t i = 0, end = points.size() ; i < end ; ++i)
    {
        testPoint(points.at(i), i + variant);
    }
}

//------------------------------------------------------------------------------

inline static const data::PlaneList::csptr& expectedPlaneList(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp     = data::PlaneList::New();
            auto& planes = tmp->getPlanes();

            for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
            {
                planes.push_back(newPlane(variant + i));
            }

            return tmp;
        };

    static std::map<std::size_t, data::PlaneList::csptr> planelists;
    const auto& it = planelists.find(variant);

    if(it == planelists.cend())
    {
        return planelists.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::PlaneList::sptr newPlaneList(const std::size_t variant = 0)
{
    const auto& planelist = data::PlaneList::New();
    planelist->deepCopy(expectedPlaneList(variant));
    return planelist;
}

//------------------------------------------------------------------------------

inline static void testPlaneList(const data::PlaneList::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& planes = actual->getPlanes();
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        testPlane(planes.at(i), variant + i);
    }
}

//------------------------------------------------------------------------------

inline static const data::ProcessObject::csptr& expectedProcessObject(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::ProcessObject::New();

            tmp->setInputValue(data::Boolean::classname(), newBoolean(variant));
            tmp->setInputValue(data::Integer::classname(), newInteger(variant));

            tmp->setOutputValue(data::Float::classname(), newFloat(variant));
            tmp->setOutputValue(data::String::classname(), newString(variant));

            return tmp;
        };

    static std::map<std::size_t, data::ProcessObject::csptr> processobjects;
    const auto& it = processobjects.find(variant);

    if(it == processobjects.cend())
    {
        return processobjects.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::ProcessObject::sptr newProcessObject(const std::size_t variant = 0)
{
    const auto& processobject = data::ProcessObject::New();
    processobject->deepCopy(expectedProcessObject(variant));
    return processobject;
}

//------------------------------------------------------------------------------

inline static void testProcessObject(const data::ProcessObject::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    testBoolean(actual->getInput<const data::Boolean>(data::Boolean::classname()), variant);
    testInteger(actual->getInput<data::Integer>(data::Integer::classname()), variant);

    testFloat(actual->getOutput<data::Float>(data::Float::classname()), variant);
    testString(actual->getOutput<data::String>(data::String::classname()), variant);
}

//------------------------------------------------------------------------------

inline static const data::Reconstruction::csptr& expectedReconstruction(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::Reconstruction::New();

            tmp->setIsVisible(variant % 2 == 0);
            tmp->setOrganName(UUID::generateUUID());
            tmp->setStructureType(UUID::generateUUID());
            tmp->setComputedMaskVolume(666.66 + static_cast<double>(variant));

            tmp->setMaterial(newMaterial(variant));

            // Image
            tmp->setImage(newImage(variant));

            // Mesh
            tmp->setMesh(newMesh(variant));

            return tmp;
        };

    static std::map<std::size_t, data::Reconstruction::csptr> reconstructions;
    const auto& it = reconstructions.find(variant);

    if(it == reconstructions.cend())
    {
        return reconstructions.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Reconstruction::sptr newReconstruction(const std::size_t variant = 0)
{
    const auto& reconstruction = data::Reconstruction::New();
    reconstruction->deepCopy(expectedReconstruction(variant));
    return reconstruction;
}

//------------------------------------------------------------------------------

inline static void testReconstruction(const data::Reconstruction::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedReconstruction(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getIsVisible(), actual->getIsVisible());
    CPPUNIT_ASSERT_EQUAL(expected->getOrganName(), actual->getOrganName());
    CPPUNIT_ASSERT_EQUAL(expected->getStructureType(), actual->getStructureType());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        expected->getComputedMaskVolume(),
        actual->getComputedMaskVolume(),
        DOUBLE_EPSILON
    );

    testMaterial(actual->getMaterial(), variant);

    // Image
    testImage(actual->getImage(), variant);

    // Mesh
    testMesh(actual->getMesh(), variant);
}

//------------------------------------------------------------------------------

inline static const data::StructureTraits::csptr& expectedStructureTraits(const std::size_t variant = 0)
{
    const data::StructureTraits::StructureClass CLASSES[] = {
        data::StructureTraits::StructureClass::ENVIRONMENT,
        data::StructureTraits::StructureClass::FUNCTIONAL,
        data::StructureTraits::StructureClass::LESION,
        data::StructureTraits::StructureClass::NO_CONSTRAINT,
        data::StructureTraits::StructureClass::ORGAN,
        data::StructureTraits::StructureClass::TOOL,
        data::StructureTraits::StructureClass::VESSEL
    };

    const data::StructureTraits::Category CATEGORIES[] = {
        data::StructureTraits::Category::ABDOMEN,
        data::StructureTraits::Category::ARM,
        data::StructureTraits::Category::BODY,
        data::StructureTraits::Category::HEAD,
        data::StructureTraits::Category::LEG,
        data::StructureTraits::Category::LIVER_SEGMENTS,
        data::StructureTraits::Category::NECK,
        data::StructureTraits::Category::OTHER,
        data::StructureTraits::Category::PELVIS,
        data::StructureTraits::Category::THORAX
    };

    const auto& generator =
        [&]
        {
            auto tmp = data::StructureTraits::New();

            tmp->setType(UUID::generateUUID());
            tmp->setClass(CLASSES[variant % std::size(CLASSES)]);
            tmp->setNativeExp(UUID::generateUUID());
            tmp->setNativeGeometricExp(UUID::generateUUID());
            tmp->setAttachmentType(UUID::generateUUID());
            tmp->setAnatomicRegion(UUID::generateUUID());
            tmp->setPropertyCategory(UUID::generateUUID());
            tmp->setPropertyType(UUID::generateUUID());

            // Categories
            auto& categories = tmp->getCategories();
            for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
            {
                categories.push_back(CATEGORIES[(i + variant) % std::size(CATEGORIES)]);
            }

            // Color
            tmp->setColor(newColor(variant));

            return tmp;
        };

    static std::map<std::size_t, data::StructureTraits::csptr> structureTraits;
    const auto& it = structureTraits.find(variant);

    if(it == structureTraits.cend())
    {
        return structureTraits.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::StructureTraits::sptr newStructureTraits(const std::size_t variant = 0)
{
    const auto& structuretraits = data::StructureTraits::New();
    structuretraits->deepCopy(expectedStructureTraits(variant));
    return structuretraits;
}

//------------------------------------------------------------------------------

inline static void testStructureTraits(const data::StructureTraits::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedStructureTraits(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getType(), actual->getType());
    CPPUNIT_ASSERT_EQUAL(expected->getClass(), actual->getClass());
    CPPUNIT_ASSERT_EQUAL(expected->getNativeExp(), actual->getNativeExp());
    CPPUNIT_ASSERT_EQUAL(expected->getNativeGeometricExp(), actual->getNativeGeometricExp());
    CPPUNIT_ASSERT_EQUAL(expected->getAttachmentType(), actual->getAttachmentType());
    CPPUNIT_ASSERT_EQUAL(expected->getAnatomicRegion(), actual->getAnatomicRegion());
    CPPUNIT_ASSERT_EQUAL(expected->getPropertyCategory(), actual->getPropertyCategory());
    CPPUNIT_ASSERT_EQUAL(expected->getPropertyType(), actual->getPropertyType());

    // Categories
    const auto& expectedCategories = expected->getCategories();
    const auto& actualCategories   = actual->getCategories();
    CPPUNIT_ASSERT_EQUAL(expectedCategories.size(), actualCategories.size());

    for(std::size_t i = 0, end = expectedCategories.size() ; i < end ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(expectedCategories[i], actualCategories[i]);
    }

    // Color
    testColor(actual->getColor(), variant);
}

//------------------------------------------------------------------------------

inline static const data::ReconstructionTraits::csptr& expectedReconstructionTraits(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::ReconstructionTraits::New();

            tmp->setIdentifier(UUID::generateUUID());

            // Reconstruction mask operator node
            tmp->setMaskOpNode(newNode(variant));

            // Reconstruction mesh operator node
            tmp->setMeshOpNode(newNode(variant + 1));

            // Associated structure traits
            tmp->setStructureTraits(newStructureTraits(variant));

            return tmp;
        };

    static std::map<std::size_t, data::ReconstructionTraits::csptr> reconstructionTraits;
    const auto& it = reconstructionTraits.find(variant);

    if(it == reconstructionTraits.cend())
    {
        return reconstructionTraits.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::ReconstructionTraits::sptr newReconstructionTraits(const std::size_t variant = 0)
{
    const auto& reconstructionTraits = data::ReconstructionTraits::New();
    reconstructionTraits->deepCopy(expectedReconstructionTraits(variant));
    return reconstructionTraits;
}

//------------------------------------------------------------------------------

inline static void testReconstructionTraits(
    const data::ReconstructionTraits::csptr& actual,
    const std::size_t variant = 0
)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedReconstructionTraits(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getIdentifier(), actual->getIdentifier());

    // Reconstruction mask operator node
    testNode(actual->getMaskOpNode(), variant);

    // Reconstruction mesh operator node
    testNode(actual->getMeshOpNode(), variant + 1);

    // Associated structure traits
    testStructureTraits(actual->getStructureTraits(), variant);
}

//------------------------------------------------------------------------------

inline static const data::Resection::csptr& expectedResection(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::Resection::New();

            tmp->setName(UUID::generateUUID());
            tmp->setIsSafePart(variant % 2 == 0);
            tmp->setIsValid(variant % 3 == 0);
            tmp->setIsVisible(variant % 4 == 0);

            auto& inputs  = tmp->getInputs();
            auto& outputs = tmp->getOutputs();
            for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
            {
                inputs.push_back(newReconstruction(variant + i));
                outputs.push_back(newReconstruction(variant + i + 1));
            }

            return tmp;
        };

    static std::map<std::size_t, data::Resection::csptr> resections;
    const auto& it = resections.find(variant);

    if(it == resections.cend())
    {
        return resections.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Resection::sptr newResection(const std::size_t variant = 0)
{
    const auto& resection = data::Resection::New();
    resection->deepCopy(expectedResection(variant));
    return resection;
}

//------------------------------------------------------------------------------

inline static void testResection(
    const data::Resection::csptr& actual,
    const std::size_t variant = 0
)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedResection(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getName(), actual->getName());
    CPPUNIT_ASSERT_EQUAL(expected->getIsSafePart(), actual->getIsSafePart());
    CPPUNIT_ASSERT_EQUAL(expected->getIsValid(), actual->getIsValid());
    CPPUNIT_ASSERT_EQUAL(expected->getIsVisible(), actual->getIsVisible());

    auto& inputs  = actual->getInputs();
    auto& outputs = actual->getOutputs();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        testReconstruction(inputs[i], variant + i);
        testReconstruction(outputs[i], variant + i + 1);
    }
}

//------------------------------------------------------------------------------

inline static const data::ResectionDB::csptr& expectedResectionDB(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::ResectionDB::New();

            tmp->setSafeResection(newResection(variant));

            for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
            {
                tmp->addResection(newResection(variant + i + 1));
            }

            return tmp;
        };

    static std::map<std::size_t, data::ResectionDB::csptr> resectionDBs;
    const auto& it = resectionDBs.find(variant);

    if(it == resectionDBs.cend())
    {
        return resectionDBs.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::ResectionDB::sptr newResectionDB(const std::size_t variant = 0)
{
    const auto& resectionDB = data::ResectionDB::New();
    resectionDB->deepCopy(expectedResectionDB(variant));
    return resectionDB;
}

//------------------------------------------------------------------------------

inline static void testResectionDB(
    const data::ResectionDB::csptr& actual,
    const std::size_t variant = 0
)
{
    CPPUNIT_ASSERT(actual);

    testResection(actual->getSafeResection(), variant);

    const auto& resections = actual->getResections();
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        testResection(resections.at(i), variant + i + 1);
    }
}

//------------------------------------------------------------------------------

inline static const data::ROITraits::csptr& expectedROITraits(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::ROITraits::New();

            tmp->setIdentifier(UUID::generateUUID());
            tmp->setEvaluatedExp(UUID::generateUUID());
            tmp->setMaskOpNode(newNode(variant));
            tmp->setStructureTraits(newStructureTraits(variant));

            return tmp;
        };

    static std::map<std::size_t, data::ROITraits::csptr> roiTraits;
    const auto& it = roiTraits.find(variant);

    if(it == roiTraits.cend())
    {
        return roiTraits.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::ROITraits::sptr newROITraits(const std::size_t variant = 0)
{
    const auto& roiTraits = data::ROITraits::New();
    roiTraits->deepCopy(expectedROITraits(variant));
    return roiTraits;
}

//------------------------------------------------------------------------------

inline static void testROITraits(const data::ROITraits::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedROITraits(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getIdentifier(), actual->getIdentifier());
    CPPUNIT_ASSERT_EQUAL(expected->getEvaluatedExp(), actual->getEvaluatedExp());

    // Node
    testNode(actual->getMaskOpNode(), variant);

    // Structure
    testStructureTraits(actual->getStructureTraits(), variant);
}

//------------------------------------------------------------------------------

inline static const data::SeriesDB::csptr& expectedSeriesDB(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp        = data::SeriesDB::New();
            auto& container = tmp->getContainer();

            for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
            {
                container.push_back(newSeries(variant + i));
            }

            return tmp;
        };

    static std::map<std::size_t, data::SeriesDB::csptr> seriesDBs;
    const auto& it = seriesDBs.find(variant);

    if(it == seriesDBs.cend())
    {
        return seriesDBs.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::SeriesDB::sptr newSeriesDB(const std::size_t variant = 0)
{
    const auto& seriesDB = data::SeriesDB::New();
    seriesDB->deepCopy(expectedSeriesDB(variant));
    return seriesDB;
}

//------------------------------------------------------------------------------

inline static void testSeriesDB(const data::SeriesDB::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& container = actual->getContainer();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        testSeries(container.at(i), variant + i);
    }
}

//------------------------------------------------------------------------------

inline static const data::StructureTraitsDictionary::csptr& expectedStructureTraitsDictionary(
    const std::size_t variant = 0
)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::StructureTraitsDictionary::New();

            auto organ = newStructureTraits(variant);
            organ->setClass(data::StructureTraits::ORGAN);
            organ->setAttachmentType("");

            tmp->addStructure(organ);

            for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
            {
                auto structure            = newStructureTraits(variant + i + 1);
                const auto structureClass = structure->getClass();

                if(structureClass != data::StructureTraits::LESION
                   && structureClass != data::StructureTraits::FUNCTIONAL)
                {
                    structure->setAttachmentType("");
                }
                else
                {
                    structure->setAttachmentType(organ->getType());
                }

                tmp->addStructure(structure);
            }

            return tmp;
        };

    static std::map<std::size_t, data::StructureTraitsDictionary::csptr> structureTraitsDictionaries;
    const auto& it = structureTraitsDictionaries.find(variant);

    if(it == structureTraitsDictionaries.cend())
    {
        return structureTraitsDictionaries.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::StructureTraitsDictionary::sptr newStructureTraitsDictionary(const std::size_t variant = 0)
{
    const auto& structureTraitsDictionary = data::StructureTraitsDictionary::New();
    structureTraitsDictionary->deepCopy(expectedStructureTraitsDictionary(variant));
    return structureTraitsDictionary;
}

//------------------------------------------------------------------------------

inline static void testStructureTraitsDictionary(
    const data::StructureTraitsDictionary::csptr& actual,
    const std::size_t variant = 0
)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedStructureTraitsDictionary(variant);

    for(const auto& name : expected->getStructureTypeNames())
    {
        const auto& actualStructure   = actual->getStructure(name);
        const auto& expectedStructure = expected->getStructure(name);

        CPPUNIT_ASSERT_EQUAL(expectedStructure->getType(), actualStructure->getType());
        CPPUNIT_ASSERT_EQUAL(expectedStructure->getClass(), actualStructure->getClass());
        CPPUNIT_ASSERT_EQUAL(expectedStructure->getNativeExp(), actualStructure->getNativeExp());
        CPPUNIT_ASSERT_EQUAL(expectedStructure->getNativeGeometricExp(), actualStructure->getNativeGeometricExp());
        CPPUNIT_ASSERT_EQUAL(expectedStructure->getAttachmentType(), actualStructure->getAttachmentType());
        CPPUNIT_ASSERT_EQUAL(expectedStructure->getAnatomicRegion(), actualStructure->getAnatomicRegion());
        CPPUNIT_ASSERT_EQUAL(expectedStructure->getPropertyCategory(), actualStructure->getPropertyCategory());
        CPPUNIT_ASSERT_EQUAL(expectedStructure->getPropertyType(), actualStructure->getPropertyType());

        // Categories
        const auto& expectedCategories = expectedStructure->getCategories();
        const auto& actualCategories   = actualStructure->getCategories();
        CPPUNIT_ASSERT_EQUAL(expectedCategories.size(), actualCategories.size());

        for(std::size_t i = 0, end = expectedCategories.size() ; i < end ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(expectedCategories[i], actualCategories[i]);
        }

        // Color
        const auto& expectedColor = expectedStructure->getColor();
        const auto& actualColor   = actualStructure->getColor();

        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor->red(), actualColor->red(), FLOAT_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor->green(), actualColor->green(), FLOAT_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor->blue(), actualColor->blue(), FLOAT_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor->alpha(), actualColor->alpha(), FLOAT_EPSILON);
    }
}

//------------------------------------------------------------------------------

inline static const data::Tag::csptr& expectedTag(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::Tag::New();

            tmp->setType(UUID::generateUUID());
            tmp->setSize(static_cast<double>(variant) * 0.666);
            tmp->setPointList(newPointList(variant));

            return tmp;
        };

    static std::map<std::size_t, data::Tag::csptr> tags;
    const auto& it = tags.find(variant);

    if(it == tags.cend())
    {
        return tags.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::Tag::sptr newTag(const std::size_t variant = 0)
{
    const auto& tag = data::Tag::New();
    tag->deepCopy(expectedTag(variant));
    return tag;
}

//------------------------------------------------------------------------------

inline static void testTag(const data::Tag::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedTag(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getType(), actual->getType());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->getSize(), actual->getSize(), DOUBLE_EPSILON);

    testPointList(actual->getPointList(), variant);
}

//------------------------------------------------------------------------------

inline static const data::TransferFunction::csptr& expectedTransferFunction(const std::size_t variant = 0)
{
    const auto& generator =
        [&]
        {
            auto tmp = data::TransferFunction::New();

            const double dVariant = static_cast<double>(variant);

            tmp->setLevel(dVariant * 1.234);
            tmp->setWindow(dVariant * 1.567);
            tmp->setName(UUID::generateUUID());
            tmp->setInterpolationMode(
                variant % 2 == 0
                ? data::TransferFunction::InterpolationMode::LINEAR
                : data::TransferFunction::InterpolationMode::NEAREST
            );
            tmp->setIsClamped(variant % 3 == 0);
            tmp->setBackgroundColor(
                data::TransferFunction::TFColor(
                    dVariant * 0.1,
                    dVariant * 0.2,
                    dVariant * 0.3,
                    dVariant * 0.4
                )
            );

            for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
            {
                const double di = static_cast<double>(i);
                tmp->addTFColor(
                    dVariant + 0.1234 * di,
                    data::TransferFunction::TFColor(
                        dVariant * 0.5 * di,
                        dVariant * 0.6 * di,
                        dVariant * 0.7 * di,
                        dVariant * 0.8 * di
                    )
                );
            }

            return tmp;
        };

    static std::map<std::size_t, data::TransferFunction::csptr> transferfunctions;
    const auto& it = transferfunctions.find(variant);

    if(it == transferfunctions.cend())
    {
        return transferfunctions.insert_or_assign(variant, generator()).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

inline static data::TransferFunction::sptr newTransferFunction(const std::size_t variant = 0)
{
    const auto& transferFunction = data::TransferFunction::New();
    transferFunction->deepCopy(expectedTransferFunction(variant));
    return transferFunction;
}

//------------------------------------------------------------------------------

inline static void testTransferFunction(const data::TransferFunction::csptr& actual, const std::size_t variant = 0)
{
    CPPUNIT_ASSERT(actual);

    const auto& expected = expectedTransferFunction(variant);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->getLevel(), actual->getLevel(), DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->getWindow(), actual->getWindow(), DOUBLE_EPSILON);
    CPPUNIT_ASSERT_EQUAL(expected->getName(), actual->getName());
    CPPUNIT_ASSERT_EQUAL(expected->getInterpolationMode(), actual->getInterpolationMode());
    CPPUNIT_ASSERT_EQUAL(expected->getIsClamped(), actual->getIsClamped());

    const auto& expectedBackgroundColor = expected->getBackgroundColor();
    const auto& actualBackgroundColor   = actual->getBackgroundColor();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedBackgroundColor.r, actualBackgroundColor.r, DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedBackgroundColor.g, actualBackgroundColor.g, DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedBackgroundColor.b, actualBackgroundColor.b, DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedBackgroundColor.a, actualBackgroundColor.a, DOUBLE_EPSILON);

    for(const auto& expectedData : expected->getTFData())
    {
        const auto& expectedColor = expectedData.second;
        const auto& actualColor   = actual->getTFData().at(expectedData.first);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor.r, actualColor.r, DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor.g, actualColor.g, DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor.b, actualColor.b, DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor.a, actualColor.a, DOUBLE_EPSILON);
    }
}

//------------------------------------------------------------------------------

void SessionTest::setUp()
{
}

//------------------------------------------------------------------------------

void SessionTest::tearDown()
{
}

//------------------------------------------------------------------------------

void SessionTest::booleanTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "booleanTest.zip";

    const std::string fieldName(UUID::generateUUID());

    // Test serialization
    {
        // Create the data::Boolean
        auto boolean = newBoolean();

        // Add a field
        boolean->setField(fieldName, newBoolean(1));

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session
        sessionWriter->setObject(boolean);
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
        auto boolean = std::dynamic_pointer_cast<data::Boolean>(sessionReader->getObject());
        testBoolean(boolean);

        // Test field
        auto boolean2 = std::dynamic_pointer_cast<data::Boolean>(boolean->getField(fieldName));
        testBoolean(boolean2, 1);
    }
}

//------------------------------------------------------------------------------

void SessionTest::integerTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "integerTest.zip";

    const std::string fieldName(UUID::generateUUID());

    // Test serialization
    {
        // Create the data::Integer
        auto integer = newInteger();

        // Add a field
        integer->setField(fieldName, newInteger(1));

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session
        sessionWriter->setObject(integer);
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
        auto integer = std::dynamic_pointer_cast<data::Integer>(sessionReader->getObject());
        testInteger(integer);

        // Test field
        auto integer2 = std::dynamic_pointer_cast<data::Integer>(integer->getField(fieldName));
        testInteger(integer2, 1);
    }
}

//------------------------------------------------------------------------------

void SessionTest::floatTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "floatTest.zip";

    const std::string fieldName(UUID::generateUUID());

    // Test serialization
    {
        // Create the data::Float
        auto real = newFloat();

        // Add a field
        real->setField(fieldName, newFloat(1));

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session
        sessionWriter->setObject(real);
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
        auto real = std::dynamic_pointer_cast<data::Float>(sessionReader->getObject());
        testFloat(real);

        // Test field
        auto real2 = std::dynamic_pointer_cast<data::Float>(real->getField(fieldName));
        testFloat(real2, 1);
    }
}

//------------------------------------------------------------------------------

void SessionTest::stringTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "stringTest.zip";

    const std::string fieldName(UUID::generateUUID());

    // Test serialization
    {
        const auto& string = newString();

        // Add a String field with a pure binary string (encrypted text)
        string->setField(fieldName, newString(1));

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(string);
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
        const auto& string = std::dynamic_pointer_cast<data::String>(sessionReader->getObject());
        testString(string);

        // Test field
        const auto& string2 = std::dynamic_pointer_cast<data::String>(string->getField(fieldName));
        testString(string2, 1);
    }
}

//------------------------------------------------------------------------------

void SessionTest::compositeTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "compositeTest.zip";

    // Test serialization
    {
        // Create composite
        auto composite = newComposite();

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
        testComposite(std::dynamic_pointer_cast<data::Composite>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::meshTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "meshTest.zip";

    // Test serialization
    {
        const auto& mesh = newMesh();

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

        // Test value
        testMesh(std::dynamic_pointer_cast<data::Mesh>(sessionReader->getObject()));
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
        const auto& equipment = newEquipment();

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

        // Test value
        testEquipment(std::dynamic_pointer_cast<data::Equipment>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::patientTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "patientTest.zip";

    // Test serialization
    {
        const auto& patient = newPatient();

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

        // Test value
        testPatient(std::dynamic_pointer_cast<data::Patient>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::studyTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "studyTest.zip";

    // Test serialization
    {
        const auto& study = newStudy();

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

        // Test value
        testStudy(std::dynamic_pointer_cast<data::Study>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::seriesTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "seriesTest.zip";

    // Test serialization
    {
        const auto& series = newSeries();

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

        // Test value
        testSeries(std::dynamic_pointer_cast<data::Series>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::activitySeriesTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "activitySeriesTest.zip";

    // Test serialization
    {
        const auto& series = newActivitySeries();

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

        // Test value
        testActivitySeries(std::dynamic_pointer_cast<data::ActivitySeries>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::arrayTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "arrayTest.zip";

    // Test serialization
    {
        // Create the array
        auto array = newArray();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(array);
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
        testArray(std::dynamic_pointer_cast<data::Array>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::imageTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "imageTest.zip";

    // Test serialization
    {
        // Create the image
        auto image = newImage();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(image);
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
        testImage(std::dynamic_pointer_cast<data::Image>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::vectorTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "vectorTest.zip";

    const std::string expectedString(UUID::generateUUID());

    // Test serialization
    {
        // Create vector
        auto vector = newVector();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(vector);
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
        testVector(std::dynamic_pointer_cast<data::Vector>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::pointTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "pointTest.zip";

    // Test serialization
    {
        // Create vector
        auto point = newPoint();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(point);
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
        testPoint(std::dynamic_pointer_cast<data::Point>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::pointListTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "pointListTest.zip";

    // Test serialization
    {
        // Create Pointlist
        auto pointList = newPointList();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(pointList);
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
        testPointList(std::dynamic_pointer_cast<data::PointList>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::calibrationInfoTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "calibrationInfoTest.zip";

    // Test serialization
    {
        // Create the CalibrationInfo
        auto calibrationInfo = newCalibrationInfo();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(calibrationInfo);
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
        testCalibrationInfo(std::dynamic_pointer_cast<data::CalibrationInfo>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::cameraTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "cameraTest.zip";

    // Test serialization
    {
        auto camera = newCamera();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(camera);
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
        testCamera(std::dynamic_pointer_cast<data::Camera>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::cameraSeriesTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "cameraSeriesTest.zip";

    // Test serialization
    {
        auto cameraSeries = newCameraSeries();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(cameraSeries);
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
        testCameraSeries(std::dynamic_pointer_cast<data::CameraSeries>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::colorTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "colorTest.zip";

    // Test serialization
    {
        // Create vector
        auto color = newColor();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(color);
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
        testColor(std::dynamic_pointer_cast<data::Color>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::edgeTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "edgeTest.zip";

    // Test serialization
    {
        // Create vector
        auto edge = newEdge();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(edge);
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
        testEdge(std::dynamic_pointer_cast<data::Edge>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::portTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "portTest.zip";

    const std::string expectedIdentifier(UUID::generateUUID());
    const std::string expectedType(UUID::generateUUID());

    // Test serialization
    {
        // Create vector
        auto port = newPort();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(port);
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
        testPort(std::dynamic_pointer_cast<data::Port>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::nodeTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "nodeTest.zip";

    // Test serialization
    {
        // Create node
        auto node = newNode();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(node);
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
        testNode(std::dynamic_pointer_cast<data::Node>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::graphTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "graphTest.zip";

    // Test serialization
    {
        // Create Graph
        auto graph = newGraph();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(graph);
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
        testGraph(std::dynamic_pointer_cast<data::Graph>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::histogramTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "histogramTest.zip";

    // Test serialization
    {
        // Create histogram
        auto histogram = newHistogram();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(histogram);
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
        testHistogram(std::dynamic_pointer_cast<data::Histogram>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::landmarksTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "landmarksTest.zip";

    // Test serialization
    {
        // Create landmarks
        auto landmarks = newLandmarks();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(landmarks);
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
        testLandmarks(std::dynamic_pointer_cast<data::Landmarks>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::lineTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "lineTest.zip";

    // Test serialization
    {
        // Create a Line
        auto line = newLine();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(line);
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
        testLine(std::dynamic_pointer_cast<data::Line>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::listTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "listTest.zip";

    // Test serialization
    {
        // Create list
        auto list = newList();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(list);
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
        testList(std::dynamic_pointer_cast<data::List>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::materialTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "materialTest.zip";

    // Test serialization
    {
        // Create material
        auto material = newMaterial();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(material);
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
        testMaterial(std::dynamic_pointer_cast<data::Material>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::matrix4Test()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "matrix4Test.zip";

    const std::string fieldName(UUID::generateUUID());

    // Test serialization
    {
        // Create the data::Matrix4
        auto matrix = newMatrix4();

        // Test Field
        matrix->setField(fieldName, newMatrix4(1));

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session
        sessionWriter->setObject(matrix);
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

        // Test Value
        auto matrix = std::dynamic_pointer_cast<data::Matrix4>(sessionReader->getObject());
        testMatrix4(matrix);

        // Test Field
        testMatrix4(std::dynamic_pointer_cast<data::Matrix4>(matrix->getField(fieldName)), 1);
    }
}

//------------------------------------------------------------------------------

void SessionTest::planeTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "planeTest.zip";

    // Test serialization
    {
        // Create plane
        auto plane = newPlane();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(plane);
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
        testPlane(std::dynamic_pointer_cast<data::Plane>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::planeListTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "planeListTest.zip";

    // Test serialization
    {
        // Create plane
        auto planeList = newPlaneList();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(planeList);
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
        testPlaneList(std::dynamic_pointer_cast<data::PlaneList>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::processObjectTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "processObjectTest.zip";

    // Test serialization
    {
        // Create processObject
        auto processObject = newProcessObject();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(processObject);
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
        testProcessObject(std::dynamic_pointer_cast<data::ProcessObject>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::reconstructionTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "reconstructionTest.zip";

    // Test serialization
    {
        // Create reconstruction
        auto reconstruction = newReconstruction();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(reconstruction);
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
        testReconstruction(std::dynamic_pointer_cast<data::Reconstruction>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::structureTraitsTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "structureTraitsTest.zip";

    // Test serialization
    {
        // Create reconstruction
        auto structure = newStructureTraits();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(structure);
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
        testStructureTraits(std::dynamic_pointer_cast<data::StructureTraits>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::reconstructionTraitsTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "reconstructionTraitsTest.zip";

    // Test serialization
    {
        // Create reconstruction
        auto reconstructionTraits = newReconstructionTraits();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(reconstructionTraits);
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
        testReconstructionTraits(std::dynamic_pointer_cast<data::ReconstructionTraits>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::resectionTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "resectionTest.zip";

    // Test serialization
    {
        // Create resection
        auto resection = newResection();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(resection);
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
        testResection(std::dynamic_pointer_cast<data::Resection>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::resectionDBTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "resectionDBTest.zip";

    // Test serialization
    {
        // Create resectionDB
        auto resectionDB = newResectionDB();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(resectionDB);
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
        testResectionDB(std::dynamic_pointer_cast<data::ResectionDB>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::roiTraitsTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "roiTraitsTest.zip";

    // Test serialization
    {
        // Create roiTraits
        auto roiTraits = newROITraits();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(roiTraits);
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
        testROITraits(std::dynamic_pointer_cast<data::ROITraits>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::seriesDBTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "seriesDBTest.zip";

    // Test serialization
    {
        // Create SeriesDB
        auto seriesDB = newSeriesDB();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(seriesDB);
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
        testSeriesDB(std::dynamic_pointer_cast<data::SeriesDB>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::structureTraitsDictionaryTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "structureTraitsDictionaryTest.zip";

    // Test serialization
    {
        // Create reconstruction
        auto structure = newStructureTraitsDictionary();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(structure);
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
        testStructureTraitsDictionary(
            std::dynamic_pointer_cast<data::StructureTraitsDictionary>(sessionReader->getObject())
        );
    }
}

//------------------------------------------------------------------------------

void SessionTest::tagTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "tagTest.zip";

    // Test serialization
    {
        // Create reconstruction
        auto structure = newTag();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(structure);
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
        testTag(std::dynamic_pointer_cast<data::Tag>(sessionReader->getObject()));
    }
}

//------------------------------------------------------------------------------

void SessionTest::transferFunctionTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "transferFunctionTest.zip";

    const std::string fieldName(UUID::generateUUID());

    // Test serialization
    {
        // Create the data::TransferFunction
        auto transferFunction = newTransferFunction();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session
        sessionWriter->setObject(transferFunction);
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
        testTransferFunction(std::dynamic_pointer_cast<data::TransferFunction>(sessionReader->getObject()));
    }
}

} // namespace ut

} // namespace sight::io::session
