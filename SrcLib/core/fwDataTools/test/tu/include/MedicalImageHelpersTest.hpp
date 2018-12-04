/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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
CPPUNIT_TEST( setPixelBufferTest );
CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void getMinMaxTest();

    // Test the getPixelBuffer method for several image pixel types
    void getPixelBufferTest();

    //------------------------------------------------------------------------------

    // Test the setPixelBuffer method for several image pixel types
    void setPixelBufferTest();
};

} // namespace ut
} // namespace fwDataTools

#endif // __FWDATATOOLS_UT_MEDICALIMAGEHELPERSTEST_HPP__
