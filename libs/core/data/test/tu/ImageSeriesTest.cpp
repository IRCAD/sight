/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ImageSeriesTest.hpp"

#include <core/tools/Type.hpp>

#include <utestData/generator/Image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::ImageSeriesTest);

namespace sight::data
{

namespace ut
{

//------------------------------------------------------------------------------

void ImageSeriesTest::setUp()
{
    // Set up context before running a test.
    m_series = data::ImageSeries::New();
}

//------------------------------------------------------------------------------

void ImageSeriesTest::tearDown()
{
    // Clean up after the test run.
    m_series.reset();
}

//------------------------------------------------------------------------------

void ImageSeriesTest::imageTest()
{
    CPPUNIT_ASSERT(m_series);

    data::Image::sptr img = data::Image::New();
    utestData::generator::Image::generateRandomImage(img, core::tools::Type("float"));
    CPPUNIT_ASSERT(img);

    m_series->setImage(img);
    CPPUNIT_ASSERT_EQUAL(img, m_series->getImage());
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::data
