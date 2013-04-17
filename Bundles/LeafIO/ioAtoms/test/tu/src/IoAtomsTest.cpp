
#include <fwTools/System.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/Object.hpp>
#include <fwData/Patient.hpp>
#include <fwData/PatientDB.hpp>

#include <fwDataTools/Patient.hpp>

#include <fwServices/registry/ServiceFactory.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTest/Exception.hpp>

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

void IoAtomsTest::writeReadFile( const ::boost::filesystem::path & filePath )
{
    ::fwData::Composite::sptr workspace = ::fwData::Composite::New();
    ::fwData::PatientDB::sptr patientDB = ::fwData::PatientDB::New();
    ::fwData::Patient::sptr patient  = ::fwData::Patient::New();
    ::fwDataTools::Patient::generatePatient(patient, 2, 2, 2);
    patientDB->addPatient( patient );
    workspace->getContainer()["patientDB"] = patientDB;
    workspace->getContainer()["processingDB"] = ::fwData::Composite::New();
    workspace->getContainer()["planningDB"] = ::fwData::Composite::New();

    ::fwRuntime::EConfigurationElement::NewSptr srvCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr fileCfg("file");
    fileCfg->setValue(filePath.string());
    srvCfg->addConfigurationElement(fileCfg);

    {
        ::fwData::Composite::sptr workspaceTmp;
        workspaceTmp = ::fwData::Object::copy(workspace);

        // Write PatientDB
        ::fwServices::IService::sptr writerSrv = ::fwServices::registry::ServiceFactory::getDefault()->create( "::io::IWriter", "::ioAtoms::SMedDataWriter" );
        CPPUNIT_ASSERT(writerSrv);

        ::fwServices::OSR::registerService( workspaceTmp , writerSrv );
        writerSrv->setConfiguration(srvCfg);
        writerSrv->configure();
        writerSrv->start();
        writerSrv->update();
        writerSrv->stop();
        ::fwServices::OSR::unregisterService( writerSrv );
    }

    // Read PatientDB
    ::fwServices::IService::sptr readerSrv = ::fwServices::registry::ServiceFactory::getDefault()->create( "::io::IReader", "::ioAtoms::SMedDataReader" );
    CPPUNIT_ASSERT(readerSrv);

    ::fwData::Composite::NewSptr workspace2;
    ::fwServices::OSR::registerService( workspace2 , readerSrv );
    readerSrv->setConfiguration(srvCfg);
    readerSrv->configure();
    readerSrv->start();
    readerSrv->update();
    readerSrv->stop();
    ::fwServices::OSR::unregisterService( readerSrv );


    // Compare
    using namespace ::fwDataCamp::visitor;
    CompareObjects visitor;

    visitor.compare(workspace2, workspace);

    SPTR(CompareObjects::PropsMapType) props = visitor.getDifferences();
    BOOST_FOREACH(CompareObjects::PropsMapType::value_type prop, (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " " << prop.second );
    }
    CPPUNIT_ASSERT_MESSAGE("Workspace not equal" , props->size() == 0 );
}

//------------------------------------------------------------------------------

void IoAtomsTest::JSONMedicalDataTest()
{
    ::boost::filesystem::path filePath = ::fwTools::System::getTemporaryFolder() / "testJSON" / "ioAtomsTest.json";
    ::boost::filesystem::create_directories( filePath.parent_path() );
    this->writeReadFile( filePath );
}

//------------------------------------------------------------------------------

void IoAtomsTest::JSONZMedicalDataTest()
{
    ::boost::filesystem::path filePath = ::fwTools::System::getTemporaryFolder() / "testJSONZ" / "ioAtomsTest.jsonz";
    ::boost::filesystem::create_directories( filePath.parent_path() );
    this->writeReadFile( filePath );
}

//------------------------------------------------------------------------------

void IoAtomsTest::XMLMedicalDataTest()
{
    ::boost::filesystem::path filePath = ::fwTools::System::getTemporaryFolder() / "testXMLZ" / "ioAtomsTest.xml";
    ::boost::filesystem::create_directories( filePath.parent_path() );
    this->writeReadFile( filePath );
}

//------------------------------------------------------------------------------

void IoAtomsTest::XMLZMedicalDataTest()
{
    ::boost::filesystem::path filePath = ::fwTools::System::getTemporaryFolder() / "testJSON" / "ioAtomsTest.xmlz";
    ::boost::filesystem::create_directories( filePath.parent_path() );
    this->writeReadFile( filePath );
}

//------------------------------------------------------------------------------

void IoAtomsTest::HDF5MedicalDataTest()
{
    ::boost::filesystem::path filePath = ::fwTools::System::getTemporaryFolder() / "testHDF5" / "ioAtomsTest.hdf5";
    ::boost::filesystem::create_directories( filePath.parent_path() );
    this->writeReadFile( filePath );
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace ioAtoms

