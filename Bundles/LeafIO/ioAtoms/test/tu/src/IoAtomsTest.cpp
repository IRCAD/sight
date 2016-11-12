/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "IoAtomsTest.hpp"

#include <fwData/Composite.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwGui/registry/worker.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <fwTest/Exception.hpp>
#include <fwTest/generator/SeriesDB.hpp>

#include <fwTools/System.hpp>

#include <boost/filesystem/operations.hpp>

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
    ::fwGui::registry::worker::init(worker);
}

//------------------------------------------------------------------------------

void IoAtomsTest::tearDown()
{
    // Clean up after the test run.
    ::fwGui::registry::worker::reset();
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
void write(const ::fwRuntime::EConfigurationElement::sptr& srvCfg, const SPTR(T)& obj, const std::string& writer)
{
    ::fwServices::IService::sptr writerSrv = ::fwServices::registry::ServiceFactory::getDefault()->create( writer );
    CPPUNIT_ASSERT(writerSrv);

    ::fwServices::OSR::registerService( obj, writerSrv );
    writerSrv->setConfiguration(srvCfg);
    writerSrv->configure();
    writerSrv->start();
    writerSrv->update();
    writerSrv->stop();
    ::fwServices::OSR::unregisterService( writerSrv );
}

template <typename T>
SPTR(T) read(const ::fwRuntime::EConfigurationElement::sptr &srvCfg, const std::string &reader)
{

    typename T::sptr readObj               = T::New();
    ::fwServices::IService::sptr readerSrv = ::fwServices::registry::ServiceFactory::getDefault()->create( reader );
    CPPUNIT_ASSERT(readerSrv);

    ::fwServices::OSR::registerService( readObj, readerSrv );
    readerSrv->setConfiguration(srvCfg);
    readerSrv->configure();
    readerSrv->start();
    readerSrv->update();
    readerSrv->stop();
    ::fwServices::OSR::unregisterService( readerSrv );

    return readObj;
}

//------------------------------------------------------------------------------

template <typename T>
void writeReadFile(const ::fwRuntime::EConfigurationElement::sptr& srvCfg, const SPTR(T)& obj,
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
    ::fwRuntime::EConfigurationElement::sptr srvCfg  = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr fileCfg = ::fwRuntime::EConfigurationElement::New("file");
    fileCfg->setValue(filePath.string());
    srvCfg->addConfigurationElement(fileCfg);

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

    // 'Change' UUID policy
    ::fwRuntime::EConfigurationElement::sptr uuidPolicyCfg = ::fwRuntime::EConfigurationElement::New("uuidPolicy");
    uuidPolicyCfg->setValue("Change");

    srvCfg->addConfigurationElement(uuidPolicyCfg);

    readSeriesDB = read< ::fwMedData::SeriesDB >(srvCfg, "::ioAtoms::SReader");

    {
        ::fwDataCamp::visitor::CompareObjects visitor;
        visitor.compare(readSeriesDB, seriesDB);
        compareLog(visitor);
        CPPUNIT_ASSERT_MESSAGE("Objects not equal", visitor.getDifferences()->empty() );
    }

    // 'Strict' UUID policy
    uuidPolicyCfg->setValue("Strict");
    readSeriesDB = read< ::fwMedData::SeriesDB >(srvCfg, "::ioAtoms::SReader");

    {
        // DuplicatedDataUUID exception should have been thrown and catch by reader before any data/atom conversion.
        // We can only check that loaded data is empty.
        CPPUNIT_ASSERT_MESSAGE("Written data should not be empty", !seriesDB->empty());
        CPPUNIT_ASSERT_MESSAGE("Loaded data should be empty", readSeriesDB->empty());
    }

    // 'Reuse' UUID policy
    uuidPolicyCfg->setValue("Reuse");

    ::fwRuntime::EConfigurationElement::sptr injectCfg = ::fwRuntime::EConfigurationElement::New("inject");
    injectCfg->setValue("seriesDB");
    srvCfg->addConfigurationElement(injectCfg);

    ::fwData::Composite::sptr composite = read< ::fwData::Composite >(srvCfg, "::ioAtoms::SReader");

    {
        ::fwDataCamp::visitor::CompareObjects visitor;
        visitor.compare(workspace, composite);
        compareLog(visitor);
        CPPUNIT_ASSERT_MESSAGE("Objects should  be different", !visitor.getDifferences()->empty() );
        CPPUNIT_ASSERT(composite->find("seriesDB") != composite->end());

        {
            ::fwMedData::SeriesDB::sptr newSeriesDB = ::fwMedData::SeriesDB::dynamicCast( (*composite)["seriesDB"] );
            CPPUNIT_ASSERT_MESSAGE("Failed to retrieve SeriesDB inside composite with key 'seriesDB'", newSeriesDB);

            ::fwDataCamp::visitor::CompareObjects visitor;
            visitor.compare(seriesDB, newSeriesDB);
            compareLog(visitor);
            CPPUNIT_ASSERT_MESSAGE("Objects not equal", visitor.getDifferences()->empty() );
        }
    }
}

//------------------------------------------------------------------------------

void IoAtomsTest::JSONTest()
{
    atomTest(::fwTools::System::getTemporaryFolder() / "JSONTest" / "ioAtomsTest.json");
}

//------------------------------------------------------------------------------

void IoAtomsTest::JSONZTest()
{
    atomTest(::fwTools::System::getTemporaryFolder() / "JSONZTest" / "ioAtomsTest.jsonz");
}

//------------------------------------------------------------------------------

void IoAtomsTest::XMLTest()
{
    atomTest(::fwTools::System::getTemporaryFolder() / "XMLTest" / "ioAtomsTest.xml");
}

//------------------------------------------------------------------------------

void IoAtomsTest::XMLZTest()
{
    atomTest(::fwTools::System::getTemporaryFolder() / "XMLZTest" / "ioAtomsTest.xmlz");
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace ioAtoms

