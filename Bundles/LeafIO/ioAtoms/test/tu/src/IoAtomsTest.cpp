#include <fwAtomConversion/exception/DuplicatedDataUUID.hpp>

#include <fwTools/System.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/Object.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Patient.hpp>
#include <fwData/PatientDB.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwDataTools/Patient.hpp>

#include <fwServices/registry/ServiceFactory.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTest/Exception.hpp>
#include <fwTest/generator/SeriesDB.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include "IoAtomsTest.hpp"

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
}

//------------------------------------------------------------------------------

void IoAtomsTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

::fwData::Composite::sptr genWorkspace()
{
    ::fwData::Composite::sptr workspace = ::fwData::Composite::New();
    ::fwData::PatientDB::sptr patientDB = ::fwData::PatientDB::New();
    ::fwData::Patient::sptr patient  = ::fwData::Patient::New();
    ::fwDataTools::Patient::generatePatient(patient, 2, 2, 2);
    patientDB->addPatient( patient );
    workspace->getContainer()["patientDB"] = patientDB;
    workspace->getContainer()["processingDB"] = ::fwData::Composite::New();
    workspace->getContainer()["planningDB"] = ::fwData::Composite::New();

    return workspace;
}


template <typename T>
void compareLog(T &comparator)
{
    SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = comparator.getDifferences();
    BOOST_FOREACH(::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop, (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " " << prop.second );
    }
}



//------------------------------------------------------------------------------

template <typename T>
void write(const ::fwRuntime::EConfigurationElement::sptr &srvCfg, const SPTR(T) &obj, const std::string &writer)
{
    ::fwServices::IService::sptr writerSrv = ::fwServices::registry::ServiceFactory::getDefault()->create( writer );
    CPPUNIT_ASSERT(writerSrv);

    ::fwServices::OSR::registerService( obj , writerSrv );
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

    typename T::sptr readObj = T::New();
    ::fwServices::IService::sptr readerSrv = ::fwServices::registry::ServiceFactory::getDefault()->create( reader );
    CPPUNIT_ASSERT(readerSrv);

    ::fwServices::OSR::registerService( readObj , readerSrv );
    readerSrv->setConfiguration(srvCfg);
    readerSrv->configure();
    readerSrv->start();
    readerSrv->update();
    readerSrv->stop();
    ::fwServices::OSR::unregisterService( readerSrv );

    return readObj;
}


template <typename T>
void writeReadFile(const ::fwRuntime::EConfigurationElement::sptr &srvCfg, const SPTR(T) &obj,
                                 const std::string &writer, const std::string &reader)
{
    write(srvCfg, obj, writer);

    SPTR(T) readObj = read<T>(srvCfg, reader);

        // Compare
    using namespace ::fwDataCamp::visitor;
    CompareObjects visitor;

    visitor.compare(readObj, obj);
    compareLog(visitor);

    CPPUNIT_ASSERT_MESSAGE("Objects not equal" , visitor.getDifferences()->empty() );
}


//------------------------------------------------------------------------------

void medicalDataTest(const ::boost::filesystem::path & filePath)
{
    ::fwRuntime::EConfigurationElement::NewSptr srvCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr fileCfg("file");
    fileCfg->setValue(filePath.string());
    srvCfg->addConfigurationElement(fileCfg);

    ::boost::filesystem::create_directories( filePath.parent_path() );
    writeReadFile< ::fwData::Composite>( srvCfg, genWorkspace(), "::ioAtoms::SMedDataWriter",  "::ioAtoms::SMedDataReader" );
}
//------------------------------------------------------------------------------

void IoAtomsTest::JSONMedicalDataTest()
{
    medicalDataTest(::fwTools::System::getTemporaryFolder() / "JSONMedicalDataTest" / "ioAtomsTest.json");
}

//------------------------------------------------------------------------------

void IoAtomsTest::JSONZMedicalDataTest()
{
    medicalDataTest(::fwTools::System::getTemporaryFolder() / "JSONMedicalDataTest" / "ioAtomsTest.jsonz");
}

//------------------------------------------------------------------------------

void IoAtomsTest::XMLMedicalDataTest()
{
    medicalDataTest(::fwTools::System::getTemporaryFolder() / "JSONMedicalDataTest" / "ioAtomsTest.xml");
}

//------------------------------------------------------------------------------

void IoAtomsTest::XMLZMedicalDataTest()
{
    medicalDataTest(::fwTools::System::getTemporaryFolder() / "JSONMedicalDataTest" / "ioAtomsTest.xmlz");
}

//------------------------------------------------------------------------------

void IoAtomsTest::HDF5MedicalDataTest()
{
    medicalDataTest(::fwTools::System::getTemporaryFolder() / "JSONMedicalDataTest" / "ioAtomsTest.hdf5");
}

//------------------------------------------------------------------------------

void atomTest(const ::boost::filesystem::path & filePath)
{
    ::fwRuntime::EConfigurationElement::NewSptr srvCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr fileCfg("file");
    fileCfg->setValue(filePath.string());
    srvCfg->addConfigurationElement(fileCfg);

    ::fwMedData::SeriesDB::sptr seriesDB = ::fwTest::generator::SeriesDB::createSeriesDB(5,5,5);
    ::fwData::Composite::sptr workspace = genWorkspace();

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
        CPPUNIT_ASSERT_MESSAGE("Objects not equal" , visitor.getDifferences()->empty() );
    }


    // 'Change' UUID policy
    ::fwRuntime::EConfigurationElement::NewSptr uuidPolicyCfg("uuidPolicy");
    uuidPolicyCfg->setValue("Change");

    srvCfg->addConfigurationElement(uuidPolicyCfg);

    readSeriesDB = read< ::fwMedData::SeriesDB >(srvCfg, "::ioAtoms::SReader");

    {
        ::fwDataCamp::visitor::CompareObjects visitor;
        visitor.compare(readSeriesDB, seriesDB);
        compareLog(visitor);
        CPPUNIT_ASSERT_MESSAGE("Objects not equal" , visitor.getDifferences()->empty() );
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

    ::fwRuntime::EConfigurationElement::NewSptr injectCfg("inject");
    injectCfg->setValue("seriesDB");
    srvCfg->addConfigurationElement(injectCfg);

    ::fwData::Composite::sptr composite = read< ::fwData::Composite >(srvCfg, "::ioAtoms::SReader");

    {
        ::fwDataCamp::visitor::CompareObjects visitor;
        visitor.compare(workspace, composite);
        compareLog(visitor);
        CPPUNIT_ASSERT_MESSAGE("Objects should  be different" , !visitor.getDifferences()->empty() );
        CPPUNIT_ASSERT(composite->find("seriesDB") != composite->end());

        {
            ::fwMedData::SeriesDB::sptr newSeriesDB = ::fwMedData::SeriesDB::dynamicCast( (*composite)["seriesDB"] );
            CPPUNIT_ASSERT_MESSAGE("Failed to retrieve SeriesDB inside composite with key 'seriesDB'", newSeriesDB);

            ::fwDataCamp::visitor::CompareObjects visitor;
            visitor.compare(seriesDB, newSeriesDB);
            compareLog(visitor);
            CPPUNIT_ASSERT_MESSAGE("Objects not equal" , visitor.getDifferences()->empty() );
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

void IoAtomsTest::HDF5Test()
{
    atomTest(::fwTools::System::getTemporaryFolder() / "HDF5Test" / "ioAtomsTest.hdf5");
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace ioAtoms

