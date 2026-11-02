/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
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

#include <core/runtime/runtime.hpp>
#include <core/type.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/real.hpp>
#include <data/series_set.hpp>
#include <service/op.hpp>
#include <utest/service_fixture.hpp>

#include <doctest/doctest.h>

//------------------------------------------------------------------------------

static std::string load_adaptors()
{
    sight::core::runtime::init();
    sight::core::runtime::load_module("sight::module::ui::qt");
    sight::core::runtime::load_module("sight::module::viz::scene3d");
    sight::core::runtime::load_module("sight::module::viz::scene3d_qt");
    sight::core::runtime::load_module("sight::module::viz::scene3d::test");
    return "sight::module::viz::scene3d::adaptor::image_series";
}

namespace
{

struct negato_settings_fixture : sight::utest::service_fixture
{
    negato_settings_fixture() :
        sight::utest::service_fixture(load_adaptors())
    {
        m_service->set_id("settings_image_series");
        m_render    = sight::service::add("sight::viz::scene3d::render");
        m_offscreen = std::make_shared<sight::data::image>();
        m_render->set_inout(m_offscreen, "offscreen");
        m_render->set_config(
            "<inout key='offscreen' uid='output'/>"
            "<scene width='64' height='64' render_mode='manual'>"
            "<layer id='default'><adaptor uid='settings_image_series'/></layer></scene>"
        );
        m_render->configure();
        m_render->start().get();
        m_series       = std::make_shared<sight::data::series_set>();
        m_transparency = std::make_shared<sight::data::real>(0.25);
        m_service->set_input(m_series, "data.series", true);
        m_service->set_input(m_transparency, "config.transparency", true);
        m_service->set_config("<config representation='negato3d' autoresetcamera='false' interactive='false'/>");
        m_service->configure();
    }

    ~negato_settings_fixture() override
    {
        if(m_service->started())
        {
            m_service->stop().get();
        }

        if(m_render->started())
        {
            m_render->stop().get();
        }

        sight::service::remove(m_render);
    }

    //------------------------------------------------------------------------------

    void add_image()
    {
        auto image = std::make_shared<sight::data::image_series>();
        image->resize({4, 4, 4}, sight::core::type::UINT8, sight::data::image::gray_scale);
        m_series->push_back(image);
    }

    //------------------------------------------------------------------------------

    void check_children(std::size_t _count)
    {
        const auto children = sight::service::get_services("sight::module::viz::scene3d::adaptor::negato3d");
        REQUIRE(children.size() == _count);
        for(const auto& child : children)
        {
            CHECK(child->input("config.transparency").lock().get_shared() == m_transparency);
        }
    }

    sight::service::base::sptr m_render;
    sight::data::image::sptr m_offscreen;
    sight::data::series_set::sptr m_series;
    sight::data::real::sptr m_transparency;
};

} // namespace

TEST_SUITE("sight::module::viz::scene3d::adaptor::image_series")
{
    TEST_CASE_FIXTURE(negato_settings_fixture, "negato_children_share_transparency_after_addition_and_restart")
    {
        add_image();
        m_service->start().get();
        check_children(1);
        m_transparency->set_value(0.75);
        add_image();
        m_service->update().get();
        check_children(2);
        m_service->stop().get();
        check_children(0);
        m_service->start().get();
        check_children(2);
    }
}
