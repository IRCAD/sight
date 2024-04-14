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

#include "show_distance_test.hpp"

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <service/op.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::metrics::ut::show_distance_test);

namespace sight::module::ui::metrics::ut
{

//------------------------------------------------------------------------------

void show_distance_test::setUp()
{
    m_show_distance = service::add("sight::module::ui::metrics::show_distance");
    CPPUNIT_ASSERT_MESSAGE(
        "Failed to create service 'sight::module::ui::metrics::show_distance'",
        m_show_distance
    );
}

//------------------------------------------------------------------------------

void show_distance_test::tearDown()
{
    if(!m_show_distance->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_show_distance->stop().get());
    }

    service::remove(m_show_distance);
}

//------------------------------------------------------------------------------

void show_distance_test::basic_test()
{
    auto image = std::make_shared<data::image>();
    image->resize({1, 2, 3}, core::type::UINT8, data::image::rgb);

    m_show_distance->set_inout(image, "image");
    CPPUNIT_ASSERT_NO_THROW(m_show_distance->configure());
    CPPUNIT_ASSERT_NO_THROW(m_show_distance->start().get());

    CPPUNIT_ASSERT(data::helper::medical_image::get_distance_visibility(*image));
    CPPUNIT_ASSERT_NO_THROW(m_show_distance->update().get());
    CPPUNIT_ASSERT(!data::helper::medical_image::get_distance_visibility(*image));
    CPPUNIT_ASSERT_NO_THROW(m_show_distance->update().get());
    CPPUNIT_ASSERT(data::helper::medical_image::get_distance_visibility(*image));
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::metrics::ut
