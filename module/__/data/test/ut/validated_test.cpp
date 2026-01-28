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

#include <service/op.hpp>

#include <utest/wait.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::module::data::validated")
{
//------------------------------------------------------------------------------

    TEST_CASE("filled_test")
    {
        using namespace std::literals::string_literals;

        auto srv   = sight::service::add("sight::module::data::validate");
        auto image = std::make_shared<sight::data::image>();

        std::optional<bool> valid;
        auto valid_slot = sight::core::com::new_slot(
            [&]()
        {
            valid = true;
        });
        valid_slot->set_worker(sight::core::thread::get_default_worker());
        srv->signal("valid")->connect(valid_slot);

        std::optional<bool> invalid;
        auto invalid_slot = sight::core::com::new_slot(
            [&]()
        {
            invalid = true;
        });
        invalid_slot->set_worker(sight::core::thread::get_default_worker());
        srv->signal("invalid")->connect(invalid_slot);

        std::optional<bool> is_valid;
        auto is_valid_slot = sight::core::com::new_slot(
            [&](bool _is_valid)
        {
            is_valid = _is_valid;
        });
        is_valid_slot->set_worker(sight::core::thread::get_default_worker());
        srv->signal("is_valid")->connect(is_valid_slot);

        std::optional<bool> is_invalid;
        auto is_invalid_slot = sight::core::com::new_slot(
            [&](bool _is_invalid)
        {
            is_invalid = _is_invalid;
        });
        is_invalid_slot->set_worker(sight::core::thread::get_default_worker());
        srv->signal("is_invalid")->connect(is_invalid_slot);

        sight::service::config_t config;
        std::stringstream config_string;
        config_string << R"(<config id="sight::data::validator::filled" />)";
        boost::property_tree::read_xml(config_string, config);
        srv->set_config(config);
        srv->set_input(image, "data");
        srv->configure();
        srv->start().get();

        {
            srv->update().get();

            SIGHT_TEST_WAIT(invalid.has_value());
            SIGHT_TEST_WAIT(is_valid.has_value());
            SIGHT_TEST_WAIT(is_invalid.has_value());

            CHECK_EQ(false, valid.has_value());
            CHECK_EQ(true, invalid.has_value());
            CHECK_EQ(true, is_valid.has_value());
            CHECK_EQ(true, is_invalid.has_value());
            CHECK_EQ(true, *invalid);
            CHECK_EQ(false, *is_valid);
            CHECK_EQ(true, *is_invalid);
        }

        valid      = std::nullopt;
        invalid    = std::nullopt;
        is_valid   = std::nullopt;
        is_invalid = std::nullopt;

        {
            image->resize({4, 4, 1}, sight::core::type::UINT8, sight::data::image::gray_scale);

            srv->update().get();

            SIGHT_TEST_WAIT(valid.has_value());
            SIGHT_TEST_WAIT(is_valid.has_value());
            SIGHT_TEST_WAIT(is_invalid.has_value());

            CHECK_EQ(true, valid.has_value());
            CHECK_EQ(false, invalid.has_value());
            CHECK_EQ(true, is_valid.has_value());
            CHECK_EQ(true, is_invalid.has_value());
            CHECK_EQ(true, *valid);
            CHECK_EQ(true, *is_valid);
            CHECK_EQ(false, *is_invalid);
        }

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::module::data::validated")
