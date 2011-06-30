/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <itkIO/ImageWriter.hpp>
#include <itkIO/ImageReader.hpp>

#include "ImageReaderWriterTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ImageReaderWriterTest );

//------------------------------------------------------------------------------

void ImageReaderWriterTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::methode1()
{
    const ::boost::filesystem::path PATH = "imageInrTest/image.inr.gz";

    // create Image
    ::fwData::Image::sptr image = createImage();

    // save image in inr
    ::boost::filesystem::create_directories( PATH.parent_path() );
    ::itkIO::ImageWriter::NewSptr myWriter;
    myWriter->setObject(image);
    myWriter->setFile(PATH);
    myWriter->write();

    // load Image
    ::fwData::Image::NewSptr image2;
    ::itkIO::ImageReader::NewSptr myReader;
    myReader->setObject(image2);
    myReader->setFile(PATH);
    myReader->read();

    ::boost::filesystem::remove_all( PATH.parent_path().string() );

    // check Image
    CPPUNIT_ASSERT_EQUAL(image->getDimension(), image2->getDimension());
    CPPUNIT_ASSERT_EQUAL(image->getManagesBuff(), image2->getManagesBuff());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->getSpacing()[0], image2->getSpacing()[0], 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->getSpacing()[1], image2->getSpacing()[1], 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(image->getSpacing()[2], image2->getSpacing()[2], 0.001);
    CPPUNIT_ASSERT_EQUAL(image->getSize()[0], image2->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(image->getSize()[1], image2->getSize()[1]);
    CPPUNIT_ASSERT_EQUAL(image->getSize()[2], image2->getSize()[2]);

    // not save in inrimage
//  CPPUNIT_ASSERT_EQUAL(image->getOrigin()[0], image2->getOrigin()[0]);
//  CPPUNIT_ASSERT_EQUAL(image->getOrigin()[1], image2->getOrigin()[1]);
//  CPPUNIT_ASSERT_EQUAL(image->getOrigin()[2], image2->getOrigin()[2]);
//  CPPUNIT_ASSERT_EQUAL(image->getWindowCenter(), image2->getWindowCenter());
//  CPPUNIT_ASSERT_EQUAL(image->getWindowWidth(), image2->getWindowWidth());
//  CPPUNIT_ASSERT_EQUAL(image->getRescaleIntercept(), image2->getRescaleIntercept());
}

//------------------------------------------------------------------------------

::fwData::Image::sptr ImageReaderWriterTest::createImage()
{
    // images informations
    const ::boost::uint8_t IMG1_DIMENSION   = 3 ;
    const bool IMG1_MANAGESBUFF             = true ;
    std::vector<double> IMG1_VECTORCREFSPACING;
    IMG1_VECTORCREFSPACING.push_back(1.2);
    IMG1_VECTORCREFSPACING.push_back(0.866);
    IMG1_VECTORCREFSPACING.push_back(0.456);
    std::vector<double> IMG1_VECTORCREFORIGIN(3,78)  ;
    std::vector< ::boost::int32_t > IMG1_VECTORCREFSIZE ;
    int sizeX = 46;
    IMG1_VECTORCREFSIZE.push_back(sizeX);
    int sizeY = 53;
    IMG1_VECTORCREFSIZE.push_back(sizeY);
    int sizeZ = 27;
    IMG1_VECTORCREFSIZE.push_back(sizeZ);
    const double IMG1_WINDOWCENTER = 86 ;
    const double IMG1_WINDOWWIDTH = 345 ;
    const double IMG1_RESCALEINTERCEPT = 1 ;
    ::fwTools::DynamicType IMG1_PIXELTYPE = ::fwTools::makeDynamicType<unsigned short>();
    unsigned short *buffer = new unsigned short[sizeX*sizeY*sizeZ];

    //create image
    ::fwData::Image::NewSptr pImage;

    pImage->setDimension(IMG1_DIMENSION);
    pImage->setPixelType(IMG1_PIXELTYPE);
    pImage->setManagesBuff(IMG1_MANAGESBUFF);
    pImage->setCRefSpacing(IMG1_VECTORCREFSPACING);
    pImage->setCRefOrigin(IMG1_VECTORCREFORIGIN);
    pImage->setCRefSize(IMG1_VECTORCREFSIZE);
    pImage->setWindowCenter(IMG1_WINDOWCENTER);
    pImage->setWindowWidth(IMG1_WINDOWWIDTH );
    pImage->setRescaleIntercept(IMG1_RESCALEINTERCEPT);
    pImage->setBuffer(buffer);

    return pImage;
}

//------------------------------------------------------------------------------
