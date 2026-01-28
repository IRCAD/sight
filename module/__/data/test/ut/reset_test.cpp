/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <data/image.hpp>
#include <data/string.hpp>

#include <service/op.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::module::data::reset")
{
//------------------------------------------------------------------------------

    TEST_CASE("string")
    {
        using namespace std::literals::string_literals;

        auto reset_srv = sight::service::add("sight::module::data::reset");
        auto string    = std::make_shared<sight::data::string>("Hello world");
        reset_srv->set_inout(string, "target");
        CHECK_NOTHROW(reset_srv->configure());

        CHECK_NOTHROW(reset_srv->start().get());
        CHECK_EQ("Hello world"s, string->get_value());

        CHECK_NOTHROW(reset_srv->update().get());
        CHECK_EQ(""s, string->get_value());

        CHECK_NOTHROW(reset_srv->stop().get());
        sight::service::remove(reset_srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("image")
    {
        using namespace std::literals::string_literals;

        auto reset_srv = sight::service::add("sight::module::data::reset");
        auto image     = std::make_shared<sight::data::image>();

        const sight::data::image::size_t size       = {10, 15, 56};
        const sight::data::image::spacing_t spacing = {12, 100, 200};
        const sight::data::image::origin_t origin   = {-96, 52, 123.4};
        image->resize(size, sight::core::type::UINT8, sight::data::image::gray_scale);
        image->set_spacing(spacing);
        image->set_origin(origin);

        reset_srv->set_inout(image, "target");
        CHECK_NOTHROW(reset_srv->configure());
        CHECK_NOTHROW(reset_srv->start().get());
        CHECK(size == image->size());
        CHECK(spacing == image->spacing());
        CHECK(origin == image->origin());

        CHECK_NOTHROW(reset_srv->update().get());
        CHECK(sight::data::image::size_t({0, 0, 0}) == image->size());
        CHECK(sight::data::image::spacing_t({0, 0, 0}) == image->spacing());
        CHECK(sight::data::image::origin_t({0, 0, 0}) == image->origin());

        CHECK_NOTHROW(reset_srv->stop().get());
        sight::service::remove(reset_srv);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::module::data::reset")
