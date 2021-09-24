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

#include "IoAtomsTest.hpp"

#include <core/runtime/EConfigurationElement.hpp>
#include <core/thread/ActiveWorkers.hpp>
#include <core/tools/System.hpp>

#include <data/Array.hpp>
#include <data/Composite.hpp>
#include <data/reflection/visitor/CompareObjects.hpp>
#include <data/SeriesDB.hpp>

#include <io/base/service/ioTypes.hpp>

#include <service/extension/Factory.hpp>
#include <service/op/Add.hpp>
#include <service/registry/ObjectService.hpp>

#include <utest/Exception.hpp>

#include <utestData/generator/SeriesDB.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::atoms::ut::IoAtomsTest);

namespace sight::module::io::atoms
{

namespace ut
{

//------------------------------------------------------------------------------

void IoAtomsTest::setUp()
{
    // Set up context before running a test.
    core::thread::Worker::sptr worker = core::thread::Worker::New();
    core::thread::ActiveWorkers::setDefaultWorker(worker);
}

//------------------------------------------------------------------------------

void IoAtomsTest::tearDown()
{
    // Clean up after the test run.
    core::thread::ActiveWorkers::getDefault()->clearRegistry();
}

//------------------------------------------------------------------------------

template<typename T>
void compareLog(T& comparator)
{
    SPTR(data::reflection::visitor::CompareObjects::PropsMapType) props = comparator.getDifferences();
    for(data::reflection::visitor::CompareObjects::PropsMapType::value_type prop : (*props))
    {
        SIGHT_ERROR("new object difference found : " << prop.first << " != " << prop.second);
    }
}

//------------------------------------------------------------------------------

template<typename T>
void write(const service::IService::ConfigType& srvCfg, const SPTR(T)& obj, const std::string& writer)
{
    service::IService::sptr writerSrv = service::add(writer);
    CPPUNIT_ASSERT(writerSrv);

    writerSrv->setInput(obj, sight::io::base::service::s_DATA_KEY);
    writerSrv->setConfiguration(srvCfg);
    writerSrv->configure();
    writerSrv->start().wait();
    writerSrv->update().wait();
    writerSrv->stop().wait();
    service::OSR::unregisterService(writerSrv);
}

template<typename T>
SPTR(T) read(const service::IService::ConfigType& srvCfg, const std::string& reader)
{
    typename T::sptr readObj          = T::New();
    service::IService::sptr readerSrv = service::add(reader);
    CPPUNIT_ASSERT(readerSrv);

    readerSrv->setInOut(readObj, sight::io::base::service::s_DATA_KEY);
    readerSrv->setConfiguration(srvCfg);
    readerSrv->configure();
    readerSrv->start().wait();
    readerSrv->update().wait();
    readerSrv->stop().wait();
    service::OSR::unregisterService(readerSrv);

    return readObj;
}

//------------------------------------------------------------------------------

template<typename T>
void writeReadFile(
    const service::IService::ConfigType& srvCfg,
    const SPTR(T)& obj,
    const std::string& writer,
    const std::string& reader
)
{
    write(srvCfg, obj, writer);

    SPTR(T) readObj = read<T>(srvCfg, reader);

    // Compare
    using namespace data::reflection::visitor;
    CompareObjects visitor;

    visitor.compare(readObj, obj);
    compareLog(visitor);

    CPPUNIT_ASSERT_MESSAGE("Objects not equal", visitor.getDifferences()->empty());
}

//------------------------------------------------------------------------------

void atomTest(const std::filesystem::path& filePath)
{
    service::IService::ConfigType srvCfg;
    srvCfg.add("file", filePath.string());

    data::SeriesDB::sptr seriesDB   = utestData::generator::SeriesDB::createSeriesDB(2, 2, 2);
    data::Composite::sptr workspace = data::Composite::New();
    workspace->getContainer()["processingDB"] = data::Composite::New();
    workspace->getContainer()["planningDB"]   = data::Composite::New();

    std::filesystem::create_directories(filePath.parent_path());
    writeReadFile<data::Composite>(
        srvCfg,
        workspace,
        "sight::module::io::atoms::SWriter",
        "sight::module::io::atoms::SReader"
    );
    writeReadFile<data::SeriesDB>(
        srvCfg,
        seriesDB,
        "sight::module::io::atoms::SWriter",
        "sight::module::io::atoms::SReader"
    );

    data::SeriesDB::sptr readSeriesDB;

    // Default policy
    readSeriesDB = read<data::SeriesDB>(srvCfg, "sight::module::io::atoms::SReader");

    {
        data::reflection::visitor::CompareObjects visitor;
        visitor.compare(readSeriesDB, seriesDB);
        compareLog(visitor);
        CPPUNIT_ASSERT_MESSAGE("Objects not equal", visitor.getDifferences()->empty());
    }

    // 'Change' UUID policy
    srvCfg.add("uuidPolicy", "Change");

    readSeriesDB = read<data::SeriesDB>(srvCfg, "sight::module::io::atoms::SReader");

    {
        data::reflection::visitor::CompareObjects visitor;
        visitor.compare(readSeriesDB, seriesDB);
        compareLog(visitor);
        CPPUNIT_ASSERT_MESSAGE("Objects not equal", visitor.getDifferences()->empty());
    }

    // 'Strict' UUID policy
    srvCfg.put("uuidPolicy", "Strict");

    readSeriesDB = read<data::SeriesDB>(srvCfg, "sight::module::io::atoms::SReader");

    {
        // DuplicatedDataUUID exception should have been thrown and catch by reader before any data/atom conversion.
        // We can only check that loaded data is empty.
        CPPUNIT_ASSERT_MESSAGE("Written data should not be empty", !seriesDB->empty());
        CPPUNIT_ASSERT_MESSAGE("Loaded data should be empty", readSeriesDB->empty());
    }
}

//------------------------------------------------------------------------------

void atomTestSimpleData(const std::filesystem::path& filePath)
{
    service::IService::ConfigType srvCfg;
    srvCfg.add("file", filePath.string());

    data::Array::sptr array = data::Array::New();
    const auto dumpLock     = array->lock();
    data::Array::SizeType size {10, 100};

    array->resize(size, core::tools::Type::s_UINT32);

    std::uint32_t count = 0;
    auto iter           = array->begin<std::uint32_t>();
    const auto end      = array->end<std::uint32_t>();

    for( ; iter != end ; ++iter)
    {
        *iter = count++;
    }

    std::filesystem::create_directories(filePath.parent_path());

    write<data::Array>(srvCfg, array, "sight::module::io::atoms::SWriter");

    data::Array::sptr readArray = read<data::Array>(srvCfg, "sight::module::io::atoms::SReader");

    {
        data::reflection::visitor::CompareObjects visitor;
        visitor.compare(array, readArray);
        compareLog(visitor);
        for(const auto& it : *visitor.getDifferences())
        {
            std::cout << "first : " << it.first << " second " << it.second << std::endl;
        }

        CPPUNIT_ASSERT_MESSAGE("Objects not equal", visitor.getDifferences()->empty());
    }
}

//------------------------------------------------------------------------------

void IoAtomsTest::JSONTest()
{
    atomTest(core::tools::System::getTemporaryFolder() / "JSONTest" / "ioAtomsTest.json");
    atomTestSimpleData(core::tools::System::getTemporaryFolder() / "JSONTest" / "ioAtomsTest2.json");
}

//------------------------------------------------------------------------------

void IoAtomsTest::JSONZTest()
{
    atomTest(core::tools::System::getTemporaryFolder() / "JSONZTest" / "ioAtomsTest.jsonz");
    atomTestSimpleData(core::tools::System::getTemporaryFolder() / "JSONZTest" / "ioAtomsTest2.jsonz");
}

//------------------------------------------------------------------------------

void IoAtomsTest::XMLTest()
{
    atomTest(core::tools::System::getTemporaryFolder() / "XMLTest" / "ioAtomsTest.xml");
    atomTestSimpleData(core::tools::System::getTemporaryFolder() / "XMLTest" / "ioAtomsTest2.xml");
}

//------------------------------------------------------------------------------

void IoAtomsTest::XMLZTest()
{
    atomTest(core::tools::System::getTemporaryFolder() / "XMLZTest" / "ioAtomsTest.xmlz");
    atomTestSimpleData(core::tools::System::getTemporaryFolder() / "XMLZTest" / "ioAtomsTest2.xmlz");
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace sight::module::io::atoms
