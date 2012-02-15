/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwTest/Data.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>

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
