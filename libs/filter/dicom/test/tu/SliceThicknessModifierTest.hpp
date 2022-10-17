/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#pragma once

#include <service/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::filter::dicom::ut
{

/**
 * @brief Test SliceThicknessModifier class
 */
class SliceThicknessModifierTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(SliceThicknessModifierTest);
CPPUNIT_TEST(simpleApplication);
CPPUNIT_TEST(applyFilterOn2DImage);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    /// Apply the filter and verify that the DicomSeries has been correctly modified
    static void simpleApplication();

    /// Apply the filter on an unsupported 2D image
    static void applyFilterOn2DImage();
};

} // namespace sight::filter::dicom::ut
