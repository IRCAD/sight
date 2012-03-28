/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/Type.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/DicomReaderTest.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Image.hpp>

#include <fwDataTools/Image.hpp>
#include <fwDataTools/Patient.hpp>

#include <gdcmIO/writer/DicomGlobalWriterManager.hpp>
#include <gdcmIO/writer/DicomPatientDBWriterManager.hpp>
#include <gdcmIO/reader/DicomPatientDBReader.hpp>

#include "RealDataDicomTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( RealDataDicomTest );

//------------------------------------------------------------------------------

void RealDataDicomTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
}

//------------------------------------------------------------------------------

void RealDataDicomTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void RealDataDicomTest::testWriteImage()
{
    // create Image
    ::fwData::Image::NewSptr image;

    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("uint8"));
    this->writeImage( image );

    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("int8"));
    this->writeImage( image );

    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("uint16"));
    this->writeImage( image );

    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));
    this->writeImage( image );

    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("uint32"));
    this->writeImage( image );

    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("int32"));
    this->writeImage( image );

//   ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("float"));
//    this->writeImage( image );

//    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("uint64"));
//    this->writeImage( image );
//
//    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("int64"));
//    this->writeImage( image );
//
//    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("double"));
//    this->writeImage( image );
}

//------------------------------------------------------------------------------

void RealDataDicomTest::writeImage( ::fwData::Image::sptr image )
{
    const ::boost::filesystem::path PATH = "imageDicomTest";
    ::boost::filesystem::create_directories( PATH );

    // Create patient from image
    ::fwData::Patient::NewSptr patient;
    ::fwDataTools::Patient::generatePatient(patient, 1, 1, 0);

    ::fwData::Study::sptr study;
    study = patient->getStudies().front();
    ::fwData::Acquisition::sptr acq;
    acq = study->getAcquisitions().front();
    acq->setImage( image );

    ::gdcmIO::writer::DicomGlobalWriterManager::NewSptr myWriter;
    myWriter->setObject(patient);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());

    ::boost::filesystem::remove_all( PATH.string() );
}

//------------------------------------------------------------------------------

void RealDataDicomTest::testWritePatient()
{
    // create Patient
    ::fwData::Patient::NewSptr patient;
    ::fwDataTools::Patient::generatePatient(patient, 2, 2, 0);

    const ::boost::filesystem::path PATH = "imageDicomTest";
    ::boost::filesystem::create_directories( PATH );

    ::gdcmIO::writer::DicomGlobalWriterManager::NewSptr myWriter;
    myWriter->setObject(patient);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());

    ::boost::filesystem::remove_all( PATH.string() );
}

//------------------------------------------------------------------------------

void RealDataDicomTest::testWritePatientDB()
{
    // create PatientDB
    ::fwData::PatientDB::NewSptr patientDB;
    ::fwData::Patient::NewSptr patient1;
    ::fwDataTools::Patient::generatePatient(patient1, 2, 2, 0);
    patientDB->addPatient( patient1 );
    ::fwData::Patient::NewSptr patient2;
    ::fwDataTools::Patient::generatePatient(patient2, 1, 3, 0);
    patientDB->addPatient( patient2 );

    const ::boost::filesystem::path PATH = "imageDicomTest";
    ::boost::filesystem::create_directories( PATH );

    ::gdcmIO::writer::DicomPatientDBWriterManager::NewSptr myWriter;
    myWriter->setObject(patientDB);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());

    ::boost::filesystem::remove_all( PATH.string() );
}

//------------------------------------------------------------------------------

void RealDataDicomTest::testReadPatientDB()
{
    // create PatientDB
    ::fwData::PatientDB::NewSptr patientDB;

    const ::boost::filesystem::path PATH = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/image_281433/";

    ::gdcmIO::reader::DicomPatientDBReader::NewSptr myReader;
    myReader->setObject(patientDB);
    myReader->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myReader->read());

    // Patient expected
    std::string nameExpected("anonymous");
    std::string firstnameExpected("anonymous");
    bool isMaleExpected = false;
    ::boost::uint32_t nbPatientExpected = 1;
    size_t nbStudyExpected = 1;
    size_t nbSeriesExpected = 1;

    //Info image expected.
    const size_t imgDimensionExpected   = 3;
    double imgSpacingX = 0.667969;
    double imgSpacingY = 0.667969;
    double imgSpacingZ = 1.5;

    ::fwData::Image::OriginType imgOriginExpected (3,0);

    size_t imgSizeX_Expected =  512;
    size_t imgSizeY_Expected =  512;
    size_t imgSizeZ_Expected =  166;

    const double imgWindowCenter = 50;
    const double imgWindowWidth = 500;
    ::fwTools::Type imgPixelType = ::fwTools::Type::create<signed int>();


    // Patient read.
    ::boost::uint32_t  nbPatient = patientDB->getNumberOfPatients();
    ::fwData::Patient::sptr patient;
    patient = patientDB->getPatients().front();
    ::fwData::Study::sptr study;
    study = patient->getStudies().front();

    CPPUNIT_ASSERT_EQUAL(nbPatient, nbPatientExpected);
    CPPUNIT_ASSERT_EQUAL(patient->getName(), nameExpected);
    CPPUNIT_ASSERT_EQUAL(patient->getFirstname(), firstnameExpected);
    CPPUNIT_ASSERT_EQUAL(patient->getIsMale(), isMaleExpected);
    CPPUNIT_ASSERT_EQUAL(patient->getNumberOfStudies(), nbStudyExpected);
    CPPUNIT_ASSERT_EQUAL(study->getNumberOfAcquisitions(), nbSeriesExpected);

    ::fwData::Acquisition::sptr acq;
    acq = study->getAcquisitions().front();

    ::fwData::Image::csptr fisrtImage = acq->getImage();
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

void RealDataDicomTest::testReadPatientDBACHGenou()
{
    // create PatientDB
    ::fwData::PatientDB::NewSptr patientDB;

    const ::boost::filesystem::path PATH = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/ACHGenou";

    ::gdcmIO::reader::DicomPatientDBReader::NewSptr myReader;
    myReader->setObject(patientDB);
    myReader->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myReader->read());

    // Get patient
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), patientDB->getNumberOfPatients());
    ::fwData::Patient::sptr patient = patientDB->getPatients().front();

    CPPUNIT_ASSERT( ::fwTest::DicomReaderTest::checkPatientACHGenou( patient ) );
}

//------------------------------------------------------------------------------

void RealDataDicomTest::testReadWritePatientDB()
{
    // Create folder
    const ::boost::filesystem::path PATH = "imageDicomTest";
    ::boost::filesystem::create_directories( PATH );

    // create and write patient1
    ::fwData::Patient::NewSptr patient1;
    ::fwDataTools::Patient::generatePatient(patient1, 1, 1, 0);
    ::gdcmIO::writer::DicomGlobalWriterManager::NewSptr myWriter;
    myWriter->setObject(patient1);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());

    // read and  create patient2
    ::fwData::PatientDB::NewSptr patientDB;
    ::gdcmIO::reader::DicomPatientDBReader::NewSptr myReader;
    myReader->setObject(patientDB);
    myReader->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myReader->read());
    ::fwData::Patient::sptr patient2 = patientDB->getPatients().front();

    // Remove folder
    ::boost::filesystem::remove_all( PATH.string() );

    // Check patient
    CPPUNIT_ASSERT(::fwDataTools::Patient::comparePatient(patient1, patient2));
}

//------------------------------------------------------------------------------
