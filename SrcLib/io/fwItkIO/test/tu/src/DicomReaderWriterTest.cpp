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
#include <fwData/Image.hpp>

#include <fwDataTools/Image.hpp>
#include <fwDataTools/Patient.hpp>

#include <itkIO/DicomImageWriter.hpp>
#include <itkIO/DicomPatientWriter.hpp>
#include <itkIO/DicomPatientDBWriter.hpp>
#include <itkIO/DicomPatientDBReader.hpp>

#include "DicomReaderWriterTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DicomReaderWriterTest );

//------------------------------------------------------------------------------

void DicomReaderWriterTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
}

//------------------------------------------------------------------------------

void DicomReaderWriterTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DicomReaderWriterTest::testWriteImage()
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

    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("float"));
    this->writeImage( image );

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

void DicomReaderWriterTest::writeImage( ::fwData::Image::sptr image )
{
    const ::boost::filesystem::path PATH = "imageDicomTest";
    ::boost::filesystem::create_directories( PATH );
    ::itkIO::DicomImageWriter::NewSptr myWriter;
    myWriter->setObject(image);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());

    ::boost::filesystem::remove_all( PATH.string() );
}

//------------------------------------------------------------------------------

void DicomReaderWriterTest::testWritePatient()
{
    // create Patient
    ::fwData::Patient::NewSptr patient;
    ::fwDataTools::Patient::generatePatient(patient, 2, 2, 0);

    const ::boost::filesystem::path PATH = "imageDicomTest";
    ::boost::filesystem::create_directories( PATH );
    ::itkIO::DicomPatientWriter::NewSptr myWriter;
    myWriter->setObject(patient);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());

    ::boost::filesystem::remove_all( PATH.string() );
}

//------------------------------------------------------------------------------

void DicomReaderWriterTest::testWritePatientDB()
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
    ::itkIO::DicomPatientDBWriter::NewSptr myWriter;
    myWriter->setObject(patientDB);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());

    ::boost::filesystem::remove_all( PATH.string() );
}

//------------------------------------------------------------------------------

void DicomReaderWriterTest::testReadPatientDB()
{
    // create PatientDB
    ::fwData::PatientDB::NewSptr patientDB;

    const ::boost::filesystem::path PATH = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/image_281433/";

    ::itkIO::DicomPatientDBReader::NewSptr myReader;
    myReader->setObject(patientDB);
    myReader->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myReader->read());

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

void DicomReaderWriterTest::testReadPatientDBACHGenou()
{
    // create PatientDB
    ::fwData::PatientDB::NewSptr patientDB;

    const ::boost::filesystem::path PATH = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/ACHGenou";

    ::itkIO::DicomPatientDBReader::NewSptr myReader;
    myReader->setObject(patientDB);
    myReader->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myReader->read());

    // Get patient
    CPPUNIT_ASSERT_EQUAL( static_cast< ::boost::uint32_t >( 1 ), patientDB->getPatientSize());
    ::fwData::Patient::sptr patient = *patientDB->getPatients().first;

    CPPUNIT_ASSERT( ::fwTest::DicomReaderTest::checkPatientACHGenou( patient ) );
}

//------------------------------------------------------------------------------

void DicomReaderWriterTest::testReadWritePatientDB()
{
    // Create folder
    const ::boost::filesystem::path PATH = "imageDicomTest";
    ::boost::filesystem::create_directories( PATH );

    // create and write patient1
    ::fwData::Patient::NewSptr patient1;
    ::fwDataTools::Patient::generatePatient(patient1, 1, 1, 0);
    ::itkIO::DicomPatientWriter::NewSptr myWriter;
    myWriter->setObject(patient1);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());

    // read and  create patient2
    ::fwData::PatientDB::NewSptr patientDB;
    ::itkIO::DicomPatientDBReader::NewSptr myReader;
    myReader->setObject(patientDB);
    myReader->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myReader->read());
    ::fwData::Patient::sptr patient2 = *patientDB->getPatients().first;

    // Remove folder
    ::boost::filesystem::remove_all( PATH.string() );

    // Check patient
    CPPUNIT_ASSERT(::fwDataTools::Patient::comparePatient(patient1, patient2));
}

//------------------------------------------------------------------------------
