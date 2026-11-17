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
        reset_srv->set_inout(string, "data.target", {}, {}, 0);
        boost::property_tree::ptree ptree;
        reset_srv->set_config(ptree);
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

        reset_srv->set_inout(image, "data.target", {}, {}, 0);
        boost::property_tree::ptree ptree;
        reset_srv->set_config(ptree);
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

    TEST_CASE("batch_strings")
    {
        using namespace std::literals::string_literals;

        auto reset_srv = sight::service::add("sight::module::data::reset");

        auto string1 = std::make_shared<sight::data::string>("Hello");
        auto string2 = std::make_shared<sight::data::string>("World");
        auto string3 = std::make_shared<sight::data::string>("Test");

        reset_srv->set_inout(string1, "data.target", {}, {}, 0);
        reset_srv->set_inout(string2, "data.target", {}, {}, 1);
        reset_srv->set_inout(string3, "data.target", {}, {}, 2);

        boost::property_tree::ptree ptree;
        reset_srv->set_config(ptree);
        CHECK_NOTHROW(reset_srv->configure());
        CHECK_NOTHROW(reset_srv->start().get());

        CHECK_EQ("Hello"s, string1->get_value());
        CHECK_EQ("World"s, string2->get_value());
        CHECK_EQ("Test"s, string3->get_value());

        CHECK_NOTHROW(reset_srv->update().get());
        CHECK_EQ(""s, string1->get_value());
        CHECK_EQ(""s, string2->get_value());
        CHECK_EQ(""s, string3->get_value());

        CHECK_NOTHROW(reset_srv->stop().get());
        sight::service::remove(reset_srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("batch_images")
    {
        auto reset_srv = sight::service::add("sight::module::data::reset");

        auto image1 = std::make_shared<sight::data::image>();
        auto image2 = std::make_shared<sight::data::image>();

        const sight::data::image::size_t size       = {10, 15, 56};
        const sight::data::image::spacing_t spacing = {12, 100, 200};
        const sight::data::image::origin_t origin   = {-96, 52, 123.4};
        image1->resize(size, sight::core::type::UINT8, sight::data::image::gray_scale);
        image1->set_spacing(spacing);
        image1->set_origin(origin);

        image2->resize(size, sight::core::type::UINT8, sight::data::image::gray_scale);
        image2->set_spacing(spacing);
        image2->set_origin(origin);

        reset_srv->set_inout(image1, "data.target", {}, {}, 0);
        reset_srv->set_inout(image2, "data.target", {}, {}, 1);

        boost::property_tree::ptree ptree;
        reset_srv->set_config(ptree);
        CHECK_NOTHROW(reset_srv->configure());
        CHECK_NOTHROW(reset_srv->start().get());

        CHECK(size == image1->size());
        CHECK(size == image2->size());
        CHECK(spacing == image1->spacing());
        CHECK(spacing == image2->spacing());

        CHECK_NOTHROW(reset_srv->update().get());

        CHECK(sight::data::image::size_t({0, 0, 0}) == image1->size());
        CHECK(sight::data::image::size_t({0, 0, 0}) == image2->size());
        CHECK(sight::data::image::spacing_t({0, 0, 0}) == image1->spacing());
        CHECK(sight::data::image::spacing_t({0, 0, 0}) == image2->spacing());

        CHECK_NOTHROW(reset_srv->stop().get());
        sight::service::remove(reset_srv);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::module::data::reset")
