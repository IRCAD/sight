/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/Image.hpp>

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

void TestRoiApply(const std::string& imageTypeName, const std::string& roiTypeName)
{
    std::stringstream ss;
    ss
        << "Test failed with types : img : " << imageTypeName
        << ", roi : " << roiTypeName;
    std::string message = ss.str();
    ::fwTools::Type imageType(imageTypeName);
    ::fwTools::Type roiType(roiTypeName);
    ::fwData::Image::sptr imageRef;
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwData::Image::sptr roi   = ::fwData::Image::New();

    ::fwTest::generator::Image::generateRandomImage(image, imageType);
    ::fwData::Image::SizeType size       = image->getSize();
    ::fwData::Image::SpacingType spacing = image->getSpacing();
    ::fwData::Image::OriginType origin   = image->getOrigin();
    ::fwTest::generator::Image::generateImage(roi, size, spacing, origin, roiType);

    imageRef = ::fwData::Object::copy(image);

    ::fwData::Array::sptr imgData;
    ::fwData::Array::sptr roiData;
    imgData = image->getDataArray();
    roiData = roi->getDataArray();

    ::fwDataTools::helper::Array roiDataHelper(roiData);

    CPPUNIT_ASSERT(imgData);
    CPPUNIT_ASSERT(imgData->getNumberOfElements());

    CPPUNIT_ASSERT(roiData);
    CPPUNIT_ASSERT(roiData->getNumberOfElements());

    ::fwTest::generator::Image::randomizeArray(roi->getDataArray());

    char* begin = roiDataHelper.begin();
    char* end   = roiDataHelper.end();
    size_t part = (end - begin)/3;

    std::fill(begin, begin + part, 0);
    std::fill(end - part, end, 0);

    ::fwDataTools::Image::applyRoi(image, roi);
    CPPUNIT_ASSERT_MESSAGE( message, ::fwDataTools::Image::isRoiApplyed(imageRef, roi, image));
}

//------------------------------------------------------------------------------

void ImageTest::roiApplyTest()
{
    TestRoiApply( "int8", "int8"  );
    TestRoiApply( "int8", "int16" );
    TestRoiApply( "int8", "int32" );
    TestRoiApply( "int8", "uint8" );
    TestRoiApply( "int8", "uint16");
    TestRoiApply( "int8", "uint32");

    TestRoiApply( "int16", "int8"  );
    TestRoiApply( "int16", "int16" );
    TestRoiApply( "int16", "int32" );
    TestRoiApply( "int16", "uint8" );
    TestRoiApply( "int16", "uint16");
    TestRoiApply( "int16", "uint32");

    TestRoiApply( "int32", "int8"  );
    TestRoiApply( "int32", "int16" );
    TestRoiApply( "int32", "int32" );
    TestRoiApply( "int32", "uint8" );
    TestRoiApply( "int32", "uint16");
    TestRoiApply( "int32", "uint32");

    TestRoiApply( "uint8", "int8"  );
    TestRoiApply( "uint8", "int16" );
    TestRoiApply( "uint8", "int32" );
    TestRoiApply( "uint8", "uint8" );
    TestRoiApply( "uint8", "uint16");
    TestRoiApply( "uint8", "uint32");

    TestRoiApply( "uint16", "int8"  );
    TestRoiApply( "uint16", "int16" );
    TestRoiApply( "uint16", "int32" );
    TestRoiApply( "uint16", "uint8" );
    TestRoiApply( "uint16", "uint16");
    TestRoiApply( "uint16", "uint32");

    TestRoiApply( "uint32", "int8"  );
    TestRoiApply( "uint32", "int16" );
    TestRoiApply( "uint32", "int32" );
    TestRoiApply( "uint32", "uint8" );
    TestRoiApply( "uint32", "uint16");
    TestRoiApply( "uint32", "uint32");

// float and double disabled : randomization generate some <nan>

//    TestRoiApply( "int8", "float" );
//    TestRoiApply( "int16" , "float" );
//    TestRoiApply( "int32" , "float" );
//    TestRoiApply( "uint8" , "float" );
//    TestRoiApply( "uint16", "float" );
//    TestRoiApply( "uint32", "float" );
//    TestRoiApply( "float" , "int8"  );
//    TestRoiApply( "float" , "int16" );
//    TestRoiApply( "float" , "int32" );
//    TestRoiApply( "float" , "uint8" );
//    TestRoiApply( "float" , "uint16");
//    TestRoiApply( "float" , "uint32");
//    TestRoiApply( "float" , "float" );
//
//#ifndef DEBUG // double disabled from default dispatcher type list in debug mode
//    TestRoiApply( "int8", "double");
//    TestRoiApply( "int16" , "double");
//    TestRoiApply( "int32" , "double");
//    TestRoiApply( "uint8" , "double");
//    TestRoiApply( "uint16", "double");
//    TestRoiApply( "uint32", "double");
//    TestRoiApply( "float" , "double");
//    TestRoiApply( "double", "int8"  );
//    TestRoiApply( "double", "int16" );
//    TestRoiApply( "double", "int32" );
//    TestRoiApply( "double", "uint8" );
//    TestRoiApply( "double", "uint16");
//    TestRoiApply( "double", "uint32");
//    TestRoiApply( "double", "float" );
//    TestRoiApply( "double", "double");
//#endif

    // 64 bit type not supported by DynamicType/Dispatcher

    // TestRoiApply( "uint64", "double");
    // TestRoiApply( "int64" , "double");
    // TestRoiApply( "double", "int64" );
    // TestRoiApply( "double", "uint64");
    //
    // TestRoiApply( "int64" , "float" );
    // TestRoiApply( "uint64", "float" );
    // TestRoiApply( "float" , "int64" );
    // TestRoiApply( "float" , "uint64");
    //
    // TestRoiApply( "int8", "int64" );
    // TestRoiApply( "int8", "uint64");
    // TestRoiApply( "int16" , "int64" );
    // TestRoiApply( "int16" , "uint64");
    // TestRoiApply( "int32" , "int64" );
    // TestRoiApply( "int32" , "uint64");
    // TestRoiApply( "int64" , "int8"  );
    // TestRoiApply( "int64" , "int16" );
    // TestRoiApply( "int64" , "int32" );
    // TestRoiApply( "int64" , "int64" );
    // TestRoiApply( "int64" , "uint8" );
    // TestRoiApply( "int64" , "uint16");
    // TestRoiApply( "int64" , "uint32");
    // TestRoiApply( "int64" , "uint64");
    // TestRoiApply( "uint8" , "int64" );
    // TestRoiApply( "uint8" , "uint64");
    // TestRoiApply( "uint16", "int64" );
    // TestRoiApply( "uint16", "uint64");
    // TestRoiApply( "uint32", "int64" );
    // TestRoiApply( "uint32", "uint64");
    // TestRoiApply( "uint64", "int8"  );
    // TestRoiApply( "uint64", "int16" );
    // TestRoiApply( "uint64", "int32" );
    // TestRoiApply( "uint64", "int64" );
    // TestRoiApply( "uint64", "uint8" );
    // TestRoiApply( "uint64", "uint16");
    // TestRoiApply( "uint64", "uint32");
    // TestRoiApply( "uint64", "uint64");

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataTools
