
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


#include "IoItkTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( IoItkTest );

//------------------------------------------------------------------------------

void IoItkTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void IoItkTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void IoItkTest::executeService( ::fwTools::Object::sptr obj, std::string srvType, std::string srvImpl, ::fwRuntime::EConfigurationElement::sptr cfg )
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

void IoItkTest::testImageWriterJPG()
{
    // Create Image
    ::fwData::Image::NewSptr image;
    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));

    // Create path
    const ::boost::filesystem::path path = "imageJPG";
    ::boost::filesystem::create_directories( path );

    // Create Config
    ::fwRuntime::EConfigurationElement::NewSptr srvCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr folderCfg("folder");
    folderCfg->setValue(path.string());
    srvCfg->addConfigurationElement(folderCfg);

    // Create and execute service
    this->executeService( image, "::io::IWriter", "::ioITK::JpgImageWriterService", srvCfg );

    // Remove path
    ::boost::filesystem::remove_all( path.string() );
}

//------------------------------------------------------------------------------

void IoItkTest::testPatientDBReaderJPG()
{
    // create a Patient
    ::boost::filesystem::path pathJPGDir = ::fwTest::Data::dir() / "fw4spl/image/jpg";
    ::fwData::PatientDB::NewSptr patientDB;

    // Create Config
    ::fwRuntime::EConfigurationElement::NewSptr srvCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr folderCfg("folder");
    folderCfg->setValue(pathJPGDir.string());
    srvCfg->addConfigurationElement(folderCfg);

    // Create and execute service
    this->executeService( patientDB, "::io::IReader", "::ioITK::JpgPatientDBReaderService", srvCfg );

    CPPUNIT_ASSERT_EQUAL(static_cast< ::boost::uint32_t >(3),patientDB->getPatientSize());
    ::fwData::Image::sptr image = (*(*(*patientDB->getPatients().first)->getStudies().first)->getAcquisitions().first)->getImage();
    CPPUNIT_ASSERT_EQUAL(static_cast< size_t >(3),image->getNumberOfDimensions());
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Image::SizeType::value_type >(512),image->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Image::SizeType::value_type >(256),image->getSize()[1]);
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Image::SizeType::value_type >(1),image->getSize()[2]);
}

//------------------------------------------------------------------------------

void IoItkTest::testSaveLoadInr()
{
    ::fwData::Image::NewSptr image;
    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));

    // inr only support image origin (0,0,0)
    ::fwData::Image::OriginType origin(3,0);
    image->setOrigin(origin);

    // save image in inr
    const ::boost::filesystem::path PATH = "imageInrTest/image.inr.gz";
    ::boost::filesystem::create_directories( PATH.parent_path() );

    // Create Config
    ::fwRuntime::EConfigurationElement::NewSptr srvCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr fileCfg("file");
    fileCfg->setValue(PATH.string());
    srvCfg->addConfigurationElement(fileCfg);

    // Create and execute service
    this->executeService( image, "::io::IWriter", "::ioITK::InrImageWriterService", srvCfg );

    // load Image
    ::fwData::Image::NewSptr image2;
    this->executeService( image2, "::io::IReader", "::ioITK::InrImageReaderService", srvCfg );

    ::boost::filesystem::remove_all( PATH.parent_path().string() );

    // check Image
    // inr only support float spacing and float origin => add tolerance for comparison (+/-0.00001)
    CPPUNIT_ASSERT(::fwDataTools::Image::compareImage(image, image2, 0.00001, 0.00001));
}

//------------------------------------------------------------------------------

void IoItkTest::testLoadInr()
{
    // path on inr
    ::boost::filesystem::path file1 = ::fwTest::Data::dir() / "fw4spl/image/inr/image.inr.gz";
    ::boost::filesystem::path file2 = ::fwTest::Data::dir() / "fw4spl/image/inr/skin.inr.gz";
    ::boost::filesystem::path folder = ::fwTest::Data::dir() / "fw4spl/image/inr";



    // Create Config 1
    ::fwRuntime::EConfigurationElement::NewSptr srvCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr fileCfg("file");
    fileCfg->setValue(file1.string());
    srvCfg->addConfigurationElement(fileCfg);

    // Create and execute service
    ::fwData::PatientDB::NewSptr patientDB;
    this->executeService( patientDB, "::io::IReader", "::ioITK::InrPatientDBReaderService", srvCfg );
    CPPUNIT_ASSERT_EQUAL(static_cast< ::boost::uint32_t >(1),patientDB->getPatientSize());



    // Create Config 2
    ::fwRuntime::EConfigurationElement::NewSptr srvCfg2("service");
    ::fwRuntime::EConfigurationElement::NewSptr fileCfg2a("file");
    fileCfg2a->setValue(file1.string());
    srvCfg2->addConfigurationElement(fileCfg2a);
    ::fwRuntime::EConfigurationElement::NewSptr fileCfg2b("file");
    fileCfg2b->setValue(file2.string());
    srvCfg2->addConfigurationElement(fileCfg2b);

    // Create and execute service
    ::fwData::PatientDB::NewSptr patientDB2;
    this->executeService( patientDB2, "::io::IReader", "::ioITK::InrPatientDBReaderService", srvCfg2 );
    CPPUNIT_ASSERT_EQUAL(static_cast< ::boost::uint32_t >(2),patientDB2->getPatientSize());



    // Create Config 3
    ::fwRuntime::EConfigurationElement::NewSptr srvCfg3("service");
    ::fwRuntime::EConfigurationElement::NewSptr folderCfg3("folder");
    folderCfg3->setValue(folder.string());
    srvCfg3->addConfigurationElement(folderCfg3);

    // Create and execute service
    ::fwData::PatientDB::NewSptr patientDB3;
    this->executeService( patientDB3, "::io::IReader", "::ioITK::InrPatientDBReaderService", srvCfg3 );
    CPPUNIT_ASSERT_EQUAL(static_cast< ::boost::uint32_t >(2),patientDB3->getPatientSize());
}

//------------------------------------------------------------------------------

void IoItkTest::testPatientDBReaderDicom()
{
    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/ACHGenou";

    // Create Config 1
    ::fwRuntime::EConfigurationElement::NewSptr srvCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr fileCfg("folder");
    fileCfg->setValue(path.string());
    srvCfg->addConfigurationElement(fileCfg);

    // Create and execute service
    ::fwData::PatientDB::NewSptr patientDB;
    this->executeService( patientDB, "::io::IReader", "::ioITK::DicomPatientDBReaderService", srvCfg );

    // Get patient
    CPPUNIT_ASSERT_EQUAL( static_cast< ::boost::uint32_t >( 1 ), patientDB->getPatientSize());
    ::fwData::Patient::sptr patient = *patientDB->getPatients().first;

    CPPUNIT_ASSERT( ::fwTest::DicomReaderTest::checkPatientACHGenou( patient ) );
}

//------------------------------------------------------------------------------

void IoItkTest::testDicomImageWriter()
{
    // Create path
    const ::boost::filesystem::path path = "imageDicomTest";
    ::boost::filesystem::create_directories( path );

    // Create data
    ::fwData::Image::NewSptr image;
    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));

    // Create Config
    ::fwRuntime::EConfigurationElement::NewSptr srvCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr fileCfg("folder");
    fileCfg->setValue(path.string());
    srvCfg->addConfigurationElement(fileCfg);

    // Create and execute service
    this->executeService( image, "::io::IWriter", "::ioITK::DicomImageWriterService", srvCfg );

    // Remove path
    ::boost::filesystem::remove_all( path.string() );
}

//------------------------------------------------------------------------------

void IoItkTest::testDicomPatientWriter()
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
    this->executeService( patient, "::io::IWriter", "::ioITK::DicomPatientWriterService", srvCfg );

    // Remove path
    ::boost::filesystem::remove_all( path.string() );
}

//------------------------------------------------------------------------------

void IoItkTest::testDicomPatientDBWriter()
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
    this->executeService( patientDB, "::io::IWriter", "::ioITK::DicomPatientDBWriterService", srvCfg );

    // Remove path
    ::boost::filesystem::remove_all( path.string() );
}

//------------------------------------------------------------------------------

