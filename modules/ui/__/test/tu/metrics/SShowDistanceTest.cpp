/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "SShowDistanceTest.hpp"

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>

#include <service/op.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::metrics::ut::SShowDistanceTest);

namespace sight::module::ui::metrics::ut
{

//------------------------------------------------------------------------------

void SShowDistanceTest::setUp()
{
    m_showDistance = service::add("sight::module::ui::metrics::SShowDistance");
    CPPUNIT_ASSERT_MESSAGE(
        "Failed to create service 'sight::module::ui::metrics::SShowDistance'",
        m_showDistance
    );
}

//------------------------------------------------------------------------------

void SShowDistanceTest::tearDown()
{
    if(!m_showDistance->isStopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_showDistance->stop().get());
    }

    service::remove(m_showDistance);
}

//------------------------------------------------------------------------------

void SShowDistanceTest::basicTest()
{
    auto image = std::make_shared<data::Image>();
    image->resize({1, 2, 3}, core::type::UINT8, data::Image::RGB);

    m_showDistance->setInOut(image, "image");
    CPPUNIT_ASSERT_NO_THROW(m_showDistance->configure());
    CPPUNIT_ASSERT_NO_THROW(m_showDistance->start().get());

    CPPUNIT_ASSERT(data::helper::MedicalImage::getDistanceVisibility(*image));
    CPPUNIT_ASSERT_NO_THROW(m_showDistance->update().get());
    CPPUNIT_ASSERT(!data::helper::MedicalImage::getDistanceVisibility(*image));
    CPPUNIT_ASSERT_NO_THROW(m_showDistance->update().get());
    CPPUNIT_ASSERT(data::helper::MedicalImage::getDistanceVisibility(*image));
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::metrics::ut
