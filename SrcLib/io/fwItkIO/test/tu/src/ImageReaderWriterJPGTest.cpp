/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <itkImageFileWriter.h>
#include <itkImageFileReader.h>

#include <fwData/Image.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>

#include <fwDataTools/Image.hpp>

#include <fwTest/Data.hpp>

#include <itkIO/itk.hpp>
#include <itkIO/JpgImageWriter.hpp>
#include <itkIO/ImageReader.hpp>
#include <itkIO/JpgPatientDBReader.hpp>

#include "ImageReaderWriterJPGTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ImageReaderWriterJPGTest );

//------------------------------------------------------------------------------

void ImageReaderWriterJPGTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
}

//------------------------------------------------------------------------------

void ImageReaderWriterJPGTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageReaderWriterJPGTest::testImageWriter()
{
    // create Image
    ::fwData::Image::NewSptr image;
    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));

    // save image in inr
    const ::boost::filesystem::path PATH = "imageJPG";
    ::boost::filesystem::create_directories( PATH );
    ::itkIO::JpgImageWriter::NewSptr myWriter;
    myWriter->setObject(image);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());

    ::boost::filesystem::remove_all( PATH.string() );
}

//------------------------------------------------------------------------------

void ImageReaderWriterJPGTest::testImageWriter2()
{
    // create Image
    ::boost::filesystem::path pathInr = ::fwTest::Data::dir() / "rd/patient/mfo/cas01/0/0/patientImage.inr.gz";
    ::fwData::Image::NewSptr image;
    ::itkIO::ImageReader::NewSptr myReader;
    myReader->setObject(image);
    myReader->setFile(pathInr);
    myReader->read();

    // save image in inr
    const ::boost::filesystem::path PATH = "imageJPG";
    ::boost::filesystem::create_directories( PATH );
    ::itkIO::JpgImageWriter::NewSptr myWriter;
    myWriter->setObject(image);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());

    ::boost::filesystem::remove_all( PATH.string() );
}

//------------------------------------------------------------------------------

void ImageReaderWriterJPGTest::testPatientDBReader()
{
    // create a Patient
    ::boost::filesystem::path pathJPGDir = ::fwTest::Data::dir() / "fw4spl/image/jpg";
    ::fwData::PatientDB::NewSptr patientDB;
    ::itkIO::JpgPatientDBReader::NewSptr myReader;
    myReader->setObject(patientDB);
    myReader->setFolder(pathJPGDir);
    CPPUNIT_ASSERT_NO_THROW(myReader->read());

    CPPUNIT_ASSERT_EQUAL(size_t(3), patientDB->getNumberOfPatients());
    ::fwData::Image::sptr image = patientDB->getPatients().front()->getStudies().front()->getAcquisitions().front()->getImage();
    CPPUNIT_ASSERT_EQUAL(size_t(3), image->getNumberOfDimensions());
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Image::SizeType::value_type >(512), image->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Image::SizeType::value_type >(256), image->getSize()[1]);
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Image::SizeType::value_type >(1), image->getSize()[2]);
}

//------------------------------------------------------------------------------
