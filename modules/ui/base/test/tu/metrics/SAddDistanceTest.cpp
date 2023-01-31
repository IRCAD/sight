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

#include "SAddDistanceTest.hpp"

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/Point.hpp>
#include <data/Vector.hpp>

#include <service/base.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::base::metrics::ut::SAddDistanceTest);

namespace sight::module::ui::base::metrics::ut
{

//------------------------------------------------------------------------------

void SAddDistanceTest::setUp()
{
    m_addDistance = service::add("sight::module::ui::base::metrics::SAddDistance");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::base::metrics::SAddDistance'", m_addDistance);
}

//------------------------------------------------------------------------------

void SAddDistanceTest::tearDown()
{
    if(!m_addDistance->isStopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_addDistance->stop().get());
    }

    service::remove(m_addDistance);
}

//------------------------------------------------------------------------------

void SAddDistanceTest::basicTest()
{
    auto image = data::Image::New();
    image->resize({1, 2, 3}, core::Type::UINT8, data::Image::RGB);
    image->setOrigin({3, 2, 1});
    image->setSpacing({1, 1, 1});

    m_addDistance->setInOut(image, "image");
    CPPUNIT_ASSERT_NO_THROW(m_addDistance->configure());
    CPPUNIT_ASSERT_NO_THROW(m_addDistance->start().get());
    CPPUNIT_ASSERT_NO_THROW(m_addDistance->update().get());

    data::Vector::sptr distances = data::helper::MedicalImage::getDistances(*image);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), distances->size());
    auto pl = data::PointList::dynamicCast((*distances)[0]);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), pl->getPoints().size());

    CPPUNIT_ASSERT_EQUAL(*data::Point::New(3., 2., 1.), *pl->getPoints()[0]);
    CPPUNIT_ASSERT_EQUAL(*data::Point::New(4., 4., 4.), *pl->getPoints()[1]);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::metrics::ut
