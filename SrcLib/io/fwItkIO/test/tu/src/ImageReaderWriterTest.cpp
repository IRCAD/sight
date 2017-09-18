/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ImageReaderWriterTest.hpp"

#include "helper.hpp"

#include <fwItkIO/ImageReader.hpp>
#include <fwItkIO/ImageWriter.hpp>

#include <fwCore/base.hpp>

#include <fwTest/generator/Image.hpp>
#include <fwTest/helper/compare.hpp>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwItkIO::ut::ImageReaderWriterTest );

namespace fwItkIO
{
namespace ut
{

//------------------------------------------------------------------------------

void ImageReaderWriterTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testSaveLoadInr()
{
    // create Image
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));
    this->checkSaveLoadInr( image );
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::stressTestInr()
{
    ::fwTools::Type type = ::fwTools::Type::create< std::int8_t >();
//    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< std::uint8_t >();
    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< std::int16_t >();
    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< std::uint16_t >();
    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< ::boost::int32_t >();
    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< std::uint32_t >();
    this->stressTestInrWithType(type, 5);

//    type = ::fwTools::Type::create< std::int64_t >();
//    this->stressTestInrWithType(type, 5);

//    type = ::fwTools::Type::create< std::uint64_t >();
//    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< float >();
    this->stressTestInrWithType(type, 5);

//    type = ::fwTools::Type::create< double >();
//    this->stressTestInrWithType(type, 5);
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::stressTestInrWithType(::fwTools::Type type, int nbTest)
{
    for (int nb = 0; nb < nbTest; ++nb)
    {
        ::fwData::Image::sptr image = ::fwData::Image::New();
        ::fwTest::generator::Image::generateRandomImage(image, type);
        this->checkSaveLoadInr(image);
    }
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::checkSaveLoadInr( ::fwData::Image::sptr image )
{
    // inr only support image origin (0,0,0)
    ::fwData::Image::OriginType origin(3, 0);
    image->setOrigin(origin);

    // save image in inr
    const ::boost::filesystem::path PATH = "imageInrTest/image.inr.gz";
    ::boost::filesystem::create_directories( PATH.parent_path() );
    ::fwItkIO::ImageWriter::sptr myWriter = ::fwItkIO::ImageWriter::New();
    myWriter->setObject(image);
    myWriter->setFile(PATH);
    myWriter->write();

    // load Image
    ::fwData::Image::sptr image2          = ::fwData::Image::New();
    ::fwItkIO::ImageReader::sptr myReader = ::fwItkIO::ImageReader::New();
    myReader->setObject(image2);
    myReader->setFile(PATH);
    myReader->read();

    ::boost::filesystem::remove_all( PATH.parent_path().string() );

    ::fwItkIO::ut::helper::roundSpacing(image2);

    ::fwTest::helper::ExcludeSetType exclude;
    exclude.insert("window_center");
    exclude.insert("window_width");
    // check Image
    // inr only support float spacing and float origin => add tolerance for comparison (+/-0.00001)
    CPPUNIT_ASSERT(::fwTest::helper::compare(image, image2, exclude));
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwItkIO
