/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <core/runtime/runtime.hpp>

#include <data/boolean.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>

#include <service/base.hpp>

#include <doctest/doctest.h>

#include <utest/service_fixture.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>

namespace
{

struct threshold_tester : sight::utest::service_fixture
{
    explicit threshold_tester(const bool _binary = false) :
        service_fixture("sight::module::filter::image::threshold"),
        input(std::make_shared<sight::data::image>()),
        output(std::make_shared<sight::data::image>()),
        lower(std::make_shared<sight::data::integer>(2)),
        upper(std::make_shared<sight::data::integer>(28)),
        binary(std::make_shared<sight::data::boolean>(_binary))
    {
        input->resize({5, 1, 1}, sight::core::type::UINT8, sight::data::image::pixel_format_t::gray_scale);
        input->set_spacing({1., 1., 1.});
        input->set_origin({0., 0., 0.});
        input->set_orientation({1., 0., 0., 0., 1., 0., 0., 0., 1.});
        {
            const auto dump_lock = input->dump_lock();
            const std::array<std::uint8_t, 5> pixels {1, 2, 10, 28, 29};
            std::ranges::copy(pixels, input->begin<std::uint8_t>());
        }

        m_service->set_input(input, "input.image", false);
        m_service->set_inout(output, "output.image", false);
        m_service->set_input(lower, "config.lower", false);
        m_service->set_input(upper, "config.upper", false);
        m_service->set_input(binary, "config.binary", false);
        REQUIRE_NOTHROW(m_service->configure());
        REQUIRE_NOTHROW(m_service->start().get());
    }

    sight::data::image::sptr input;
    sight::data::image::sptr output;
    sight::data::integer::sptr lower;
    sight::data::integer::sptr upper;
    sight::data::boolean::sptr binary;
};

struct threshold_tester_binary : threshold_tester
{
    explicit threshold_tester_binary() :
        threshold_tester(true)
    {
    }
};

} // namespace

TEST_SUITE("sight::module::filter::image::threshold")
{
//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(threshold_tester_binary, "binary_mode")
    {
        sight::core::runtime::init();
        REQUIRE(sight::core::runtime::load_module("sight::module::filter::image"));

        REQUIRE_NOTHROW(m_service->update().get());
        const auto dump_lock = output->dump_lock();
        const std::array<std::uint8_t, 5> expected {0, 255, 255, 255, 0};
        CHECK(std::equal(expected.begin(), expected.end(), output->begin<std::uint8_t>()));
    }

//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(threshold_tester, "non_binary_mode_preserves_in_range_values")
    {
        sight::core::runtime::init();
        REQUIRE(sight::core::runtime::load_module("sight::module::filter::image"));
        REQUIRE_NOTHROW(m_service->update().get());

        const auto dump_lock = output->dump_lock();
        const std::array<std::uint8_t, 5> expected {0, 2, 10, 28, 0};
        CHECK(std::equal(expected.begin(), expected.end(), output->begin<std::uint8_t>()));
    }

//-----------------------------------------------------------------------------
} // TEST_SUITE("sight::module::filter::image::threshold")
