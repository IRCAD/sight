/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#pragma once

#include <data/array.hpp>
#include <data/image.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::vtk::ut
{

/**
 * @brief Test many methods to create image.
 */
class image_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(image_test);
CPPUNIT_TEST(testImageToVtk);
CPPUNIT_TEST(testFromVtk);

CPPUNIT_TEST(fromToVtkTest);
CPPUNIT_TEST(mhdReaderTest);
CPPUNIT_TEST(mhdWriterTest);
CPPUNIT_TEST(vtiReaderTest);
CPPUNIT_TEST(vtiWriterTest);
CPPUNIT_TEST(vtkReaderTest);
CPPUNIT_TEST(vtkWriterTest);

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void testImageToVtk();
    static void testFromVtk();

    static void fromToVtkTest();
    static void mhdReaderTest();
    static void mhdWriterTest();
    static void vtiReaderTest();
    static void vtiWriterTest();
    static void vtkReaderTest();
    static void vtkWriterTest();
};

} // namespace sight::io::vtk::ut
