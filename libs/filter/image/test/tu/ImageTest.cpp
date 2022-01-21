/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <core/tools/Type.hpp>

#include <filter/image/Image.hpp>

#include <utestData/generator/Image.hpp>

#include <sstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::ImageTest);

namespace sight::filter::image
{

namespace ut
{

//------------------------------------------------------------------------------

void ImageTest::setUp()
{
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
    const std::string message = ss.str();
    const core::tools::Type imageType(imageTypeName);
    const core::tools::Type roiType(roiTypeName);
    data::Image::sptr imageRef;
    data::Image::sptr image = data::Image::New();
    data::Image::sptr roi   = data::Image::New();

    // generate a random image
    utestData::generator::Image::generateRandomImage(image, imageType);
    const data::Image::Size size       = image->getSize();
    const data::Image::Spacing spacing = image->getSpacing();
    const data::Image::Origin origin   = image->getOrigin();
    utestData::generator::Image::generateImage(
        roi,
        size,
        spacing,
        origin,
        roiType,
        data::Image::PixelFormat::GRAY_SCALE
    );

    imageRef = data::Object::copy(image);

    // fill roi with random values
    utestData::generator::Image::randomizeImage(roi);

    const auto dumpLock    = roi->lock();
    auto begin             = roi->begin();
    const auto end         = roi->end();
    const std::size_t part = (end - begin) / 3;

    // keep random values in 1/3 of the image (else fill with 0)
    std::fill(begin, begin + part, 0);
    std::fill(end - part, end, 0);

    // apply roi and check
    filter::image::applyRoi(image, roi);
    CPPUNIT_ASSERT_MESSAGE(message, filter::image::isRoiApplyed(imageRef, roi, image));
}

//------------------------------------------------------------------------------

void ImageTest::roiApplyTest()
{
    TestRoiApply("int8", "int8");
    TestRoiApply("int8", "int16");
    TestRoiApply("int8", "int32");
    TestRoiApply("int8", "uint8");
    TestRoiApply("int8", "uint16");
    TestRoiApply("int8", "uint32");

    TestRoiApply("int16", "int8");
    TestRoiApply("int16", "int16");
    TestRoiApply("int16", "int32");
    TestRoiApply("int16", "uint8");
    TestRoiApply("int16", "uint16");
    TestRoiApply("int16", "uint32");

    TestRoiApply("int32", "int8");
    TestRoiApply("int32", "int16");
    TestRoiApply("int32", "int32");
    TestRoiApply("int32", "uint8");
    TestRoiApply("int32", "uint16");
    TestRoiApply("int32", "uint32");

    TestRoiApply("uint8", "int8");
    TestRoiApply("uint8", "int16");
    TestRoiApply("uint8", "int32");
    TestRoiApply("uint8", "uint8");
    TestRoiApply("uint8", "uint16");
    TestRoiApply("uint8", "uint32");

    TestRoiApply("uint16", "int8");
    TestRoiApply("uint16", "int16");
    TestRoiApply("uint16", "int32");
    TestRoiApply("uint16", "uint8");
    TestRoiApply("uint16", "uint16");
    TestRoiApply("uint16", "uint32");

    TestRoiApply("uint32", "int8");
    TestRoiApply("uint32", "int16");
    TestRoiApply("uint32", "int32");
    TestRoiApply("uint32", "uint8");
    TestRoiApply("uint32", "uint16");
    TestRoiApply("uint32", "uint32");

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

} //namespace sight::filter::image
