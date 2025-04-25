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

#include "validated_test.hpp"

#include <data/image.hpp>

#include <service/op.hpp>

#include <utest/wait.hpp>

#include <boost/property_tree/xml_parser.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::validated);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void validated::setUp()
{
}

//------------------------------------------------------------------------------

void validated::tearDown()
{
}

//------------------------------------------------------------------------------

void validated::filled_test()
{
    using namespace std::literals::string_literals;

    auto srv   = service::add("sight::module::data::validate");
    auto image = std::make_shared<sight::data::image>();

    std::optional<bool> valid;
    auto valid_slot = core::com::new_slot(
        [&]()
        {
            valid = true;
        });
    valid_slot->set_worker(sight::core::thread::get_default_worker());
    srv->signal("valid")->connect(valid_slot);

    std::optional<bool> invalid;
    auto invalid_slot = core::com::new_slot(
        [&]()
        {
            invalid = true;
        });
    invalid_slot->set_worker(sight::core::thread::get_default_worker());
    srv->signal("invalid")->connect(invalid_slot);

    std::optional<bool> is_valid;
    auto is_valid_slot = core::com::new_slot(
        [&](bool _is_valid)
        {
            is_valid = _is_valid;
        });
    is_valid_slot->set_worker(sight::core::thread::get_default_worker());
    srv->signal("is_valid")->connect(is_valid_slot);

    std::optional<bool> is_invalid;
    auto is_invalid_slot = core::com::new_slot(
        [&](bool _is_invalid)
        {
            is_invalid = _is_invalid;
        });
    is_invalid_slot->set_worker(sight::core::thread::get_default_worker());
    srv->signal("is_invalid")->connect(is_invalid_slot);

    service::config_t config;
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

        CPPUNIT_ASSERT_EQUAL(false, valid.has_value());
        CPPUNIT_ASSERT_EQUAL(true, invalid.has_value());
        CPPUNIT_ASSERT_EQUAL(true, is_valid.has_value());
        CPPUNIT_ASSERT_EQUAL(true, is_invalid.has_value());
        CPPUNIT_ASSERT_EQUAL(true, *invalid);
        CPPUNIT_ASSERT_EQUAL(false, *is_valid);
        CPPUNIT_ASSERT_EQUAL(true, *is_invalid);
    }

    valid      = std::nullopt;
    invalid    = std::nullopt;
    is_valid   = std::nullopt;
    is_invalid = std::nullopt;

    {
        image->resize({4, 4, 1}, core::type::UINT8, sight::data::image::gray_scale);

        srv->update().get();

        SIGHT_TEST_WAIT(valid.has_value());
        SIGHT_TEST_WAIT(is_valid.has_value());
        SIGHT_TEST_WAIT(is_invalid.has_value());

        CPPUNIT_ASSERT_EQUAL(true, valid.has_value());
        CPPUNIT_ASSERT_EQUAL(false, invalid.has_value());
        CPPUNIT_ASSERT_EQUAL(true, is_valid.has_value());
        CPPUNIT_ASSERT_EQUAL(true, is_invalid.has_value());
        CPPUNIT_ASSERT_EQUAL(true, *valid);
        CPPUNIT_ASSERT_EQUAL(true, *is_valid);
        CPPUNIT_ASSERT_EQUAL(false, *is_invalid);
    }

    CPPUNIT_ASSERT_NO_THROW(srv->stop().get());
    service::remove(srv);
}

//------------------------------------------------------------------------------

} // namespace sight::module::data::ut
