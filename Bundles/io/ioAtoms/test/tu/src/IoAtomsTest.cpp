/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <fwData/Array.hpp>
#include <fwData/Composite.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwIO/ioTypes.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <fwTest/Exception.hpp>
#include <fwTest/generator/SeriesDB.hpp>

#include <fwTools/System.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ptree.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::ioAtoms::ut::IoAtomsTest );

namespace ioAtoms
{
namespace ut
{

static ::fwTest::Exception fwTestException(""); // force link with fwTest

//------------------------------------------------------------------------------

void IoAtomsTest::setUp()
{
    // Set up context before running a test.
    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();
    ::fwServices::registry::ActiveWorkers::setDefaultWorker(worker);
}

//------------------------------------------------------------------------------

void IoAtomsTest::tearDown()
{
    // Clean up after the test run.
    ::fwServices::registry::ActiveWorkers::getDefault()->clearRegistry();
}

//------------------------------------------------------------------------------

template <typename T>
void compareLog(T& comparator)
{
    SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = comparator.getDifferences();
    for(::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop :  (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " != " << prop.second );
    }
}

//------------------------------------------------------------------------------

template <typename T>
void write(const ::fwServices::IService::ConfigType& srvCfg, const SPTR(T)& obj, const std::string& writer)
{
    ::fwServices::IService::sptr writerSrv = ::fwServices::add( writer );
    CPPUNIT_ASSERT(writerSrv);

    ::fwServices::OSR::registerService( obj, ::fwIO::s_DATA_KEY, ::fwServices::IService::AccessType::INOUT, writerSrv );
    writerSrv->setConfiguration(srvCfg);
    writerSrv->configure();
    writerSrv->start().wait();
    writerSrv->update().wait();
    writerSrv->stop().wait();
    ::fwServices::OSR::unregisterService( writerSrv );
}

template <typename T>
SPTR(T) read(const ::fwServices::IService::ConfigType& srvCfg, const std::string& reader)
{

    typename T::sptr readObj = T::New();
    ::fwServices::IService::sptr readerSrv = ::fwServices::add( reader );
    CPPUNIT_ASSERT(readerSrv);

    ::fwServices::OSR::registerService( readObj, ::fwIO::s_DATA_KEY, ::fwServices::IService::AccessType::INOUT,
                                        readerSrv );
    readerSrv->setConfiguration(srvCfg);
    readerSrv->configure();
    readerSrv->start().wait();
    readerSrv->update().wait();
    readerSrv->stop().wait();
    ::fwServices::OSR::unregisterService( readerSrv );

    return readObj;
}

template <typename T>
SPTR(T) readOut(const ::fwServices::IService::ConfigType& srvCfg, const std::string& reader)
{
    ::fwServices::IService::ConfigType config(srvCfg);
    config.add("out.<xmlattr>.key", ::fwIO::s_DATA_KEY);

    ::fwServices::IService::sptr readerSrv = ::fwServices::add( reader );
    CPPUNIT_ASSERT(readerSrv);

    readerSrv->setConfiguration(config);
    readerSrv->configure();
    readerSrv->start().wait();
    readerSrv->update().wait();
    typename T::sptr readObj = readerSrv->getOutput<T>(::fwIO::s_DATA_KEY);
    readerSrv->stop().wait();
    ::fwServices::OSR::unregisterService( readerSrv );

    return readObj;
}

//------------------------------------------------------------------------------

template <typename T>
void writeReadFile(const ::fwServices::IService::ConfigType& srvCfg, const SPTR(T)& obj,
                   const std::string& writer, const std::string& reader)
{
    write(srvCfg, obj, writer);

    SPTR(T) readObj = read<T>(srvCfg, reader);

    // Compare
    using namespace ::fwDataCamp::visitor;
    CompareObjects visitor;

    visitor.compare(readObj, obj);
    compareLog(visitor);

    CPPUNIT_ASSERT_MESSAGE("Objects not equal", visitor.getDifferences()->empty() );
}

//------------------------------------------------------------------------------

void atomTest(const ::boost::filesystem::path& filePath)
{
    ::fwServices::IService::ConfigType srvCfg;
    srvCfg.add("file", filePath.string());

    ::fwMedData::SeriesDB::sptr seriesDB      = ::fwTest::generator::SeriesDB::createSeriesDB(2, 2, 2);
    ::fwData::Composite::sptr workspace       = ::fwData::Composite::New();
    workspace->getContainer()["processingDB"] = ::fwData::Composite::New();
    workspace->getContainer()["planningDB"]   = ::fwData::Composite::New();

    ::boost::filesystem::create_directories( filePath.parent_path() );
    writeReadFile< ::fwData::Composite>( srvCfg, workspace, "::ioAtoms::SWriter",  "::ioAtoms::SReader" );
    writeReadFile< ::fwMedData::SeriesDB >( srvCfg, seriesDB, "::ioAtoms::SWriter",  "::ioAtoms::SReader" );

    ::fwMedData::SeriesDB::sptr readSeriesDB;

    // Default policy
    readSeriesDB = read< ::fwMedData::SeriesDB >(srvCfg, "::ioAtoms::SReader");

    {
        ::fwDataCamp::visitor::CompareObjects visitor;
        visitor.compare(readSeriesDB, seriesDB);
        compareLog(visitor);
        CPPUNIT_ASSERT_MESSAGE("Objects not equal", visitor.getDifferences()->empty() );
    }

    readSeriesDB = readOut< ::fwMedData::SeriesDB >(srvCfg, "::ioAtoms::SReader");

    {
        ::fwDataCamp::visitor::CompareObjects visitor;
        visitor.compare(readSeriesDB, seriesDB);
        compareLog(visitor);
        CPPUNIT_ASSERT_MESSAGE("Objects not equal", visitor.getDifferences()->empty() );
    }

    // 'Change' UUID policy
    srvCfg.add("uuidPolicy", "Change");

    readSeriesDB = read< ::fwMedData::SeriesDB >(srvCfg, "::ioAtoms::SReader");

    {
        ::fwDataCamp::visitor::CompareObjects visitor;
        visitor.compare(readSeriesDB, seriesDB);
        compareLog(visitor);
        CPPUNIT_ASSERT_MESSAGE("Objects not equal", visitor.getDifferences()->empty() );
    }

    // Output with 'Change' UUID policy
    readSeriesDB = readOut< ::fwMedData::SeriesDB >(srvCfg, "::ioAtoms::SReader");

    {
        ::fwDataCamp::visitor::CompareObjects visitor;
        visitor.compare(readSeriesDB, seriesDB);
        compareLog(visitor);
        CPPUNIT_ASSERT_MESSAGE("Objects not equal", visitor.getDifferences()->empty() );
    }

    // 'Strict' UUID policy
    srvCfg.put("uuidPolicy", "Strict");

    readSeriesDB = read< ::fwMedData::SeriesDB >(srvCfg, "::ioAtoms::SReader");

    {
        // DuplicatedDataUUID exception should have been thrown and catch by reader before any data/atom conversion.
        // We can only check that loaded data is empty.
        CPPUNIT_ASSERT_MESSAGE("Written data should not be empty", !seriesDB->empty());
        CPPUNIT_ASSERT_MESSAGE("Loaded data should be empty", readSeriesDB->empty());
    }

    // Output with 'Reuse' UUID policy
    srvCfg.put("uuidPolicy", "Reuse");

    readSeriesDB = readOut< ::fwMedData::SeriesDB >(srvCfg, "::ioAtoms::SReader");

    {
        CPPUNIT_ASSERT_MESSAGE("Failed to retrieve output SeriesDB", readSeriesDB);

        CPPUNIT_ASSERT_MESSAGE("Data have not the same pointer", seriesDB == readSeriesDB);

        ::fwDataCamp::visitor::CompareObjects visitor;
        visitor.compare(seriesDB, readSeriesDB);
        compareLog(visitor);
        CPPUNIT_ASSERT_MESSAGE("Objects not equal", visitor.getDifferences()->empty() );
    }
}

//------------------------------------------------------------------------------

void atomTestSimpleData(const ::boost::filesystem::path& filePath)
{
    ::fwServices::IService::ConfigType srvCfg;
    srvCfg.add("file", filePath.string());

    ::fwData::Array::sptr array = ::fwData::Array::New();
    ::fwDataTools::helper::Array arrayHelper(array);
    const size_t NB_COMPONENT = 1;
    ::fwData::Array::SizeType size {10, 100};

    array->resize("uint32", size, NB_COMPONENT, true);

    unsigned int count = 0;
    unsigned int* iter = arrayHelper.begin<unsigned int>();

    for (; iter != arrayHelper.end<unsigned int>(); ++iter)
    {
        *iter = count++;
    }

    ::boost::filesystem::create_directories( filePath.parent_path() );

    write< ::fwData::Array >(srvCfg, array, "::ioAtoms::SWriter");

    ::fwData::Array::sptr readArray = read< ::fwData::Array >(srvCfg, "::ioAtoms::SReader");

    {
        ::fwDataCamp::visitor::CompareObjects visitor;
        visitor.compare(array, readArray);
        compareLog(visitor);
        for(const auto& it : *visitor.getDifferences())
        {
            std::cout<<"first : "<<it.first<<" second "<<it.second<<std::endl;
        }

        CPPUNIT_ASSERT_MESSAGE("Objects not equal", visitor.getDifferences()->empty() );
    }
}

//------------------------------------------------------------------------------

void IoAtomsTest::JSONTest()
{
    atomTest(::fwTools::System::getTemporaryFolder() / "JSONTest" / "ioAtomsTest.json");
    atomTestSimpleData(::fwTools::System::getTemporaryFolder() / "JSONTest" / "ioAtomsTest2.json");
}

//------------------------------------------------------------------------------

void IoAtomsTest::JSONZTest()
{
    atomTest(::fwTools::System::getTemporaryFolder() / "JSONZTest" / "ioAtomsTest.jsonz");
    atomTestSimpleData(::fwTools::System::getTemporaryFolder() / "JSONZTest" / "ioAtomsTest2.jsonz");
}

//------------------------------------------------------------------------------

void IoAtomsTest::XMLTest()
{
    atomTest(::fwTools::System::getTemporaryFolder() / "XMLTest" / "ioAtomsTest.xml");
    atomTestSimpleData(::fwTools::System::getTemporaryFolder() / "XMLTest" / "ioAtomsTest2.xml");
}

//------------------------------------------------------------------------------

void IoAtomsTest::XMLZTest()
{
    atomTest(::fwTools::System::getTemporaryFolder() / "XMLZTest" / "ioAtomsTest.xmlz");
    atomTestSimpleData(::fwTools::System::getTemporaryFolder() / "XMLZTest" / "ioAtomsTest2.xmlz");
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace ioAtoms
