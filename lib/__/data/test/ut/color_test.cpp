/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <data/color.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::color")
{
//------------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        //-----------test values
        const float r = 0.2F;
        const float g = 0.8F;
        const float b = 0.5F;
        const float a = 0.6F;

        auto color = std::make_shared<sight::data::color>(r, g, b, a);

        CHECK_EQ(color->red(), r);
        CHECK_EQ(color->green(), g);
        CHECK_EQ(color->blue(), b);
        CHECK_EQ(color->alpha(), a);

        CHECK(color->is_type_of("sight::data::color"));
        CHECK(color->is_type_of("sight::data::string_serializable"));

        auto color2 = std::make_shared<sight::data::color>(r, g, b, a);
        CHECK(*color == *color2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("accessors")
    {
        //-----------test values
        const float r = 0.2F;
        const float g = 0.8F;
        const float b = 0.5F;
        const float a = 0.6F;

        auto color = std::make_shared<sight::data::color>();

        sight::data::color::array_t array;
        array[0] = r;
        array[1] = g;
        array[2] = b;
        array[3] = a;

        color->set_rgba(array);

        CHECK_EQ(color->rgba()[0], r);
        CHECK_EQ(color->rgba()[1], g);
        CHECK_EQ(color->rgba()[2], b);
        CHECK_EQ(color->rgba()[3], a);

        auto color2 = std::make_shared<sight::data::color>();
        CHECK(*color != *color2);

        color2->set_rgba(array);
        CHECK(*color == *color2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("string")
    {
        // fuchsia string value
        const std::string fuchsia = "#FF006E";
        // fuchsia float values
        const float r = 1.F;
        const float g = 0.F;
        const float b = 110.F / 255.F;
        const float a = 1.F;

        sight::data::color::sptr color = std::make_shared<sight::data::color>();

        color->from_string(fuchsia);

        CHECK_EQ(color->rgba()[0], r);
        CHECK_EQ(color->rgba()[1], g);
        CHECK_EQ(color->rgba()[2], b);
        CHECK_EQ(color->rgba()[3], a);

        auto color2 = std::make_shared<sight::data::color>();
        CHECK(*color != *color2);

        color2->from_string(fuchsia);
        CHECK(*color == *color2);

        const std::string fuchsia_converted_to_str = color->to_string();
        CHECK_EQ(std::string("#FF006EFF"), fuchsia_converted_to_str);

        sight::data::color color3;
        color3.from_string("1.0;0.;0.4314;1.0");
        CHECK_EQ(color->to_string(), color3.to_string());

        sight::data::color color4("#339966CC");
        CHECK_EQ(sight::data::color(0.2F, 0.6F, 0.4F, 0.8F), color4);
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto color1 = std::make_shared<sight::data::color>();
        auto color2 = std::make_shared<sight::data::color>();

        CHECK(*color1 == *color2);
        CHECK(!(*color1 != *color2));

        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(...) \
            color1->set_rgba(__VA_ARGS__); \
            CHECK_MESSAGE( \
                *color1 != *color2, \
                "The colors should be different when the first is set with " #__VA_ARGS__ \
            ); \
            CHECK_MESSAGE( \
                !(*color1 == *color2), \
                "The colors should be different when the first is set with " #__VA_ARGS__ \
            ); \
            color2->set_rgba(color1->rgba()); \
            CHECK_MESSAGE( \
                *color1 == *color2, \
                "The colors should be equal when both are set with " #__VA_ARGS__ \
            ); \
            CHECK_MESSAGE( \
                !(*color1 != *color2), \
                "The colors should be equal when both are set with " #__VA_ARGS__ \
            );

        TEST(1.F, 0.F, 0.F, 0.F);
        TEST(0.F, 1.F, 0.F, 0.F);
        TEST(0.F, 0.F, 1.F, 0.F);
        TEST(0.F, 0.F, 0.F, 1.F);

    #undef TEST
    }

//------------------------------------------------------------------------------

    TEST_CASE("rgba_u8")
    {
        const uint8_t r = 0xFF;
        const uint8_t g = 0x45;
        const uint8_t b = 0x98;
        const uint8_t a = 0x12;

        auto color = std::make_shared<sight::data::color>();
        color->set_rgba(r, g, b, a);

        const auto rgba = color->rgba_u8();

        CHECK_EQ(rgba[0], r);
        CHECK_EQ(rgba[1], g);
        CHECK_EQ(rgba[2], b);
        CHECK_EQ(rgba[3], a);
    }
} // TEST_SUITE("sight::data::color")
