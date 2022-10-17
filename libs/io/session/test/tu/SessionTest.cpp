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

#include <data/Activity.hpp>
#include <data/ActivitySet.hpp>
#include <data/Array.hpp>
#include <data/Boolean.hpp>
#include <data/CalibrationInfo.hpp>
#include <data/Camera.hpp>
#include <data/CameraSet.hpp>
#include <data/Color.hpp>
#include <data/Composite.hpp>
#include <data/DicomSeries.hpp>
#include <data/Edge.hpp>
#include <data/Float.hpp>
#include <data/Graph.hpp>
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
#include <data/Plane.hpp>
#include <data/PlaneList.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/Port.hpp>
#include <data/Reconstruction.hpp>
#include <data/ReconstructionTraits.hpp>
#include <data/Resection.hpp>
#include <data/ResectionDB.hpp>
#include <data/ROITraits.hpp>
#include <data/Series.hpp>
#include <data/SeriesSet.hpp>
#include <data/Set.hpp>
#include <data/String.hpp>
#include <data/StructureTraits.hpp>
#include <data/StructureTraitsDictionary.hpp>
#include <data/TransferFunction.hpp>
#include <data/Vector.hpp>

#include <geometry/data/Mesh.hpp>

#include <io/dicom/reader/SeriesSet.hpp>
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

namespace sight::io::session::ut
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
    static std::mt19937 generator {std::random_device {}()};

    return distributor(generator);
}

//------------------------------------------------------------------------------

inline static std::string generateTM(std::size_t variant)
{
    std::stringstream hh;
    hh << std::setfill('0') << std::setw(2) << (variant % 24);

    std::stringstream mm;
    mm << std::setfill('0') << std::setw(2) << ((variant + 1) % 60);

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << ((variant + 2) % 60);

    std::stringstream ffffff;
    ffffff << std::setfill('0') << std::setw(6) << ((variant + 3) % 1000000);

    return hh.str() + mm.str() + ss.str() + "." + ffffff.str();
}

//------------------------------------------------------------------------------

inline static std::string generateDA(std::size_t variant)
{
    std::stringstream yyyy;
    yyyy << std::setfill('0') << std::setw(4) << (variant % 10000);

    std::stringstream mm;
    mm << std::setfill('0') << std::setw(2) << ((variant + 1) % 12);

    std::stringstream dd;
    dd << std::setfill('0') << std::setw(2) << ((variant + 2) % 32);

    return yyyy.str() + mm.str() + dd.str();
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
inline typename T::sptr generate(const std::size_t /*unused*/)
{
    return T::New(static_cast<typename T::ValueType>(random<typename T::ValueType>()));
}

//------------------------------------------------------------------------------

template<typename T>
inline const typename T::csptr& getExpected(const std::size_t variant)
{
    static std::map<std::size_t, typename T::csptr> MAP;
    const auto& it = MAP.find(variant);

    if(it == MAP.cend())
    {
        const auto& object = generate<T>(variant);
        object->setDescription(UUID::generateUUID());

        return MAP.insert_or_assign(variant, object).first->second;
    }

    return it->second;
}

//------------------------------------------------------------------------------

template<typename T>
inline typename T::sptr create(const std::size_t variant)
{
    const auto& object = T::New();
    object->deepCopy(getExpected<T>(variant));
    return object;
}

//------------------------------------------------------------------------------

template<typename T>
inline void compare(const typename T::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);
    CPPUNIT_ASSERT_EQUAL(getExpected<T>(variant)->getValue(), actual->getValue());
}

//------------------------------------------------------------------------------

template<typename T>
inline void test(const bool encrypt, const bool raw)
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
        auto object = create<T>(0);

        // Add a field
        object->setField(fieldName, create<T>(1));

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
        compare<T>(object, 0);

        // Test field
        auto fieldObject = std::dynamic_pointer_cast<T>(object->getField(fieldName));
        compare<T>(fieldObject, 1);
    }

    std::filesystem::remove_all(tmpfolder);
}

//------------------------------------------------------------------------------

template<typename T>
inline void testCombine()
{
    test<T>(false, false);
    test<T>(false, true);
    test<T>(true, false);
}

//------------------------------------------------------------------------------

template<>
inline data::Boolean::sptr generate<data::Boolean>(const std::size_t variant)
{
    return data::Boolean::New(variant % 2 == 0);
}

//------------------------------------------------------------------------------

void SessionTest::booleanTest()
{
    testCombine<data::Boolean>();
}

//------------------------------------------------------------------------------

void SessionTest::integerTest()
{
    testCombine<data::Integer>();
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Float>(const data::Float::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(getExpected<data::Float>(variant)->getValue(), actual->getValue(), FLOAT_EPSILON);
}

//------------------------------------------------------------------------------

void SessionTest::floatTest()
{
    testCombine<data::Float>();
}

//------------------------------------------------------------------------------

template<>
inline data::String::sptr generate<data::String>(const std::size_t /*unused*/)
{
    return data::String::New(UUID::generateUUID());
}

//------------------------------------------------------------------------------

void SessionTest::stringTest()
{
    testCombine<data::String>();
}

//------------------------------------------------------------------------------

template<>
inline data::Composite::sptr generate<data::Composite>(const std::size_t variant)
{
    auto object = data::Composite::New();
    (*object)[data::Boolean::classname()] = create<data::Boolean>(variant);
    (*object)[data::Integer::classname()] = create<data::Integer>(variant);
    (*object)[data::Float::classname()]   = create<data::Float>(variant);
    (*object)[data::String::classname()]  = create<data::String>(variant);

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Composite>(const data::Composite::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    compare<data::Boolean>(
        std::dynamic_pointer_cast<data::Boolean>(actual->at(data::Boolean::classname())),
        variant
    );
    compare<data::Integer>(
        std::dynamic_pointer_cast<data::Integer>(actual->at(data::Integer::classname())),
        variant
    );
    compare<data::Float>(std::dynamic_pointer_cast<data::Float>(actual->at(data::Float::classname())), variant);
    compare<data::String>(std::dynamic_pointer_cast<data::String>(actual->at(data::String::classname())), variant);
}

//------------------------------------------------------------------------------

void SessionTest::compositeTest()
{
    testCombine<data::Composite>();
}

//------------------------------------------------------------------------------

template<>
inline data::Mesh::sptr create<data::Mesh>(const std::size_t variant)
{
    const auto& object = data::Mesh::New();
    object->deepCopy(getExpected<data::Mesh>(variant));
    object->shrinkToFit();
    return object;
}

//------------------------------------------------------------------------------

template<>
inline data::Mesh::sptr generate<data::Mesh>(const std::size_t /*unused*/)
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
inline void compare<data::Mesh>(const data::Mesh::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Mesh>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

    CPPUNIT_ASSERT_EQUAL(expected->numCells(), actual->numCells());
    CPPUNIT_ASSERT_EQUAL(expected->numPoints(), actual->numPoints());
    CPPUNIT_ASSERT_EQUAL(expected->getCellSize(), actual->getCellSize());
    CPPUNIT_ASSERT_EQUAL(expected->getDataSizeInBytes(), actual->getDataSizeInBytes());

    // This is needed to use iterators
    data::mt::locked_ptr<const data::Mesh> expectedGuard(expected);
    data::mt::locked_ptr<const data::Mesh> actualGuard(actual);

    namespace point = data::iterator::point;
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
    testCombine<data::Mesh>();
}

//------------------------------------------------------------------------------

template<>
inline data::Series::sptr generate<data::Series>(const std::size_t variant)
{
    auto object = data::Series::New();

    // Fill trivial attributes
    object->setModality(UUID::generateUUID());
    object->setSeriesDescription(UUID::generateUUID());
    object->setSeriesInstanceUID(UUID::generateUUID());
    object->setSeriesNumber(std::int32_t(variant));
    object->setLaterality(UUID::generateUUID());
    object->setSeriesDate(generateDA(variant));
    object->setSeriesTime(generateTM(variant));
    object->setPerformingPhysicianName(UUID::generateUUID() + "\\" + UUID::generateUUID());
    object->setProtocolName(UUID::generateUUID());
    object->setBodyPartExamined(UUID::generateUUID());
    object->setPatientPosition(UUID::generateUUID());
    object->setAnatomicalOrientationType(UUID::generateUUID());
    object->setPerformedProcedureStepID(UUID::generateUUID());
    object->setPerformedProcedureStepStartDate(generateDA(variant));
    object->setPerformedProcedureStepStartTime(generateTM(variant));
    object->setPerformedProcedureStepEndDate(generateDA(variant));
    object->setPerformedProcedureStepEndTime(generateTM(variant));
    object->setPerformedProcedureStepDescription(UUID::generateUUID());
    object->setCommentsOnThePerformedProcedureStep(UUID::generateUUID());

    // Equipment Module
    object->setInstitutionName(UUID::generateUUID());

    // Patient Module
    object->setPatientName(UUID::generateUUID());
    object->setPatientID(UUID::generateUUID());
    object->setPatientBirthDate(generateDA(variant));
    object->setPatientSex(UUID::generateUUID());

    // Study Module
    object->setStudyDescription(UUID::generateUUID());
    object->setStudyInstanceUID(UUID::generateUUID());
    object->setStudyID(UUID::generateUUID());
    object->setStudyDate(generateDA(variant));
    object->setStudyTime(generateTM(variant));
    object->setReferringPhysicianName(UUID::generateUUID());

    // Patient Study Module
    object->setPatientAge(UUID::generateUUID());
    object->setPatientSize(double(variant));
    object->setPatientWeight(double(variant));

    // Generate specific instance data
    for(std::size_t i = 0 ; i < variant + 3 ; ++i)
    {
        object->setImagePositionPatient(
            {
                double(variant + i),
                double(variant + i + 1),
                double(variant + i + 2)
            },
            i
        );

        object->setImageOrientationPatient(
            {
                double(variant + i), double(variant + i + 1), double(variant + i + 2),
                double(variant + i + 3), double(variant + i + 4), double(variant + i + 5)
            },
            i
        );
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Series>(const data::Series::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Series>(variant);

    // Trivial attributes
    CPPUNIT_ASSERT_EQUAL(expected->getModality(), actual->getModality());
    CPPUNIT_ASSERT_EQUAL(expected->getSeriesDescription(), actual->getSeriesDescription());
    CPPUNIT_ASSERT_EQUAL(expected->getSeriesInstanceUID(), actual->getSeriesInstanceUID());
    CPPUNIT_ASSERT_EQUAL(expected->getSeriesNumber().has_value(), actual->getSeriesNumber().has_value());
    CPPUNIT_ASSERT_EQUAL(expected->getSeriesNumber().value_or(0), actual->getSeriesNumber().value_or(0));
    CPPUNIT_ASSERT_EQUAL(expected->getLaterality(), actual->getLaterality());
    CPPUNIT_ASSERT_EQUAL(expected->getSeriesDate(), actual->getSeriesDate());
    CPPUNIT_ASSERT_EQUAL(expected->getSeriesTime(), actual->getSeriesTime());

    const auto& expectedNames = expected->getPerformingPhysicianName();
    const auto& actualNames   = actual->getPerformingPhysicianName();
    CPPUNIT_ASSERT_EQUAL(expectedNames.size(), actualNames.size());

    for(std::size_t i = 0 ; i < expectedNames.size() ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(expectedNames[i], actualNames[i]);
    }

    CPPUNIT_ASSERT_EQUAL(expected->getProtocolName(), actual->getProtocolName());
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
    CPPUNIT_ASSERT_EQUAL(
        expected->getCommentsOnThePerformedProcedureStep(),
        actual->getCommentsOnThePerformedProcedureStep()
    );

    // Equipment Module
    CPPUNIT_ASSERT_EQUAL(expected->getInstitutionName(), actual->getInstitutionName());

    // Patient Module
    CPPUNIT_ASSERT_EQUAL(expected->getPatientName(), actual->getPatientName());
    CPPUNIT_ASSERT_EQUAL(expected->getPatientID(), actual->getPatientID());
    CPPUNIT_ASSERT_EQUAL(expected->getPatientBirthDate(), actual->getPatientBirthDate());
    CPPUNIT_ASSERT_EQUAL(expected->getPatientSex(), actual->getPatientSex());

    // Study Module
    CPPUNIT_ASSERT_EQUAL(expected->getStudyDescription(), actual->getStudyDescription());
    CPPUNIT_ASSERT_EQUAL(expected->getStudyInstanceUID(), actual->getStudyInstanceUID());
    CPPUNIT_ASSERT_EQUAL(expected->getStudyID(), actual->getStudyID());
    CPPUNIT_ASSERT_EQUAL(expected->getStudyDate(), actual->getStudyDate());
    CPPUNIT_ASSERT_EQUAL(expected->getStudyTime(), actual->getStudyTime());
    CPPUNIT_ASSERT_EQUAL(expected->getReferringPhysicianName(), actual->getReferringPhysicianName());

    // Patient Study Module
    CPPUNIT_ASSERT_EQUAL(expected->getPatientAge(), actual->getPatientAge());

    CPPUNIT_ASSERT_EQUAL(expected->getPatientSize().has_value(), actual->getPatientSize().has_value());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        expected->getPatientSize().value_or(0.0),
        actual->getPatientSize().value_or(0.0),
        DOUBLE_EPSILON
    );

    CPPUNIT_ASSERT_EQUAL(expected->getPatientWeight().has_value(), actual->getPatientWeight().has_value());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        expected->getPatientWeight().value_or(0.0),
        actual->getPatientWeight().value_or(0.0),
        DOUBLE_EPSILON
    );
}

//------------------------------------------------------------------------------

void SessionTest::seriesTest()
{
    testCombine<data::Series>();
}

//------------------------------------------------------------------------------

template<>
inline data::Activity::sptr generate<data::Activity>(const std::size_t variant)
{
    auto object = data::Activity::New();

    object->setData(create<data::Composite>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Activity>(const data::Activity::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Activity>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

    CPPUNIT_ASSERT_EQUAL(expected->getActivityConfigId(), actual->getActivityConfigId());

    // test Data
    compare<data::Composite>(actual->getData(), variant);
}

//------------------------------------------------------------------------------

void SessionTest::activityTest()
{
    testCombine<data::Activity>();
}

//------------------------------------------------------------------------------

template<>
inline data::Array::sptr generate<data::Array>(const std::size_t variant)
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
inline void compare<data::Array>(const data::Array::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Array>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

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
    testCombine<data::Array>();
}

//------------------------------------------------------------------------------

template<>
inline data::Image::sptr generate<data::Image>(const std::size_t variant)
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
inline void compare<data::Image>(const data::Image::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Image>(variant);

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
    testCombine<data::Image>();
}

//------------------------------------------------------------------------------

template<>
inline data::Vector::sptr generate<data::Vector>(const std::size_t variant)
{
    auto object = data::Vector::New();

    object->push_back(create<data::Boolean>(variant));
    object->push_back(create<data::Integer>(variant));
    object->push_back(create<data::Float>(variant));
    object->push_back(create<data::String>(variant));
    object->push_back(create<data::Activity>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Vector>(const data::Vector::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    auto it = actual->cbegin();

    compare<data::Boolean>(std::dynamic_pointer_cast<data::Boolean>(*it++), variant);
    compare<data::Integer>(std::dynamic_pointer_cast<data::Integer>(*it++), variant);
    compare<data::Float>(std::dynamic_pointer_cast<data::Float>(*it++), variant);
    compare<data::String>(std::dynamic_pointer_cast<data::String>(*it++), variant);
    compare<data::Activity>(std::dynamic_pointer_cast<data::Activity>(*it++), variant);
}

//------------------------------------------------------------------------------

void SessionTest::vectorTest()
{
    testCombine<data::Vector>();
}

//------------------------------------------------------------------------------

template<>
inline data::Point::sptr generate<data::Point>(const std::size_t /*unused*/)
{
    auto object = data::Point::New();

    object->setCoord({random<double>(), random<double>(), random<double>()});

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Point>(const data::Point::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Point>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

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
    testCombine<data::Point>();
}

//------------------------------------------------------------------------------

template<>
inline data::PointList::sptr generate<data::PointList>(const std::size_t variant)
{
    auto object = data::PointList::New();

    auto& points = object->getPoints();
    for(std::size_t i = 0, end = variant + 3 ; i < end ; ++i)
    {
        points.push_back(create<data::Point>(i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::PointList>(const data::PointList::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::PointList>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

    const auto& expectedPoints = expected->getPoints();
    const auto& actualPoints   = actual->getPoints();
    CPPUNIT_ASSERT_EQUAL(expectedPoints.size(), actualPoints.size());

    for(std::size_t i = 0, end = expectedPoints.size() ; i < end ; ++i)
    {
        compare<data::Point>(actualPoints.at(i), i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::pointListTest()
{
    testCombine<data::PointList>();
}

//------------------------------------------------------------------------------

template<>
inline data::CalibrationInfo::sptr generate<data::CalibrationInfo>(const std::size_t variant)
{
    auto object = data::CalibrationInfo::New();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        // Create the Image
        auto image = create<data::Image>(variant + i);

        // Create the PointList
        auto pointList = create<data::PointList>(variant + i);

        object->addRecord(image, pointList);
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::CalibrationInfo>(const data::CalibrationInfo::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        const auto& image = actual->getImage(i);
        compare<data::Image>(image, variant + i);

        const auto& pointList = actual->getPointList(image);
        compare<data::PointList>(pointList, variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::calibrationInfoTest()
{
    testCombine<data::CalibrationInfo>();
}

//------------------------------------------------------------------------------

template<>
inline data::Camera::sptr generate<data::Camera>(const std::size_t variant)
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
    object->setCameraID(UUID::generateUUID());
    object->setMaximumFrameRate(random<float>());
    constexpr std::array pixelFormats {
        data::Camera::PixelFormat::ADOBEDNG,
        data::Camera::PixelFormat::ARGB32,
        data::Camera::PixelFormat::ARGB32_PREMULTIPLIED,
        data::Camera::PixelFormat::RGB32,
        data::Camera::PixelFormat::RGB24,
        data::Camera::PixelFormat::RGB565,
        data::Camera::PixelFormat::RGB555,
        data::Camera::PixelFormat::ARGB8565_PREMULTIPLIED,
        data::Camera::PixelFormat::BGRA32,
        data::Camera::PixelFormat::BGRA32_PREMULTIPLIED,
        data::Camera::PixelFormat::BGR32,
        data::Camera::PixelFormat::BGR24,
        data::Camera::PixelFormat::BGR565,
        data::Camera::PixelFormat::BGR555,
        data::Camera::PixelFormat::BGRA5658_PREMULTIPLIED,
        data::Camera::PixelFormat::AYUV444,
        data::Camera::PixelFormat::AYUV444_PREMULTIPLIED,
        data::Camera::PixelFormat::YUV444,
        data::Camera::PixelFormat::YUV420P,
        data::Camera::PixelFormat::YV12,
        data::Camera::PixelFormat::UYVY,
        data::Camera::PixelFormat::YUYV,
        data::Camera::PixelFormat::NV12,
        data::Camera::PixelFormat::NV21,
        data::Camera::PixelFormat::IMC1,
        data::Camera::PixelFormat::IMC2,
        data::Camera::PixelFormat::IMC3,
        data::Camera::PixelFormat::IMC4,
        data::Camera::PixelFormat::Y8,
        data::Camera::PixelFormat::Y16,
        data::Camera::PixelFormat::JPEG,
        data::Camera::PixelFormat::CAMERARAW,
        data::Camera::PixelFormat::ADOBEDNG,
        data::Camera::PixelFormat::RGBA32,
        data::Camera::PixelFormat::USER,
        data::Camera::PixelFormat::INVALID
    };
    object->setPixelFormat(pixelFormats[variant % 35]);
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
inline void compare<data::Camera>(const data::Camera::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Camera>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

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
    testCombine<data::Camera>();
}

//------------------------------------------------------------------------------

template<>
inline data::Color::sptr generate<data::Color>(const std::size_t /*unused*/)
{
    auto object = data::Color::New();

    object->setRGBA(random<float>(), random<float>(), random<float>(), random<float>());

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Color>(const data::Color::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Color>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->red(), actual->red(), FLOAT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->green(), actual->green(), FLOAT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->blue(), actual->blue(), FLOAT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->alpha(), actual->alpha(), FLOAT_EPSILON);
}

//------------------------------------------------------------------------------

void SessionTest::colorTest()
{
    testCombine<data::Color>();
}

//------------------------------------------------------------------------------

template<>
inline data::Edge::sptr generate<data::Edge>(const std::size_t /*unused*/)
{
    auto object = data::Edge::New();

    object->setNature(UUID::generateUUID());
    object->setIdentifiers(UUID::generateUUID(), UUID::generateUUID());

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Edge>(const data::Edge::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Edge>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

    CPPUNIT_ASSERT_EQUAL(expected->getFromPortID(), actual->getFromPortID());
    CPPUNIT_ASSERT_EQUAL(expected->getToPortID(), actual->getToPortID());
    CPPUNIT_ASSERT_EQUAL(expected->getNature(), actual->getNature());
}

//------------------------------------------------------------------------------

void SessionTest::edgeTest()
{
    testCombine<data::Edge>();
}

//------------------------------------------------------------------------------

template<>
inline data::Port::sptr generate<data::Port>(const std::size_t /*unused*/)
{
    auto object = data::Port::New();

    object->setIdentifier(UUID::generateUUID());
    object->setType(UUID::generateUUID());

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Port>(const data::Port::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Port>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

    CPPUNIT_ASSERT_EQUAL(expected->getIdentifier(), actual->getIdentifier());
    CPPUNIT_ASSERT_EQUAL(expected->getType(), actual->getType());
}

//------------------------------------------------------------------------------

void SessionTest::portTest()
{
    testCombine<data::Port>();
}

//------------------------------------------------------------------------------

template<>
inline data::Node::sptr generate<data::Node>(const std::size_t variant)
{
    auto object = data::Node::New();

    object->setObject(create<data::String>(variant));

    const std::size_t portCount = variant + 2;
    for(std::size_t i = 0, end = portCount ; i < end ; ++i)
    {
        object->addInputPort(create<data::Port>(variant + i));
    }

    for(std::size_t i = portCount, end = 2 * (portCount) ; i < end ; ++i)
    {
        object->addOutputPort(create<data::Port>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Node>(const data::Node::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    compare<data::String>(std::dynamic_pointer_cast<data::String>(actual->getObject()), variant);

    const auto& inputs = actual->getInputPorts();
    for(std::size_t i = 0, end = inputs.size() ; i < end ; ++i)
    {
        compare<data::Port>(inputs.at(i), variant + i);
    }

    const auto& outputs = actual->getOutputPorts();
    for(std::size_t i = 0, end = outputs.size() ; i < end ; ++i)
    {
        compare<data::Port>(outputs.at(i), inputs.size() + variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::nodeTest()
{
    testCombine<data::Node>();
}

//------------------------------------------------------------------------------

template<>
inline data::Graph::sptr generate<data::Graph>(const std::size_t variant)
{
    auto object = data::Graph::New();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        auto upNode   = create<data::Node>(variant + i);
        auto downNode = create<data::Node>(variant + i + 1);

        object->addNode(upNode);
        object->addNode(downNode);
        object->addEdge(create<data::Edge>(variant + i), upNode, downNode);
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Graph>(const data::Graph::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    std::size_t i = 0;
    for(const auto& connection : actual->getConnections())
    {
        compare<data::Edge>(connection.first, i + variant);
        compare<data::Node>(connection.second.first, i + variant);
        compare<data::Node>(connection.second.second, ++i + variant);
    }
}

//------------------------------------------------------------------------------

void SessionTest::graphTest()
{
    testCombine<data::Graph>();
}

//------------------------------------------------------------------------------

template<>
inline data::Landmarks::sptr generate<data::Landmarks>(const std::size_t variant)
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
inline void compare<data::Landmarks>(const data::Landmarks::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Landmarks>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

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
    testCombine<data::Landmarks>();
}

//------------------------------------------------------------------------------

template<>
inline data::Line::sptr generate<data::Line>(const std::size_t variant)
{
    auto object = data::Line::New();

    object->setPosition(create<data::Point>(variant));
    object->setDirection(create<data::Point>(variant + 1));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Line>(const data::Line::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    compare<data::Point>(actual->getPosition(), variant);
    compare<data::Point>(actual->getDirection(), variant + 1);
}

//------------------------------------------------------------------------------

void SessionTest::lineTest()
{
    testCombine<data::Line>();
}

//------------------------------------------------------------------------------

template<>
inline data::Material::sptr generate<data::Material>(const std::size_t variant)
{
    auto object = data::Material::New();

    std::array shading {
        data::Material::ShadingType::AMBIENT,
        data::Material::ShadingType::FLAT,
        data::Material::ShadingType::PHONG
    };

    std::array representation {
        data::Material::RepresentationType::EDGE,
        data::Material::RepresentationType::POINT,
        data::Material::RepresentationType::SURFACE,
        data::Material::RepresentationType::WIREFRAME
    };

    std::array options {
        data::Material::OptionsType::CELLS_NORMALS,
        data::Material::OptionsType::NORMALS,
        data::Material::OptionsType::STANDARD
    };

    // Set ambient color
    object->setAmbient(create<data::Color>(variant));

    // Set diffuse color
    object->setDiffuse(create<data::Color>(variant + 1));

    // Set diffuse texture
    object->setDiffuseTexture(create<data::Image>(variant));

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
inline void compare<data::Material>(const data::Material::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Material>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

    // Test ambient
    compare<data::Color>(actual->ambient(), variant);

    // Test diffuse
    compare<data::Color>(actual->diffuse(), variant + 1);

    // Test diffuse texture
    compare<data::Image>(actual->getDiffuseTexture(), variant);

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
    testCombine<data::Material>();
}

//------------------------------------------------------------------------------

template<>
inline data::Matrix4::sptr generate<data::Matrix4>(const std::size_t /*unused*/)
{
    auto object = data::Matrix4::New();

    auto& coefficients = object->getCoefficients();
    for(double& coefficient : coefficients)
    {
        coefficient = random<double>();
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Matrix4>(const data::Matrix4::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Matrix4>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

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
    testCombine<data::Matrix4>();
}

//------------------------------------------------------------------------------

template<>
inline data::Plane::sptr generate<data::Plane>(const std::size_t variant)
{
    auto object = data::Plane::New();

    auto& points = object->getPoints();
    for(std::size_t i = 0, end = points.size() ; i < end ; ++i)
    {
        points[i] = create<data::Point>(i + variant);
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Plane>(const data::Plane::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    const auto& points = actual->getPoints();
    for(std::size_t i = 0, end = points.size() ; i < end ; ++i)
    {
        compare<data::Point>(points.at(i), i + variant);
    }
}

//------------------------------------------------------------------------------

void SessionTest::planeTest()
{
    testCombine<data::Plane>();
}

//------------------------------------------------------------------------------

template<>
inline data::PlaneList::sptr generate<data::PlaneList>(const std::size_t variant)
{
    auto object = data::PlaneList::New();

    auto& planes = object->getPlanes();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        planes.push_back(create<data::Plane>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::PlaneList>(const data::PlaneList::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    const auto& planes = actual->getPlanes();
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        compare<data::Plane>(planes.at(i), variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::planeListTest()
{
    testCombine<data::PlaneList>();
}

//------------------------------------------------------------------------------

template<>
inline data::Reconstruction::sptr generate<data::Reconstruction>(const std::size_t variant)
{
    auto object = data::Reconstruction::New();

    object->setIsVisible(variant % 3 == 0);
    object->setOrganName(UUID::generateUUID());
    object->setStructureType(UUID::generateUUID());
    object->setComputedMaskVolume(random<double>());

    // Material
    object->setMaterial(create<data::Material>(variant));

    // Image
    object->setImage(create<data::Image>(variant));

    // Mesh
    object->setMesh(create<data::Mesh>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Reconstruction>(const data::Reconstruction::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Reconstruction>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

    CPPUNIT_ASSERT_EQUAL(expected->getIsVisible(), actual->getIsVisible());
    CPPUNIT_ASSERT_EQUAL(expected->getOrganName(), actual->getOrganName());
    CPPUNIT_ASSERT_EQUAL(expected->getStructureType(), actual->getStructureType());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        expected->getComputedMaskVolume(),
        actual->getComputedMaskVolume(),
        DOUBLE_EPSILON
    );

    // Material
    compare<data::Material>(actual->getMaterial(), variant);

    // Image
    compare<data::Image>(actual->getImage(), variant);

    // Mesh
    compare<data::Mesh>(actual->getMesh(), variant);
}

//------------------------------------------------------------------------------

void SessionTest::reconstructionTest()
{
    testCombine<data::Reconstruction>();
}

//------------------------------------------------------------------------------

template<>
inline data::StructureTraits::sptr generate<data::StructureTraits>(const std::size_t variant)
{
    auto object = data::StructureTraits::New();

    const std::array CLASSES {
        data::StructureTraits::StructureClass::ENVIRONMENT,
        data::StructureTraits::StructureClass::FUNCTIONAL,
        data::StructureTraits::StructureClass::LESION,
        data::StructureTraits::StructureClass::NO_CONSTRAINT,
        data::StructureTraits::StructureClass::ORGAN,
        data::StructureTraits::StructureClass::TOOL,
        data::StructureTraits::StructureClass::VESSEL
    };

    const std::array CATEGORIES {
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
    object->setColor(create<data::Color>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::StructureTraits>(const data::StructureTraits::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::StructureTraits>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

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
    compare<data::Color>(actual->getColor(), variant);
}

//------------------------------------------------------------------------------

void SessionTest::structureTraitsTest()
{
    testCombine<data::StructureTraits>();
}

//------------------------------------------------------------------------------

template<>
inline data::StructureTraitsDictionary::sptr generate<data::StructureTraitsDictionary>(const std::size_t variant)
{
    auto object = data::StructureTraitsDictionary::New();

    auto organ = create<data::StructureTraits>(variant);
    organ->setClass(data::StructureTraits::ORGAN);
    organ->setAttachmentType("");

    object->addStructure(organ);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        auto structure            = create<data::StructureTraits>(variant + i + 1);
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
inline void compare<data::StructureTraitsDictionary>(
    const data::StructureTraitsDictionary::csptr& actual,
    const std::size_t variant
)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::StructureTraitsDictionary>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

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
    testCombine<data::StructureTraitsDictionary>();
}

//------------------------------------------------------------------------------

template<>
inline data::ReconstructionTraits::sptr generate<data::ReconstructionTraits>(const std::size_t variant)
{
    auto object = data::ReconstructionTraits::New();

    object->setIdentifier(UUID::generateUUID());

    // Reconstruction mask operator node
    object->setMaskOpNode(create<data::Node>(variant));

    // Reconstruction mesh operator node
    object->setMeshOpNode(create<data::Node>(variant + 1));

    // Associated structure traits
    object->setStructureTraits(create<data::StructureTraits>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::ReconstructionTraits>(
    const data::ReconstructionTraits::csptr& actual,
    const std::size_t variant
)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::ReconstructionTraits>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

    CPPUNIT_ASSERT_EQUAL(expected->getIdentifier(), actual->getIdentifier());

    // Reconstruction mask operator node
    compare<data::Node>(actual->getMaskOpNode(), variant);

    // Reconstruction mesh operator node
    compare<data::Node>(actual->getMeshOpNode(), variant + 1);

    // Associated structure traits
    compare<data::StructureTraits>(actual->getStructureTraits(), variant);
}

//------------------------------------------------------------------------------

void SessionTest::reconstructionTraitsTest()
{
    testCombine<data::ReconstructionTraits>();
}

//------------------------------------------------------------------------------

template<>
inline data::Resection::sptr generate<data::Resection>(const std::size_t variant)
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
        inputs.push_back(create<data::Reconstruction>(variant + i));
        outputs.push_back(create<data::Reconstruction>(variant + i + 1));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Resection>(const data::Resection::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::Resection>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

    CPPUNIT_ASSERT_EQUAL(expected->getName(), actual->getName());
    CPPUNIT_ASSERT_EQUAL(expected->getIsSafePart(), actual->getIsSafePart());
    CPPUNIT_ASSERT_EQUAL(expected->getIsValid(), actual->getIsValid());
    CPPUNIT_ASSERT_EQUAL(expected->getIsVisible(), actual->getIsVisible());

    const auto& inputs  = actual->getInputs();
    const auto& outputs = actual->getOutputs();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        compare<data::Reconstruction>(inputs.at(i), variant + i);
        compare<data::Reconstruction>(outputs.at(i), variant + i + 1);
    }
}

//------------------------------------------------------------------------------

void SessionTest::resectionTest()
{
    testCombine<data::Resection>();
}

//------------------------------------------------------------------------------

template<>
inline data::ResectionDB::sptr generate<data::ResectionDB>(const std::size_t variant)
{
    auto object = data::ResectionDB::New();

    object->setSafeResection(create<data::Resection>(variant));

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->addResection(create<data::Resection>(variant + i + 1));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::ResectionDB>(const data::ResectionDB::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    compare<data::Resection>(actual->getSafeResection(), variant);

    const auto& resections = actual->getResections();
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        compare<data::Resection>(resections.at(i), variant + i + 1);
    }
}

//------------------------------------------------------------------------------

void SessionTest::resectionDBTest()
{
    testCombine<data::ResectionDB>();
}

//------------------------------------------------------------------------------

template<>
inline data::ROITraits::sptr generate<data::ROITraits>(const std::size_t variant)
{
    auto object = data::ROITraits::New();

    object->setIdentifier(UUID::generateUUID());
    object->setEvaluatedExp(UUID::generateUUID());
    object->setMaskOpNode(create<data::Node>(variant));
    object->setStructureTraits(create<data::StructureTraits>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::ROITraits>(const data::ROITraits::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::ROITraits>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

    CPPUNIT_ASSERT_EQUAL(expected->getIdentifier(), actual->getIdentifier());
    CPPUNIT_ASSERT_EQUAL(expected->getEvaluatedExp(), actual->getEvaluatedExp());

    // Node
    compare<data::Node>(actual->getMaskOpNode(), variant);

    // Structure
    compare<data::StructureTraits>(actual->getStructureTraits(), variant);
}

//------------------------------------------------------------------------------

void SessionTest::roiTraitsTest()
{
    testCombine<data::ROITraits>();
}

//------------------------------------------------------------------------------

template<>
inline data::TransferFunction::sptr generate<data::TransferFunction>(const std::size_t variant)
{
    auto object = data::TransferFunction::New();

    object->setLevel(random<double>());
    object->setWindow(random<double>());
    object->setName(UUID::generateUUID());
    object->setBackgroundColor(
        data::TransferFunction::color_t(
            random<double>(),
            random<double>(),
            random<double>(),
            random<double>()
        )
    );

    auto tfData = object->pieces().emplace_back(data::TransferFunctionPiece::New());
    tfData->setInterpolationMode(
        variant % 3 == 0
        ? data::TransferFunction::InterpolationMode::LINEAR
        : data::TransferFunction::InterpolationMode::NEAREST
    );
    tfData->setClamped(variant % 4 == 0);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        tfData->insert(
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
inline void compare<data::TransferFunction>(const data::TransferFunction::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::TransferFunction>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->level(), actual->level(), DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected->window(), actual->window(), DOUBLE_EPSILON);
    CPPUNIT_ASSERT_EQUAL(expected->name(), actual->name());

    const auto& expectedBackgroundColor = expected->backgroundColor();
    const auto& actualBackgroundColor   = actual->backgroundColor();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedBackgroundColor.r, actualBackgroundColor.r, DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedBackgroundColor.g, actualBackgroundColor.g, DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedBackgroundColor.b, actualBackgroundColor.b, DOUBLE_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedBackgroundColor.a, actualBackgroundColor.a, DOUBLE_EPSILON);

    auto expectedPiece = expected->pieces().front();
    auto actualPiece   = actual->pieces().front();

    CPPUNIT_ASSERT_EQUAL(expectedPiece->interpolationMode(), actualPiece->interpolationMode());
    CPPUNIT_ASSERT_EQUAL(expectedPiece->clamped(), actualPiece->clamped());

    for(const auto& expectedData : *expectedPiece)
    {
        const auto& expectedColor = expectedData.second;
        const auto& actualColor   = actualPiece->at(expectedData.first);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor.r, actualColor.r, DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor.g, actualColor.g, DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor.b, actualColor.b, DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedColor.a, actualColor.a, DOUBLE_EPSILON);
    }
}

//------------------------------------------------------------------------------

void SessionTest::transferFunctionTest()
{
    testCombine<data::TransferFunction>();
}

//------------------------------------------------------------------------------

template<>
inline data::DicomSeries::sptr generate<data::DicomSeries>(const std::size_t variant)
{
    data::DicomSeries::sptr dicomSeries;

    // Only load the real dicom once
    if(variant == 0)
    {
        // Setup the SeriesSet to be able to read
        auto series_set                  = data::SeriesSet::New();
        const std::filesystem::path path = utestData::Data::dir()
                                           / "sight/Patient/Dicom/DicomDB/86-CT-Skull";

        CPPUNIT_ASSERT_MESSAGE(
            "The dicom directory '" + path.string() + "' does not exist",
            std::filesystem::exists(path)
        );

        // Read source Dicom
        auto reader = io::dicom::reader::SeriesSet::New();
        reader->setObject(series_set);
        reader->setFolder(path);

        CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), series_set->size());

        dicomSeries = std::dynamic_pointer_cast<data::DicomSeries>(series_set->front());
    }
    else
    {
        // Take the first variant as basis
        dicomSeries = data::DicomSeries::New();
        dicomSeries->shallowCopy(getExpected<data::DicomSeries>(0));
    }

    // Randomize a bit the dicomSeries
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        dicomSeries->addSOPClassUID(UUID::generateUUID());
        dicomSeries->addComputedTagValue(UUID::generateUUID(), UUID::generateUUID());
    }

    // Inherited attributes
    dicomSeries->Series::shallowCopy(getExpected<data::Series>(variant));

    return dicomSeries;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::DicomSeries>(const data::DicomSeries::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::DicomSeries>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

    // Test inherited attributes
    compare<data::Series>(actual, variant);

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

    testCombine<data::DicomSeries>();
}

//------------------------------------------------------------------------------

template<>
inline data::ImageSeries::sptr generate<data::ImageSeries>(const std::size_t variant)
{
    auto object = data::ImageSeries::New();

    object->setContrastBolusAgent(UUID::generateUUID());
    object->setContrastBolusRoute(UUID::generateUUID());
    object->setContrastBolusVolume(double(variant));
    object->setContrastBolusStartTime(generateTM(variant));
    object->setContrastBolusStopTime(generateTM(variant));
    object->setContrastBolusTotalDose(double(variant));

    object->setContrastFlowRate(
        std::to_string(variant)
        + "\\"
        + std::to_string(variant + 1)
        + "\\"
        + std::to_string(variant + 2)
    );

    object->setContrastFlowDuration(
        std::to_string(variant + 4)
        + "\\"
        + std::to_string(variant + 5)
        + "\\"
        + std::to_string(variant + 6)
    );

    object->setContrastBolusIngredient(UUID::generateUUID());
    object->setContrastBolusIngredientConcentration(double(variant));
    object->setAcquisitionDate(generateDA(variant));
    object->setAcquisitionTime(generateTM(variant));

    object->setDicomReference(create<data::DicomSeries>(variant));

    // Inherited attributes
    object->Series::shallowCopy(getExpected<data::Series>(variant));
    object->Image::shallowCopy(getExpected<data::Image>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::ImageSeries>(const data::ImageSeries::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Retrieve the expected variant
    const auto& expected = getExpected<data::ImageSeries>(variant);

    // Test Object attributes
    CPPUNIT_ASSERT_EQUAL(expected->getDescription(), actual->getDescription());

    // Test inherited attributes
    compare<data::Series>(actual, variant);
    compare<data::Image>(actual, variant);

    CPPUNIT_ASSERT_EQUAL(expected->getContrastBolusAgent(), actual->getContrastBolusAgent());
    CPPUNIT_ASSERT_EQUAL(expected->getContrastBolusRoute(), actual->getContrastBolusRoute());

    CPPUNIT_ASSERT_EQUAL(expected->getContrastBolusVolume().has_value(), actual->getContrastBolusVolume().has_value());

    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        expected->getContrastBolusVolume().value_or(0.0),
        actual->getContrastBolusVolume().value_or(0.0),
        DOUBLE_EPSILON
    );

    CPPUNIT_ASSERT_EQUAL(expected->getContrastBolusStartTime(), actual->getContrastBolusStartTime());
    CPPUNIT_ASSERT_EQUAL(expected->getContrastBolusStopTime(), actual->getContrastBolusStopTime());

    CPPUNIT_ASSERT_EQUAL(
        expected->getContrastBolusTotalDose().has_value(),
        actual->getContrastBolusTotalDose().has_value()
    );

    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        expected->getContrastBolusTotalDose().value_or(0.0),
        actual->getContrastBolusTotalDose().value_or(0.0),
        DOUBLE_EPSILON
    );

    CPPUNIT_ASSERT_EQUAL(expected->getContrastFlowRate(), actual->getContrastFlowRate());
    CPPUNIT_ASSERT_EQUAL(expected->getContrastFlowDuration(), actual->getContrastFlowDuration());
    CPPUNIT_ASSERT_EQUAL(expected->getContrastBolusIngredient(), actual->getContrastBolusIngredient());

    CPPUNIT_ASSERT_EQUAL(
        expected->getContrastBolusIngredientConcentration().has_value(),
        actual->getContrastBolusIngredientConcentration().has_value()
    );

    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        expected->getContrastBolusIngredientConcentration().value_or(0.0),
        actual->getContrastBolusIngredientConcentration().value_or(0.0),
        DOUBLE_EPSILON
    );

    CPPUNIT_ASSERT_EQUAL(expected->getAcquisitionDate(), actual->getAcquisitionDate());
    CPPUNIT_ASSERT_EQUAL(expected->getAcquisitionTime(), actual->getAcquisitionTime());

    compare<data::DicomSeries>(actual->getDicomReference(), variant);
}

//------------------------------------------------------------------------------

void SessionTest::imageSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    testCombine<data::ImageSeries>();
}

//------------------------------------------------------------------------------

template<>
inline data::ModelSeries::sptr generate<data::ModelSeries>(const std::size_t variant)
{
    auto object = data::ModelSeries::New();

    object->setDicomReference(create<data::DicomSeries>(variant));

    std::vector<data::Reconstruction::sptr> reconstructionDB;
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        reconstructionDB.push_back(create<data::Reconstruction>(variant + i));
    }

    object->setReconstructionDB(reconstructionDB);

    // Inherited attributes
    object->Series::shallowCopy(getExpected<data::Series>(variant));

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::ModelSeries>(const data::ModelSeries::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    // Test inherited attributes
    compare<data::Series>(actual, variant);

    // Test other attributes
    compare<data::DicomSeries>(actual->getDicomReference(), variant);

    const auto& reconstructionDB = actual->getReconstructionDB();
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        compare<data::Reconstruction>(reconstructionDB.at(i), variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::modelSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    testCombine<data::ModelSeries>();
}

//------------------------------------------------------------------------------

template<>
inline data::ActivitySet::sptr generate<data::ActivitySet>(const std::size_t variant)
{
    auto object = data::ActivitySet::New();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<data::Activity>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::ActivitySet>(const data::ActivitySet::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        compare<data::Activity>(actual->at(i), variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::activitySetTest()
{
    testCombine<data::ActivitySet>();
}

//------------------------------------------------------------------------------

template<>
inline data::CameraSet::sptr generate<data::CameraSet>(const std::size_t variant)
{
    auto object = data::CameraSet::New();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        auto camera = create<data::Camera>(variant + i);
        auto matrix = create<data::Matrix4>(variant + i);
        object->push_back(std::make_pair(camera, matrix));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::CameraSet>(const data::CameraSet::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        compare<data::Camera>(actual->at(i).first, variant + i);
        compare<data::Matrix4>(actual->at(i).second, variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::cameraSetTest()
{
    testCombine<data::CameraSet>();
}

//------------------------------------------------------------------------------

template<>
inline data::SeriesSet::sptr generate<data::SeriesSet>(const std::size_t variant)
{
    auto object = data::SeriesSet::New();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<data::Series>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::SeriesSet>(const data::SeriesSet::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        compare<data::Series>(actual->at(i), variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::seriesSetTest()
{
    testCombine<data::SeriesSet>();
}

//------------------------------------------------------------------------------

template<>
inline data::Set::sptr generate<data::Set>(const std::size_t variant)
{
    auto object = data::Set::New();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<data::Series>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

template<>
inline void compare<data::Set>(const data::Set::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        compare<data::Series>(std::dynamic_pointer_cast<data::Series>(actual->at(i)), variant + i);
    }
}

//------------------------------------------------------------------------------

void SessionTest::setTest()
{
    testCombine<data::Set>();
}

//------------------------------------------------------------------------------

inline static void customSerialize(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Cast to the right type
    const auto string = Helper::safe_cast<data::String>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::String>(tree, 666);

    Helper::writeString(tree, "custom", string->getValue());
}

//------------------------------------------------------------------------------

inline static data::String::sptr customDeserialize(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& /*unused*/,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto string = Helper::cast_or_create<data::String>(object);

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
        auto object = create<data::String>(0);

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
        compare<data::String>(object, 0);
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

} // namespace sight::io::session::ut
