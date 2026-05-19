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

// Include before wait.hpp
#include <doctest/doctest.h>

#include <utest/service_fixture.hpp>
#include <utest/wait.hpp>

namespace
{

class validate_fixture : public sight::utest::service_fixture
{
public:

    validate_fixture() :
        sight::utest::service_fixture("sight::module::data::validate")
    {
    }
};

} // namespace

TEST_SUITE("sight::module::data::validate")
{
    TEST_CASE_FIXTURE(validate_fixture, "equals")
    {
        using namespace std::literals::string_literals;

        auto string = std::make_shared<sight::data::string>();

        std::optional<bool> valid;
        auto valid_slot = new_slot([&](){valid = true;});
        m_service->signal("valid")->connect(valid_slot);

        std::optional<bool> invalid;
        auto invalid_slot = new_slot([&](){invalid = true;});
        m_service->signal("invalid")->connect(invalid_slot);

        std::optional<bool> is_valid;
        auto is_valid_slot = new_slot([&](bool _is_valid){is_valid = _is_valid;});
        m_service->signal("is_valid")->connect(is_valid_slot);

        std::optional<bool> is_invalid;
        auto is_invalid_slot = new_slot([&](bool _is_invalid){is_invalid = _is_invalid;});
        m_service->signal("is_invalid")->connect(is_invalid_slot);

        m_service->set_config("<config id='sight::data::validator::equals'><value>expected</value></config>");
        m_service->set_input(string, "data");
        REQUIRE_NOTHROW(m_service->configure());
        REQUIRE_NOTHROW(m_service->start().get());

        {
            REQUIRE_NOTHROW(m_service->update().get());

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
            string->set_value("test_value");
            REQUIRE_NOTHROW(m_service->update().get());

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
            string->set_value("expected");
            REQUIRE_NOTHROW(m_service->update().get());

            SIGHT_TEST_WAIT(invalid.has_value());
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
    }
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(validate_fixture, "filled")
    {
        using namespace std::literals::string_literals;

        auto image = std::make_shared<sight::data::image>();

        std::optional<bool> valid;
        auto valid_slot = new_slot([&](){valid = true;});
        m_service->signal("valid")->connect(valid_slot);

        std::optional<bool> invalid;
        auto invalid_slot = new_slot([&](){invalid = true;});
        m_service->signal("invalid")->connect(invalid_slot);

        std::optional<bool> is_valid;
        auto is_valid_slot = new_slot([&](bool _is_valid){is_valid = _is_valid;});
        m_service->signal("is_valid")->connect(is_valid_slot);

        std::optional<bool> is_invalid;
        auto is_invalid_slot = new_slot([&](bool _is_invalid){is_invalid = _is_invalid;});
        m_service->signal("is_invalid")->connect(is_invalid_slot);

        m_service->set_config("<config id='sight::data::validator::filled' />");
        m_service->set_input(image, "data");
        REQUIRE_NOTHROW(m_service->configure());
        REQUIRE_NOTHROW(m_service->start().get());

        {
            REQUIRE_NOTHROW(m_service->update().get());

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

            REQUIRE_NOTHROW(m_service->update().get());

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
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(validate_fixture, "on_change_true")
    {
        using namespace std::literals::string_literals;

        auto string    = std::make_shared<sight::data::string>();
        auto on_change = std::make_shared<sight::data::boolean>(true);

        std::atomic<int> valid_count      = 0;
        std::atomic<int> invalid_count    = 0;
        std::atomic<int> is_valid_count   = 0;
        std::atomic<int> is_invalid_count = 0;

        auto valid_slot = new_slot([&](){valid_count++;});
        m_service->signal("valid")->connect(valid_slot);

        auto invalid_slot = new_slot([&](){invalid_count++;});
        m_service->signal("invalid")->connect(invalid_slot);

        auto is_valid_slot = new_slot([&](bool){is_valid_count++;});
        m_service->signal("is_valid")->connect(is_valid_slot);

        auto is_invalid_slot = new_slot([&](bool){is_invalid_count++;});
        m_service->signal("is_invalid")->connect(is_invalid_slot);

        m_service->set_config("<config id='sight::data::validator::equals'><value>expected</value></config>");
        m_service->set_input(string, "data");
        m_service->set_inout(on_change, "on_change");
        REQUIRE_NOTHROW(m_service->configure());
        REQUIRE_NOTHROW(m_service->start().get());

        {
            // First update: should emit signals (state change from none to invalid)
            REQUIRE_NOTHROW(m_service->update().get());

            SIGHT_TEST_FAIL_WAIT(valid_count == 0);
            SIGHT_TEST_FAIL_WAIT(invalid_count == 1);
            SIGHT_TEST_FAIL_WAIT(is_valid_count == 1);
            SIGHT_TEST_FAIL_WAIT(is_invalid_count == 1);
        }

        {
            // Second update with same data: should NOT emit signals (state unchanged)
            REQUIRE_NOTHROW(m_service->update().get());

            SIGHT_TEST_FAIL_WAIT(valid_count == 0);
            SIGHT_TEST_FAIL_WAIT(invalid_count == 1);
            SIGHT_TEST_FAIL_WAIT(is_valid_count == 1);
            SIGHT_TEST_FAIL_WAIT(is_invalid_count == 1);
        }

        {
            // Change value to valid: should emit signals (state change from invalid to valid)
            string->set_value("expected");
            REQUIRE_NOTHROW(m_service->update().get());

            SIGHT_TEST_FAIL_WAIT(valid_count == 1);
            SIGHT_TEST_FAIL_WAIT(invalid_count == 1);
            SIGHT_TEST_FAIL_WAIT(is_valid_count == 2);
            SIGHT_TEST_FAIL_WAIT(is_invalid_count == 2);
        }

        {
            // Third update with same valid data: should NOT emit signals (state unchanged)
            REQUIRE_NOTHROW(m_service->update().get());

            SIGHT_TEST_FAIL_WAIT(valid_count == 1);
            SIGHT_TEST_FAIL_WAIT(invalid_count == 1);
            SIGHT_TEST_FAIL_WAIT(is_valid_count == 2);
            SIGHT_TEST_FAIL_WAIT(is_invalid_count == 2);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(validate_fixture, "on_change_false")
    {
        using namespace std::literals::string_literals;

        auto string    = std::make_shared<sight::data::string>();
        auto on_change = std::make_shared<sight::data::boolean>(false);

        std::atomic<int> valid_count      = 0;
        std::atomic<int> invalid_count    = 0;
        std::atomic<int> is_valid_count   = 0;
        std::atomic<int> is_invalid_count = 0;

        auto valid_slot = new_slot([&](){valid_count++;});
        m_service->signal("valid")->connect(valid_slot);

        auto invalid_slot = new_slot([&](){invalid_count++;});
        m_service->signal("invalid")->connect(invalid_slot);

        auto is_valid_slot = new_slot([&](bool){is_valid_count++;});
        m_service->signal("is_valid")->connect(is_valid_slot);

        auto is_invalid_slot = new_slot([&](bool){is_invalid_count++;});
        m_service->signal("is_invalid")->connect(is_invalid_slot);

        m_service->set_config("<config id='sight::data::validator::equals'><value>expected</value></config>");
        m_service->set_input(string, "data");
        m_service->set_inout(on_change, "on_change");
        REQUIRE_NOTHROW(m_service->configure());
        REQUIRE_NOTHROW(m_service->start().get());

        {
            // First update: invalid
            REQUIRE_NOTHROW(m_service->update().get());

            SIGHT_TEST_FAIL_WAIT(valid_count == 0);
            SIGHT_TEST_FAIL_WAIT(invalid_count == 1);
            SIGHT_TEST_FAIL_WAIT(is_valid_count == 1);
            SIGHT_TEST_FAIL_WAIT(is_invalid_count == 1);
        }

        {
            // Second update with same data: should emit signals again (on_change is false)
            REQUIRE_NOTHROW(m_service->update().get());

            SIGHT_TEST_FAIL_WAIT(valid_count == 0);
            SIGHT_TEST_FAIL_WAIT(invalid_count == 2);
            SIGHT_TEST_FAIL_WAIT(is_valid_count == 2);
            SIGHT_TEST_FAIL_WAIT(is_invalid_count == 2);
        }

        {
            // Change value to valid: should emit signals (state change from invalid to valid)
            string->set_value("expected");
            REQUIRE_NOTHROW(m_service->update().get());

            SIGHT_TEST_FAIL_WAIT(valid_count == 1);
            SIGHT_TEST_FAIL_WAIT(invalid_count == 2);
            SIGHT_TEST_FAIL_WAIT(is_valid_count == 3);
            SIGHT_TEST_FAIL_WAIT(is_invalid_count == 3);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(validate_fixture, "valid_property")
    {
        auto image      = std::make_shared<sight::data::image>();
        auto valid_prop = std::make_shared<sight::data::boolean>(false);

        std::atomic<int> modified_count = 0;
        auto modified_slot              = new_slot([&](){modified_count++;});
        valid_prop->signal(sight::data::signals::MODIFIED)->connect(modified_slot);

        m_service->set_config("<config id='sight::data::validator::filled' />");
        m_service->set_input(image, "data");
        m_service->set_inout(valid_prop, "valid");
        REQUIRE_NOTHROW(m_service->configure());
        REQUIRE_NOTHROW(m_service->start().get());

        {
            // Empty image: invalid, property starts at false -> value unchanged, no MODIFIED emitted
            REQUIRE_NOTHROW(m_service->update().get());

            SIGHT_TEST_WAIT(modified_count == 0);
            CHECK_EQ(0, modified_count.load());
            CHECK_EQ(false, valid_prop->value());
        }

        {
            // Fill image: valid, property changes false -> true -> MODIFIED emitted once
            image->resize({4, 4, 1}, sight::core::type::UINT8, sight::data::image::gray_scale);
            REQUIRE_NOTHROW(m_service->update().get());

            SIGHT_TEST_FAIL_WAIT(modified_count == 1);
            CHECK_EQ(true, valid_prop->value());
        }

        {
            // Same valid image: property stays true -> no new MODIFIED
            REQUIRE_NOTHROW(m_service->update().get());

            SIGHT_TEST_WAIT(modified_count == 1);
            CHECK_EQ(1, modified_count.load());
            CHECK_EQ(true, valid_prop->value());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(validate_fixture, "invalid_property")
    {
        auto image        = std::make_shared<sight::data::image>();
        auto invalid_prop = std::make_shared<sight::data::boolean>(true);

        std::atomic<int> modified_count = 0;
        auto modified_slot              = new_slot([&](){modified_count++;});
        invalid_prop->signal(sight::data::signals::MODIFIED)->connect(modified_slot);

        m_service->set_config("<config id='sight::data::validator::filled' />");
        m_service->set_input(image, "data");
        m_service->set_inout(invalid_prop, "invalid");
        REQUIRE_NOTHROW(m_service->configure());
        REQUIRE_NOTHROW(m_service->start().get());

        {
            // Empty image: invalid, property starts at true -> value unchanged, no MODIFIED emitted
            REQUIRE_NOTHROW(m_service->update().get());

            SIGHT_TEST_WAIT(modified_count == 0);
            CHECK_EQ(0, modified_count.load());
            CHECK_EQ(true, invalid_prop->value());
        }

        {
            // Fill image: valid, property changes true -> false -> MODIFIED emitted once
            image->resize({4, 4, 1}, sight::core::type::UINT8, sight::data::image::gray_scale);
            REQUIRE_NOTHROW(m_service->update().get());

            SIGHT_TEST_FAIL_WAIT(modified_count == 1);
            CHECK_EQ(false, invalid_prop->value());
        }

        {
            // Same valid image: property stays false -> no new MODIFIED
            REQUIRE_NOTHROW(m_service->update().get());

            SIGHT_TEST_WAIT(modified_count == 1);
            CHECK_EQ(1, modified_count.load());
            CHECK_EQ(false, invalid_prop->value());
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::module::data::validate")
