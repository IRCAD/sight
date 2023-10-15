/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include <service/macros.hpp>

#include <data/image.hpp>

namespace sight::io::itk::ut
{

class ImageConversionTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(ImageConversionTest);
CPPUNIT_TEST(testConversion);
CPPUNIT_TEST(stressTest);
CPPUNIT_TEST(testConversion2D);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;
    static void testConversion();
    void stressTest();
    static void testConversion2D();

    static void roundSpacing(data::image::sptr image);

    template<class TYPE>
    void stressTestForAType();
};

} // namespace sight::io::itk::ut

#include "ImageConversionTest.hxx"
