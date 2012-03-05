/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>
#include <fwTools/Type.hpp>

#include <fwDataTools/Image.hpp>

#include "ImageTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataTools::ut::ImageTest );

namespace fwDataTools
{
namespace ut
{

//------------------------------------------------------------------------------

void ImageTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
}

//------------------------------------------------------------------------------

void ImageTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageTest::testGetterSetter3D()
{
    ::fwData::Image::NewSptr image;
    const ::boost::uint8_t dim = 3;
    ::fwData::Image::SizeType size(dim);
    size[0] = 10;
    size[1] = 15;
    size[2] = 23;
    std::vector<double> spacing(dim);
    spacing[0] = 0.85;
    spacing[1] = 2.6;
    spacing[2] = 1.87;
    std::vector<double> origin(dim);
    origin[0] = -45.6;
    origin[1] = 25.97;
    origin[2] = -53.9;
    ::fwTools::Type type = ::fwTools::Type::create< ::boost::int32_t >();

    ::fwDataTools::Image::generateImage(image, size, spacing, origin, type);
    CPPUNIT_ASSERT(::fwDataTools::Image::compareImage(image, size, spacing, origin, type));
}

//------------------------------------------------------------------------------

void ImageTest::testGetterSetter2D()
{
    ::fwData::Image::NewSptr image;
    const ::boost::uint8_t dim = 2;
    ::fwData::Image::SizeType size(dim);
    size[0] = 10;
    size[1] = 15;
    std::vector<double> spacing(dim);
    spacing[0] = 0.85;
    spacing[1] = 2.6;
    std::vector<double> origin(dim);
    origin[0] = -45.6;
    origin[1] = 25.97;
    ::fwTools::Type type = ::fwTools::Type::create< ::boost::int16_t >();

    ::fwDataTools::Image::generateImage(image, size, spacing, origin, type);
    CPPUNIT_ASSERT(::fwDataTools::Image::compareImage(image, size, spacing, origin, type));
}

//------------------------------------------------------------------------------

void ImageTest::testDeepCopy()
{
    ::fwData::Image::NewSptr image;
    ::fwData::Image::NewSptr image2;
    ::fwTools::Type type = ::fwTools::Type::create< ::boost::int32_t >();
    ::fwDataTools::Image::generateRandomImage(image, type);

    image2->deepCopy(image);
    CPPUNIT_ASSERT(::fwDataTools::Image::compareImage(image, image2));
}

//------------------------------------------------------------------------------

void ImageTest::stressTest()
{
    ::fwTools::Type type = ::fwTools::Type::create< ::boost::int8_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< ::boost::uint8_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< ::boost::int16_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< ::boost::uint16_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< ::boost::int32_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< ::boost::uint32_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< ::boost::int64_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< ::boost::uint64_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< float >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< double >();
    this->stressTestWithType(type, 10);
}

//------------------------------------------------------------------------------

void ImageTest::stressTestWithType(::fwTools::Type type, int nbTest)
{
    for (int nb=0 ; nb < nbTest ; ++nb)
    {
        ::fwData::Image::NewSptr image;
        ::fwData::Image::NewSptr image2;
        ::fwDataTools::Image::generateRandomImage(image, type);

        image2->deepCopy(image);
        CPPUNIT_ASSERT(::fwDataTools::Image::compareImage(image, image2));
    }
}

//------------------------------------------------------------------------------

void ImageTest::roiApplyTest()
{

#define TestRoiApplyMacro(imageTypeName, roiTypeName)                                                  \
    {                                                                                                  \
        std::stringstream ss;                                                                          \
        ss                                                                                             \
            << "Test failed with types : img : " << imageTypeName                                      \
            << ", roi : " << roiTypeName;                                                              \
        std::string message = ss.str();\
        ::fwTools::Type imageType(imageTypeName);                                                      \
        ::fwTools::Type roiType(roiTypeName);                                                          \
        ::fwData::Image::NewSptr imageRef;                                                             \
        ::fwData::Image::NewSptr image;                                                                \
        ::fwData::Image::NewSptr roi;                                                                  \
                                                                                                       \
        ::fwDataTools::Image::generateRandomImage(image, imageType);                                   \
        ::fwData::Image::SizeType    size = image->getSize();                                          \
        ::fwData::Image::SpacingType spacing = image->getSpacing();                                    \
        ::fwData::Image::OriginType  origin = image->getOrigin();                                      \
        ::fwDataTools::Image::generateImage(roi, size, spacing, origin, roiType);                      \
                                                                                                       \
        imageRef->deepCopy(image);                                                                     \
                                                                                                       \
        ::fwData::Array::sptr imgData;                                                                 \
        ::fwData::Array::sptr roiData;                                                                 \
        imgData = image->getDataArray();                                                               \
        roiData = roi->getDataArray();                                                                 \
                                                                                                       \
        CPPUNIT_ASSERT(imgData);                                                                       \
        CPPUNIT_ASSERT(imgData->getNumberOfElements());                                                \
                                                                                                       \
        CPPUNIT_ASSERT(roiData);                                                                       \
        CPPUNIT_ASSERT(roiData->getNumberOfElements());                                                \
                                                                                                       \
        ::fwDataTools::Image::randomizeArray(roi->getDataArray());                                     \
                                                                                                       \
        char *begin = roiData->begin();                                                                \
        char *end   = roiData->end();                                                                  \
        size_t part = (end - begin)/3;                                                                 \
                                                                                                       \
        std::fill(begin, begin + part, 0);                                                             \
        std::fill(end - part, end, 0);                                                                 \
                                                                                                       \
        ::fwDataTools::Image::applyRoi(image, roi);                                                    \
        CPPUNIT_ASSERT_MESSAGE( message, ::fwDataTools::Image::isRoiApplyed(imageRef, roi, image));   \
    }



    TestRoiApplyMacro( "int8", "int8"  );
    TestRoiApplyMacro( "int8", "int16" );
    TestRoiApplyMacro( "int8", "int32" );
    TestRoiApplyMacro( "int8", "uint8" );
    TestRoiApplyMacro( "int8", "uint16");
    TestRoiApplyMacro( "int8", "uint32");

    TestRoiApplyMacro( "int16" , "int8"  );
    TestRoiApplyMacro( "int16" , "int16" );
    TestRoiApplyMacro( "int16" , "int32" );
    TestRoiApplyMacro( "int16" , "uint8" );
    TestRoiApplyMacro( "int16" , "uint16");
    TestRoiApplyMacro( "int16" , "uint32");

    TestRoiApplyMacro( "int32" , "int8"  );
    TestRoiApplyMacro( "int32" , "int16" );
    TestRoiApplyMacro( "int32" , "int32" );
    TestRoiApplyMacro( "int32" , "uint8" );
    TestRoiApplyMacro( "int32" , "uint16");
    TestRoiApplyMacro( "int32" , "uint32");

    TestRoiApplyMacro( "uint8" , "int8"  );
    TestRoiApplyMacro( "uint8" , "int16" );
    TestRoiApplyMacro( "uint8" , "int32" );
    TestRoiApplyMacro( "uint8" , "uint8" );
    TestRoiApplyMacro( "uint8" , "uint16");
    TestRoiApplyMacro( "uint8" , "uint32");

    TestRoiApplyMacro( "uint16", "int8"  );
    TestRoiApplyMacro( "uint16", "int16" );
    TestRoiApplyMacro( "uint16", "int32" );
    TestRoiApplyMacro( "uint16", "uint8" );
    TestRoiApplyMacro( "uint16", "uint16");
    TestRoiApplyMacro( "uint16", "uint32");

    TestRoiApplyMacro( "uint32", "int8"  );
    TestRoiApplyMacro( "uint32", "int16" );
    TestRoiApplyMacro( "uint32", "int32" );
    TestRoiApplyMacro( "uint32", "uint8" );
    TestRoiApplyMacro( "uint32", "uint16");
    TestRoiApplyMacro( "uint32", "uint32");

// float and double disabled : randomization generate some <nan>

//    TestRoiApplyMacro( "int8", "float" );
//    TestRoiApplyMacro( "int16" , "float" );
//    TestRoiApplyMacro( "int32" , "float" );
//    TestRoiApplyMacro( "uint8" , "float" );
//    TestRoiApplyMacro( "uint16", "float" );
//    TestRoiApplyMacro( "uint32", "float" );
//    TestRoiApplyMacro( "float" , "int8"  );
//    TestRoiApplyMacro( "float" , "int16" );
//    TestRoiApplyMacro( "float" , "int32" );
//    TestRoiApplyMacro( "float" , "uint8" );
//    TestRoiApplyMacro( "float" , "uint16");
//    TestRoiApplyMacro( "float" , "uint32");
//    TestRoiApplyMacro( "float" , "float" );
//
//#ifndef DEBUG // double disabled from default dispatcher type list in debug mode
//    TestRoiApplyMacro( "int8", "double");
//    TestRoiApplyMacro( "int16" , "double");
//    TestRoiApplyMacro( "int32" , "double");
//    TestRoiApplyMacro( "uint8" , "double");
//    TestRoiApplyMacro( "uint16", "double");
//    TestRoiApplyMacro( "uint32", "double");
//    TestRoiApplyMacro( "float" , "double");
//    TestRoiApplyMacro( "double", "int8"  );
//    TestRoiApplyMacro( "double", "int16" );
//    TestRoiApplyMacro( "double", "int32" );
//    TestRoiApplyMacro( "double", "uint8" );
//    TestRoiApplyMacro( "double", "uint16");
//    TestRoiApplyMacro( "double", "uint32");
//    TestRoiApplyMacro( "double", "float" );
//    TestRoiApplyMacro( "double", "double");
//#endif


    // 64 bit type not supported by DynamicType/Dispatcher

    // TestRoiApplyMacro( "uint64", "double");
    // TestRoiApplyMacro( "int64" , "double");
    // TestRoiApplyMacro( "double", "int64" );
    // TestRoiApplyMacro( "double", "uint64");
    //
    // TestRoiApplyMacro( "int64" , "float" );
    // TestRoiApplyMacro( "uint64", "float" );
    // TestRoiApplyMacro( "float" , "int64" );
    // TestRoiApplyMacro( "float" , "uint64");
    //
    // TestRoiApplyMacro( "int8", "int64" );
    // TestRoiApplyMacro( "int8", "uint64");
    // TestRoiApplyMacro( "int16" , "int64" );
    // TestRoiApplyMacro( "int16" , "uint64");
    // TestRoiApplyMacro( "int32" , "int64" );
    // TestRoiApplyMacro( "int32" , "uint64");
    // TestRoiApplyMacro( "int64" , "int8"  );
    // TestRoiApplyMacro( "int64" , "int16" );
    // TestRoiApplyMacro( "int64" , "int32" );
    // TestRoiApplyMacro( "int64" , "int64" );
    // TestRoiApplyMacro( "int64" , "uint8" );
    // TestRoiApplyMacro( "int64" , "uint16");
    // TestRoiApplyMacro( "int64" , "uint32");
    // TestRoiApplyMacro( "int64" , "uint64");
    // TestRoiApplyMacro( "uint8" , "int64" );
    // TestRoiApplyMacro( "uint8" , "uint64");
    // TestRoiApplyMacro( "uint16", "int64" );
    // TestRoiApplyMacro( "uint16", "uint64");
    // TestRoiApplyMacro( "uint32", "int64" );
    // TestRoiApplyMacro( "uint32", "uint64");
    // TestRoiApplyMacro( "uint64", "int8"  );
    // TestRoiApplyMacro( "uint64", "int16" );
    // TestRoiApplyMacro( "uint64", "int32" );
    // TestRoiApplyMacro( "uint64", "int64" );
    // TestRoiApplyMacro( "uint64", "uint8" );
    // TestRoiApplyMacro( "uint64", "uint16");
    // TestRoiApplyMacro( "uint64", "uint32");
    // TestRoiApplyMacro( "uint64", "uint64");

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataTools
