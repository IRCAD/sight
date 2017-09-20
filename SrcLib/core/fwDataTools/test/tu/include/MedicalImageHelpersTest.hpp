/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_UT_MEDICALIMAGEHELPERSTEST_HPP__
#define __FWDATATOOLS_UT_MEDICALIMAGEHELPERSTEST_HPP__

#include <fwCore/base.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwDataTools
{
namespace ut
{

class MedicalImageHelpersTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( MedicalImageHelpersTest );
CPPUNIT_TEST( getMinMaxTest );
CPPUNIT_TEST( getPixelBufferTest );
CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void getMinMaxTest();
    // Test the getPixelBuffer method for several image pixel types
    void getPixelBufferTest();
    //------------------------------------------------------------------------------

    template <class P>
    void getPixelBufferTestHelper(P pixelValue[3])
    {
        constexpr size_t IMG_DIMENSIONS = 100;

        // Create a new image
        auto image = ::fwData::Image::New();
        ::fwData::Image::SizeType size(3);
        std::fill_n(size.begin(), 3, IMG_DIMENSIONS);
        image->allocate(size, ::fwTools::Type::create<P>(), 3);
        image->setSpacing(::fwData::Image::SpacingType(3, 1));
        image->setOrigin(::fwData::Image::OriginType(3, 0));

        // Zero the buffer
        auto array           = image->getDataArray();
        const auto szArray   = image->getAllocatedSizeInBytes();
        void* const arrayPtr = array->getBufferObject()->getBuffer();
        std::fill_n(static_cast<uint8_t*>(arrayPtr), szArray, 0);

        // Pick some random coordinates and store the given pixel there
        size_t coords[3];
        std::generate_n(coords, 3, [&] () { return rand() % IMG_DIMENSIONS; });
        P* pixelPtr = static_cast<P*>(arrayPtr) +
                      ((coords[0] + coords[1] * size[0] + coords[2] * size[1] * size[0]) * 3);
        std::copy(pixelValue, pixelValue + 3, pixelPtr);

        // Test that the helper returned pixel value is correct
        ::fwDataTools::helper::ImageGetter constHelper(image);
        ::fwDataTools::helper::Image helper(image);
        P* pixelHelperPtr1 = static_cast<P*>(constHelper.getPixelBuffer(coords[0], coords[1], coords[2]));
        P* pixelHelperPtr2 = static_cast<P*>(helper.getPixelBuffer(coords[0], coords[1], coords[2]));
        if(std::is_floating_point<P>::value)
        {
            for(auto i = 0; i != 3; ++i)
            {
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Pixel values are not equal", pixelHelperPtr1[i], pixelValue[i],
                                                     0.00001);
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Pixel values are not equal", pixelHelperPtr2[i], pixelValue[i],
                                                     0.00001);
            }
        }
        else
        {
            for(auto i = 0; i != 3; ++i)
            {
                CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", pixelHelperPtr1[i], pixelValue[i]);
                CPPUNIT_ASSERT_EQUAL_MESSAGE("Pixel values are not equal", pixelHelperPtr2[i], pixelValue[i]);
            }
        }
    }

};

} // namespace ut
} // namespace fwDataTools

#endif // __FWDATATOOLS_UT_MEDICALIMAGEHELPERSTEST_HPP__
