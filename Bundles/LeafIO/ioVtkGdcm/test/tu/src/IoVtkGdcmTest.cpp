/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/System.hpp>


#include <fwServices/Base.hpp>
#include <fwServices/AppConfigManager.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwData/PatientDB.hpp>
#include <fwDataTools/Patient.hpp>

#include <fwTest/Data.hpp>

#include "IoVtkGdcmTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( IoVtkGdcmTest );

//------------------------------------------------------------------------------

void IoVtkGdcmTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void IoVtkGdcmTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void IoVtkGdcmTest::testReader()
{

    ::boost::filesystem::path dicomDataPath(::fwTest::Data::dir() / "fw4spl/Patient/Dicom/image_281433");

    ::fwData::PatientDB::NewSptr patientDB;
    ::fwRuntime::EConfigurationElement::NewSptr readerCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr folderCfg("folder");
    folderCfg->setValue(dicomDataPath.string());
    readerCfg->addConfigurationElement(folderCfg);

    ::fwServices::IService::sptr srv = ::fwServices::registry::ServiceFactory::getDefault()->create( "::io::IReader", "::ioVtkGdcm::DicomPatientDBReaderService" );
    CPPUNIT_ASSERT(srv);

    ::fwServices::OSR::registerService( patientDB , srv );

    srv->setConfiguration( readerCfg );
    srv->configure();
    srv->start();
    srv->update();
    srv->stop();
    ::fwServices::OSR::unregisterService( srv );

    // Patient expected
    std::string nameExpected("anonymous");
    std::string firstnameExpected("anonymous");
    bool isMaleExpected = false;
    ::boost::uint32_t nbPatientExpected = 1;
    ::boost::uint32_t nbStudyExpected = 1;
    ::boost::uint32_t nbSeriesExpected = 1;

    //Info image expected.
    const size_t imgDimensionExpected   = 3;
    double imgSpacingX = 0.667969;
    double imgSpacingY = 0.667969;
    double imgSpacingZ = 1.5;

    ::fwData::Image::OriginType imgOriginExpected (3,0);

    size_t imgSizeX_Expected =  512;
    size_t imgSizeY_Expected =  512;
    size_t imgSizeZ_Expected =  166;
    int imgSize = imgSizeX_Expected*imgSizeY_Expected*imgSizeZ_Expected;

    const double imgWindowCenter = 50;
    const double imgWindowWidth = 500;
    ::fwTools::Type imgPixelType = ::fwTools::Type::create<signed int>();


    // Patient read.
    ::boost::uint32_t  nbPatient = patientDB->getPatientSize();
    ::fwData::PatientDB::PatientIterator patientIter;
    patientIter = patientDB->getPatients().first;
    ::fwData::Patient::StudyIterator studyIter;
    studyIter = (*patientIter)->getStudies().first;

    CPPUNIT_ASSERT_EQUAL(nbPatient, nbPatientExpected);
    CPPUNIT_ASSERT_EQUAL((*patientIter)->getName(), nameExpected);
    CPPUNIT_ASSERT_EQUAL((*patientIter)->getFirstname(), firstnameExpected);
    CPPUNIT_ASSERT_EQUAL((*patientIter)->getIsMale(), isMaleExpected);
    CPPUNIT_ASSERT_EQUAL((*patientIter)->getStudySize(), nbStudyExpected);
    CPPUNIT_ASSERT_EQUAL((*studyIter)->getAcquisitionSize(), nbSeriesExpected);

    ::fwData::Study::AcquisitionIterator acqIter;
    acqIter = (*studyIter)->getAcquisitions().first;

    ::fwData::Image::csptr fisrtImage = (*acqIter)->getImage();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed on image dimension.", fisrtImage->getNumberOfDimensions(), imgDimensionExpected);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed on origin on X ", static_cast< ::fwData::Image::OriginType::value_type > (fisrtImage->getOrigin()[0]), imgOriginExpected[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed on origin on Y ", static_cast< ::fwData::Image::OriginType::value_type > (fisrtImage->getOrigin()[1]), imgOriginExpected[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed on origin on Z ", static_cast< ::fwData::Image::OriginType::value_type > (fisrtImage->getOrigin()[2]), imgOriginExpected[2]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed on window center ", fisrtImage->getWindowCenter(), imgWindowCenter);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed on window width ", fisrtImage->getWindowWidth(), imgWindowWidth);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed on size x ", static_cast< ::fwData::Image::SizeType::value_type > (fisrtImage->getSize()[0]), static_cast< ::fwData::Image::SizeType::value_type > (imgSizeX_Expected));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed on size y ", static_cast< ::fwData::Image::SizeType::value_type > (fisrtImage->getSize()[1]), static_cast< ::fwData::Image::SizeType::value_type > (imgSizeY_Expected));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed on size z ", static_cast< ::fwData::Image::SizeType::value_type > (fisrtImage->getSize()[2]), static_cast< ::fwData::Image::SizeType::value_type > (imgSizeZ_Expected));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed on spacing x ", static_cast< ::fwData::Image::SpacingType::value_type > (fisrtImage->getSpacing()[0]), imgSpacingX);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed on spacing y ", static_cast< ::fwData::Image::SpacingType::value_type > (fisrtImage->getSpacing()[1]), imgSpacingY);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed on spacing z ", static_cast< ::fwData::Image::SpacingType::value_type > (fisrtImage->getSpacing()[2]), imgSpacingZ);

}

//------------------------------------------------------------------------------
void IoVtkGdcmTest::testWriter()
{
    // create patientDB
    const size_t nbPatientExpected = 1;
    ::fwData::Patient::NewSptr pPatient;
    ::fwDataTools::Patient::generatePatient(pPatient, 1, 1, 0);

    // vtkMedicalImageProperties doesn't allow to save birthTime so we keeep only the date.
    boost::posix_time::ptime birthdate = boost::posix_time::ptime(pPatient->getBirthdate().date());
    pPatient->setBirthdate(birthdate);

    ::fwData::Patient::StudyIterator pStudy = pPatient->getStudies().first;
    ::fwData::Study::AcquisitionIterator pAcq = (*pStudy)->getAcquisitions().first;

    (*pStudy)->setModality("CT");

    ::fwData::PatientDB::NewSptr pPatientDB;
    pPatientDB->addPatient(pPatient);

    const ::boost::filesystem::path PATH = ::fwTools::System::getTemporaryFolder() / "DicomWriterTest";

    ::boost::filesystem::create_directories( PATH );

    ::fwRuntime::EConfigurationElement::NewSptr srvConfig("service");
    ::fwRuntime::EConfigurationElement::NewSptr folderCfg("folder");
    folderCfg->setValue(PATH.string());
    srvConfig->addConfigurationElement(folderCfg);

    ::fwServices::IService::sptr writerSrv = ::fwServices::registry::ServiceFactory::getDefault()->create( "::io::IWriter", "::ioVtkGdcm::DicomPatientDBWriterService" );
    CPPUNIT_ASSERT(writerSrv);

    ::fwServices::OSR::registerService( pPatientDB, writerSrv );

    writerSrv->setConfiguration( srvConfig );
    writerSrv->configure();
    writerSrv->start();
    writerSrv->update();
    writerSrv->stop();
    ::fwServices::OSR::unregisterService( writerSrv );


    // Load Dicom from disk
    ::fwData::PatientDB::NewSptr pReadPatientDB;

    ::fwServices::IService::sptr readerSrv = ::fwServices::registry::ServiceFactory::getDefault()->create( "::io::IReader", "::ioVtkGdcm::DicomPatientDBReaderService" );
    CPPUNIT_ASSERT(readerSrv);

    ::fwServices::OSR::registerService( pReadPatientDB , readerSrv );

    readerSrv->setConfiguration( srvConfig ); // use same config as writer
    readerSrv->configure();
    readerSrv->start();
    readerSrv->update();
    readerSrv->stop();
    ::fwServices::OSR::unregisterService( readerSrv );

    // Clean the written data
    ::boost::filesystem::remove_all( PATH.string() );

    size_t  nbPatient = pReadPatientDB->getPatientSize();
    CPPUNIT_ASSERT_EQUAL(nbPatientExpected, nbPatient);

    ::fwData::PatientDB::PatientIterator readPatientIter;
    readPatientIter = pReadPatientDB->getPatients().first;
    ::fwData::Patient::sptr pReadPatient = (*readPatientIter);
    //
    // Attribut BdID not a dicom field
    pReadPatient->setDbID(pPatient->getDbID());
    // Manage space added
    pReadPatient->setIDDicom(pReadPatient->getIDDicom());

    ::fwData::Patient::StudyIterator pReadStudy = pReadPatient->getStudies().first;

    // Manage space added
    (*pReadStudy)->setHospital((*pReadStudy)->getHospital());
    // Set a valid modality.
    (*pReadStudy)->setModality("CT");
    (*pReadStudy)->setUID((*pStudy)->getUID());

    // Attribute ris ID is not supported by vtkMedicalImageProperties interface used to write the dicom file
    (*pReadStudy)->setRISId((*pStudy)->getRISId());
    (*pReadStudy)->setDbID((*pStudy)->getDbID());
    (*pReadStudy)->setRISId((*pStudy)->getRISId());
    (*pReadStudy)->setDbID((*pStudy)->getDbID());

    ::fwData::Study::AcquisitionIterator pReadAcq = (*pReadStudy)->getAcquisitions().first;
    (*pReadAcq)->setDbID((*pAcq)->getDbID());
    (*pReadAcq)->setLaboID((*pAcq)->getLaboID());
    (*pReadAcq)->setNetID((*pAcq)->getNetID());
    (*pReadAcq)->setPatientSize((*pAcq)->getPatientSize());
    (*pReadAcq)->setPatientWeight((*pAcq)->getPatientWeight());
    (*pReadAcq)->setRadiations((*pAcq)->getRadiations());
    (*pReadAcq)->setMedicalPrinter((*pAcq)->getMedicalPrinter());
    (*pReadAcq)->setMedicalPrinterCorp((*pAcq)->getMedicalPrinterCorp());
    (*pReadAcq)->setPatientPosition((*pAcq)->getPatientPosition());
    (*pReadAcq)->setDateSendToLaboAt((*pAcq)->getDateSendToLaboAt());
    (*pReadAcq)->setDateReceiveFromLaboAt((*pAcq)->getDateReceiveFromLaboAt());
    (*pReadAcq)->setDateSendToBDDAt((*pAcq)->getDateSendToBDDAt());
    (*pReadAcq)->setDateDisponibilityAt((*pAcq)->getDateDisponibilityAt());
    (*pReadAcq)->setImageType((*pAcq)->getImageType());
    (*pReadAcq)->setImageFormat((*pAcq)->getImageFormat());
    (*pReadAcq)->setUID((*pAcq)->getUID());
    (*pReadAcq)->setAxe((*pAcq)->getAxe());

    // check patient
    CPPUNIT_ASSERT(::fwDataTools::Patient::comparePatient(pPatient, pReadPatient));

}

//------------------------------------------------------------------------------
