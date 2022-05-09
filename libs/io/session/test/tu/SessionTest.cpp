/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include <core/crypto/Base64.hpp>
#include <core/tools/System.hpp>
#include <core/tools/UUID.hpp>

#include <data/ActivitySeries.hpp>
#include <data/ActivitySet.hpp>
#include <data/Array.hpp>
#include <data/Boolean.hpp>
#include <data/CalibrationInfo.hpp>
#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>
#include <data/CameraSet.hpp>
#include <data/Color.hpp>
#include <data/Composite.hpp>
#include <data/DicomSeries.hpp>
#include <data/Edge.hpp>
#include <data/Equipment.hpp>
#include <data/Float.hpp>
#include <data/Graph.hpp>
#include <data/IContainer.hxx>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Integer.hpp>
#include <data/Landmarks.hpp>
#include <data/Line.hpp>
#include <data/Material.hpp>
#include <data/Matrix4.hpp>
#include <data/ModelSeries.hpp>
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
#include <data/SeriesSet.hpp>
#include <data/String.hpp>
#include <data/StructureTraits.hpp>
#include <data/StructureTraitsDictionary.hpp>
#include <data/Study.hpp>
#include <data/Tag.hpp>
#include <data/TransferFunction.hpp>
#include <data/Vector.hpp>

#include <geometry/data/Mesh.hpp>

#include <io/dicom/reader/SeriesDB.hpp>
#include <io/session/detail/SessionDeserializer.hpp>
#include <io/session/detail/SessionSerializer.hpp>
#include <io/session/Helper.hpp>
#include <io/session/SessionReader.hpp>
#include <io/session/SessionWriter.hpp>
#include <io/zip/exception/Read.hpp>
#include <io/zip/exception/Write.hpp>

#include <utest/Filter.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Image.hpp>
#include <utestData/generator/Mesh.hpp>

#include <random>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::session::ut::SessionTest);

namespace sight::io::session
{

namespace ut
{

// For UUID::generateUUID();
using core::tools::UUID;

static constexpr auto FLOAT_EPSILON = std::numeric_limits<float>::epsilon();

static constexpr auto DOUBLE_EPSILON = std::numeric_limits<double>::epsilon();

//------------------------------------------------------------------------------

template<typename T>
inline T random()
{
    using uniform_distribution = typename std::conditional<
        std::is_floating_point<T>::value,
        std::uniform_real_distribution<T>,
        std::uniform_int_distribution<T>
                                 >::type;

    static uniform_distribution distributor(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
    static std::mt19937 generator;

    return distributor(generator);
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

template<typename T>
inline typename T::sptr _generate(const std::size_t)
{
    return T::New(static_cast<typename T::ValueType>(random<typename T::ValueType>()));
}

//------------------------------------------------------------------------------

template<typename T>
inline const typename T::csptr& _expected(const std::size_t variant)
{
    static std::map<std::size_t, typename T::csptr> MAP;
    const auto& it = MAP.find(variant);

    if(it == MAP.cend())
    {
        return MAP.insert_or_assign(variant, _generate<T>(variant)).first->second;
    }
    else
    {
        return it->second;
    }
}

//------------------------------------------------------------------------------

template<typename T>
inline typename T::sptr _new(const std::size_t variant)
{
    const auto& object = T::New();
    object->deepCopy(_expected<T>(variant));
    return object;
}

//------------------------------------------------------------------------------

template<typename T>
inline void _compare(const typename T::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);
    CPPUNIT_ASSERT_EQUAL(_expected<T>(variant)->getValue(), actual->getValue());
}

//------------------------------------------------------------------------------

template<typename T>
inline void _test(const bool encrypt, const bool raw)
{
    static constexpr auto password = "password";

    const auto& test_id = T::leafClassname() + "_" + std::to_string(encrypt) + "_" + std::to_string(raw);

    // Create a temporary directory
    const auto tmpfolder = core::tools::System::getTemporaryFolder() / UUID::generateUUID();
    std::filesystem::remove_all(tmpfolder);
    std::filesystem::create_directories(tmpfolder);
    const auto testPath = tmpfolder / (test_id + (raw ? ".json" : ".zip"));

    static constexpr auto fieldName = "field";

    // Test serialization
    {
        // Create the data object
        auto object = _new<T>(0);

        // Add a field
        object->setField(fieldName, _new<T>(1));

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(object);
        sessionWriter->setFile(testPath);

        if(raw)
        {
            sessionWriter->setArchiveFormat(io::zip::Archive::ArchiveFormat::FILESYSTEM);
        }
        else if(encrypt)
        {
            sessionWriter->set_password(password);
        }

        // Write the session
        CPPUNIT_ASSERT_NO_THROW(sessionWriter->write());

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);

        // Configure the session reader
        sessionReader->setFile(testPath);

        if(raw)
        {
            sessionReader->setArchiveFormat(io::zip::Archive::ArchiveFormat::FILESYSTEM);
        }
        else if(encrypt)
        {
            sessionReader->set_password(password);
        }

        // Read the session
        CPPUNIT_ASSERT_NO_THROW(sessionReader->read());

        // Test value
        auto object = std::dynamic_pointer_cast<T>(sessionReader->getObject());
        _compare<T>(object, 0);

        // Test field
        auto fieldObject = std::dynamic_pointer_cast<T>(object->getField(fieldName));
        _compare<T>(fieldObject, 1);
    }

    std::filesystem::remove_all(tmpfolder);
}

//------------------------------------------------------------------------------

template<typename T>
inline void _test_combine()
{
    _test<T>(false, false);
    _test<T>(false, true);
    _test<T>(true, false);
}

//------------------------------------------------------------------------------

template<>
inline data::Boolean::sptr _generate<data::Boolean>(const std::size_t variant)
{
    return data::Boolean::New(variant % 2 == 0);
}

//------------------------------------------------------------------------------

void SessionTest::booleanTest()
{
    _test_combine<data::Boolean>();
}

//------------------------------------------------------------------------------

void SessionTest::integerTest()
{
    _test_combine<data::Integer>();
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Float>(const data::Float::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(_expected<data::Float>(variant)->getValue(), actual->getValue(), FLOAT_EPSILON);
}

//------------------------------------------------------------------------------

void SessionTest::floatTest()
{
    _test_combine<data::Float>();
}

//------------------------------------------------------------------------------

template<>
inline data::String::sptr _generate<data::String>(const std::size_t)
{
    return data::String::New(UUID::generateUUID());
}

//------------------------------------------------------------------------------

void SessionTest::stringTest()
{
    _test_combine<data::String>();
}

//------------------------------------------------------------------------------

template<>
inline data::Composite::sptr _generate<data::Composite>(const std::size_t variant)
{
    auto object = data::Composite::New();
    (*object)[data::Boolean::classname()] = _new<data::Boolean>(variant);
    (*object)[data::Integer::classname()] = _new<data::Integer>(variant);
    (*object)[data::Float::classname()]   = _new<data::Float>(variant);
    (*object)[data::String::classname()]  = _new<data::String>(variant);

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Composite>(const data::Composite::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    _compare<data::Boolean>(
        std::dynamic_pointer_cast<data::Boolean>(actual->at(data::Boolean::classname())),
        variant
    );
    _compare<data::Integer>(
        std::dynamic_pointer_cast<data::Integer>(actual->at(data::Integer::classname())),
        variant
    );
    _compare<data::Float>(std::dynamic_pointer_cast<data::Float>(actual->at(data::Float::classname())), variant);
    _compare<data::String>(std::dynamic_pointer_cast<data::String>(actual->at(data::String::classname())), variant);
}

//------------------------------------------------------------------------------

void SessionTest::compositeTest()
{
    _test_combine<data::Composite>();
}

//------------------------------------------------------------------------------

template<>
inline data::Mesh::sptr _new<data::Mesh>(const std::size_t variant)
{
    const auto& object = data::Mesh::New();
    object->deepCopy(_expected<data::Mesh>(variant));
    object->shrinkToFit();
    return object;
}

//------------------------------------------------------------------------------

template<>
inline data::Mesh::sptr _generate<data::Mesh>(const std::size_t)
{
    auto object = data::Mesh::New();

    utestData::generator::Mesh::generateTriangleQuadMesh(object);
    geometry::data::Mesh::shakePoint(object);
    geometry::data::Mesh::colorizeMeshPoints(object);
    geometry::data::Mesh::colorizeMeshCells(object);
    geometry::data::Mesh::generatePointNormals(object);
    geometry::data::Mesh::generateCellNormals(object);
    object->shrinkToFit();

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Mesh>(const data::Mesh::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Mesh>(variant);

    CPPUNIT_ASSERT_EQUAL(expected->numCells(), actual->numCells());
    CPPUNIT_ASSERT_EQUAL(expected->numPoints(), actual->numPoints());
    CPPUNIT_ASSERT_EQUAL(expected->getCellSize(), actual->getCellSize());
    CPPUNIT_ASSERT_EQUAL(expected->getDataSizeInBytes(), actual->getDataSizeInBytes());

    // This is needed to use iterators
    data::mt::locked_ptr<const data::Mesh> expectedGuard(expected);
    data::mt::locked_ptr<const data::Mesh> actualGuard(actual);

    using namespace data::iterator;
    const auto expectedRange = expected->czip_range<point::xyz, point::nxyz>();
    const auto actualRange   = actual->czip_range<point::xyz, point::nxyz>();

    for(const auto& [orig, cur] : boost::combine(expectedRange, actualRange))
    {
        const auto& [p1, n1] = orig;
        const auto& [p2, n2] = cur;

        CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.x, p2.x, FLOAT_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.y, p2.y, FLOAT_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(p1.z, p2.z, FLOAT_EPSILON);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(n1.nx, n2.nx, FLOAT_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(n1.ny, n2.ny, FLOAT_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(n1.nz, n2.nz, FLOAT_EPSILON);
    }
}

//------------------------------------------------------------------------------

void SessionTest::meshTest()
{
    _test_combine<data::Mesh>();
}

//------------------------------------------------------------------------------

template<>
inline data::Equipment::sptr _generate<data::Equipment>(const std::size_t)
{
    auto object = data::Equipment::New();
    object->setInstitutionName(UUID::generateUUID());

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Equipment>(const data::Equipment::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Equipment>(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getInstitutionName(), actual->getInstitutionName());
}

//------------------------------------------------------------------------------

void SessionTest::equipmentTest()
{
    _test_combine<data::Equipment>();
}

//------------------------------------------------------------------------------

template<>
inline data::Patient::sptr _generate<data::Patient>(const std::size_t)
{
    auto object = data::Patient::New();
    object->setName(UUID::generateUUID());
    object->setPatientId(UUID::generateUUID());
    object->setBirthdate(UUID::generateUUID());
    object->setSex(UUID::generateUUID());

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Patient>(const data::Patient::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Patient>(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getName(), actual->getName());
    CPPUNIT_ASSERT_EQUAL(expected->getPatientId(), actual->getPatientId());
    CPPUNIT_ASSERT_EQUAL(expected->getBirthdate(), actual->getBirthdate());
    CPPUNIT_ASSERT_EQUAL(expected->getSex(), actual->getSex());
}

//------------------------------------------------------------------------------

void SessionTest::patientTest()
{
    _test_combine<data::Patient>();
}

//------------------------------------------------------------------------------

template<>
inline data::Study::sptr _generate<data::Study>(const std::size_t)
{
    auto object = data::Study::New();

    object->setInstanceUID(UUID::generateUUID());
    object->setStudyID(UUID::generateUUID());
    object->setDate(UUID::generateUUID());
    object->setTime(UUID::generateUUID());
    object->setReferringPhysicianName(UUID::generateUUID());
    object->setConsultingPhysicianName(UUID::generateUUID());
    object->setDescription(UUID::generateUUID());
    object->setPatientAge(UUID::generateUUID());
    object->setPatientSize(UUID::generateUUID());
    object->setPatientWeight(UUID::generateUUID());
    object->setPatientBodyMassIndex(UUID::generateUUID());
    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Study>(const data::Study::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Study>(variant);

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

void SessionTest::studyTest()
{
    _test_combine<data::Study>();
}

//------------------------------------------------------------------------------

template<>
inline data::Series::sptr _generate<data::Series>(const std::size_t variant)
{
    auto object = data::Series::New();

    object->setPatient(_new<data::Patient>(variant));
    object->setStudy(_new<data::Study>(variant));
    object->setEquipment(_new<data::Equipment>(variant));

    // Fill trivial attributes
    object->setModality(UUID::generateUUID());
    object->setInstanceUID(UUID::generateUUID());
    object->setNumber(UUID::generateUUID());
    object->setLaterality(UUID::generateUUID());
    object->setDate(UUID::generateUUID());
    object->setTime(UUID::generateUUID());
    object->setPerformingPhysiciansName(
            {
                UUID::generateUUID(),
                UUID::generateUUID(),
                UUID::generateUUID()
            });
    object->setProtocolName(UUID::generateUUID());
    object->setDescription(UUID::generateUUID());
    object->setBodyPartExamined(UUID::generateUUID());
    object->setPatientPosition(UUID::generateUUID());
    object->setAnatomicalOrientationType(UUID::generateUUID());
    object->setPerformedProcedureStepID(UUID::generateUUID());
    object->setPerformedProcedureStepStartDate(UUID::generateUUID());
    object->setPerformedProcedureStepStartTime(UUID::generateUUID());
    object->setPerformedProcedureStepEndDate(UUID::generateUUID());
    object->setPerformedProcedureStepEndTime(UUID::generateUUID());
    object->setPerformedProcedureStepDescription(UUID::generateUUID());
    object->setPerformedProcedureComments(UUID::generateUUID());

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Series>(const data::Series::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Series>(variant);

    // Equipment
    _compare<data::Equipment>(actual->getEquipment(), variant);

    // Study
    _compare<data::Study>(actual->getStudy(), variant);

    // Patient
    _compare<data::Patient>(actual->getPatient(), variant);

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
    CPPUNIT_ASSERT_EQUAL(
        expected->getPerformedProcedureStepStartDate(),
        actual->getPerformedProcedureStepStartDate()
    );
    CPPUNIT_ASSERT_EQUAL(
        expected->getPerformedProcedureStepStartTime(),
        actual->getPerformedProcedureStepStartTime()
    );
    CPPUNIT_ASSERT_EQUAL(expected->getPerformedProcedureStepEndDate(), actual->getPerformedProcedureStepEndDate());
    CPPUNIT_ASSERT_EQUAL(expected->getPerformedProcedureStepEndTime(), actual->getPerformedProcedureStepEndTime());
    CPPUNIT_ASSERT_EQUAL(
        expected->getPerformedProcedureStepDescription(),
        actual->getPerformedProcedureStepDescription()
    );
    CPPUNIT_ASSERT_EQUAL(expected->getPerformedProcedureComments(), actual->getPerformedProcedureComments());
}

//------------------------------------------------------------------------------

void SessionTest::seriesTest()
{
    _test_combine<data::Series>();
}

//------------------------------------------------------------------------------

template<>
inline data::ActivitySeries::sptr _generate<data::ActivitySeries>(const std::size_t variant)
{
    auto object = data::ActivitySeries::New();

    object->setData(_new<data::Composite>(variant));

    // Inherited attributes
    object->data::Series::shallowCopy(_expected<data::Series>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::ActivitySeries>(const data::ActivitySeries::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::ActivitySeries>(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getActivityConfigId(), actual->getActivityConfigId());

    // Test inherited attributes
    _compare<data::Series>(actual, variant);

    // test Data
    _compare<data::Composite>(actual->getData(), variant);
}

//------------------------------------------------------------------------------

void SessionTest::activitySeriesTest()
{
    _test_combine<data::ActivitySeries>();
}

//------------------------------------------------------------------------------

template<>
inline data::Array::sptr _generate<data::Array>(const std::size_t variant)
{
    auto object = data::Array::New();

    const auto lock = object->dump_lock();

    auto fill =
        [&](auto type)
        {
            using T = decltype(type);

            object->resize(
                {variant + 2, variant + 2},
                std::is_same<T, double>::value
                ? core::Type::DOUBLE
                : std::is_same<T, float>::value
                ? core::Type::FLOAT
                : std::is_same<T, std::uint8_t>::value
                ? core::Type::UINT8
                : std::is_same<T, std::uint16_t>::value
                ? core::Type::UINT16
                : std::is_same<T, std::uint32_t>::value
                ? core::Type::UINT32
                : std::is_same<T, std::uint64_t>::value
                ? core::Type::UINT64
                : std::is_same<T, std::int8_t>::value
                ? core::Type::INT8
                : std::is_same<T, std::int16_t>::value
                ? core::Type::INT16
                : std::is_same<T, std::int32_t>::value
                ? core::Type::INT32
                : std::is_same<T, std::int64_t>::value
                ? core::Type::INT64
                : core::Type::NONE,
                true
            );

            T counter = static_cast<T>(0);
            for(auto it = object->begin<T>(),
                end = object->end<T>() ;
                it != end ;
                ++it)
            {
                *it = static_cast<T>(variant) + counter++;
            }
        };

    switch(variant % 5)
    {
        case 0:
            fill(static_cast<std::uint8_t>(0));
            break;

        case 1:
            fill(static_cast<std::uint16_t>(0));
            break;

        case 2:
            fill(static_cast<std::uint32_t>(0));
            break;

        case 3:
            fill(0.0);
            break;

        case 4:
            fill(0.0F);
            break;

        default:
            CPPUNIT_FAIL("Unknown variant.");
            break;
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Array>(const data::Array::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Array>(variant);

    const auto& expectedSize = expected->getSize();
    const auto& actualSize   = actual->getSize();

    for(std::size_t i = 0, end = expectedSize.size() ; i < end ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(expectedSize[i], actualSize[i]);
    }

    auto compare =
        [&](auto type)
        {
            using T = decltype(type);

            const auto dumpLockExpected = expected->dump_lock();
            const auto dumpLockActual   = actual->dump_lock();

            for(auto expectedIt = expected->begin<T>(),
                expectedEnd = expected->end<T>(),
                actualIt = actual->begin<T>(),
                actualEnd = actual->end<T>() ;
                expectedIt != expectedEnd && actualIt != actualEnd ;
                ++expectedIt, ++actualIt)
            {
                if(std::is_same<T, double>::value)
                {
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(
                        static_cast<double>(*expectedIt),
                        static_cast<double>(*actualIt),
                        DOUBLE_EPSILON
                    );
                }
                else if(std::is_same<T, float>::value)
                {
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(
                        static_cast<float>(*expectedIt),
                        static_cast<float>(*actualIt),
                        FLOAT_EPSILON
                    );
                }
                else
                {
                    CPPUNIT_ASSERT_EQUAL(*expectedIt, *actualIt);
                }
            }
        };

    switch(variant % 5)
    {
        case 0:
            compare(static_cast<std::uint8_t>(0));
            break;

        case 1:
            compare(static_cast<std::uint16_t>(0));
            break;

        case 2:
            compare(static_cast<std::uint32_t>(0));
            break;

        case 3:
            compare(0.0);
            break;

        case 4:
            compare(0.0F);
            break;

        default:
            CPPUNIT_FAIL("Unknown variant.");
            break;
    }
}

//------------------------------------------------------------------------------

void SessionTest::arrayTest()
{
    _test_combine<data::Array>();
}

//------------------------------------------------------------------------------

template<>
inline data::Image::sptr _generate<data::Image>(const std::size_t variant)
{
    auto object = data::Image::New();

    const auto lock = object->dump_lock();

    auto fill =
        [&](auto type)
        {
            using T = decltype(type);

            // Warning: generateImage use reflection that cannot deal with double value (truncated to float precision),
            // thus the 0.1 + static_cast<double>(variant)
            utestData::generator::Image::generateImage(
                object,
                {variant + 5, variant + 5, variant + 5},
                {
                    0.1 + static_cast<double>(variant),
                    0.2 + static_cast<double>(variant),
                    0.3 + static_cast<double>(variant)
                },
                {
                    0.4 + static_cast<double>(variant),
                    0.5 + static_cast<double>(variant),
                    0.6 + static_cast<double>(variant)
                },
                std::is_same<T, double>::value
                ? core::Type::DOUBLE
                : std::is_same<T, float>::value
                ? core::Type::FLOAT
                : std::is_same<T, std::uint8_t>::value
                ? core::Type::UINT8
                : std::is_same<T, std::uint16_t>::value
                ? core::Type::UINT16
                : std::is_same<T, std::uint32_t>::value
                ? core::Type::UINT32
                : std::is_same<T, std::uint64_t>::value
                ? core::Type::UINT64
                : std::is_same<T, std::int8_t>::value
                ? core::Type::INT8
                : std::is_same<T, std::int16_t>::value
                ? core::Type::INT16
                : std::is_same<T, std::int32_t>::value
                ? core::Type::INT32
                : std::is_same<T, std::int64_t>::value
                ? core::Type::INT64
                : core::Type::NONE,

                std::is_same<T, double>::value
                ? data::Image::PixelFormat::GRAY_SCALE
                : std::is_same<T, float>::value
                ? data::Image::PixelFormat::GRAY_SCALE
                : std::is_same<T, std::uint8_t>::value
                ? data::Image::PixelFormat::BGR
                : std::is_same<T, std::uint16_t>::value
                ? data::Image::PixelFormat::BGRA
                : std::is_same<T, std::uint32_t>::value
                ? data::Image::PixelFormat::RGB
                : std::is_same<T, std::uint64_t>::value
                ? data::Image::PixelFormat::RGBA
                : std::is_same<T, std::int8_t>::value
                ? data::Image::PixelFormat::GRAY_SCALE
                : std::is_same<T, std::int16_t>::value
                ? data::Image::PixelFormat::BGR
                : std::is_same<T, std::int32_t>::value
                ? data::Image::PixelFormat::BGRA
                : std::is_same<T, std::int64_t>::value
                ? data::Image::PixelFormat::RGB
                : data::Image::PixelFormat::UNDEFINED
            );

            utestData::generator::Image::randomizeImage(object);
        };

    switch(variant % 5)
    {
        case 0:
            fill(static_cast<std::uint8_t>(0));
            break;

        case 1:
            fill(static_cast<std::uint16_t>(0));
            break;

        case 2:
            fill(static_cast<std::uint32_t>(0));
            break;

        case 3:
            fill(0.0);
            break;

        case 4:
            fill(0.0F);
            break;

        default:
            CPPUNIT_FAIL("Unknown variant.");
            break;
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Image>(const data::Image::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Image>(variant);

    const auto& expectedSize = expected->getSize();
    const auto& actualSize   = actual->getSize();

    for(std::size_t i = 0, end = expectedSize.size() ; i < end ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(expectedSize[i], actualSize[i]);
    }

    const auto& expectedSpacing = expected->getSpacing();
    const auto& actualSpacing   = actual->getSpacing();

    for(std::size_t i = 0, end = expectedSpacing.size() ; i < end ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedSpacing[i], actualSpacing[i], DOUBLE_EPSILON);
    }

    const auto& expectedOrigin = expected->getOrigin();
    const auto& actualOrigin   = actual->getOrigin();

    for(std::size_t i = 0, end = expectedOrigin.size() ; i < end ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedOrigin[i], actualOrigin[i], DOUBLE_EPSILON);
    }

    CPPUNIT_ASSERT_EQUAL(expected->getType(), actual->getType());

    auto compare =
        [&](auto type)
        {
            using T = decltype(type);

            const auto dumpLockExpected = expected->dump_lock();
            const auto dumpLockActual   = actual->dump_lock();

            for(auto expectedIt = expected->begin<T>(),
                expectedEnd = expected->end<T>(),
                actualIt = actual->begin<T>(),
                actualEnd = actual->end<T>() ;
                expectedIt != expectedEnd && actualIt != actualEnd ;
                ++expectedIt, ++actualIt)
            {
                if(std::is_same<T, double>::value)
                {
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(
                        static_cast<double>(*expectedIt),
                        static_cast<double>(*actualIt),
                        DOUBLE_EPSILON
                    );
                }
                else if(std::is_same<T, float>::value)
                {
                    CPPUNIT_ASSERT_DOUBLES_EQUAL(
                        static_cast<float>(*expectedIt),
                        static_cast<float>(*actualIt),
                        FLOAT_EPSILON
                    );
                }
                else
                {
                    CPPUNIT_ASSERT_EQUAL(*expectedIt, *actualIt);
                }
            }
        };

    switch(variant % 5)
    {
        case 0:
            compare(static_cast<std::uint8_t>(0));
            break;

        case 1:
            compare(static_cast<std::uint16_t>(0));
            break;

        case 2:
            compare(static_cast<std::uint32_t>(0));
            break;

        case 3:
            compare(0.0);
            break;

        case 4:
            compare(0.0F);
            break;

        default:
            CPPUNIT_FAIL("Unknown variant.");
            break;
    }
}

//------------------------------------------------------------------------------

void SessionTest::imageTest()
{
    _test_combine<data::Image>();
}

//------------------------------------------------------------------------------

template<>
inline data::Vector::sptr _generate<data::Vector>(const std::size_t variant)
{
    auto object = data::Vector::New();

    object->push_back(_new<data::Boolean>(variant));
    object->push_back(_new<data::Integer>(variant));
    object->push_back(_new<data::Float>(variant));
    object->push_back(_new<data::String>(variant));
    object->push_back(_new<data::ActivitySeries>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Vector>(const data::Vector::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    auto it = actual->cbegin();

    _compare<data::Boolean>(std::dynamic_pointer_cast<data::Boolean>(*it++), variant);
    _compare<data::Integer>(std::dynamic_pointer_cast<data::Integer>(*it++), variant);
    _compare<data::Float>(std::dynamic_pointer_cast<data::Float>(*it++), variant);
    _compare<data::String>(std::dynamic_pointer_cast<data::String>(*it++), variant);
    _compare<data::ActivitySeries>(std::dynamic_pointer_cast<data::ActivitySeries>(*it++), variant);
}

//------------------------------------------------------------------------------

void SessionTest::vectorTest()
{
    _test_combine<data::Vector>();
}

//------------------------------------------------------------------------------

template<>
inline data::Point::sptr _generate<data::Point>(const std::size_t)
{
    auto object = data::Point::New();

    object->setCoord({random<double>(), random<double>(), random<double>()});

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Point>(const data::Point::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Point>(variant);

    const auto& expectedCoord = expected->getCoord();
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

void SessionTest::pointTest()
{
    _test_combine<data::Point>();
}

//------------------------------------------------------------------------------

template<>
inline data::PointList::sptr _generate<data::PointList>(const std::size_t variant)
{
    auto object = data::PointList::New();

    auto& points = object->getPoints();
    for(std::size_t i = 0, end = variant + 3 ; i < end ; ++i)
    {
        points.push_back(_new<data::Point>(i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::PointList>(const data::PointList::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::PointList>(variant);

    const auto& expectedPoints = expected->getPoints();
    const auto& actualPoints   = actual->getPoints();
    CPPUNIT_ASSERT_EQUAL(expectedPoints.size(), actualPoints.size());

    for(std::size_t i = 0, end = expectedPoints.size() ; i < end ; ++i)
    {
        _compare<data::Point>(actualPoints.at(i), i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::pointListTest()
{
    _test_combine<data::PointList>();
}

//------------------------------------------------------------------------------

template<>
inline data::CalibrationInfo::sptr _generate<data::CalibrationInfo>(const std::size_t variant)
{
    auto object = data::CalibrationInfo::New();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        // Create the Image
        auto image = _new<data::Image>(variant + i);

        // Create the PointList
        auto pointList = _new<data::PointList>(variant + i);

        object->addRecord(image, pointList);
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::CalibrationInfo>(const data::CalibrationInfo::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        const auto& image = actual->getImage(i);
        _compare<data::Image>(image, variant + i);

        const auto& pointList = actual->getPointList(image);
        _compare<data::PointList>(pointList, variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::calibrationInfoTest()
{
    _test_combine<data::CalibrationInfo>();
}

//------------------------------------------------------------------------------

template<>
inline data::Camera::sptr _generate<data::Camera>(const std::size_t variant)
{
    auto object = data::Camera::New();

    object->setWidth(random<std::size_t>());
    object->setHeight(random<std::size_t>());
    object->setFx(random<double>());
    object->setFy(random<double>());
    object->setCx(random<double>());
    object->setCy(random<double>());
    object->setDistortionCoefficient(
        random<double>(),
        random<double>(),
        random<double>(),
        random<double>(),
        random<double>()
    );
    object->setSkew(random<double>());
    object->setIsCalibrated(variant % 2 == 0);
    object->setDescription(UUID::generateUUID());
    object->setCameraID(UUID::generateUUID());
    object->setMaximumFrameRate(random<float>());
    object->setPixelFormat(
        variant % 35 == 0
        ? data::Camera::PixelFormat::ADOBEDNG
        : variant % 35 == 1
        ? data::Camera::PixelFormat::ARGB32
        : variant % 35 == 2
        ? data::Camera::PixelFormat::ARGB32_PREMULTIPLIED
        : variant % 35 == 3
        ? data::Camera::PixelFormat::RGB32
        : variant % 35 == 4
        ? data::Camera::PixelFormat::RGB24
        : variant % 35 == 5
        ? data::Camera::PixelFormat::RGB565
        : variant % 35 == 6
        ? data::Camera::PixelFormat::RGB555
        : variant % 35 == 7
        ? data::Camera::PixelFormat::ARGB8565_PREMULTIPLIED
        : variant % 35 == 8
        ? data::Camera::PixelFormat::BGRA32
        : variant % 35 == 9
        ? data::Camera::PixelFormat::BGRA32_PREMULTIPLIED
        : variant % 35 == 10
        ? data::Camera::PixelFormat::BGR32
        : variant % 35 == 11
        ? data::Camera::PixelFormat::BGR24
        : variant % 35 == 12
        ? data::Camera::PixelFormat::BGR565
        : variant % 35 == 13
        ? data::Camera::PixelFormat::BGR555
        : variant % 35 == 14
        ? data::Camera::PixelFormat::BGRA5658_PREMULTIPLIED
        : variant % 35 == 15
        ? data::Camera::PixelFormat::AYUV444
        : variant % 35 == 16
        ? data::Camera::PixelFormat::AYUV444_PREMULTIPLIED
        : variant % 35 == 17
        ? data::Camera::PixelFormat::YUV444
        : variant % 35 == 18
        ? data::Camera::PixelFormat::YUV420P
        : variant % 35 == 19
        ? data::Camera::PixelFormat::YV12
        : variant % 35 == 20
        ? data::Camera::PixelFormat::UYVY
        : variant % 35 == 21
        ? data::Camera::PixelFormat::YUYV
        : variant % 35 == 22
        ? data::Camera::PixelFormat::NV12
        : variant % 35 == 23
        ? data::Camera::PixelFormat::NV21
        : variant % 35 == 24
        ? data::Camera::PixelFormat::IMC1
        : variant % 35 == 25
        ? data::Camera::PixelFormat::IMC2
        : variant % 35 == 26
        ? data::Camera::PixelFormat::IMC3
        : variant % 35 == 27
        ? data::Camera::PixelFormat::IMC4
        : variant % 35 == 28
        ? data::Camera::PixelFormat::Y8
        : variant % 35 == 29
        ? data::Camera::PixelFormat::Y16
        : variant % 35 == 30
        ? data::Camera::PixelFormat::JPEG
        : variant % 35 == 31
        ? data::Camera::PixelFormat::CAMERARAW
        : variant % 35 == 32
        ? data::Camera::PixelFormat::ADOBEDNG
        : variant % 35 == 33
        ? data::Camera::PixelFormat::RGBA32
        : variant % 35 == 34
        ? data::Camera::PixelFormat::USER
        : data::Camera::PixelFormat::INVALID
    );
    object->setVideoFile("/" + UUID::generateUUID());
    object->setStreamUrl(UUID::generateUUID());
    object->setCameraSource(
        variant % 3 == 0
        ? data::Camera::SourceType::DEVICE
        : variant % 3 == 1
        ? data::Camera::SourceType::FILE
        : variant % 3 == 2
        ? data::Camera::SourceType::STREAM
        : data::Camera::SourceType::UNKNOWN
    );
    object->setScale(random<double>());

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Camera>(const data::Camera::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Camera>(variant);

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

void SessionTest::cameraTest()
{
    _test_combine<data::Camera>();
}

//------------------------------------------------------------------------------

template<>
inline data::CameraSeries::sptr _generate<data::CameraSeries>(const std::size_t variant)
{
    auto object = data::CameraSeries::New();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->addCamera(_new<data::Camera>(variant + i));
    }

    // Inherited attributes
    object->data::Series::shallowCopy(_expected<data::Series>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::CameraSeries>(const data::CameraSeries::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    for(std::size_t i = 0, end = actual->numCameras() ; i < end ; ++i)
    {
        _compare<data::Camera>(actual->getCamera(i), variant + i);
    }

    _compare<data::Series>(actual, variant);
}

//------------------------------------------------------------------------------

void SessionTest::cameraSeriesTest()
{
    _test_combine<data::CameraSeries>();
}

//------------------------------------------------------------------------------

template<>
inline data::Color::sptr _generate<data::Color>(const std::size_t)
{
    auto object = data::Color::New();

    object->setRGBA(random<float>(), random<float>(), random<float>(), random<float>());

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Color>(const data::Color::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Color>(variant);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->red(), actual->red(), FLOAT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->green(), actual->green(), FLOAT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->blue(), actual->blue(), FLOAT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->alpha(), actual->alpha(), FLOAT_EPSILON);
}

//------------------------------------------------------------------------------

void SessionTest::colorTest()
{
    _test_combine<data::Color>();
}

//------------------------------------------------------------------------------

template<>
inline data::Edge::sptr _generate<data::Edge>(const std::size_t)
{
    auto object = data::Edge::New();

    object->setNature(UUID::generateUUID());
    object->setIdentifiers(UUID::generateUUID(), UUID::generateUUID());

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Edge>(const data::Edge::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Edge>(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getFromPortID(), actual->getFromPortID());
    CPPUNIT_ASSERT_EQUAL(expected->getToPortID(), actual->getToPortID());
    CPPUNIT_ASSERT_EQUAL(expected->getNature(), actual->getNature());
}

//------------------------------------------------------------------------------

void SessionTest::edgeTest()
{
    _test_combine<data::Edge>();
}

//------------------------------------------------------------------------------

template<>
inline data::Port::sptr _generate<data::Port>(const std::size_t)
{
    auto object = data::Port::New();

    object->setIdentifier(UUID::generateUUID());
    object->setType(UUID::generateUUID());

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Port>(const data::Port::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Port>(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getIdentifier(), actual->getIdentifier());
    CPPUNIT_ASSERT_EQUAL(expected->getType(), actual->getType());
}

//------------------------------------------------------------------------------

void SessionTest::portTest()
{
    _test_combine<data::Port>();
}

//------------------------------------------------------------------------------

template<>
inline data::Node::sptr _generate<data::Node>(const std::size_t variant)
{
    auto object = data::Node::New();

    object->setObject(_new<data::String>(variant));

    const std::size_t portCount = variant + 2;
    for(std::size_t i = 0, end = portCount ; i < end ; ++i)
    {
        object->addInputPort(_new<data::Port>(variant + i));
    }

    for(std::size_t i = portCount, end = 2 * (portCount) ; i < end ; ++i)
    {
        object->addOutputPort(_new<data::Port>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Node>(const data::Node::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    _compare<data::String>(std::dynamic_pointer_cast<data::String>(actual->getObject()), variant);

    const auto& inputs = actual->getInputPorts();
    for(std::size_t i = 0, end = inputs.size() ; i < end ; ++i)
    {
        _compare<data::Port>(inputs.at(i), variant + i);
    }

    const auto& outputs = actual->getOutputPorts();
    for(std::size_t i = 0, end = outputs.size() ; i < end ; ++i)
    {
        _compare<data::Port>(outputs.at(i), inputs.size() + variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::nodeTest()
{
    _test_combine<data::Node>();
}

//------------------------------------------------------------------------------

template<>
inline data::Graph::sptr _generate<data::Graph>(const std::size_t variant)
{
    auto object = data::Graph::New();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        auto upNode   = _new<data::Node>(variant + i);
        auto downNode = _new<data::Node>(variant + i + 1);

        object->addNode(upNode);
        object->addNode(downNode);
        object->addEdge(_new<data::Edge>(variant + i), upNode, downNode);
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Graph>(const data::Graph::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    std::size_t i = 0;
    for(const auto& connection : actual->getConnections())
    {
        _compare<data::Edge>(connection.first, i + variant);
        _compare<data::Node>(connection.second.first, i + variant);
        _compare<data::Node>(connection.second.second, ++i + variant);
    }
}

//------------------------------------------------------------------------------

void SessionTest::graphTest()
{
    _test_combine<data::Graph>();
}

//------------------------------------------------------------------------------

template<>
inline data::Landmarks::sptr _generate<data::Landmarks>(const std::size_t variant)
{
    auto object = data::Landmarks::New();

    for(std::size_t i = 0, i_end = variant + 2 ; i < i_end ; ++i)
    {
        const std::string name = UUID::generateUUID();

        object->addGroup(
            name,
            {random<float>(), random<float>(), random<float>(), random<float>()},
            random<float>(),
            variant % 2 == 0
            ? data::Landmarks::Shape::CUBE
            : data::Landmarks::Shape::SPHERE,
            variant % 3 == 0
        );

        for(std::size_t j = 0, j_end = variant + 2 ; j < j_end ; ++j)
        {
            object->addPoint(name, {random<double>(), random<double>(), random<double>()});
        }
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Landmarks>(const data::Landmarks::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Landmarks>(variant);

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
        for(std::size_t i = 0, end = expectedGroup.m_color.size() ; i < end ; ++i)
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
        for(std::size_t i = 0, i_end = expectedGroup.m_points.size() ; i < i_end ; ++i)
        {
            for(std::size_t j = 0, j_end = expectedGroup.m_points[i].size() ; j < j_end ; ++j)
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

void SessionTest::landmarksTest()
{
    _test_combine<data::Landmarks>();
}

//------------------------------------------------------------------------------

template<>
inline data::Line::sptr _generate<data::Line>(const std::size_t variant)
{
    auto object = data::Line::New();

    object->setPosition(_new<data::Point>(variant));
    object->setDirection(_new<data::Point>(variant + 1));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Line>(const data::Line::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    _compare<data::Point>(actual->getPosition(), variant);
    _compare<data::Point>(actual->getDirection(), variant + 1);
}

//------------------------------------------------------------------------------

void SessionTest::lineTest()
{
    _test_combine<data::Line>();
}

//------------------------------------------------------------------------------

template<>
inline data::Material::sptr _generate<data::Material>(const std::size_t variant)
{
    auto object = data::Material::New();

    data::Material::ShadingType shading[] = {
        data::Material::ShadingType::AMBIENT,
        data::Material::ShadingType::FLAT,
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

    // Set ambient color
    object->setAmbient(_new<data::Color>(variant));

    // Set diffuse color
    object->setDiffuse(_new<data::Color>(variant + 1));

    // Set diffuse texture
    object->setDiffuseTexture(_new<data::Image>(variant));

    // Others
    object->setShadingMode(shading[variant % std::size(shading)]);
    object->setRepresentationMode(representation[variant % std::size(representation)]);
    object->setOptionsMode(options[variant % std::size(options)]);
    object->setDiffuseTextureFiltering(
        variant % 3 == 0
        ? data::Material::FilteringType::LINEAR
        : data::Material::FilteringType::NEAREST
    );
    object->setDiffuseTextureWrapping(
        variant % 3 == 0
        ? data::Material::WrappingType::CLAMP
        : data::Material::WrappingType::REPEAT
    );

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Material>(const data::Material::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Material>(variant);

    // Test ambient
    _compare<data::Color>(actual->ambient(), variant);

    // Test diffuse
    _compare<data::Color>(actual->diffuse(), variant + 1);

    // Test diffuse texture
    _compare<data::Image>(actual->getDiffuseTexture(), variant);

    // Test other attributes
    CPPUNIT_ASSERT_EQUAL(expected->getShadingMode(), actual->getShadingMode());
    CPPUNIT_ASSERT_EQUAL(expected->getRepresentationMode(), actual->getRepresentationMode());
    CPPUNIT_ASSERT_EQUAL(expected->getOptionsMode(), actual->getOptionsMode());
    CPPUNIT_ASSERT_EQUAL(expected->getDiffuseTextureFiltering(), actual->getDiffuseTextureFiltering());
    CPPUNIT_ASSERT_EQUAL(expected->getDiffuseTextureWrapping(), actual->getDiffuseTextureWrapping());
}

//------------------------------------------------------------------------------

void SessionTest::materialTest()
{
    _test_combine<data::Material>();
}

//------------------------------------------------------------------------------

template<>
inline data::Matrix4::sptr _generate<data::Matrix4>(const std::size_t)
{
    auto object = data::Matrix4::New();

    auto& coefficients = object->getCoefficients();
    for(auto it = coefficients.begin(), end = coefficients.end() ; it != end ; ++it)
    {
        *it = random<double>();
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Matrix4>(const data::Matrix4::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Matrix4>(variant);

    const auto& expectedCoefficients = expected->getCoefficients();
    const auto& actualCoefficients   = actual->getCoefficients();

    CPPUNIT_ASSERT_EQUAL(expectedCoefficients.size(), actualCoefficients.size());

    for(std::size_t i = 0, end = expectedCoefficients.size() ; i != end ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedCoefficients[i], actualCoefficients[i], DOUBLE_EPSILON);
    }
}

//------------------------------------------------------------------------------

void SessionTest::matrix4Test()
{
    _test_combine<data::Matrix4>();
}

//------------------------------------------------------------------------------

template<>
inline data::Plane::sptr _generate<data::Plane>(const std::size_t variant)
{
    auto object = data::Plane::New();

    auto& points = object->getPoints();
    for(std::size_t i = 0, end = points.size() ; i < end ; ++i)
    {
        points[i] = _new<data::Point>(i + variant);
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Plane>(const data::Plane::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    const auto& points = actual->getPoints();
    for(std::size_t i = 0, end = points.size() ; i < end ; ++i)
    {
        _compare<data::Point>(points.at(i), i + variant);
    }
}

//------------------------------------------------------------------------------

void SessionTest::planeTest()
{
    _test_combine<data::Plane>();
}

//------------------------------------------------------------------------------

template<>
inline data::PlaneList::sptr _generate<data::PlaneList>(const std::size_t variant)
{
    auto object = data::PlaneList::New();

    auto& planes = object->getPlanes();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        planes.push_back(_new<data::Plane>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::PlaneList>(const data::PlaneList::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    const auto& planes = actual->getPlanes();
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        _compare<data::Plane>(planes.at(i), variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::planeListTest()
{
    _test_combine<data::PlaneList>();
}

//------------------------------------------------------------------------------

template<>
inline data::ProcessObject::sptr _generate<data::ProcessObject>(const std::size_t variant)
{
    auto object = data::ProcessObject::New();

    object->setInputValue(data::Boolean::classname(), _new<data::Boolean>(variant));
    object->setInputValue(data::Integer::classname(), _new<data::Integer>(variant));

    object->setOutputValue(data::Float::classname(), _new<data::Float>(variant));
    object->setOutputValue(data::String::classname(), _new<data::String>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::ProcessObject>(const data::ProcessObject::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    _compare<data::Boolean>(actual->getInput<data::Boolean>(data::Boolean::classname()), variant);
    _compare<data::Integer>(actual->getInput<data::Integer>(data::Integer::classname()), variant);

    _compare<data::Float>(actual->getOutput<data::Float>(data::Float::classname()), variant);
    _compare<data::String>(actual->getOutput<data::String>(data::String::classname()), variant);
}

//------------------------------------------------------------------------------

void SessionTest::processObjectTest()
{
    _test_combine<data::ProcessObject>();
}

//------------------------------------------------------------------------------

template<>
inline data::Reconstruction::sptr _generate<data::Reconstruction>(const std::size_t variant)
{
    auto object = data::Reconstruction::New();

    object->setIsVisible(variant % 3 == 0);
    object->setOrganName(UUID::generateUUID());
    object->setStructureType(UUID::generateUUID());
    object->setComputedMaskVolume(random<double>());

    // Material
    object->setMaterial(_new<data::Material>(variant));

    // Image
    object->setImage(_new<data::Image>(variant));

    // Mesh
    object->setMesh(_new<data::Mesh>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Reconstruction>(const data::Reconstruction::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Reconstruction>(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getIsVisible(), actual->getIsVisible());
    CPPUNIT_ASSERT_EQUAL(expected->getOrganName(), actual->getOrganName());
    CPPUNIT_ASSERT_EQUAL(expected->getStructureType(), actual->getStructureType());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        expected->getComputedMaskVolume(),
        actual->getComputedMaskVolume(),
        DOUBLE_EPSILON
    );

    // Material
    _compare<data::Material>(actual->getMaterial(), variant);

    // Image
    _compare<data::Image>(actual->getImage(), variant);

    // Mesh
    _compare<data::Mesh>(actual->getMesh(), variant);
}

//------------------------------------------------------------------------------

void SessionTest::reconstructionTest()
{
    _test_combine<data::Reconstruction>();
}

//------------------------------------------------------------------------------

template<>
inline data::StructureTraits::sptr _generate<data::StructureTraits>(const std::size_t variant)
{
    auto object = data::StructureTraits::New();

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

    object->setType(UUID::generateUUID());
    object->setClass(CLASSES[variant % std::size(CLASSES)]);
    object->setNativeExp(UUID::generateUUID());
    object->setNativeGeometricExp(UUID::generateUUID());
    object->setAttachmentType(UUID::generateUUID());
    object->setAnatomicRegion(UUID::generateUUID());
    object->setPropertyCategory(UUID::generateUUID());
    object->setPropertyType(UUID::generateUUID());

    // Categories

    // Reset categories.
    data::StructureTraits::CategoryContainer categories;

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        categories.push_back(CATEGORIES[(i + variant) % std::size(CATEGORIES)]);
    }

    object->setCategories(categories);

    // Color
    object->setColor(_new<data::Color>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::StructureTraits>(const data::StructureTraits::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::StructureTraits>(variant);

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
    _compare<data::Color>(actual->getColor(), variant);
}

//------------------------------------------------------------------------------

void SessionTest::structureTraitsTest()
{
    _test_combine<data::StructureTraits>();
}

//------------------------------------------------------------------------------

template<>
inline data::StructureTraitsDictionary::sptr _generate<data::StructureTraitsDictionary>(const std::size_t variant)
{
    auto object = data::StructureTraitsDictionary::New();

    auto organ = _new<data::StructureTraits>(variant);
    organ->setClass(data::StructureTraits::ORGAN);
    organ->setAttachmentType("");

    object->addStructure(organ);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        auto structure            = _new<data::StructureTraits>(variant + i + 1);
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

        object->addStructure(structure);
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::StructureTraitsDictionary>(
    const data::StructureTraitsDictionary::csptr& actual,
    const std::size_t variant
)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::StructureTraitsDictionary>(variant);

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

void SessionTest::structureTraitsDictionaryTest()
{
    _test_combine<data::StructureTraitsDictionary>();
}

//------------------------------------------------------------------------------

template<>
inline data::ReconstructionTraits::sptr _generate<data::ReconstructionTraits>(const std::size_t variant)
{
    auto object = data::ReconstructionTraits::New();

    object->setIdentifier(UUID::generateUUID());

    // Reconstruction mask operator node
    object->setMaskOpNode(_new<data::Node>(variant));

    // Reconstruction mesh operator node
    object->setMeshOpNode(_new<data::Node>(variant + 1));

    // Associated structure traits
    object->setStructureTraits(_new<data::StructureTraits>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::ReconstructionTraits>(
    const data::ReconstructionTraits::csptr& actual,
    const std::size_t variant
)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::ReconstructionTraits>(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getIdentifier(), actual->getIdentifier());

    // Reconstruction mask operator node
    _compare<data::Node>(actual->getMaskOpNode(), variant);

    // Reconstruction mesh operator node
    _compare<data::Node>(actual->getMeshOpNode(), variant + 1);

    // Associated structure traits
    _compare<data::StructureTraits>(actual->getStructureTraits(), variant);
}

//------------------------------------------------------------------------------

void SessionTest::reconstructionTraitsTest()
{
    _test_combine<data::ReconstructionTraits>();
}

//------------------------------------------------------------------------------

template<>
inline data::Resection::sptr _generate<data::Resection>(const std::size_t variant)
{
    auto object = data::Resection::New();

    object->setName(UUID::generateUUID());
    object->setIsSafePart(variant % 2 == 0);
    object->setIsValid(variant % 3 == 0);
    object->setIsVisible(variant % 4 == 0);

    auto& inputs  = object->getInputs();
    auto& outputs = object->getOutputs();
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        inputs.push_back(_new<data::Reconstruction>(variant + i));
        outputs.push_back(_new<data::Reconstruction>(variant + i + 1));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Resection>(const data::Resection::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Resection>(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getName(), actual->getName());
    CPPUNIT_ASSERT_EQUAL(expected->getIsSafePart(), actual->getIsSafePart());
    CPPUNIT_ASSERT_EQUAL(expected->getIsValid(), actual->getIsValid());
    CPPUNIT_ASSERT_EQUAL(expected->getIsVisible(), actual->getIsVisible());

    const auto& inputs  = actual->getInputs();
    const auto& outputs = actual->getOutputs();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        _compare<data::Reconstruction>(inputs.at(i), variant + i);
        _compare<data::Reconstruction>(outputs.at(i), variant + i + 1);
    }
}

//------------------------------------------------------------------------------

void SessionTest::resectionTest()
{
    _test_combine<data::Resection>();
}

//------------------------------------------------------------------------------

template<>
inline data::ResectionDB::sptr _generate<data::ResectionDB>(const std::size_t variant)
{
    auto object = data::ResectionDB::New();

    object->setSafeResection(_new<data::Resection>(variant));

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->addResection(_new<data::Resection>(variant + i + 1));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::ResectionDB>(const data::ResectionDB::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    _compare<data::Resection>(actual->getSafeResection(), variant);

    const auto& resections = actual->getResections();
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        _compare<data::Resection>(resections.at(i), variant + i + 1);
    }
}

//------------------------------------------------------------------------------

void SessionTest::resectionDBTest()
{
    _test_combine<data::ResectionDB>();
}

//------------------------------------------------------------------------------

template<>
inline data::ROITraits::sptr _generate<data::ROITraits>(const std::size_t variant)
{
    auto object = data::ROITraits::New();

    object->setIdentifier(UUID::generateUUID());
    object->setEvaluatedExp(UUID::generateUUID());
    object->setMaskOpNode(_new<data::Node>(variant));
    object->setStructureTraits(_new<data::StructureTraits>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::ROITraits>(const data::ROITraits::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::ROITraits>(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getIdentifier(), actual->getIdentifier());
    CPPUNIT_ASSERT_EQUAL(expected->getEvaluatedExp(), actual->getEvaluatedExp());

    // Node
    _compare<data::Node>(actual->getMaskOpNode(), variant);

    // Structure
    _compare<data::StructureTraits>(actual->getStructureTraits(), variant);
}

//------------------------------------------------------------------------------

void SessionTest::roiTraitsTest()
{
    _test_combine<data::ROITraits>();
}

//------------------------------------------------------------------------------

template<>
inline data::SeriesDB::sptr _generate<data::SeriesDB>(const std::size_t variant)
{
    auto object = data::SeriesDB::New();

    auto& container = object->getContainer();
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        container.push_back(_new<data::Series>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::SeriesDB>(const data::SeriesDB::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    const auto& container = actual->getContainer();
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        _compare<data::Series>(container.at(i), variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::seriesDBTest()
{
    _test_combine<data::SeriesDB>();
}

//------------------------------------------------------------------------------

template<>
inline data::Tag::sptr _generate<data::Tag>(const std::size_t variant)
{
    auto object = data::Tag::New();

    object->setType(UUID::generateUUID());
    object->setSize(random<double>());
    object->setPointList(_new<data::PointList>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::Tag>(const data::Tag::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::Tag>(variant);

    CPPUNIT_ASSERT_EQUAL(expected->getType(), actual->getType());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->getSize(), actual->getSize(), DOUBLE_EPSILON);

    _compare<data::PointList>(actual->getPointList(), variant);
}

//------------------------------------------------------------------------------

void SessionTest::tagTest()
{
    _test_combine<data::Tag>();
}

//------------------------------------------------------------------------------

template<>
inline data::TransferFunction::sptr _generate<data::TransferFunction>(const std::size_t variant)
{
    auto object = data::TransferFunction::New();

    object->setLevel(random<double>());
    object->setWindow(random<double>());
    object->setName(UUID::generateUUID());
    object->setInterpolationMode(
        variant % 3 == 0
        ? data::TransferFunction::InterpolationMode::LINEAR
        : data::TransferFunction::InterpolationMode::NEAREST
    );
    object->setClamped(variant % 4 == 0);
    object->setBackgroundColor(
        data::TransferFunction::color_t(
            random<double>(),
            random<double>(),
            random<double>(),
            random<double>()
        )
    );

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->insert(
                {
                    random<double>(),
                    data::TransferFunction::color_t(
                        random<double>(),
                        random<double>(),
                        random<double>(),
                        random<double>()
                    )
                });
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::TransferFunction>(const data::TransferFunction::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::TransferFunction>(variant);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->level(), actual->level(), DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->window(), actual->window(), DOUBLE_EPSILON);
    CPPUNIT_ASSERT_EQUAL(expected->name(), actual->name());
    CPPUNIT_ASSERT_EQUAL(expected->interpolationMode(), actual->interpolationMode());
    CPPUNIT_ASSERT_EQUAL(expected->clamped(), actual->clamped());

    const auto& expectedBackgroundColor = expected->backgroundColor();
    const auto& actualBackgroundColor   = actual->backgroundColor();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedBackgroundColor.r, actualBackgroundColor.r, DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedBackgroundColor.g, actualBackgroundColor.g, DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedBackgroundColor.b, actualBackgroundColor.b, DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedBackgroundColor.a, actualBackgroundColor.a, DOUBLE_EPSILON);

    for(const auto& expectedData : *expected)
    {
        const auto& expectedColor = expectedData.second;
        const auto& actualColor   = actual->at(expectedData.first);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor.r, actualColor.r, DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor.g, actualColor.g, DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor.b, actualColor.b, DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor.a, actualColor.a, DOUBLE_EPSILON);
    }
}

//------------------------------------------------------------------------------

void SessionTest::transferFunctionTest()
{
    _test_combine<data::TransferFunction>();
}

//------------------------------------------------------------------------------

template<>
inline data::DicomSeries::sptr _generate<data::DicomSeries>(const std::size_t variant)
{
    data::DicomSeries::sptr dicomSeries;

    // Only load the real dicom once
    if(variant == 0)
    {
        // Setup the SeriesDB to be able to read
        auto seriesDB                    = data::SeriesDB::New();
        const std::filesystem::path path = utestData::Data::dir()
                                           / "sight/Patient/Dicom/DicomDB/86-CT-Skull";

        CPPUNIT_ASSERT_MESSAGE(
            "The dicom directory '" + path.string() + "' does not exist",
            std::filesystem::exists(path)
        );

        // Read source Dicom
        auto reader = io::dicom::reader::SeriesDB::New();
        reader->setObject(seriesDB);
        reader->setFolder(path);

        CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), seriesDB->size());

        dicomSeries = std::dynamic_pointer_cast<data::DicomSeries>(seriesDB->getContainer().front());
    }
    else
    {
        // Take the first variant as basis
        dicomSeries = data::DicomSeries::New();
        dicomSeries->shallowCopy(_expected<data::DicomSeries>(0));
    }

    // Randomize a bit the dicomSeries
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        dicomSeries->addSOPClassUID(UUID::generateUUID());
        dicomSeries->addComputedTagValue(UUID::generateUUID(), UUID::generateUUID());
    }

    // Inherited attributes
    dicomSeries->data::Series::shallowCopy(_expected<data::Series>(variant));

    return dicomSeries;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::DicomSeries>(const data::DicomSeries::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::DicomSeries>(variant);

    // Test inherited attributes
    _compare<data::Series>(actual, variant);

    const auto& expectedSOPClassUIDs = expected->getSOPClassUIDs();
    const auto& actualSOPClassUIDs   = actual->getSOPClassUIDs();
    CPPUNIT_ASSERT_EQUAL(expectedSOPClassUIDs.size(), actualSOPClassUIDs.size());

    for(const auto& expectedSOPClassUID : expectedSOPClassUIDs)
    {
        CPPUNIT_ASSERT(actualSOPClassUIDs.find(expectedSOPClassUID) != actualSOPClassUIDs.cend());
    }
}

//------------------------------------------------------------------------------

void SessionTest::dicomSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    _test_combine<data::DicomSeries>();
}

//------------------------------------------------------------------------------

template<>
inline data::ImageSeries::sptr _generate<data::ImageSeries>(const std::size_t variant)
{
    auto object = data::ImageSeries::New();

    object->setContrastAgent(UUID::generateUUID());
    object->setContrastRoute(UUID::generateUUID());
    object->setContrastVolume(UUID::generateUUID());
    object->setContrastStartTime(UUID::generateUUID());
    object->setContrastStopTime(UUID::generateUUID());
    object->setContrastTotalDose(UUID::generateUUID());
    object->setContrastFlowRate(UUID::generateUUID());
    object->setContrastFlowDuration(UUID::generateUUID());
    object->setContrastIngredient(UUID::generateUUID());
    object->setContrastIngredientConcentration(UUID::generateUUID());
    object->setAcquisitionDate(UUID::generateUUID());
    object->setAcquisitionTime(UUID::generateUUID());

    object->setImage(_new<data::Image>(variant));
    object->setDicomReference(_new<data::DicomSeries>(variant));

    // Inherited attributes
    object->data::Series::shallowCopy(_expected<data::Series>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::ImageSeries>(const data::ImageSeries::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = _expected<data::ImageSeries>(variant);

    // Test inherited attributes
    _compare<data::Series>(actual, variant);

    CPPUNIT_ASSERT_EQUAL(expected->getContrastAgent(), actual->getContrastAgent());
    CPPUNIT_ASSERT_EQUAL(expected->getContrastRoute(), actual->getContrastRoute());
    CPPUNIT_ASSERT_EQUAL(expected->getContrastVolume(), actual->getContrastVolume());
    CPPUNIT_ASSERT_EQUAL(expected->getContrastStartTime(), actual->getContrastStartTime());
    CPPUNIT_ASSERT_EQUAL(expected->getContrastStopTime(), actual->getContrastStopTime());
    CPPUNIT_ASSERT_EQUAL(expected->getContrastTotalDose(), actual->getContrastTotalDose());
    CPPUNIT_ASSERT_EQUAL(expected->getContrastFlowRate(), actual->getContrastFlowRate());
    CPPUNIT_ASSERT_EQUAL(expected->getContrastFlowDuration(), actual->getContrastFlowDuration());
    CPPUNIT_ASSERT_EQUAL(expected->getContrastIngredient(), actual->getContrastIngredient());
    CPPUNIT_ASSERT_EQUAL(
        expected->getContrastIngredientConcentration(),
        actual->getContrastIngredientConcentration()
    );
    CPPUNIT_ASSERT_EQUAL(expected->getAcquisitionDate(), actual->getAcquisitionDate());
    CPPUNIT_ASSERT_EQUAL(expected->getAcquisitionTime(), actual->getAcquisitionTime());

    _compare<data::Image>(actual->getImage(), variant);
    _compare<data::DicomSeries>(actual->getDicomReference(), variant);
}

//------------------------------------------------------------------------------

void SessionTest::imageSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    _test_combine<data::ImageSeries>();
}

//------------------------------------------------------------------------------

template<>
inline data::ModelSeries::sptr _generate<data::ModelSeries>(const std::size_t variant)
{
    auto object = data::ModelSeries::New();

    object->setDicomReference(_new<data::DicomSeries>(variant));

    std::vector<data::Reconstruction::sptr> reconstructionDB;
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        reconstructionDB.push_back(_new<data::Reconstruction>(variant + i));
    }

    object->setReconstructionDB(reconstructionDB);

    // Inherited attributes
    object->data::Series::shallowCopy(_expected<data::Series>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::ModelSeries>(const data::ModelSeries::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Test inherited attributes
    _compare<data::Series>(actual, variant);

    // Test other attributes
    _compare<data::DicomSeries>(actual->getDicomReference(), variant);

    const auto& reconstructionDB = actual->getReconstructionDB();
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        _compare<data::Reconstruction>(reconstructionDB.at(i), variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::modelSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    _test_combine<data::ModelSeries>();
}

//------------------------------------------------------------------------------

template<>
inline data::ActivitySet::sptr _generate<data::ActivitySet>(const std::size_t variant)
{
    auto object = data::ActivitySet::New();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->push_back(_new<data::ActivitySeries>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::ActivitySet>(const data::ActivitySet::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        _compare<data::ActivitySeries>(actual->at(i), variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::activitySetTest()
{
    _test_combine<data::ActivitySet>();
}

//------------------------------------------------------------------------------

template<>
inline data::CameraSet::sptr _generate<data::CameraSet>(const std::size_t variant)
{
    auto object = data::CameraSet::New();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        auto camera = _new<data::Camera>(variant + i);
        auto matrix = _new<data::Matrix4>(variant + i);
        object->push_back(std::make_pair(camera, matrix));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::CameraSet>(const data::CameraSet::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        _compare<data::Camera>(actual->at(i).first, variant + i);
        _compare<data::Matrix4>(actual->at(i).second, variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::cameraSetTest()
{
    _test_combine<data::CameraSet>();
}

//------------------------------------------------------------------------------

template<>
inline data::SeriesSet::sptr _generate<data::SeriesSet>(const std::size_t variant)
{
    auto object = data::SeriesSet::New();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->push_back(_new<data::Series>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void _compare<data::SeriesSet>(const data::SeriesSet::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        _compare<data::Series>(actual->at(i), variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::seriesSetTest()
{
    _test_combine<data::SeriesSet>();
}

//------------------------------------------------------------------------------

inline static void customSerialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>&,
    const core::crypto::secure_string& = ""
)
{
    // Cast to the right type
    const auto string = Helper::safeCast<data::String>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::String>(tree, 666);

    Helper::writeString(tree, "custom", string->getValue());
}

//------------------------------------------------------------------------------

inline static data::String::sptr customDeserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>&,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto string = Helper::safeCast<data::String>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::String>(tree, 0, 666);

    // Assign the value
    string->setValue(Helper::readString(tree, "custom"));

    return string;
}

//------------------------------------------------------------------------------

void SessionTest::customSerializerTest()
{
    // Create a temporary directory
    const auto tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const auto testPath = tmpfolder / "customSerializerTest.zip";
    std::filesystem::remove(testPath);

    // Register custom serializers
    io::session::SessionWriter::setDefaultSerializer(data::String::classname(), customSerialize);
    io::session::SessionReader::setDefaultDeserializer(data::String::classname(), customDeserialize);

    // Test serialization
    {
        // Create the data object
        auto object = _new<data::String>(0);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(object);
        sessionWriter->setFile(testPath);

        // Write the session
        CPPUNIT_ASSERT_NO_THROW(sessionWriter->write());

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);

        // Configure the session reader
        sessionReader->setFile(testPath);

        // Read the session
        CPPUNIT_ASSERT_NO_THROW(sessionReader->read());

        // Test value
        auto object = std::dynamic_pointer_cast<data::String>(sessionReader->getObject());
        _compare<data::String>(object, 0);
    }

    // Restore default serializers
    io::session::SessionWriter::setDefaultSerializer(data::String::classname());
    io::session::SessionReader::setDefaultDeserializer(data::String::classname());

    // Test again deserialization, it should fail since the deserializer is not the good one
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);

        // Configure the session reader
        sessionReader->setFile(testPath);

        // Read the session, since we don't use version 666 anymore, an exception should be raised
        CPPUNIT_ASSERT_THROW(sessionReader->read(), sight::core::Exception);

        // Retry with the custom deserializer on deserializer instance
        sessionReader->setDeserializer(data::String::classname(), customDeserialize);
        CPPUNIT_ASSERT_NO_THROW(sessionReader->read());
    }

    // Test again deserialization, to be sure the custom instance deserializer is really gone
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);

        // Configure the session reader
        sessionReader->setFile(testPath);

        // Read the session, since we don't use version 666 anymore, an exception should be raised
        CPPUNIT_ASSERT_THROW(sessionReader->read(), sight::core::Exception);
    }
}

} // namespace ut

} // namespace sight::io::session
