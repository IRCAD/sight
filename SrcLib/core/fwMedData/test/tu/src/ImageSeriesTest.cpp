/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTest/generator/Image.hpp>
#include <fwTools/Type.hpp>

#include "fwMedData/ImageSeries.hpp"

#include "ImageSeriesTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedData::ut::ImageSeriesTest );

namespace fwMedData
{
namespace ut
{

//------------------------------------------------------------------------------

void ImageSeriesTest::setUp()
{
    // Set up context before running a test.
    m_series = ::fwMedData::ImageSeries::New();
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

    ::fwData::Image::sptr img = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img, ::fwTools::Type("float"));
    CPPUNIT_ASSERT(img);

    m_series->setImage(img);
    CPPUNIT_ASSERT_EQUAL(img, m_series->getImage());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedData
