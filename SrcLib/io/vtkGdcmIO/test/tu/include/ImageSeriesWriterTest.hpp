/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __VTKGDCMIO_UT_IMAGESERIESWRITERTEST_HPP__
#define __VTKGDCMIO_UT_IMAGESERIESWRITERTEST_HPP__

#include <fwServices/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace vtkGdcmIO
{
namespace ut
{

class ImageSeriesWriterTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ImageSeriesWriterTest );
CPPUNIT_TEST( writeReadTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void writeReadTest();

    // HACK: force link with fwMedDataCamp. Needed when calling ::fwDataCamp::visitor::CompareObjects::compare.
    int m_medDataCampVersion;
};

} // namespace ut
} // namespace vtkGdcmIO

#endif // __VTKGDCMIO_UT_IMAGESERIESWRITERTEST_HPP__
