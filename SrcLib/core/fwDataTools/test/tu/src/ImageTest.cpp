/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "ImageTest.hpp"

#include <fwDataTools/Image.hpp>
#include <fwDataTools/helper/Array.hpp>

#include <fwTest/generator/Image.hpp>

#include <fwTools/Type.hpp>

#include <sstream>

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

void ImageTest::roiApplyTest()
{

#define TestRoiApplyMacro(imageTypeName, roiTypeName)                                                   \
    {                                                                                                   \
        std::stringstream ss;                                                                           \
        ss                                                                                              \
            << "Test failed with types : img : " << imageTypeName                                       \
            << ", roi : " << roiTypeName;                                                               \
        std::string message = ss.str();                                                                 \
        ::fwTools::Type imageType(imageTypeName);                                                       \
        ::fwTools::Type roiType(roiTypeName);                                                           \
        ::fwData::Image::sptr imageRef;                                                                 \
        ::fwData::Image::sptr image = ::fwData::Image::New();                                                                 \
        ::fwData::Image::sptr roi   = ::fwData::Image::New();                                                                   \
                                                                                                        \
        ::fwTest::generator::Image::generateRandomImage(image, imageType);                                    \
        ::fwData::Image::SizeType size       = image->getSize();                                           \
        ::fwData::Image::SpacingType spacing = image->getSpacing();                                     \
        ::fwData::Image::OriginType origin   = image->getOrigin();                                       \
        ::fwTest::generator::Image::generateImage(roi, size, spacing, origin, roiType);                       \
                                                                                                        \
        imageRef = ::fwData::Object::copy(image);                                                       \
                                                                                                        \
        ::fwData::Array::sptr imgData;                                                                  \
        ::fwData::Array::sptr roiData;                                                                  \
        imgData = image->getDataArray();                                                                \
        roiData = roi->getDataArray();                                                                  \
                                                                                                        \
        ::fwDataTools::helper::Array roiDataHelper(roiData);                                                \
                                                                                                        \
        CPPUNIT_ASSERT(imgData);                                                                        \
        CPPUNIT_ASSERT(imgData->getNumberOfElements());                                                 \
                                                                                                        \
        CPPUNIT_ASSERT(roiData);                                                                        \
        CPPUNIT_ASSERT(roiData->getNumberOfElements());                                                 \
                                                                                                        \
        ::fwTest::generator::Image::randomizeArray(roi->getDataArray());                                      \
                                                                                                        \
        char* begin = roiDataHelper.begin();                                                            \
        char* end   = roiDataHelper.end();                                                              \
        size_t part = (end - begin)/3;                                                                  \
                                                                                                        \
        std::fill(begin, begin + part, 0);                                                              \
        std::fill(end - part, end, 0);                                                                  \
                                                                                                        \
        ::fwDataTools::Image::applyRoi(image, roi);                                                     \
        CPPUNIT_ASSERT_MESSAGE( message, ::fwDataTools::Image::isRoiApplyed(imageRef, roi, image));     \
    }



    TestRoiApplyMacro( "int8", "int8"  );
    TestRoiApplyMacro( "int8", "int16" );
    TestRoiApplyMacro( "int8", "int32" );
    TestRoiApplyMacro( "int8", "uint8" );
    TestRoiApplyMacro( "int8", "uint16");
    TestRoiApplyMacro( "int8", "uint32");

    TestRoiApplyMacro( "int16", "int8"  );
    TestRoiApplyMacro( "int16", "int16" );
    TestRoiApplyMacro( "int16", "int32" );
    TestRoiApplyMacro( "int16", "uint8" );
    TestRoiApplyMacro( "int16", "uint16");
    TestRoiApplyMacro( "int16", "uint32");

    TestRoiApplyMacro( "int32", "int8"  );
    TestRoiApplyMacro( "int32", "int16" );
    TestRoiApplyMacro( "int32", "int32" );
    TestRoiApplyMacro( "int32", "uint8" );
    TestRoiApplyMacro( "int32", "uint16");
    TestRoiApplyMacro( "int32", "uint32");

    TestRoiApplyMacro( "uint8", "int8"  );
    TestRoiApplyMacro( "uint8", "int16" );
    TestRoiApplyMacro( "uint8", "int32" );
    TestRoiApplyMacro( "uint8", "uint8" );
    TestRoiApplyMacro( "uint8", "uint16");
    TestRoiApplyMacro( "uint8", "uint32");

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
