/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

namespace sight::io::dicom::helper::ut
{

class DicomDataToolsTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(DicomDataToolsTest);
CPPUNIT_TEST(getPixelTypeTest);
CPPUNIT_TEST(getPhotometricInterpretationTest);
CPPUNIT_TEST(convertToPresentationTypeTest);
CPPUNIT_TEST(convertToRepresentationModeTest);
CPPUNIT_TEST(convertPointToFrameNumberNominalTest);
CPPUNIT_TEST(convertPointToFrameNumberFailureTest);
CPPUNIT_TEST(convertFrameNumberToZCoordinateNominalTest);
CPPUNIT_TEST(convertFrameNumberToZCoordinateFailureTest);
CPPUNIT_TEST_SUITE_END();

public:

    static void getPixelTypeTest();
    static void getPhotometricInterpretationTest();
    static void convertToPresentationTypeTest();
    static void convertToRepresentationModeTest();
    static void convertPointToFrameNumberNominalTest();
    static void convertPointToFrameNumberFailureTest();
    static void convertFrameNumberToZCoordinateNominalTest();
    static void convertFrameNumberToZCoordinateFailureTest();
};

} // namespace sight::io::dicom::helper::ut
