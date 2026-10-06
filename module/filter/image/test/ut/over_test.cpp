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

#include <algorithm>
#include <core/runtime/runtime.hpp>

#include <data/image.hpp>

#include <service/base.hpp>

#include <doctest/doctest.h>

#include <utest/service_fixture.hpp>

#include <array>
#include <cstdint>
#include <memory>

namespace
{

struct over_tester : sight::utest::service_fixture
{
    over_tester() :
        service_fixture("sight::module::filter::image::over")
    {
    }
};

} // namespace

TEST_SUITE("sight::module::filter::image::over")
{
//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(over_tester, "repeated_input_key_preserves_order")
    {
        sight::core::runtime::init();
        const auto module = sight::core::runtime::load_module("sight::module::filter::image");
        REQUIRE(module);

        auto first  = std::make_shared<sight::data::image>();
        auto second = std::make_shared<sight::data::image>();
        auto third  = std::make_shared<sight::data::image>();
        auto output = std::make_shared<sight::data::image>();

        const auto initialize = [](const sight::data::image::sptr& _image, const std::array<std::uint8_t, 4>& _pixels)
                                {
                                    _image->resize(
                                        {4, 1, 1},
                                        sight::core::type::UINT8,
                                        sight::data::image::pixel_format_t::gray_scale
                                    );
                                    _image->set_spacing({1., 1., 1.});
                                    _image->set_origin({0., 0., 0.});
                                    _image->set_orientation({1., 0., 0., 0., 1., 0., 0., 0., 1.});
                                    const auto dump_lock = _image->dump_lock();
                                    std::ranges::copy(_pixels, _image->begin<std::uint8_t>());
                                };

        initialize(first, {1, 2, 3, 0});
        initialize(second, {0, 4, 0, 0});
        initialize(third, {5, 0, 6, 7});

        m_service->set_input(first, "input.image", false, false, 0);
        m_service->set_input(second, "input.image", false, false, 1);
        m_service->set_input(third, "input.image", false, false, 2);
        m_service->set_inout(output, "output.image");
        m_service->configure();
        m_service->start().get();
        m_service->update().get();

        const auto dump_lock = output->dump_lock();
        const std::array<std::uint8_t, 4> expected {5, 4, 6, 7};
        CHECK(std::equal(expected.begin(), expected.end(), output->begin<std::uint8_t>()));
    }

//-----------------------------------------------------------------------------
} // TEST_SUITE("sight::module::filter::image::over")
