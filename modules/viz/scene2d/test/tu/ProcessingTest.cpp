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

#include "ProcessingTest.hpp"

#include <core/runtime/EConfigurationElement.hpp>

#include <data/Histogram.hpp>
#include <data/Image.hpp>

#include <service/base.hpp>
#include <service/registry/ObjectService.hpp>

#include <utest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::viz::scene2d::ut::ProcessingTest);

namespace sight::module::viz::scene2d
{

namespace ut
{

//------------------------------------------------------------------------------

void ProcessingTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ProcessingTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ProcessingTest::histogramTest()
{
    typedef signed short ImageType;
    const std::size_t sizeX     = 50;
    const std::size_t sizeY     = 50;
    const std::size_t sizeZ     = 50;
    const std::size_t imageSize = sizeX * sizeY * sizeZ;

    std::string implementation = "sight::module::viz::scene2d::processing::SComputeHistogram";

    // Configure data hierarchy
    data::Image::sptr image         = data::Image::New();
    data::Histogram::sptr histogram = data::Histogram::New();

    // Create image.
    image->resize({sizeX, sizeY, sizeZ}, core::tools::Type::s_INT16, data::Image::GRAY_SCALE);

    const auto dumpLock = image->lock();
    auto itr            = image->begin<ImageType>();
    const auto itrEnd   = image->end<ImageType>();

    int count = 0;
    for( ; itr != itrEnd ; ++itr)
    {
        if(count < imageSize / 4)
        {
            *itr = 10;
        }
        else if(count < imageSize / 2)
        {
            *itr = 20;
        }
        else if(count < 3 * imageSize / 4)
        {
            *itr = 30;
        }
        else if(count < imageSize)
        {
            *itr = 40;
        }

        ++count;
    }

    auto srv = service::add(implementation, "");
    CPPUNIT_ASSERT_MESSAGE("Impossible to create the service '" + implementation + "'", srv);

    service::IService::ConfigType config;
    config.add("binsWidth", 1.0f);

    srv->setInput(image, "image");
    srv->setInOut(histogram, "histogram");

    srv->setConfiguration(config);
    srv->configure();
    srv->start().wait();
    srv->update().wait();
    srv->stop().wait();
    service::remove(srv);

    data::Histogram::fwHistogramValues values = histogram->getValues();
    CPPUNIT_ASSERT_EQUAL((std::size_t) 40 - 10 + 1, values.size());

    CPPUNIT_ASSERT_EQUAL((float) 10, histogram->getMinValue());

    CPPUNIT_ASSERT_EQUAL((float) 40, histogram->getMaxValue());

    CPPUNIT_ASSERT_EQUAL((long) imageSize / 4, histogram->getNbPixels(10, 11));
    CPPUNIT_ASSERT_EQUAL((long) imageSize / 4, histogram->getNbPixels(20, 21));
    CPPUNIT_ASSERT_EQUAL((long) imageSize / 4, histogram->getNbPixels(30, 31));
    CPPUNIT_ASSERT_EQUAL((long) imageSize / 4, histogram->getNbPixels(40, 41));
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace sight::module::viz::scene2d
