/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "reset_test.hpp"

#include <data/image.hpp>
#include <data/string.hpp>

#include <service/op.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::reset_test);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void reset_test::setUp()
{
}

//------------------------------------------------------------------------------

void reset_test::tearDown()
{
}

//------------------------------------------------------------------------------

void reset_test::string_test()
{
    using namespace std::literals::string_literals;

    auto reset_srv = service::add("sight::module::data::reset");
    auto string    = std::make_shared<sight::data::string>("Hello world");
    reset_srv->set_inout(string, "target");
    CPPUNIT_ASSERT_NO_THROW(reset_srv->configure());

    CPPUNIT_ASSERT_NO_THROW(reset_srv->start().get());
    CPPUNIT_ASSERT_EQUAL("Hello world"s, string->get_value());

    CPPUNIT_ASSERT_NO_THROW(reset_srv->update().get());
    CPPUNIT_ASSERT_EQUAL(""s, string->get_value());

    CPPUNIT_ASSERT_NO_THROW(reset_srv->stop().get());
    service::remove(reset_srv);
}

//------------------------------------------------------------------------------

void reset_test::image_test()
{
    using namespace std::literals::string_literals;

    auto reset_srv = service::add("sight::module::data::reset");
    auto image     = std::make_shared<sight::data::image>();

    const sight::data::image::size_t size       = {10, 15, 56};
    const sight::data::image::spacing_t spacing = {12, 100, 200};
    const sight::data::image::origin_t origin   = {-96, 52, 123.4};
    image->resize(size, core::type::UINT8, sight::data::image::gray_scale);
    image->set_spacing(spacing);
    image->set_origin(origin);

    reset_srv->set_inout(image, "target");
    CPPUNIT_ASSERT_NO_THROW(reset_srv->configure());
    CPPUNIT_ASSERT_NO_THROW(reset_srv->start().get());
    CPPUNIT_ASSERT(size == image->size());
    CPPUNIT_ASSERT(spacing == image->spacing());
    CPPUNIT_ASSERT(origin == image->origin());

    CPPUNIT_ASSERT_NO_THROW(reset_srv->update().get());
    CPPUNIT_ASSERT(sight::data::image::size_t({0, 0, 0}) == image->size());
    CPPUNIT_ASSERT(sight::data::image::spacing_t({0, 0, 0}) == image->spacing());
    CPPUNIT_ASSERT(sight::data::image::origin_t({0, 0, 0}) == image->origin());

    CPPUNIT_ASSERT_NO_THROW(reset_srv->stop().get());
    service::remove(reset_srv);
}

//------------------------------------------------------------------------------

} // namespace sight::module::data::ut
