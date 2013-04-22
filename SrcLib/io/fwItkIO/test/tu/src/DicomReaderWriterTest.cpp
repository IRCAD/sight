/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
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

#include <fwTest/generator/Image.hpp>

#include <gdcmTrace.h>
#include <itkIO/DicomImageWriter.hpp>
#include <itkIO/DicomPatientWriter.hpp>
#include <itkIO/DicomPatientDBWriter.hpp>
#include <itkIO/DicomPatientDBReader.hpp>

#include "DicomReaderWriterTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwItkIO::ut::DicomReaderWriterTest );

namespace fwItkIO
{
namespace ut
{

//------------------------------------------------------------------------------

void DicomReaderWriterTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
    ::gdcm::Trace::SetStream(m_gdcmTraceStream);
}

//------------------------------------------------------------------------------

void DicomReaderWriterTest::tearDown()
{
    // Clean up after the test run.
    SLM_WARN( m_gdcmTraceStream.str() );
    m_gdcmTraceStream.str( std::string() );
}

//------------------------------------------------------------------------------

void DicomReaderWriterTest::testWriteImage()
{
    // create Image
    ::fwData::Image::NewSptr image;

    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("uint8"));
    this->writeImage( image );

    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int8"));
    this->writeImage( image );

    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("uint16"));
    this->writeImage( image );

    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));
    this->writeImage( image );

    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("uint32"));
    this->writeImage( image );

    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int32"));
    this->writeImage( image );

    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("float"));
    this->writeImage( image );

//    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("uint64"));
//    this->writeImage( image );
//
//    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int64"));
//    this->writeImage( image );
//
//    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("double"));
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

} //namespace ut
} //namespace fwItkIO
