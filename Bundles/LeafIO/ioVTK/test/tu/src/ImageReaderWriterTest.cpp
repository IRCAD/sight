/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fstream>
#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwTools/System.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/AppConfigManager.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwData/Image.hpp>
#include <fwDataTools/Image.hpp>

#include <fwTest/Data.hpp>

#include "ImageReaderWriterTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ImageReaderWriterTest );
static const double epsilon = 0.00001;

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


::fwRuntime::EConfigurationElement::sptr getIOConfiguration(const ::boost::filesystem::path &file)
{
    ::fwRuntime::EConfigurationElement::NewSptr readerSrvCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr readerCfg("file");
    readerCfg->setValue(file.string());
    readerSrvCfg->addConfigurationElement(readerCfg);

    return readerSrvCfg;
    
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testVtkImageReader()
{

    const ::boost::filesystem::path file = ::fwTest::Data::dir() / "fw4spl/image/vtk/img.vtk";

    ::fwData::Image::NewSptr image;
    
    // Data expected
    const size_t dim = 3;
    ::fwData::Image::SpacingType spacingExpected(dim);
    spacingExpected[0] = 1.732;
    spacingExpected[1] = 1.732;
    spacingExpected[2] = 3.2;
    
    ::fwData::Image::OriginType originExpected(dim);
    originExpected[0] = 34.64;
    originExpected[1] = 86.6;
    originExpected[2] = 56;
    
    ::fwData::Image::SizeType sizeExpected(dim);
    sizeExpected[0] = 230;
    sizeExpected[1] = 170;
    sizeExpected[2] = 58;
    
    this->runImageSrv("::io::IReader","::ioVTK::ImageReaderService",getIOConfiguration(file), image); 

    // Data read.
    ::fwData::Image::SpacingType spacingRead = image->getSpacing();
    ::fwData::Image::SpacingType originRead = image->getOrigin();
    ::fwData::Image::SizeType sizeRead = image->getSize();

    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size() );
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size() );
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size() );

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2], epsilon);
        
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testVtiImageReader()
{

    const ::boost::filesystem::path file = ::fwTest::Data::dir() /"fw4spl/image/vti/BostonTeapot.vti";

    ::fwData::Image::NewSptr image;
    this->runImageSrv("::io::IReader","::ioVTK::ImageReaderService",getIOConfiguration(file), image); 
    
    // Data expected
    const size_t dim = 3;
    ::fwData::Image::SpacingType spacingExpected(dim);
    spacingExpected[0] = 1.0;
    spacingExpected[1] = 1.0;
    spacingExpected[2] = 1.0;
    
    ::fwData::Image::OriginType originExpected(dim);
    originExpected[0] = 1.1;
    originExpected[1] = 2.2;
    originExpected[2] = 3.3;
    
    ::fwData::Image::SizeType sizeExpected(dim);
    sizeExpected[0] = 256;
    sizeExpected[1] = 256;
    sizeExpected[2] = 178;

    ::fwTools::Type expectedType("int8"); // MHD File image type : MET_CHAR

    // Data read.
    ::fwData::Image::SpacingType spacingRead = image->getSpacing();
    ::fwData::Image::SpacingType originRead = image->getOrigin();
    ::fwData::Image::SizeType sizeRead = image->getSize();

    
    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size() );
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size() );
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size() );

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2], epsilon);
    
    CPPUNIT_ASSERT_EQUAL( expectedType, image->getType());
}

//------------------------------------------------------------------------------
void ImageReaderWriterTest::testMhdImageReader()
{

    const ::boost::filesystem::path file = ::fwTest::Data::dir() / "fw4spl/image/mhd/BostonTeapot.mhd";

    ::fwData::Image::NewSptr image;
    this->runImageSrv("::io::IReader","::ioVTK::ImageReaderService",getIOConfiguration(file), image); 
    
    // Data expected
    const size_t dim = 3;
    ::fwData::Image::SpacingType spacingExpected(dim);
    spacingExpected[0] = 1.0;
    spacingExpected[1] = 1.0;
    spacingExpected[2] = 1.0;
    
    ::fwData::Image::OriginType originExpected(dim);
    originExpected[0] = 1.1;
    originExpected[1] = 2.2;
    originExpected[2] = 3.3;
    
    ::fwData::Image::SizeType sizeExpected(dim);
    sizeExpected[0] = 256;
    sizeExpected[1] = 256;
    sizeExpected[2] = 178;
    
    ::fwTools::Type expectedType("int8"); // MHD File image type : MET_CHAR

    // Data read.
    ::fwData::Image::SpacingType spacingRead = image->getSpacing();
    ::fwData::Image::SpacingType originRead = image->getOrigin();
    ::fwData::Image::SizeType sizeRead = image->getSize();


    
    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size() );
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size() );
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size() );

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2], epsilon);

    CPPUNIT_ASSERT_EQUAL( expectedType, image->getType());

}
//------------------------------------------------------------------------------

void ImageReaderWriterTest::testImageReaderExtension()
{
    const ::boost::filesystem::path file = ::fwTools::System::getTemporaryFolder() / "img.xxx";

    std::ofstream ofile;
    ofile.open(file.string().c_str());
    ofile.close();

    ::fwData::Image::NewSptr image;


    CPPUNIT_ASSERT_THROW( 
            this->runImageSrv("::io::IReader","::ioVTK::ImageReaderService",getIOConfiguration(file), image),
            ::fwTools::Failed
            );

    ::boost::filesystem::remove(file);

}

//------------------------------------------------------------------------------
void ImageReaderWriterTest::testVtkImageWriter()
{
    // Data to write
    const size_t dim = 3;
    ::fwTools::Type type("uint8");
    ::fwData::Image::SizeType sizeExpected(dim);
    sizeExpected[0] = 10;
    sizeExpected[1] = 20;
    sizeExpected[2] = 30;
    ::fwData::Image::SpacingType spacingExpected(dim);
    spacingExpected[0] = 0.24;
    spacingExpected[1] = 1.07;
    spacingExpected[2] = 2.21;
    ::fwData::Image::OriginType originExpected(dim);
    originExpected[0] = -05.6;
    originExpected[1] = 15.16;
    originExpected[2] = 11.11;

    ::fwData::Image::NewSptr image;
    ::fwDataTools::Image::generateImage(image, sizeExpected, spacingExpected, originExpected, type);    

    // Write to vtk image.
    const ::boost::filesystem::path file = ::fwTools::System::getTemporaryFolder() / "temporaryFile.vtk";

    this->runImageSrv("::io::IWriter","::ioVTK::ImageWriterService",getIOConfiguration(file), image); 


    // Read image from disk 
    ::fwData::Image::NewSptr imageFromDisk;
    this->runImageSrv("::io::IReader","::ioVTK::ImageReaderService",getIOConfiguration(file), imageFromDisk); 
    
    ::boost::filesystem::remove(file);

    // Data read 
    ::fwData::Image::SpacingType spacingRead = imageFromDisk->getSpacing();
    ::fwData::Image::SpacingType originRead = imageFromDisk->getOrigin();
    ::fwData::Image::SizeType sizeRead = imageFromDisk->getSize();

    
    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size() );
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size() );
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size() );

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2], epsilon);


    char *ptrOnGeneratedImage = static_cast<char*>(image->getBuffer());
    char *ptrOnReadImage = static_cast<char*>(imageFromDisk->getBuffer());

    CPPUNIT_ASSERT_EQUAL( image->getType(), imageFromDisk->getType() );
    CPPUNIT_ASSERT( std::equal(ptrOnGeneratedImage, ptrOnGeneratedImage + image->getSizeInBytes(), ptrOnReadImage) );

}
//------------------------------------------------------------------------------
void ImageReaderWriterTest::testVtiImageWriter()
{
    // Data to write
    const size_t dim = 3;
    ::fwTools::Type type("uint8");
    ::fwData::Image::SizeType sizeExpected(dim);
    sizeExpected[0] = 10;
    sizeExpected[1] = 20;
    sizeExpected[2] = 30;
    ::fwData::Image::SpacingType spacingExpected(dim);
    spacingExpected[0] = 0.24;
    spacingExpected[1] = 1.07;
    spacingExpected[2] = 2.21;
    ::fwData::Image::OriginType originExpected(dim);
    originExpected[0] = -05.6;
    originExpected[1] = 15.16;
    originExpected[2] = 11.11;

    ::fwData::Image::NewSptr image;
    ::fwDataTools::Image::generateImage(image, sizeExpected, spacingExpected, originExpected, type);    

    // Write to vtk image.
    const ::boost::filesystem::path file = ::fwTools::System::getTemporaryFolder() / "temporaryFile.vti";

    this->runImageSrv("::io::IWriter","::ioVTK::ImageWriterService",getIOConfiguration(file), image); 


    // Read image from disk 
    ::fwData::Image::NewSptr imageFromDisk;
    this->runImageSrv("::io::IReader","::ioVTK::ImageReaderService",getIOConfiguration(file), imageFromDisk); 
    
    // Data read 
    ::fwData::Image::SpacingType spacingRead = imageFromDisk->getSpacing();
    ::fwData::Image::SpacingType originRead = imageFromDisk->getOrigin();
    ::fwData::Image::SizeType sizeRead = imageFromDisk->getSize();

    
    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size() );
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size() );
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size() );

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2], epsilon);


    char *ptrOnGeneratedImage = static_cast<char*>(image->getBuffer());
    char *ptrOnReadImage = static_cast<char*>(imageFromDisk->getBuffer());

    CPPUNIT_ASSERT_EQUAL( image->getType(), imageFromDisk->getType());
    CPPUNIT_ASSERT( std::equal(ptrOnGeneratedImage, ptrOnGeneratedImage + image->getSizeInBytes(), ptrOnReadImage) );


}
//------------------------------------------------------------------------------
void ImageReaderWriterTest::testMhdImageWriter()
{
    // Data to write
    const size_t dim = 3;
    ::fwTools::Type type("uint8");
    ::fwData::Image::SizeType sizeExpected(dim);
    sizeExpected[0] = 10;
    sizeExpected[1] = 20;
    sizeExpected[2] = 30;
    ::fwData::Image::SpacingType spacingExpected(dim);
    spacingExpected[0] = 0.24;
    spacingExpected[1] = 1.07;
    spacingExpected[2] = 2.21;
    ::fwData::Image::OriginType originExpected(dim);
    originExpected[0] = -05.6;
    originExpected[1] = 15.16;
    originExpected[2] = 11.11;

    ::fwData::Image::NewSptr image;
    ::fwDataTools::Image::generateImage(image, sizeExpected, spacingExpected, originExpected, type);    

    // Write to vtk image.
    const ::boost::filesystem::path file = ::fwTools::System::getTemporaryFolder()/ "temporaryFile.mhd";

    this->runImageSrv("::io::IWriter","::ioVTK::ImageWriterService",getIOConfiguration(file), image); 

    // Read image from disk 
    ::fwData::Image::NewSptr imageFromDisk;
    this->runImageSrv("::io::IReader","::ioVTK::ImageReaderService",getIOConfiguration(file), imageFromDisk); 

    // Data read 
    ::fwData::Image::SpacingType spacingRead = imageFromDisk->getSpacing();
    ::fwData::Image::SpacingType originRead = imageFromDisk->getOrigin();
    ::fwData::Image::SizeType sizeRead = imageFromDisk->getSize();

    
    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size() );
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size() );
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size() );

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2], epsilon);


    char *ptrOnGeneratedImage = static_cast<char*>(image->getBuffer());
    char *ptrOnReadImage = static_cast<char*>(imageFromDisk->getBuffer());

    CPPUNIT_ASSERT_EQUAL( image->getType(), imageFromDisk->getType());
    CPPUNIT_ASSERT( std::equal(ptrOnGeneratedImage, ptrOnGeneratedImage + image->getSizeInBytes(), ptrOnReadImage) );

}
//------------------------------------------------------------------------------
void ImageReaderWriterTest::testImageWriterExtension()
{
    // Data to write
    const size_t dim = 3;
    ::fwTools::Type type("uint8");
    ::fwData::Image::SizeType sizeExpected(dim);
    sizeExpected[0] = 10;
    sizeExpected[1] = 20;
    sizeExpected[2] = 30;
    ::fwData::Image::SpacingType spacingExpected(dim);
    spacingExpected[0] = 0.24;
    spacingExpected[1] = 1.07;
    spacingExpected[2] = 2.21;
    ::fwData::Image::OriginType originExpected(dim);
    originExpected[0] = -05.6;
    originExpected[1] = 15.16;
    originExpected[2] = 11.11;

    ::fwData::Image::NewSptr image;
    ::fwDataTools::Image::generateImage(image, sizeExpected, spacingExpected, originExpected, type);

    // Write to vtk image.
    const ::boost::filesystem::path file = ::fwTools::System::getTemporaryFolder()/ "temporaryFile.xxx";


    CPPUNIT_ASSERT_THROW(
            this->runImageSrv("::io::IWriter","::ioVTK::ImageWriterService",getIOConfiguration(file), image),
            ::fwTools::Failed
            );
}
//------------------------------------------------------------------------------
void ImageReaderWriterTest::runImageSrv(const std::string &srvtype, const std::string &srvname, const ::fwRuntime::EConfigurationElement::sptr cfg, ::fwData::Image::sptr image)
{

    ::fwServices::IService::sptr srv;
    srv = ::fwServices::registry::ServiceFactory::getDefault()->create( srvtype, srvname );

    CPPUNIT_ASSERT_MESSAGE(srvname,srv);

    ::fwServices::OSR::registerService( image , srv );
    srv->setConfiguration( cfg );
    srv->configure();
    srv->start();
    srv->update();
    srv->stop();
    ::fwServices::OSR::unregisterService( srv );
}

//------------------------------------------------------------------------------
