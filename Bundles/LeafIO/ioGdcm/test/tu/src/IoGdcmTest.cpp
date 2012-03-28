#include <fwTools/dateAndTime.hpp>
#include <fwTools/System.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/Object.hpp>
#include <fwData/Patient.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Study.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/DicomReaderTest.hpp>

#include <fwDataTools/Patient.hpp>
#include <fwDataTools/Image.hpp>

#include <fwServices/registry/ServiceFactory.hpp>
#include <fwServices/registry/ObjectService.hpp>


#include "IoGdcmTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::ioGdcm::ut::IoGdcmTest );

namespace ioGdcm
{
namespace ut
{

//------------------------------------------------------------------------------

void IoGdcmTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void IoGdcmTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void IoGdcmTest::executeService( ::fwTools::Object::sptr obj, std::string srvType, std::string srvImpl, ::fwRuntime::EConfigurationElement::sptr cfg )
{
    ::fwServices::IService::sptr srv = ::fwServices::registry::ServiceFactory::getDefault()->create( srvType, srvImpl );
    CPPUNIT_ASSERT(srv);
    ::fwServices::OSR::registerService( obj , srv );
    srv->setConfiguration(cfg);
    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start());
    CPPUNIT_ASSERT_NO_THROW(srv->update());
    CPPUNIT_ASSERT_NO_THROW(srv->stop());
    ::fwServices::OSR::unregisterService( srv );
}

//------------------------------------------------------------------------------

void IoGdcmTest::testPatientDBReaderDicom()
{
    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/ACHGenou";

    // Create Config 1
    ::fwRuntime::EConfigurationElement::NewSptr srvCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr fileCfg("folder");
    fileCfg->setValue(path.string());
    srvCfg->addConfigurationElement(fileCfg);

    // Create and execute service
    ::fwData::PatientDB::NewSptr patientDB;
    this->executeService( patientDB, "::io::IReader", "::ioGdcm::DicomPatientDBReaderService", srvCfg );

    // Get patient
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), patientDB->getNumberOfPatients());
    ::fwData::Patient::sptr patient = patientDB->getPatients().front();

    CPPUNIT_ASSERT( ::fwTest::DicomReaderTest::checkPatientACHGenou( patient ) );
}

//------------------------------------------------------------------------------

void IoGdcmTest::testDicomPatientWriter()
{
    // Create path
    const ::boost::filesystem::path path = "imageDicomTest";
    ::boost::filesystem::create_directories( path );

    // Create data
    ::fwData::Patient::NewSptr patient;
    ::fwDataTools::Patient::generatePatient(patient, 2, 2, 0);

    // Create Config
    ::fwRuntime::EConfigurationElement::NewSptr srvCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr fileCfg("folder");
    fileCfg->setValue(path.string());
    srvCfg->addConfigurationElement(fileCfg);

    // Create and execute service
    this->executeService( patient, "::io::IWriter", "::ioGdcm::DicomPatientWriterService", srvCfg );

    // Remove path
    ::boost::filesystem::remove_all( path.string() );
}

//------------------------------------------------------------------------------

void IoGdcmTest::testDicomPatientDBWriter()
{
    // Create path
    const ::boost::filesystem::path path = "imageDicomTest";
    ::boost::filesystem::create_directories( path );

    // Create data
    ::fwData::PatientDB::NewSptr patientDB;
    ::fwData::Patient::NewSptr patient1;
    ::fwDataTools::Patient::generatePatient(patient1, 2, 2, 0);
    patientDB->addPatient( patient1 );
    ::fwData::Patient::NewSptr patient2;
    ::fwDataTools::Patient::generatePatient(patient2, 1, 3, 0);
    patientDB->addPatient( patient2 );

    // Create Config
    ::fwRuntime::EConfigurationElement::NewSptr srvCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr fileCfg("folder");
    fileCfg->setValue(path.string());
    srvCfg->addConfigurationElement(fileCfg);

    // Create and execute service
    this->executeService( patientDB, "::io::IWriter", "::ioGdcm::DicomPatientDBWriterService", srvCfg );

    // Remove path
    ::boost::filesystem::remove_all( path.string() );
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace ioGdcm
